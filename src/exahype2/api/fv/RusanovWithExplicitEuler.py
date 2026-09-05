# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..Solver import Solver


class RusanovWithExplicitEuler(Solver.NumericalScheme):
    """!

    Finite Volumes with a Rusanov Riemann solver and explicit Euler time
    stepping, realised over a block-structured mesh.

    """

    def __init__(self, solver: Solver):
        super().__init__(solver)

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         time_step):
        """!

        """
        peano4.api.Logger().log("Create task graphs for Finite Volume scheme over block-structured meshes with Rusanov flux")
        pass
