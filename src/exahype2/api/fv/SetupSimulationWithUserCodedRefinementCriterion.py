# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..Solver import Solver


class SetupSimulationWithUserCodedRefinementCriterion(Solver.GridConstruction):
    """!

    Setup phase counterpart to RusanovWithExplicitEuler: sets the initial
    condition for a Finite Volume scheme with a Rusanov Riemann solver,
    realised over a block-structured mesh, and prepares the data required
    for the first time step.

    Unlike SetupSimulationOverRegularGrid, this variant repeats its "Refine
    cell" task (task_may_repeat=True): the mesh is refined adaptively,
    driven by a user-coded refinement criterion checked afresh every grid
    sweep, rather than built once and left alone.

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

        refine_cell_task            = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Refine cell", entity=peano4.api.taskgraph.Task.GridEntity.CELL, task_may_repeat=True)
        initial_condition_task      = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Initial condition", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task       = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)

        initial_condition_task.add_dependency(refine_cell_task)
        project_on_faces_task.add_dependency(initial_condition_task)

        setup_simulation.add_task(refine_cell_task)
        setup_simulation.add_task(initial_condition_task)
        setup_simulation.add_task(project_on_faces_task)
        
#        refine_cell_task.add_action_set()

        if peano4.api.Logger().is_visualisation:
            setup_simulation.visualise(title="Setup task graph for {}".format(self._solver.name))
