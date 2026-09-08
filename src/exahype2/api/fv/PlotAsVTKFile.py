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

    def __init__(self, solver: Solver):
        super().__init__(solver)

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         plot):
        """!

        """
        peano4.api.Logger().log("Create plot task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        write_vtk_file_task         = peano4.api.taskgraph.Task(name="Write VTK file", entity=peano4.api.taskgraph.Task.GridEntity.CELL)

        plot.add_task(write_vtk_file_task)

        if peano4.api.Logger().is_visualisation:
            plot.visualise(title="Plot task graph for {}".format(self._solver.name))
