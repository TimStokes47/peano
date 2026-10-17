# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class UpdateCellInterior(AbstractFVActionSet):
    """!

    Update the patch interior of a Finite Volume patch

    Counterpart of exahype2.api.fv.actionsets.UpdateCell which updates only
    the patch interior, i.e. the patch without its shell (@see
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler). The update of the
    patch interior does not need any halo data. The action set hence reads
    the patch solver._patch rather than its halo-augmented reconstruction,
    invokes the patch interior compute kernel, and writes the result into
    solver._patch_interior.

    The maximum eigenvalue of the patch interior goes into the third entry
    of solver._max_eigenvalue. The second entry holds the estimated time
    step size (@see exahype2.api.fv.actionsets.SpawnTaskToUpdateCellInterior).

    """

    def __init__(self, solver, guard, time_step_size=None):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._patch, solver._patch_interior,
          solver._max_eigenvalue and
          solver.compute_kernel_call_on_patch_interior set - @see
          exahype2.api.fv.RusanovWithOptimisticExplicitEuler.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch interior is updated.

        @param time_step_size: str
          C++ expression for the time step size the patch interior is
          advanced by. Defaults to the solver's getMinTimeStepSize().

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard
        self.time_step_size = time_step_size

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            time_step_size = self.time_step_size
            if time_step_size is None:
                time_step_size = f"repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize()"
            result = f"""
  {{
    double timeStamp    = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStamp();
    double timeStepSize = {time_step_size};
    ::exahype2::CellData patchData(fineGridCell{self._solver._patch.name}.value, marker.x(), marker.h(), timeStamp, timeStepSize, fineGridCell{self._solver._patch_interior.name}.value);
    {self._solver.compute_kernel_call_on_patch_interior}
    fineGridCell{self._solver._max_eigenvalue.name}.setMaxEigenvalue( 2, patchData.maxEigenvalue[0] );
  }}
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
