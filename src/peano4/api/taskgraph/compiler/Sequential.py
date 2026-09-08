# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List

import peano4.api.solversteps

from ..Task import Task


def translate(graph) -> List["peano4.api.solversteps.Step"]:
    """!

    Simplest possible task graph compiler

    Linearises the task graph into one mesh traversal (Observer) per task, in
    an order that respects every dependency: a task's Observer always comes
    after the Observers of all tasks it depends on. This throws away any
    parallelism the task graph might encode - it is the "no fusion, no
    reordering" baseline compiler that smarter compilers can be measured
    against.

    In the Peano terminology, a Step corresponds to an Observer: for every
    mesh traversal, the traversal automaton is handed an observer that
    distributes the grid events among the action sets tied to it.
    @see peano4.api.solversteps.Step

    graph: peano4.api.taskgraph.Graph
      The task graph to linearise. Assumed to be acyclic - translate() does
      not check for cycles and will recurse indefinitely on one.

    @return [peano4.api.solversteps.Step]
      One Observer per task, ordered so that every task's Observer comes
      after the Observers of all tasks it depends on.

    """
    observer_of_task = {}
    order: List[Task] = []

    def visit(task: Task):
        if task not in observer_of_task:
            for dependency in task.dependencies:
                visit(dependency)
            observer_of_task[task] = peano4.api.solversteps.Step(
                name=task.name.title().replace(" ", ""),
                add_user_defined_actions=False,
            )
            order.append(task)

    for task in graph.tasks:
        visit(task)

    return [observer_of_task[task] for task in order]
