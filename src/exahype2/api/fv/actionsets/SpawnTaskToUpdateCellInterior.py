# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class SpawnTaskToUpdateCellInterior(AbstractFVActionSet):
    """!

    Spawn a task which updates the patch interior

    The spawned task will run exahype2.api.fv.actionsets.UpdateCellInterior
    (@see exahype2.api.fv.RusanovWithOptimisticExplicitEuler).

    @todo Empty for the time being, i.e. the action set does nothing yet.

    """

    def __init__(self, solver, guard):
        """!

        @param solver: exahype2.api.Solver
          The solver the patch interior belongs to.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether a task is spawned for that cell.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        return ""

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
