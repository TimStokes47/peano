# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class ReadjustTimeStepSizeForOptimisticGlobalTimestepping(AbstractFVActionSet):
    """!

    Validate the estimated time step size of an optimistic patch interior update

    The second entry of solver._max_eigenvalue holds the time step size the
    patch interior has been updated with optimistically (@see
    exahype2.api.fv.actionsets.SpawnTaskToUpdateCellInterior). The action
    set compares it to the solver's current time step size:

    - If the estimate is bigger than the solver's time step size, we have
      overestimated the admissible time step size. The action set
      invalidates the estimate, i.e. sets it to the maximum double value.
    - If the estimate is smaller than or equal to the solver's time step
      size, subject to floating point accuracy (tarch::la::equals()), the
      action set enforces the estimate as the solver's minimum and maximum
      time step size (@see exahype2::Solver::enforceTimeStepSize()).

    """

    def __init__(self, solver, guard):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._max_eigenvalue set - @see
          exahype2.api.fv.RusanovWithOptimisticExplicitEuler.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's estimate is checked.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            solver = f"repositories::{self._solver.get_name_of_global_instance()}"
            data = f"fineGridCell{self._solver._max_eigenvalue.name}"
            result = f"""
  double estimatedTimeStepSize = {data}.getMaxEigenvalue(1);
  double solverTimeStepSize    = {solver}.getMinTimeStepSize();
  if (estimatedTimeStepSize < solverTimeStepSize or tarch::la::equals(estimatedTimeStepSize, solverTimeStepSize)) {{
    {solver}.enforceTimeStepSize(estimatedTimeStepSize);
  }}
  else {{
    {data}.setMaxEigenvalue(1, std::numeric_limits<double>::max());
  }}
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
