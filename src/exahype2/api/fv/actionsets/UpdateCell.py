# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

from ...AuxiliaryVariables import (
    code_to_assemble_kernel_input,
    code_to_allocate_kernel_output,
    code_to_copy_back_kernel_output,
)
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

    def __init__(self, solver, guard, max_eigenvalue=None):
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
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            solver = self._solver
            unknowns_in  = f"fineGridCell{solver._patch_with_halo.name}.value"
            unknowns_out = f"fineGridCell{solver._patch.name}.value"
            has_parameters = len(solver.auxiliary_variables) > 0
            kernel_in  = "kernelIn.data()" if has_parameters else unknowns_in
            kernel_out = "kernelOut.data()" if has_parameters else unknowns_out
            result = f"""
  double timeStamp    = repositories::{solver.get_name_of_global_instance()}.getMinTimeStamp();
  double timeStepSize = repositories::{solver.get_name_of_global_instance()}.getMinTimeStepSize();
  {code_to_assemble_kernel_input(solver, "kernelIn", unknowns_in, solver.numerical_scheme.halo_size)}
  {code_to_allocate_kernel_output(solver, "kernelOut")}
  ::exahype2::CellData patchData({kernel_in}, marker.x(), marker.h(), timeStamp, timeStepSize, {kernel_out});
  {solver.compute_kernel_call}
  {code_to_copy_back_kernel_output(solver, "kernelOut", unknowns_out)}

  fineGridCell{self._max_eigenvalue.name}.setMaxEigenvalue( 0, patchData.maxEigenvalue[0] );
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
