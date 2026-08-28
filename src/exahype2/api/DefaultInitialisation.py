# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from exahype2.initialisation.api import HardcodedInitialisation


class DefaultInitialisation(HardcodedInitialisation):
    """!

    Default realisation of exahype2.initialisation.api.HardcodedInitialisation.

    """

    def __init__(self, solver):
        super().__init__(solver)

    def construct_output(self, output, verbose):
        """!
        @todo Not implemented yet
        """
        pass
