# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4.api
import toolbox.blockstructured.api

from ..Quantities import resolve_quantities
from ..Solver import Solver


class _PlotPatchesIfPlotterIsActive(
    toolbox.blockstructured.api.PlotPatchesInPeanoBlockFormat
):
    """!

    PlotPatchesInPeanoBlockFormat which only does something if its plotter
    is active

    The toolbox action set opens a new snapshot file in beginTraversal()
    and adds it to the index file. That has to be skipped altogether for a
    plotter which is not active, so the guards of all operations are
    combined with the plotter's is<Name>Active(). The predicate does not
    change throughout the plot sweep, as finish<Name>() - which moves the
    plotter on to its next output - is only called in unprepareTraversal()
    (@ref exahype2.api.Solver.Solver.Plotter).

    """

    def __init__(self, is_active_predicate, finish_call, **kwargs):
        super().__init__(**kwargs)
        self._is_active_predicate = is_active_predicate
        self._finish_call = finish_call

    def get_guard_of_operation(self, operation_name):
        return (
            self._is_active_predicate
            + " and ("
            + super().get_guard_of_operation(operation_name)
            + ")"
        )

    def get_body_of_unprepareTraversal(self):
        return "  " + self._finish_call


class PlotAsVTKFile(Solver.Plotter):
    """!

    Plotting counterpart to RusanovWithExplicitEuler: maps the patch data of
    a Finite Volume scheme with a Rusanov Riemann solver, realised over a
    block-structured mesh, onto a plot mesh and writes it out as a VTK file.

    The plotter is active in regular time intervals
    (@ref exahype2.api.Solver.Solver.Plotter): It is active as soon as the
    solver's min time stamp has reached the time stamp of its next plot.
    After each plot, this time stamp is incremented by
    time_in_between_plots. A solver can have several plotters, e.g. two
    PlotAsVTKFile plotters with different plot intervals and filters, as
    long as they write into different files.

    If the solver's variables carry names, the plotter writes them as
    comma-separated list into the description of the output files (@ref
    exahype2.api.Solver.Solver.variable_names_per_entry()). Paraview then
    shows them in the name of the data array, and
    tarch.plotter.api.notebook.show_solution() uses them to label the
    unknowns. Otherwise, the description is the solver's unknown identifier.

    """

    def __init__(
        self,
        solver: Solver,
        first_plot_time_stamp=0.0,
        time_in_between_plots=0.1,
        plotter_precision=5,
        output_path=".",
        filename="solution",
        name: str = None,
        quantities=None,
    ):
        """!

        @param solver: exahype2.api.Solver
          The solver whose solution is plotted.

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

        @param filename: str
          The plot files are called filename-SolverName. Plotters of the
          same solver have to use different file names or output paths.

        @param name: str or None
          Name of the plotter (@ref exahype2.api.Solver.Solver.Plotter).

        @param quantities: List[str or int] or None
          The entries of Q which are written per volume. A string is the
          name of a variable as passed to the solver and selects all of its
          entries, an int is the index of one entry of Q. Auxiliary
          variables can be selected, too
          (@ref exahype2.api.Quantities.resolve_quantities()). None writes
          all entries.

        """
        super().__init__(solver, name)

        if time_in_between_plots <= 0:
            raise Exception("Time in between plots has to be bigger than 0")
        if plotter_precision <= 0:
            raise Exception("Plotter precision has to be bigger than 0")
        self._first_plot_time_stamp = first_plot_time_stamp
        self._time_in_between_plots = time_in_between_plots
        self._plotter_precision = plotter_precision
        self._plot_filters = []
        self._output_path = output_path
        self._filename = filename
        self._quantities = quantities

    def output_file_prefix(self):
        return os.path.join(self._output_path, self._filename + "-" + self._solver.name)

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
                "new exahype2.api - PlotAsVTKFile only supports spatial "
                "(offset/size) plot filters for now. Call add_plot_filter() "
                "with frequency=1 (the default)."
            )
        self._plot_filters.append((offset, size))

    def construct_output(self, exahype2_project, peano_project, plot):
        """!

        Add one task to the plot graph which writes the solver's patches

        """
        peano4.api.Logger().log(
            "Create plot task graph for Finite Volume scheme over block-structured meshes with Rusanov flux"
        )

        assert hasattr(self._solver, "_patch"), (
            f"Solver {self._solver.name} has no attribute _patch. "
            "PlotAsVTKFile assumes the numerical scheme has added this field "
            "to the solver immediately upon construction."
        )

        self._add_time_interval_bookkeeping_to_solver(
            self._first_plot_time_stamp, self._time_in_between_plots
        )

        write_vtk_file_task = peano4.api.taskgraph.Task(
            name=f"{self._solver.name}_{self.name}_Write VTK file",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
            may_spawn_background_tasks=False,
        )
        plot.add_task(write_vtk_file_task)

        write_vtk_file_task.add_action_set(
            _PlotPatchesIfPlotterIsActive(
                is_active_predicate=self.create_is_active_predicate(),
                finish_call=self.create_finish_call(),
                filename=self.output_file_prefix(),
                patch=self._solver._patch,
                dataset_name=self._solver._unknown_identifier(),
                description=self._create_description(),
                guard=self._create_guard(),
                select_dofs=self._create_selected_entries(),
                additional_includes="""
#include "../repositories/SolverRepository.h"
""",
                precision=self._plotter_precision,
                time_stamp_evaluation="0.5*(repositories::getMinTimeStamp()+repositories::getMaxTimeStamp())",
            )
        )
        write_vtk_file_task.use_cell(
            self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE
        )

        self._log_plot_graph(plot)

    def _log_plot_graph(self, plot):
        if peano4.api.Logger().is_visualisation:
            plot.visualise(title="Plot task graph for {}".format(self._solver.name))
        if peano4.api.Logger().is_text:
            plot.log_as_text(title="Plot task graph for {}".format(self._solver.name))

    def _create_selected_entries(self):
        """!

        Indices of the entries of Q which are written, or None for all of
        them

        """
        if self._quantities is None:
            return None
        return resolve_quantities(
            self._solver, self._quantities, include_auxiliary_variables=True
        )

    def _create_description(self):
        """!

        Comma-separated names of the entries which are written, or the
        solver's unknown identifier if its variables carry no names

        """
        names = self._solver.variable_names_per_entry()
        if not names:
            return self._solver._unknown_identifier()
        selected_entries = self._create_selected_entries()
        if selected_entries is not None:
            names = [names[index] for index in selected_entries]
        return ", ".join(names)

    def _create_guard(self):
        """!

        Guard of the cells which are plotted

        The guard is built directly from self._plot_filters (known here, in
        Python) rather than delegating to some project-wide C++ filter
        object: the generated code carries the exact offset/size values this
        plotter was configured with, and nothing else. Only the cells
        selected by the solver's grid selector are plotted, as only those
        are updated (@ref exahype2.api.fv.actionsets.UpdateCell). Other cells
        would hold outdated data overlapping the actual solution.

        @return str
          C++ predicate over marker.

        """
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
        return guard
