""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

from typing import Dict, Union

import exahype2.api

from .FOSOCCZ4Solver import FOSOCCZ4Solver
from .FOSOReconstructionSolver import FOSOReconstructionSolver


class FOSOSolver:
    """!

    CCZ4 in the first-/second-order reconstructed formulation as a coupled pair

    The object hosts two solvers, which take turns in the time stepping task
    graph (@ref page_exagrype_fo_so_ccz4, @ref page_exahype_programmers_guide_coupling):

    - ccz4 (exagrype.api.FOSOCCZ4Solver) evolves the 25 primary unknowns of
      CCZ4. It holds the 34 derivatives as auxiliary variables.
    - reconstruction (exagrype.api.FOSOReconstructionSolver) evolves the
      derivatives, i.e. it recomputes them from the primary unknowns in
      front of each Runge-Kutta stage of ccz4.

    Configure ccz4 exactly as any other solver (numerical scheme, grid
    construction, plotter, constraint enforcement, ...), and then call
    add_to_project(). The reconstruction needs no configuration: it takes
    patch size and halo from ccz4.

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    solver = exagrype.api.FOSOSolver("SO")
    solver.ccz4.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(solver.ccz4, patch_size, rk_order=4, ...)
    ...
    solver.add_to_project(project)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    """

    def __init__(
        self,
        name: str,
        parameters: Dict[str, Union[int, float]] = None,
        initial_condition: exahype2.api.InitPointWisely = None,
        reconstruction_name: str = None,
    ):
        """!

        @param name: str
          Name of the CCZ4 solver, @see exahype2.api.Solver.

        @param parameters: Dict[str, Union[int, float]]
          CCZ4 parameters, @see exagrype.api.FOSOCCZ4Solver.

        @param initial_condition: exahype2.api.InitPointWisely
          Initial condition, @see exagrype.api.FOSOCCZ4Solver.

        @param reconstruction_name: str
          Name of the reconstruction solver. Defaults to name plus
          "Reconstruction".

        """
        self.ccz4 = FOSOCCZ4Solver(name, parameters, initial_condition)
        """! The solver that evolves the primary unknowns """
        self.reconstruction = FOSOReconstructionSolver(
            reconstruction_name
            if reconstruction_name is not None
            else name + "Reconstruction",
            self.ccz4,
        )
        """! The solver that reconstructs the derivatives """

    def add_to_project(self, project: exahype2.api.Project):
        """!

        Register both solvers with the project

        The CCZ4 solver has to be realised first, so it is registered first.

        @param project: exahype2.api.Project
          The project that hosts the pair.

        """
        project.add_solver(self.ccz4)
        project.add_solver(self.reconstruction)
