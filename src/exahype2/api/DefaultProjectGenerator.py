# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations


class DefaultProjectGenerator:
    """!

    Default realisation of exahype2.api.Project.generator

    An exahype2.api.Project first asks every one of its solvers to
    realise itself (via Solver.generate_solver_realisation()), which lets
    each solver contribute to the Peano 4 project directly. This class'
    generate_Peano4_project() then mangles the remaining, solver-independent
    bits into the same Peano 4 project.

    """

    def generate_Peano4_project(self, exahype2_project, peano_project, verbosity):
        """!

        Construct a Peano 4 project and return it

        @param exahype2_project: exahype2.api.Project

        @param peano_project: peano4.api.Project
               The (freshly constructed) Peano 4 project that
               exahype2_project.generate_Peano4_project() is assembling.

        """
        peano_project.solversteps.add_step(exahype2_project.create_grid)
        peano_project.solversteps.add_step(exahype2_project.init_grid)
        peano_project.solversteps.add_step(exahype2_project.create_grid_but_postpone_refinement)
        peano_project.solversteps.add_step(exahype2_project.create_grid_and_converge_lb)
        peano_project.solversteps.add_step(exahype2_project.plot_solution)
        peano_project.solversteps.add_step(exahype2_project.checkpoint_solution)
        peano_project.solversteps.add_step(exahype2_project.perform_time_step)

        return peano_project
