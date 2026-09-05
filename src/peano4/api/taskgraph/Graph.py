# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

from typing import List

from .Task import Task


class Graph:
    """!

    Holds a sequence of Task instances

    """

    def __init__(self):
        self.tasks: List[Task] = []

    def add_task(self, task: Task):
        self.tasks.append(task)
