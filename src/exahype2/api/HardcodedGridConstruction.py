# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Solver import Solver


class HardcodedGridConstruction(Solver.GridConstruction):
    """!

    Default realisation of Solver.GridConstruction.

    """

    def __init__(self, solver: Solver):
        super().__init__(solver)

    def construct_output(self, output, verbose):
        """!
        @todo Not implemented yet
        """
        pass
