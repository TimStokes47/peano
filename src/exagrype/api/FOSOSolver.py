""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

from typing import Dict, Union

import exahype2.api

from .FOSOCCZ4Solver import FOSOCCZ4Solver
from .FOSOReconstructionSolver import FOSOReconstructionSolver


class FOSOSolver(exahype2.api.SolverHost):
    """!

    CCZ4 in the first-/second-order reconstructed formulation as a coupled pair

    The object hosts two solvers, which take turns in the time stepping task
    graph (@ref page_exagrype_fo_so_ccz4, @ref page_exahype_programmers_guide_coupling):

    - ccz4 (exagrype.api.FOSOCCZ4Solver) evolves the 25 primary unknowns of
      CCZ4. It holds the 34 derivatives as auxiliary variables. It is the
      solver that owns the global tasks, i.e. the load balancing and the
      dynamic mesh refinement (global_tasks_solver, see @ref exahype2.api.SolverHost).
    - reconstruction (exagrype.api.FOSOReconstructionSolver) evolves the
      derivatives, i.e. it recomputes them from the primary unknowns in
      front of each Runge-Kutta stage of ccz4.

    Configure ccz4 exactly as any other solver (numerical scheme, grid
    construction, plotter, constraint enforcement, ...), and then add the
    host to the project with exahype2.api.Project.add_solver(). The
    reconstruction needs no configuration: it takes patch size and halo from
    ccz4.

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    solver = exagrype.api.FOSOSolver("SO")
    solver.ccz4.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(solver.ccz4, patch_size, rk_order=4, ...)
    ...
    project.add_solver(solver)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    ## Task graph

    Both solvers build a task graph of their own, and report it. The reconstruction
    solver's graph is a chain per stage (see @ref exagrype.api.FOSOReconstructionSolver),
    and the chains of different stages are not connected.
    The host concatenates the graphs and wires them (couple()): The first tasks of
    each stage's chain take over the dependencies of the CCZ4 solver's reconstruction
    of this stage, and the latter depends on the last task of the chain. The host
    reports the outcome.

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
        # The CCZ4 solver is realised first, as the reconstruction needs its data
        super().__init__(name, [self.ccz4, self.reconstruction])

    def couple(self, exahype2_project, peano_project, own_graphs, host_graphs):
        """!

        Let each stage of the CCZ4 solver wait for the reconstruction of this stage

        The first reconstruction tasks of a stage wait for whatever the CCZ4
        solver's reconstruction of the stage waits for, and the latter waits
        for the last reconstruction task of the stage.

        @param exahype2_project: exahype2.api.Project
          The project that hosts the pair (unused).

        @param peano_project: peano4.api.Project
          The Peano project that the exahype2 project assembles (unused).

        @param own_graphs: Dict[exahype2.api.Solver, exahype2.api.PhaseGraphs]
          The graphs of the two solvers (unused).

        @param host_graphs: exahype2.api.PhaseGraphs
          The concatenated graphs. The time step graph receives the dependencies.

        """
        scheme = self.reconstruction.numerical_scheme
        time_step = host_graphs.time_step
        for stage, (first_tasks, last_task, partner_name) in enumerate(
            zip(
                scheme.stage_first_tasks,
                scheme.stage_last_tasks,
                scheme.names_of_partner_reconstruction_tasks,
            )
        ):
            partner = [t for t in time_step.tasks if t.name == partner_name]
            if len(partner) != 1:
                raise ValueError(
                    f"Host {self.name}: cannot find the reconstruction {partner_name} "
                    f"of stage {stage} of solver {self.ccz4.name} in its time stepping task graph"
                )
            partner = partner[0]
            for dependency in partner.dependencies:
                for first_task in first_tasks:
                    first_task.add_dependency(dependency)
            partner.add_dependency(last_task)
