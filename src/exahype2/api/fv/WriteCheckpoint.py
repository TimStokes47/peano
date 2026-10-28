# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4.api
import toolbox.blockstructured.api

from ..Solver import Solver
from .PlotAsVTKFile import _PlotPatchesIfPlotterIsActive
from .actionsets import ApplyDirichletBoundaryConditions


class WriteCheckpoint(Solver.Plotter):
    """!

    Write checkpoints of a Finite Volume solver

    The checkpoint is configured like PlotAsVTKFile, i.e. it is written in
    regular time intervals. Different to a plot, it writes the whole state
    of the solver such that a simulation can continue from it. It hence has
    neither plot filters nor a selection of quantities: It writes all
    entries of Q, i.e. the unknowns and the auxiliary variables, of all
    cells which the solver updates.

    - It writes each patch together with its halo. Before it writes the
      data, it reconstructs the halo exactly as the time step does: It
      sets the ghost volumes along the domain boundary (Dirichlet
      boundary conditions), interpolates the hanging faces on adaptive
      meshes and gathers patch and face data into solver._patch_with_halo
      (@ref toolbox.blockstructured.api.ReconstructPatchWithHalo.ReconstructPatchWithHalo). A patch
      with N volumes per axis and a halo of M volumes is written as a patch
      with N+2M volumes per axis. Its box covers the cell plus the halo, so
      every volume sits at its true position, and neighbouring patches
      overlap.
    - It writes all values without loss: with 17 significant digits by
      default, small values are not set to zero, and the patch offsets and
      sizes carry 17 significant digits, too
      (@ref tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::setExactOutput()).
    - The meta-data block in the header of each file holds the solver's
      state (@ref exahype2::Solver::toCheckpointMetaData()): its time
      stamps, time step sizes, admissible time step size and mesh sizes,
      plus the attributes the Python API adds to the solver's class, e.g.
      the time stamps of the next outputs of all of its plotters.

    The files are called checkpoint-SolverName by default. A solver can
    have a checkpoint next to its other plotters
    (@ref exahype2.api.Solver.Solver.add_plotter()).
    A simulation continues from a checkpoint with
    @ref exahype2.api.blockstructured.LoadCheckpoint.LoadCheckpoint instead of
    its usual grid construction.

    The reconstruction only reads the patch and face data, i.e. a
    checkpoint does not change the solution. The Dirichlet boundary
    conditions overwrite the ghost volumes with the values they have at the
    current time stamp anyway, and the next time step sets them again.

    """

    def __init__(
        self,
        solver: Solver,
        first_plot_time_stamp=0.0,
        time_in_between_plots=0.1,
        plotter_precision=17,
        output_path=".",
        filename="checkpoint",
        name: str = None,
    ):
        """!

        @param solver: exahype2.api.Solver
          The solver whose state is written.

        @param first_plot_time_stamp: float
          Time stamp of the first checkpoint.

        @param time_in_between_plots: float
          Time in between two checkpoints. Has to be positive.

        @param plotter_precision: int
          Number of significant digits written per value. The default of 17
          ensures that the values are read back without loss.

        @param output_path: str
          Directory the checkpoint files are written into.

        @param filename: str
          The checkpoint files are called filename-SolverName.

        @param name: str or None
          Name of the plotter (@ref exahype2.api.Solver.Solver.Plotter).

        """
        super().__init__(solver, name)

        if time_in_between_plots <= 0:
            raise Exception("Time in between checkpoints has to be bigger than 0")
        if plotter_precision <= 0:
            raise Exception("Precision has to be bigger than 0")
        self._first_plot_time_stamp = first_plot_time_stamp
        self._time_in_between_plots = time_in_between_plots
        self._plotter_precision = plotter_precision
        self._output_path = output_path
        self._filename = filename

    def output_file_prefix(self):
        return os.path.join(self._output_path, self._filename + "-" + self._solver.name)

    def construct_output(self, exahype2_project, peano_project, plot):
        """!

        Add the reconstruction of the halo and the writing of the checkpoint
        to the plot graph

        All action sets are guarded by the checkpoint's is<Name>Active(),
        i.e. the tasks do no work in plot phases without checkpoint. The
        writing task is the last one, so it moves the checkpoint on to its
        next time stamp.

        """
        peano4.api.Logger().log(
            "Create checkpoint task graph for Finite Volume scheme over block-structured meshes"
        )

        for attribute in [
            "_patch",
            "_patch_with_halo",
            "_patch_overlap_current_time_step",
        ]:
            assert hasattr(self._solver, attribute), (
                f"Solver {self._solver.name} has no attribute {attribute}. "
                "WriteCheckpoint assumes the numerical scheme has added this field "
                "to the solver immediately upon construction."
            )

        self._add_time_interval_bookkeeping_to_solver(
            self._first_plot_time_stamp, self._time_in_between_plots
        )
        self._solver._add_checkpoint_meta_data_to_solver()

        Access = peano4.api.taskgraph.Task.DataAccess
        is_active = self.create_is_active_predicate()
        cell_guard = (
            "("
            + is_active
            + ") and ("
            + self._solver.grid_selector.createCellGuard()
            + ")"
        )
        includes = (
            self._solver._get_default_includes() + self._solver.user_solver_includes
        )

        reconstruction_task = peano4.api.taskgraph.Task(
            name=f"{self._solver.name}_{self.name}_Reconstruct",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
            may_spawn_background_tasks=False,
        )
        write_checkpoint_task = peano4.api.taskgraph.Task(
            name=f"{self._solver.name}_{self.name}_Write checkpoint",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
            may_spawn_background_tasks=False,
        )
        plot.add_task(reconstruction_task)
        plot.add_task(write_checkpoint_task)

        ## Reconstruct the halo. Patch and face data have to survive it.
        reconstruction_task.use_cell(self._solver._patch, Access.READ_AND_WRITE)
        reconstruction_task.use_cell(self._solver._patch_with_halo, Access.WRITE)
        reconstruction_task.use_face(
            self._solver._patch_overlap_current_time_step, Access.READ_AND_WRITE
        )
        reconstruction_task.add_action_set(
            toolbox.blockstructured.api.ReconstructPatchWithHalo(
                patch=self._solver._patch,
                patch_overlap=self._solver._patch_overlap_current_time_step,
                reconstructed_patch=self._solver._patch_with_halo,
                guard=cell_guard,
                add_assertions_to_halo_exchange=False,
                additional_includes=includes,
            )
        )
        ## On adaptive meshes, interpolate the coarse face data onto the
        ## hanging faces of fine cells. Only schemes which support adaptive
        ## meshes have an interpolation scheme.
        interpolation = getattr(self._solver.numerical_scheme, "interpolation", None)
        if interpolation is not None:
            reconstruction_task.add_action_set(
                toolbox.blockstructured.api.InterpolateHangingFaces(
                    self._solver._patch_overlap_current_time_step,
                    scheme=interpolation,
                    guard=is_active,
                    additional_includes=includes,
                )
            )
        ## Set the ghost volumes along the domain boundary
        ApplyDirichletBoundaryConditions.add_task_to_time_step(
            self._solver,
            exahype2_project,
            plot,
            reconstruction_task,
            name_suffix=f" for {self.name}",
            additional_guard=is_active,
        )

        ## Write the patches with their halos
        write_checkpoint_task.add_dependency(reconstruction_task)
        write_checkpoint_task.use_cell(
            self._solver._patch_with_halo, Access.READ_AND_DISCARD
        )
        write_checkpoint_task.add_action_set(
            _PlotPatchesIfPlotterIsActive(
                is_active_predicate=is_active,
                finish_call=self.create_finish_call(),
                filename=self.output_file_prefix(),
                patch=self._solver._patch_with_halo,
                dataset_name=self._solver._patch_with_halo.name,
                description=", ".join(self._solver.variable_names_per_entry())
                or self._solver._unknown_identifier(),
                guard=self._solver.grid_selector.createCellGuard(),
                additional_includes="""
#include "../repositories/SolverRepository.h"
""",
                precision=self._plotter_precision,
                time_stamp_evaluation="0.5*(repositories::getMinTimeStamp()+repositories::getMaxTimeStamp())",
                patch_scaling=self._solver._patch_with_halo.dim[0]
                / self._solver._patch.dim[0],
                metadata_evaluation="repositories::"
                + self._solver.get_name_of_global_instance()
                + ".toCheckpointMetaData()",
                exact_output=True,
            )
        )

        if peano4.api.Logger().is_visualisation:
            plot.visualise(title="Plot task graph for {}".format(self._solver.name))
        if peano4.api.Logger().is_text:
            plot.log_as_text(title="Plot task graph for {}".format(self._solver.name))
