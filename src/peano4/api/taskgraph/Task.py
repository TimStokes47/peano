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

    """

    class GridEntity(enum.Enum):
        """! Which kind of mesh entity a Task works on. """

        VERTEX = 1
        FACE = 2
        CELL = 3
        GLOBAL = 4

    def __init__(self, name: str, entity: "Task.GridEntity"):
        self.name = name
        self.entity = entity
        self.dependencies: List["Task"] = []

    def add_dependency(self, task: "Task"):
        self.dependencies.append(task)
