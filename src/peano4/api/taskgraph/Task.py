# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

import enum
from typing import List


class Task:
    """!

    One task within a peano4.api.taskgraph.Graph

    Every task works on exactly one kind of mesh entity - a vertex, a face
    or a cell - which has to be specified through entity when the task is
    constructed.

    Every task also holds dependencies, i.e. other Task instances that have
    to be completed before this task can run. This sequence is empty by
    default; use add_dependency() to add further tasks to it.

    Every task furthermore holds an arbitrary number of Peano 4 action sets
    (peano4.api.solversteps.ActionSet), i.e. the actual grid-sweep logic the
    task realises once it is compiled into an Observer. This sequence is
    empty by default; use add_action_set() to add further action sets to it.
    Their order matters - see peano4.api.solversteps.Step.add_action_set().

    """

    class GridEntity(enum.Enum):
        """! Which kind of mesh entity a Task works on. """

        VERTEX = 1
        FACE = 2
        CELL = 3
        GLOBAL = 4

    def __init__(self, name: str, entity: "Task.GridEntity", task_may_repeat: bool = False):
        self.name = name
        self.entity = entity
        self.dependencies: List["Task"] = []
        self.action_sets: List["peano4.api.solversteps.ActionSet"] = []
        self.may_repeat = task_may_repeat

    def add_dependency(self, task: "Task"):
        self.dependencies.append(task)

    def add_action_set(self, action_set: "peano4.api.solversteps.ActionSet"):
        """!
        
        A task can hold an arbitrary number of Peano 4 action sets. Eventually
        the overarching task graph will be mapped onto a sequence of steps
        (observers) and those steps then grab those action sets from the tasks and execute them. 
        
        """
        self.action_sets.append(action_set)
