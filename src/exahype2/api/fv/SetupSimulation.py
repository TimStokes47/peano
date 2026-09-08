# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..Solver import Solver


class SetupSimulation(Solver.GridConstruction):
    """!

    Setup phase counterpart to RusanovWithExplicitEuler: sets the initial
    condition for a Finite Volume scheme with a Rusanov Riemann solver,
    realised over a block-structured mesh, and prepares the data required
    for the first time step.

    """

    def __init__(self, solver: Solver):
        super().__init__(solver)

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         setup_simulation):
        """!

        """
        peano4.api.Logger().log("Create setup task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        initial_condition_task      = peano4.api.taskgraph.Task(name="Initial condition", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task       = peano4.api.taskgraph.Task(name="Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        refine_cell_task            = peano4.api.taskgraph.Task(name="Refine cell", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        load_balancing_task         = peano4.api.taskgraph.Task(name="Load balancing", entity=peano4.api.taskgraph.Task.GridEntity.GLOBAL)
        empty_traversal_to_let_grid_construction_catch_up_task = peano4.api.taskgraph.Task(name="Empty grid traversal", entity=peano4.api.taskgraph.Task.GridEntity.CELL)

        compute_cfl_condition_task.add_dependency(initial_condition_task)
        project_on_faces_task.add_dependency(initial_condition_task)
        compute_time_step_size_task.add_dependency(compute_cfl_condition_task)

        setup_simulation.add_task(initial_condition_task)
        setup_simulation.add_task(compute_cfl_condition_task)
        setup_simulation.add_task(project_on_faces_task)
        setup_simulation.add_task(compute_time_step_size_task)

        if peano4.api.Logger().is_visualisation:
            setup_simulation.visualise(title="Setup task graph for {}".format(self._solver.name))
