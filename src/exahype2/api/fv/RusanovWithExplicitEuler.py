# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..Solver import Solver


class RusanovWithExplicitEuler(Solver.NumericalScheme):
    """!

    Finite Volumes with a Rusanov Riemann solver and explicit Euler time
    stepping, realised over a block-structured mesh.

    """

    def __init__(self, solver: Solver):
        super().__init__(solver)

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         time_step):
        """!

        """
        peano4.api.Logger().log("Create time stepping task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")
        
        reconstruction_task         = peano4.api.taskgraph.Task(name="Reconstruction", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        update_cell_task            = peano4.api.taskgraph.Task(name="Update cell", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        compute_cfl_condition_task  = peano4.api.taskgraph.Task(name="Compute CFL condition", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task       = peano4.api.taskgraph.Task(name="Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        compute_time_step_size_task = peano4.api.taskgraph.Task(name="Compute time step size", entity=peano4.api.taskgraph.Task.GridEntity.GLOBAL)
        
        update_cell_task.add_dependency(reconstruction_task)
        compute_cfl_condition_task.add_dependency(update_cell_task)
        project_on_faces_task.add_dependency(update_cell_task)
        compute_time_step_size_task.add_dependency(compute_cfl_condition_task)

        time_step.add_task(reconstruction_task)
        time_step.add_task(update_cell_task)
        time_step.add_task(compute_cfl_condition_task)
        time_step.add_task(project_on_faces_task)
        time_step.add_task(compute_time_step_size_task)
        
        if peano4.api.Logger().is_visualisation:
            time_step.visualise(title="Time stepping task graph for {}".format(self._solver.name))
