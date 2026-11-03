# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

from exahype2.api.Solver import Solver
import peano4.api


class UpdateCellGPU(AbstractFVActionSet):
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

    def __init__(self, solver: Solver, guard, max_eigenvalue=None):
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

        @param max_eigenvalue: peano4.api.datamodel.DaStGen2
          The cell data the maximum eigenvalue goes into (first entry).
          Defaults to solver._max_eigenvalue.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard
        self._max_eigenvalue = max_eigenvalue if max_eigenvalue is not None else solver._max_eigenvalue

    def get_body_of_operation(self, operation_name):
        result = ""

        stride_per_dof = self._solver.unknowns + self._solver.auxiliary_variables
        input_dofs_per_axis = self._solver.numerical_scheme.patch_size + 2 * self._solver.numerical_scheme.halo_size
        output_dofs_per_axis = self._solver.numerical_scheme.patch_size


        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            result = f"""
  #if Dimensions == 2
    int inputStride = {stride_per_dof} * {input_dofs_per_axis} * {input_dofs_per_axis};
    int outputStride = {stride_per_dof} * {output_dofs_per_axis} * {output_dofs_per_axis};
  #else
    int inputStride = {stride_per_dof} * {input_dofs_per_axis} * {input_dofs_per_axis} * {input_dofs_per_axis};
    int outputStride = {stride_per_dof} * {output_dofs_per_axis} * {output_dofs_per_axis} * {output_dofs_per_axis};
  #endif

  double timeStamp    = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStamp();
  double timeStepSize = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize();
  ::exahype2::CellData patchData(fineGridCell{self._solver._patch_with_halo.name}.value, marker.x(), marker.h(), timeStamp, timeStepSize, fineGridCell{self._solver._patch.name}.value);
  ::exahype2::CopyCellDataGPU gpuPatchData(patchData, inputStride, outputStride);
  {self._solver.compute_kernel_call}
  gpuPatchData.copyBackAndDestroy();

  fineGridCell{self._max_eigenvalue.name}.setMaxEigenvalue( 0, patchData.maxEigenvalue[0] );
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
