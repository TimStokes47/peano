# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List

import peano4.api.solversteps

from ..Graph import Graph
from ..Task import Task


def translate(graph: Graph, identifier: str) -> List["peano4.api.solversteps.Step"]:
    """!

    Simplest possible task graph compiler

    Linearises the task graph into one mesh traversal (Observer) per task, in
    an order that respects every dependency: a task's Observer always comes
    after the Observers of all tasks it depends on. This throws away any
    parallelism the task graph might encode - it is the "no fusion, no
    reordering" baseline compiler that smarter compilers can be measured
    against.

    ## Implementation
        
    The implementation of the serialisation is rather straightforward using a 
    recursive implementation: We start from an arbitrary node in the graph.
    If it has no predecessor, we can add it. If not, we call the graph 
    traversal for the predecessor first. 

    graph: peano4.api.taskgraph.Graph
      The task graph to linearise. Assumed to be acyclic - translate() does
      not check for cycles and will recurse indefinitely on one.

    identifier: str
      Identifies which of the surrounding project's algorithmic phases this
      graph belongs to (e.g. "SetupSimulation", "TimeStep", "Plot"). Every
      generated Observer's name is prefixed with this identifier, joined by
      an underscore, so Observers coming from different phases never clash
      even if their tasks happen to share a name.

    @return [peano4.api.solversteps.Step]
      One Observer per task, ordered so that every task's Observer comes
      after the Observers of all tasks it depends on, and carrying that
      task's action_sets (in the order they were added to the Task).

    """
    observer_of_task = {}
    order: List[Task] = []

    def visit(task: Task):
        if task not in observer_of_task:
            for dependency in task.dependencies:
                visit(dependency)
            # Turn every whitespace-separated word into its own name
            # segment, capitalising only its first letter - a blind
            # .title() would also lower-case the rest of the word, which
            # mangles existing camel case and acronyms (e.g. "EulerFVSmooth"
            # would become "Eulerfvsmooth", "CFL" would become "Cfl").
            task_name = "_".join(word[:1].upper() + word[1:] for word in task.name.split())
            step = peano4.api.solversteps.Step(
                name=f"{identifier}_{task_name}",
                add_user_defined_actions=False,
            )
            step.has_to_repeat = task.has_to_repeat
            for action_set in task.action_sets:
                step.add_action_set(action_set)
            observer_of_task[task] = step
            order.append(task)

    for task in graph.tasks:
        visit(task)

    return [observer_of_task[task] for task in order]
