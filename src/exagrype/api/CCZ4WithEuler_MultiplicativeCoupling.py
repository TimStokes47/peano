""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import exahype2.api
import peano4.api
import peano4.api.taskgraph

from .actionsets import SynchroniseTimeStepSizes
from .CCZ4WithEuler_AdditiveCoupling import CCZ4WithEuler_AdditiveCoupling


class CCZ4WithEuler_MultiplicativeCoupling(CCZ4WithEuler_AdditiveCoupling):
    """!

    First-order CCZ4 and an Euler fluid, run one after the other within each time step

    The host is the sibling of exagrype.api.CCZ4WithEuler_AdditiveCoupling: It
    creates the same solvers and the same auxiliary variables, and it uses the
    same prep tasks. It only wires the task graphs differently
    (@ref page_exahype_programmers_guide_coupling). Per time step, we get

        prep (from Euler) -> graph of CCZ4 -> prep (from CCZ4) -> graph of Euler -> synchronise

    i.e. CCZ4 sees the Euler fields of the begin of the time step, and Euler sees the CCZ4
    fields after CCZ4 has completed its step. Each solver brings its own numerical scheme,
    set through the host (set_numerical_scheme()). They do not exchange data within
    their Runge-Kutta stages.

    The last tasks of each solver graph are not its sink tasks: each graph ends in a
    GLOBAL_END_OF_STEP task that computes the next time step size, and no
    ordinary task may depend on such a task. We therefore wire Euler to the last
    cell tasks of CCZ4. The end-of-step tasks remain at the very end of the time step.
    A final GLOBAL_END_OF_STEP task enforces the minimum of both time step sizes.

    """

    @staticmethod
    def _last_cell_tasks(graph):
        """!

        The tasks of the graph that no other cell or face task depends on,
        i.e. the end of the graph if we ignore the global end-of-step tasks.

        """
        Entity = peano4.api.taskgraph.Task.GridEntity
        ignore = (Entity.GLOBAL, Entity.GLOBAL_END_OF_STEP)
        tasks = [task for task in graph.tasks if task.entity not in ignore]
        return [
            task
            for task in tasks
            if not any(task in other.dependencies for other in tasks)
        ]

    def couple(self, exahype2_project, peano_project, own_graphs, host_graphs):
        """!

        Chain the graph of CCZ4 and the graph of Euler through the prep tasks

        @param exahype2_project: exahype2.api.Project
          The project that hosts the system.

        @param peano_project: peano4.api.Project
          The Peano project that the exahype2 project assembles.

        @param own_graphs: Dict[exahype2.api.Solver, exahype2.api.PhaseGraphs]
          The graphs of the two solvers.

        @param host_graphs: exahype2.api.PhaseGraphs
          The concatenated graphs. The time step graph receives the new tasks.

        """
        time_step = host_graphs.time_step
        peano4.api.Logger().new_section(
            "Coupling " + self.ccz4.name + " and " + self.euler.name
        )
        Entity = peano4.api.taskgraph.Task.GridEntity

        ccz4_graph = own_graphs[self.ccz4].time_step
        euler_graph = own_graphs[self.euler].time_step
        first_of_ccz4 = ccz4_graph.source_tasks()
        first_of_euler = euler_graph.source_tasks()
        last_of_ccz4 = self._last_cell_tasks(ccz4_graph)
        last_tasks = [
            task for task in time_step.sink_tasks() if task.entity != Entity.GLOBAL
        ]

        prep_from_euler = self._create_prep_task(
            time_step, self.euler, self.ccz4, self.euler_fields
        )
        prep_from_ccz4 = self._create_prep_task(
            time_step, self.ccz4, self.euler, self.ccz4_fields
        )
        for task in first_of_ccz4:
            task.add_dependency(prep_from_euler)
        for task in last_of_ccz4:
            prep_from_ccz4.add_dependency(task)
        for task in first_of_euler:
            task.add_dependency(prep_from_ccz4)

        synchronise = peano4.api.taskgraph.Task(
            name=self.SYNCHRONISE_TASK_NAME, entity=Entity.GLOBAL_END_OF_STEP
        )
        for task in last_tasks:
            synchronise.add_dependency(task)
        synchronise.add_action_set(SynchroniseTimeStepSizes([self.ccz4, self.euler]))
        time_step.add_task(synchronise)
