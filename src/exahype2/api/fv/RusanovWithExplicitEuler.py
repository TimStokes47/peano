# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.datamodel
import peano4.toolbox.blockstructured.api

from ..Solver import Solver
from .actionsets import ProjectPatchOntoFaces


class RusanovWithExplicitEuler(Solver.NumericalScheme):
    """!

    Finite Volumes with a Rusanov Riemann solver and explicit Euler time
    stepping, realised over a block-structured mesh.

    """

    def __init__(self, solver: Solver, patch_size: int, overlap: int = 1):
        """!
        
        The constructor memorises the attributes. However, it does not do this
        within this class, but it adds the important properties straight to the 
        solver, so anybody else working against this solver knows about the 
        properties introduced by the numerical scheme.
        
        @see @ref __create_data_structures()

        patch_size: int
          Number of Finite Volumes per axis within one patch (i.e. within
          one mesh cell).

        overlap: int
          Number of Finite Volumes the halo layer reconstructed around a
          patch - from the neighbouring patches' faces - is wide.

        """
        super().__init__(solver)
        self.patch_size = patch_size
        self.overlap = overlap
        self.__create_data_structures()


    def __create_data_structures(self):
        """!

        Create the mesh data structures (a Finite Volume patch per cell,
        plus the face-based overlap patches used to reconstruct its halo)
        this numerical scheme requires.

        Ported from the legacy exahype2.solvers.fv.FV.create_data_structures():
        one @ref peano4.api.datamodel.Patch of patch_size^d Finite Volumes
        per mesh cell, plus three overlap patches of width 2*overlap
        attached to the mesh faces:

        - _patch_overlap_old: a backup of the previous halo, so schemes that
          need to interpolate in time (e.g. local time stepping) don't have
          to back up the whole patch, just its overlap.
        - _patch_overlap_new: the halo actually used to reconstruct a patch
          together with its neighbours before the cell update (@see
          exahype2.api.fv.actionsets.ReconstructPatchWithHalo).
        - _patch_overlap_update: the update accumulated by the current
          sweep. Rolled over into _patch_overlap_new (which is itself first
          backed up into _patch_overlap_old) by
          exahype2.api.fv.actionsets.RollOverUpdatedFace once the update is
          complete. This indirection is required for AMR, where a patch may
          receive several partial updates before it is complete - going
          through _patch_overlap_new directly would lose the "old" data
          before every single one of them.

        All four are assigned onto self._solver (the shared
        exahype2.api.Solver instance both this numerical scheme and its
        surrounding solver's Solver.GridConstruction/Solver.Plotter
        counterparts hold a reference to as self._solver), not onto self,
        since every FV action set - @see
        exahype2.api.fv.actionsets.ReconstructPatchWithHalo,
        exahype2.api.fv.actionsets.RollOverUpdatedFace,
        exahype2.api.fv.actionsets.AdaptivityCriterion - is handed that
        shared solver and looks the patches up as solver._patch /
        solver._patch_overlap_*.

        Storage is hardwired to smart pointers (peano4.api.datamodel's
        PatchToDoubleArrayWithSmartPointer), matching the legacy default
        (Storage.SmartPointers for both cell and face data) - the legacy
        Storage enum's other two backends (CallStack, Heap) are not
        exposed here.

        Two pieces of the legacy method are deliberately not ported yet:
        - The cell/face label creation
          (exahype2.grid.create_cell_label()/create_face_label()) - that
          module has not been ported to the new API.
        - The load/store/compute guards
          (FV._provide_cell_data_to_compute_kernels_default_guard() and
          friends) - they are keyed off the legacy SolverState state
          machine, which the task-graph architecture has no equivalent of:
          a task only ever runs within its own phase's task graph in the
          first place, so "are we still constructing the grid" does not
          need to be re-checked at runtime the same way.

        """
        total_unknowns = self._solver.unknowns + self._solver.auxiliary_variables
        identifier = self._solver._unknown_identifier()

        self._solver._patch = peano4.api.datamodel.Patch(
            (self.patch_size, self.patch_size, self.patch_size),
            total_unknowns,
            identifier,
        )
        self._solver._patch_overlap_current_time_step = peano4.api.datamodel.Patch(
            (2 * self.overlap, self.patch_size, self.patch_size),
            total_unknowns,
            identifier + "CurrentTimeStep",
        )
        self._solver._patch_overlap_update = peano4.api.datamodel.Patch(
            (2 * self.overlap, self.patch_size, self.patch_size),
            total_unknowns,
            identifier + "Update",
        )


    def __add_data_to_repository(self, peano_project):
        """!
        
        Consult @ref page_exahype_programmers_guide_new_solver for some information
        how this routine fits to the typical workflow when we introduce a new
        solver.
        
        """
        peano4.api.Logger().log("""
Patch data
----------
{}
Patch overlap data
------------------
{}
{}""".format( str(self._solver._patch), str(self._solver._patch_overlap_current_time_step), str(self._solver._patch_overlap_update) ) )

        peano_project.datamodel.add_cell(self._solver._patch)
        peano_project.datamodel.add_face(self._solver._patch_overlap_current_time_step)
        peano_project.datamodel.add_face(self._solver._patch_overlap_update)

        self._solver._patch.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch, "double"
        )
        self._solver._patch_overlap_current_time_step.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_overlap_current_time_step, "double"
        )
        self._solver._patch_overlap_update.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_overlap_update, "double"
        )

        return
        
        self._solver._patch_overlap_old.generator.merge_method_definition = (
            peano4.toolbox.blockstructured.api.get_face_merge_implementation(
                self._solver._patch_overlap_old
            )
        )
        self._solver._patch_overlap_new.generator.merge_method_definition = (
            peano4.toolbox.blockstructured.api.get_face_merge_implementation(
                self._solver._patch_overlap_new
            )
        )
        self._solver._patch.generator.merge_method_definition = (
            peano4.toolbox.blockstructured.api.get_cell_merge_implementation(
                self._solver._patch
            )
        )

        self._solver._patch_overlap_old.generator.includes += """
#include "peano4/utils/Loop.h"
#include "repositories/SolverRepository.h"
"""
        self._solver._patch_overlap_new.generator.includes += """
#include "peano4/utils/Loop.h"
#include "repositories/SolverRepository.h"
"""

        self._solver._patch_overlap_old.generator.send_condition = "false"
        self._solver._patch_overlap_old.generator.receive_and_merge_condition = "false"
        self._solver._patch_overlap_new.generator.send_condition = "false"
        self._solver._patch_overlap_new.generator.receive_and_merge_condition = "false"
        self._solver._patch_overlap_update.generator.send_condition = "false"
        self._solver._patch_overlap_update.generator.receive_and_merge_condition = "false"

        self._solver._patch.generator.includes += """
#include "../repositories/SolverRepository.h"
"""
        self._solver._patch_overlap_old.generator.includes += """
#include "../repositories/SolverRepository.h"
"""
        self._solver._patch_overlap_new.generator.includes += """
#include "../repositories/SolverRepository.h"
"""
        self._solver._patch_overlap_update.generator.includes += """
#include "../repositories/SolverRepository.h"
"""

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         time_step):
        """!

        """
        peano4.api.Logger().log("Create time stepping task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        self.__add_data_to_repository(peano_project)
        
        reconstruction_task         = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Reconstruction", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        update_cell_task            = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Update cell", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        compute_cfl_condition_task  = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Compute CFL condition", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task       = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        compute_time_step_size_task = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Compute time step size", entity=peano4.api.taskgraph.Task.GridEntity.GLOBAL)
        
        update_cell_task.add_dependency(reconstruction_task)
        compute_cfl_condition_task.add_dependency(update_cell_task)
        project_on_faces_task.add_dependency(update_cell_task)
        compute_time_step_size_task.add_dependency(compute_cfl_condition_task)

        time_step.add_task(reconstruction_task)
        time_step.add_task(update_cell_task)
        time_step.add_task(compute_cfl_condition_task)
        time_step.add_task(project_on_faces_task)
        time_step.add_task(compute_time_step_size_task)

        project_on_faces_task.add_action_set(
            ProjectPatchOntoFaces(self._solver, guard="true")
        )

        reconstruction_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        update_cell_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        compute_cfl_condition_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        project_on_faces_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        project_on_faces_task.use_face(self._solver._patch_overlap_update, peano4.api.taskgraph.Task.DataAccess.WRITE)
        compute_time_step_size_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        
        if peano4.api.Logger().is_visualisation:
            time_step.visualise(title="Time stepping task graph for {}".format(self._solver.name))
        elif peano4.api.Logger().is_text:
            time_step.log_as_text(title="Time stepping task graph for {}".format(self._solver.name))
