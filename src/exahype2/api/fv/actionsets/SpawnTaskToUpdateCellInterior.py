# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet
from .UpdateCellInterior import UpdateCellInterior

import peano4.api


class SpawnTaskToUpdateCellInterior(AbstractFVActionSet):
    """!

    Update the patch interior with a damped time step size

    The action set takes the admissible time step size of the current time
    step, i.e. the fourth entry of solver._max_eigenvalue_and_time_step_sizes
    (@see exahype2.api.fv.actionsets.ReadjustTimeStepSizeForOptimisticGlobalTimestepping),
    multiplies it with damp_time_step_size, and stores the result as
    estimated time step size in the second entry. Afterwards, it updates the
    patch interior with this damped time step size (@see
    exahype2.api.fv.actionsets.UpdateCellInterior and
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler). As the admissible
    time step size is never damped itself, the estimates do not shrink from
    time step to time step.

    The action set has to run before
    exahype2.api.fv.actionsets.ComputeGlobalCFLCondition, which is why
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler makes the CFL
    condition's task depend on the spawn's task. This way, the order in
    which the tasks are added to the task graph does not matter. The CFL
    condition advances the solver's time stamp and time step size
    straightaway. Before it runs, the solver still holds the time stamp
    and time step size of the current time step. The patch, however, has
    already been advanced by the current time step. The patch interior
    update hence starts from the solver's time stamp plus its time step
    size.

    If the admissible time step size is zero, i.e. in the very first time
    step, there is nothing to estimate from. The action set then does not
    update the patch interior. It sets the maximum eigenvalue of the patch
    interior (third entry) to zero and the estimated time step size
    (second entry) to the maximum double value, so the next time step
    takes the traditional route.

    @todo The update runs straightaway for the time being. It does not
          spawn a task yet.

    """

    def __init__(self, solver, damp_time_step_size, guard):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._patch, solver._patch_interior,
          solver._max_eigenvalue_and_time_step_sizes and
          solver.compute_kernel_call_on_patch_interior set - @see
          exahype2.api.fv.RusanovWithOptimisticExplicitEuler.

        @param damp_time_step_size: float
          Factor the solver's current time step size is scaled with to
          obtain the estimated time step size of the patch interior update.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether the patch interior of that cell is updated.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.damp_time_step_size = damp_time_step_size
        self.guard = guard
        self._update_cell_interior = UpdateCellInterior(
            solver,
            guard,
            time_step_size="dampedTimeStepSize",
            time_stamp=f"repositories::{solver.get_name_of_global_instance()}.getMinTimeStamp() + "
            f"repositories::{solver.get_name_of_global_instance()}.getMinTimeStepSize()",
        )

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            solver = f"repositories::{self._solver.get_name_of_global_instance()}"
            data = f"fineGridCell{self._solver._max_eigenvalue_and_time_step_sizes.name}"
            result = f"""
  if ( tarch::la::equals({data}.getMaxEigenvalue(3), 0.0) ) {{
    {data}.setMaxEigenvalue( 1, std::numeric_limits<double>::max() );
    {data}.setMaxEigenvalue( 2, 0.0 );
  }}
  else {{
    double dampedTimeStepSize = {self.damp_time_step_size} * {data}.getMaxEigenvalue(3);
    {data}.setMaxEigenvalue( 1, dampedTimeStepSize );
""" + self._update_cell_interior.get_body_of_operation(operation_name) + """
  }
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_includes(self):
        return super().get_includes() + """
#include <limits>
#include "tarch/la/ScalarOperations.h"
"""

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
