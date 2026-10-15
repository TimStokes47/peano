# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class UpdateCell(AbstractFVActionSet):
    """!

    Update a Finite Volume patch (solver._patch) from its halo-augmented
    reconstruction (solver._patch_with_halo, see
    toolbox.blockstructured.api.ReconstructPatchWithHalo) by applying the
    solver's numerical scheme (Rusanov flux, explicit Euler time stepping)
    once per volume.

    Global time stepping: all cells share one time stamp and time step
    size, which are taken from the solver (getMinTimeStamp() and
    getMinTimeStepSize()) rather than from per-cell bookkeeping.

    The compute kernel returns the maximum eigenvalue over the patch it has
    just updated. The action set stores it in the cell data
    solver._max_eigenvalue. exahype2.api.fv.actionsets.ComputeGlobalCFLCondition
    reduces these values in a subsequent mesh traversal and derives the
    next time step size from them.

    """

    def __init__(self, solver, guard):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._patch, solver._patch_with_halo and
          solver._max_eigenvalue set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch is updated. Typically the
          output of the solver's grid_selector (@see
          peano4.api.toolbox.GridSelector).

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            result = f"""
  double timeStamp    = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStamp();
  double timeStepSize = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize();
  ::exahype2::CellData patchData(fineGridCell{self._solver._patch_with_halo.name}.value, marker.x(), marker.h(), timeStamp, timeStepSize, fineGridCell{self._solver._patch.name}.value);
  {self._solver.compute_kernel_call}

  fineGridCell{self._solver._max_eigenvalue.name}.setMaxEigenvalue( 0, patchData.maxEigenvalue[0] );
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
