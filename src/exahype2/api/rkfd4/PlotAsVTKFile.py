# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4.api
import toolbox.blockstructured.api

from ..Solver import Solver


class _PlotPatchesIfSolverWantsToPlot(
    toolbox.blockstructured.api.PlotPatchesInPeanoBlockFormat
):
    """!

    PlotPatchesInPeanoBlockFormat which only does something if its solver
    wants to plot

    The toolbox action set opens a new snapshot file in beginTraversal()
    and adds it to the index file. That has to be skipped altogether for a
    solver which does not want to plot, so the guards of all operations
    are combined with a check of the solver's wantsToPlot(). wantsToPlot() does not
    change throughout the plot sweep, as finishPlottingStep() - which moves
    the solver on to its next plot time stamp - is only called in
    unprepareTraversal().

    """

    def __init__(self, solver_instance, **kwargs):
        super().__init__(**kwargs)
        self._solver_instance = solver_instance

    def get_guard_of_operation(self, operation_name):
        return (
            "repositories::"
            + self._solver_instance
            + ".wantsToPlot() and ("
            + super().get_guard_of_operation(operation_name)
            + ")"
        )

    def get_body_of_unprepareTraversal(self):
        return "  repositories::" + self._solver_instance + ".finishPlottingStep();\n"


class PlotAsVTKFile(Solver.Plotter):
    """!

    Plotting counterpart of the fourth-order Finite Differences with
    Runge-Kutta time stepping (@ref exahype2.api.rkfd4): writes the patch
    data solver._patch of a block-structured mesh out as a VTK file. As the
    Finite Differences are cell-centred, each grid cell of a patch is
    plotted as one cell of the plot mesh.

    The plotter decides itself when to plot (@ref exahype2.api.Solver.Solver.Plotter): it adds
    the attribute _nextPlotTimeStamp plus the methods wantsToPlot() and
    finishPlottingStep() to the solver's C++ class. The solver wants to plot
    as soon as its min time stamp has reached _nextPlotTimeStamp. After each
    plot, finishPlottingStep() increments _nextPlotTimeStamp by
    time_in_between_plots.

    If the solver's variables carry names, the plotter writes them as
    comma-separated list into the description of the output files (@ref
    exahype2.api.Solver.Solver.variable_names_per_entry()). Paraview then shows them in the
    name of the data array, and tarch.plotter.api.notebook.show_solution()
    uses them to label the unknowns. Otherwise, the description is the
    solver's unknown identifier.

    The class is a copy of exahype2.api.fv.PlotAsVTKFile, as rkfd4 does not
    depend on the Finite Volume package.

    """

    def __init__(
        self,
        solver: Solver,
        first_plot_time_stamp=0.0,
        time_in_between_plots=0.1,
        plotter_precision=5,
        output_path=".",
    ):
        """!

        @param first_plot_time_stamp: float
          Time stamp of the first plot.

        @param time_in_between_plots: float
          Has to be positive. If you don't want to have any plots for a
          solver, don't give it a plotter.

        @param plotter_precision: int
          Number of significant digits written per value. Has to be
          positive.

        @param output_path: str
          Directory the plot files are written into.

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
        """!

        Add a new filter to Peano/ExaHyPE

        Restrict plotting to patches overlapping a given box. Filters are
        baked directly into the guard of the generated plot action set (see
        construct_output()), i.e. the generated C++ code evaluates the exact
        offset/size this filter was constructed with - it never consults any
        project-wide, separately configured filter object.

        @param offset: (float,float,float)

        @param size: (float,float,float)

        @param frequency: int
          A positive value. Peano makes snapshots every dt simulation
          units. This is something you specify once per simulation. But
          you might decide only to splot every k of these snapshots.

          @todo Not evaluated yet: every filter currently plots on every
            snapshot it spatially overlaps with, regardless of frequency.
        """
        if frequency != 1:
            raise Exception(
                "Frequency-based plot filtering is not wired up yet in the "
                "new exahype2.api - exahype2.api.rkfd4.PlotAsVTKFile only supports spatial "
                "(offset/size) plot filters for now. Call add_plot_filter() "
                "with frequency=1 (the default)."
            )
        self._plot_filters.append((offset, size))

    def construct_output(self, exahype2_project, peano_project, plot):
        """!"""
        peano4.api.Logger().log(
            "Create plot task graph for fourth-order Finite Differences with Runge-Kutta over block-structured meshes"
        )

        write_vtk_file_task = peano4.api.taskgraph.Task(
            name=f"{self._solver.name}_Write VTK file",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
            may_spawn_background_tasks=False,
        )

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
        # Only the cells selected by the solver's grid selector are plotted,
        # as only those are updated. Other cells would hold outdated data overlapping the actual
        # solution.
        guard = "(" + self._solver.grid_selector.createCellGuard() + ")"
        if self._plot_filters:
            guard += (
                " and ("
                + " or ".join(
                    "marker.overlaps({"
                    + ",".join(str(x) for x in offset)
                    + "},{"
                    + ",".join(str(x) for x in size)
                    + "})"
                    for offset, size in self._plot_filters
                )
                + ")"
            )

        self._add_plot_bookkeeping_to_solver()

        plot_patches_action_set = _PlotPatchesIfSolverWantsToPlot(
            solver_instance=self._solver.get_name_of_global_instance(),
            filename=os.path.join(self._output_path, "solution-" + self._solver.name),
            patch=self._solver._patch,
            dataset_name=self._solver._unknown_identifier(),
            description=", ".join(self._solver.variable_names_per_entry())
            or self._solver._unknown_identifier(),
            guard=guard,
            additional_includes="""
#include "../repositories/SolverRepository.h"
""",
            precision=self._plotter_precision,
            time_stamp_evaluation="0.5*(repositories::getMinTimeStamp()+repositories::getMaxTimeStamp())",
        )

        write_vtk_file_task.add_action_set(plot_patches_action_set)
        write_vtk_file_task.use_cell(
            self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE
        )

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
        attribute = (
            "double _nextPlotTimeStamp = " + str(self._first_plot_time_stamp) + ";"
        )
        if attribute in self._solver._abstract_solver_attributes:
            return

        self._solver.add_abstract_solver_attribute(attribute)
        self._solver.add_abstract_solver_method(
            "bool wantsToPlot() const;",
            lambda qualified_class_name: "bool "
            + qualified_class_name
            + "::wantsToPlot() const {\n"
            "  return tarch::la::greaterEquals(getMinTimeStamp(), _nextPlotTimeStamp);\n"
            "}\n",
        )
        self._solver.add_abstract_solver_method(
            "void finishPlottingStep() override;",
            lambda qualified_class_name: "void "
            + qualified_class_name
            + "::finishPlottingStep() {\n"
            "  if (wantsToPlot()) {\n"
            "    _nextPlotTimeStamp += " + str(self._time_in_between_plots) + ";\n"
            "  }\n"
            "}\n",
        )
