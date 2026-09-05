# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org


class Task:
    """!

    One task within a peano4.api.taskgraph.Graph

    """

    def __init__(self, name: str):
        self.name = name
