# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class AbstractRKFD4ActionSet(ActionSet):
    """!

    Base class of the action sets of @ref exahype2.api.rkfd4 which work
    against the solver

    It memorises the solver and provides the includes all of these action
    sets need.

    """

    def __init__(self, solver):
        """!

        @param solver: exahype2.api.Solver
          The solver the action set works for.

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self._solver = solver

    def get_action_set_name(self):
        """!

        You should replicate this function in each subclass, so you get
        meaningful action set names (otherwise, it will be
        AbstractRKFD4ActionSet0,1,2,...).

        """
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_includes(self):
        """!
        The includes cover the kernels the numerical scheme generates (@ref
        exahype2.api.rkfd4.CentralDifferencesWithRungeKutta), so all action
        sets can invoke them.
        """
        return (
            f"""
#include <algorithm>
#include <functional>
#include "exahype2/CellData.h"
#include "exahype2/fd/PatchUtils.h"
#include "kernels/{self._solver.name}_fd4.h"
#include "kernels/rungekutta_linear_combination.h"
"""
            + self._solver._get_default_includes()
            + self._solver.user_solver_includes
        )
