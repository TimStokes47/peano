# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


def wait_for_patch_interior_update(solver, task_number) -> str:
    """!

    C++ code which waits for the task updating the patch interior

    The code waits only if the cell holds a task number, and never in the
    very first time step, where the task number has not been set yet (@see
    exahype2.api.fv.actionsets.SpawnTaskToUpdateCellInterior). Afterwards,
    it releases the task number.

    @param solver: exahype2.api.Solver
      The solver the patch interior belongs to.

    @param task_number: peano4.api.datamodel.DaStGen2
      Cell data holding the task number.

    """
    data = f"fineGridCell{task_number.name}"
    return f"""
  if (
    not tarch::la::equals(repositories::{solver.get_name_of_global_instance()}.getAdmissibleTimeStepSize(), 0.0)
    and
    {data}.getTaskNumber()>=0
  ) {{
    tarch::multicore::waitForTask( {data}.getTaskNumber() );
    {data}.getTaskEnumerator().releaseNumber( {data}.getTaskNumber() );
    {data}.setTaskNumber( tarch::multicore::TaskEnumerator::NoNumber );
  }}
"""


WAIT_INCLUDES = """
#include "tarch/la/ScalarOperations.h"
#include "tarch/multicore/multicore.h"
#include "tarch/multicore/TaskEnumerator.h"
"""


class DiscardPatchInterior(AbstractFVActionSet):
    """!

    Discard the patch interior

    Counterpart of exahype2.api.fv.actionsets.InsertPatchInterior
    (@see exahype2.api.fv.RusanovWithOptimisticExplicitEuler).

    If the patch interior is updated by a task (@see
    exahype2.api.fv.actionsets.SpawnTaskToUpdateCellInterior), the action
    set waits for this task, as it would otherwise still write into the
    patch interior while the next task is spawned. It does nothing with the
    task's outcome afterwards. If the patch interior is not updated by a
    task, the action set does nothing at all.

    """

    def __init__(self, solver, guard, task_number=None):
        """!

        @param solver: exahype2.api.Solver
          The solver the patch interior belongs to.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch interior is discarded.

        @param task_number: peano4.api.datamodel.DaStGen2
          Cell data holding the number of the task updating the patch
          interior, or None if the patch interior is not updated by a task.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.task_number = task_number
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        if (
            self.task_number is not None
            and operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
        ):
            return wait_for_patch_interior_update(self._solver, self.task_number)
        return ""

    def get_includes(self):
        result = super().get_includes()
        if self.task_number is not None:
            result += WAIT_INCLUDES
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
