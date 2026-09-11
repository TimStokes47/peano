# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..Solver import Solver


class PlotAsVTKFile(Solver.Plotter):
    """!

    Plotting counterpart to RusanovWithExplicitEuler: maps the patch data of
    a Finite Volume scheme with a Rusanov Riemann solver, realised over a
    block-structured mesh, onto a plot mesh and writes it out as a VTK file.

    """

    def __init__(self, solver: Solver, plotter_precision=5):
        super().__init__(solver)

        if plotter_precision <= 0:
            raise Exception("Plotter precision has to be bigger than 0")
        self._plotter_precision = plotter_precision
        self._plot_filters = []

    def add_plot_filter(self, offset, size, frequency=1):
        """
        Add a new filter to Peano/ExaHyPE

        offset: (float,float,float)

        size: (float,float,float)

        frequency: int
          A positive value. Peano makes snapshots every dt simulation
          units. This is something you specify once per simulation. But
          you might decide only to splot every k of these snapshots.
        """
        new_entry = "{{"
        new_entry += str(offset[0])
        for i in offset[1:]:
            new_entry += ","
            new_entry += str(i)
        new_entry += "},{"
        new_entry += str(size[0])
        for i in size[1:]:
            new_entry += ","
            new_entry += str(i)
        new_entry += "},"
        new_entry += str(frequency)
        new_entry += "}"

        self._plot_filters.append(new_entry)

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         plot):
        """!

        """
        peano4.api.Logger().log("Create plot task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        peano_project.constants.export("PlotterPrecision", str(self._plotter_precision))

        write_vtk_file_task         = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Write VTK file", entity=peano4.api.taskgraph.Task.GridEntity.CELL)

        plot.add_task(write_vtk_file_task)

        if peano4.api.Logger().is_visualisation:
            plot.visualise(title="Plot task graph for {}".format(self._solver.name))
