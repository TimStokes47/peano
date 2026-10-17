# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet
from .UpdateCellInterior import UpdateCellInterior

import peano4.api


class SpawnTaskToUpdateCellInterior(AbstractFVActionSet):
    """!

    Update the patch interior with a damped time step size

    The action set takes the solver's current time step size, multiplies
    it with damp_time_step_size, and stores the result as estimated time
    step size in the second entry of solver._max_eigenvalue. Afterwards, it
    updates the patch interior with this damped time step size (@see
    exahype2.api.fv.actionsets.UpdateCellInterior and
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler).

    @todo The update runs straightaway for the time being. It does not
          spawn a task yet.

    """

    def __init__(self, solver, damp_time_step_size, guard):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._patch, solver._patch_interior,
          solver._max_eigenvalue and
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
        self._update_cell_interior = UpdateCellInterior(solver, guard, time_step_size="dampedTimeStepSize")

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            result = f"""
  double dampedTimeStepSize = {self.damp_time_step_size} * repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize();
  fineGridCell{self._solver._max_eigenvalue.name}.setMaxEigenvalue( 1, dampedTimeStepSize );
""" + self._update_cell_interior.get_body_of_operation(operation_name)
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
