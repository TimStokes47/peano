# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4.api
import toolbox.blockstructured.api

from ..Solver import Solver


class _PlotPatchesIfSolverWantsToPlot(toolbox.blockstructured.api.PlotPatchesInPeanoBlockFormat):
    """!

    PlotPatchesInPeanoBlockFormat which only does something if its solver
    wants to plot

    The toolbox action set opens a new snapshot file in beginTraversal()
    and adds it to the index file. That has to be skipped altogether for a
    solver which does not want to plot, so all operation bodies are wrapped
    into a check of the solver's wantsToPlot(). wantsToPlot() does not
    change throughout the plot sweep, as finishPlottingStep() - which moves
    the solver on to its next plot time stamp - is only called in
    unprepareTraversal().

    """

    def __init__(self, solver_instance, **kwargs):
        super().__init__(**kwargs)
        self._solver_instance = solver_instance

    def get_body_of_operation(self, operation_name):
        return (
            "\n  if (repositories::" + self._solver_instance + ".wantsToPlot()) {\n"
            + super().get_body_of_operation(operation_name)
            + "\n  }\n"
        )

    def get_body_of_unprepareTraversal(self):
        return "  repositories::" + self._solver_instance + ".finishPlottingStep();\n"


class PlotAsVTKFile(Solver.Plotter):
    """!

    Plotting counterpart to RusanovWithExplicitEuler: maps the patch data of
    a Finite Volume scheme with a Rusanov Riemann solver, realised over a
    block-structured mesh, onto a plot mesh and writes it out as a VTK file.

    The plotter decides itself when to plot (@see Solver.Plotter): it adds
    the attribute _nextPlotTimeStamp plus the methods wantsToPlot() and
    finishPlottingStep() to the solver's C++ class. The solver wants to plot
    as soon as its min time stamp has reached _nextPlotTimeStamp. After each
    plot, finishPlottingStep() increments _nextPlotTimeStamp by
    time_in_between_plots.

    """

    def __init__(self,
                 solver: Solver,
                 first_plot_time_stamp=0.0,
                 time_in_between_plots=0.1,
                 plotter_precision=5,
                 output_path = "."):
        """!

        first_plot_time_stamp: float
          Time stamp of the first plot.

        time_in_between_plots: float
          Has to be positive. If you don't want to have any plots for a
          solver, don't give it a plotter.

        """
        super().__init__(solver)

        if time_in_between_plots <= 0:
            raise Exception("Time in between plots has to be bigger than 0")
        if plotter_precision <= 0:
            raise Exception("Plotter precision has to be bigger than 0")
        self._first_plot_time_stamp = first_plot_time_stamp
        self._time_in_between_plots = time_in_between_plots
        self._plotter_precision = plotter_precision
        self._plot_filters = []
        self._output_path = output_path

    def add_plot_filter(self, offset, size, frequency=1):
        """
        Add a new filter to Peano/ExaHyPE

        Restrict plotting to patches overlapping a given box. Filters are
        baked directly into the guard of the generated plot action set (@see
        construct_output()), i.e. the generated C++ code evaluates the exact
        offset/size this filter was constructed with - it never consults any
        project-wide, separately configured filter object.

        offset: (float,float,float)

        size: (float,float,float)

        frequency: int
          A positive value. Peano makes snapshots every dt simulation
          units. This is something you specify once per simulation. But
          you might decide only to splot every k of these snapshots.

          @todo Not evaluated yet: every filter currently plots on every
            snapshot it spatially overlaps with, regardless of frequency.
        """
        if frequency != 1:
            raise Exception(
                "Frequency-based plot filtering is not wired up yet in the "
                "new exahype2.api - PlotAsVTKFile only supports spatial "
                "(offset/size) plot filters for now. Call add_plot_filter() "
                "with frequency=1 (the default)."
            )
        self._plot_filters.append((offset, size))

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         plot):
        """!

        """
        peano4.api.Logger().log("Create plot task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        write_vtk_file_task         = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Write VTK file", entity=peano4.api.taskgraph.Task.GridEntity.CELL)

        plot.add_task(write_vtk_file_task)

        assert hasattr(self._solver, "_patch"), (
            f"Solver {self._solver.name} has no attribute _patch. "
            "PlotAsVTKFile assumes the numerical scheme has added this field "
            "to the solver immediately upon construction."
        )

        # Guard is built directly from self._plot_filters (known here, in
        # Python) rather than delegating to some project-wide C++ filter
        # object: the generated code below carries the exact offset/size
        # values this plotter was configured with, and nothing else.
        # Only leaf cells are plotted: refined cells are not updated (@see
        # UpdateCellAndComputeGlobalTimeStep's guard), so their patches still
        # hold the initial condition and would overlap the actual solution.
        guard = "not marker.willBeRefined() and not marker.hasBeenRefined()"
        if self._plot_filters:
            guard += " and (" + " or ".join(
                "marker.overlaps({"
                + ",".join(str(x) for x in offset)
                + "},{"
                + ",".join(str(x) for x in size)
                + "})"
                for offset, size in self._plot_filters
            ) + ")"

        self._add_plot_bookkeeping_to_solver()

        plot_patches_action_set = _PlotPatchesIfSolverWantsToPlot(
            solver_instance=self._solver.get_name_of_global_instance(),
            filename=os.path.join(self._output_path, "solution-" + self._solver.name),
            patch=self._solver._patch,
            dataset_name=self._solver._unknown_identifier(),
            description=self._solver._unknown_identifier(),
            guard=guard,
            additional_includes="""
#include "../repositories/SolverRepository.h"
""",
            precision=self._plotter_precision,
            time_stamp_evaluation="0.5*(repositories::getMinTimeStamp()+repositories::getMaxTimeStamp())",
        )

        write_vtk_file_task.add_action_set( plot_patches_action_set)
        write_vtk_file_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)

        if peano4.api.Logger().is_visualisation:
            plot.visualise(title="Plot task graph for {}".format(self._solver.name))
        if peano4.api.Logger().is_text:
            plot.log_as_text(title="Plot task graph for {}".format(self._solver.name))

    def _add_plot_bookkeeping_to_solver(self):
        """!

        Add the plot state and wantsToPlot()/finishPlottingStep() to the
        solver's C++ class. The solver keeps these members once added, so
        we add them only once, even if the Peano 4 project is generated
        multiple times.

        """
        attribute = "double _nextPlotTimeStamp = " + str(self._first_plot_time_stamp) + ";"
        if attribute in self._solver._abstract_solver_attributes:
            return

        self._solver.add_abstract_solver_attribute(attribute)
        self._solver.add_abstract_solver_method(
            "bool wantsToPlot() const;",
            lambda qualified_class_name:
                "bool " + qualified_class_name + "::wantsToPlot() const {\n"
                "  return tarch::la::greaterEquals(getMinTimeStamp(), _nextPlotTimeStamp);\n"
                "}\n"
        )
        self._solver.add_abstract_solver_method(
            "void finishPlottingStep() override;",
            lambda qualified_class_name:
                "void " + qualified_class_name + "::finishPlottingStep() {\n"
                "  if (wantsToPlot()) {\n"
                "    _nextPlotTimeStamp += " + str(self._time_in_between_plots) + ";\n"
                "  }\n"
                "}\n"
        )
