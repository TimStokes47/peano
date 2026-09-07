# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import abc


class HardcodedInitialisation(abc.ABC):
    """!

    How a Solver sets up its initial condition on the mesh. Abstract -
    concrete solvers have to provide a subclass that implements
    construct_output().

    """

    def __init__(self, solver):
        self._solver = solver

    @abc.abstractmethod
    def construct_output(self,
                         exahype2_project,
                         solver_realisation,
                         verbose):
        """!
        Has to be implemented by subclass
        """
