# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class UpdateCell(AbstractFVActionSet):
    """!

    Update a Finite Volume patch (solver._patch) from its halo-augmented
    reconstruction (solver._patch_with_halo, @see
    toolbox.blockstructured.api.ReconstructPatchWithHalo) by applying the
    solver's numerical scheme (Rusanov flux, explicit Euler time stepping)
    once per volume.

    @todo Not implemented yet: the actual Riemann solver / PDE kernel call,
    the per-cell time stamp/time step size bookkeeping (a CellLabel, in the
    legacy version - @see toolbox.blockstructured.api.ReconstructPatchWithHalo's
    own @todo) and feeding the resulting eigenvalues into the solver's
    global time step size are all missing here. solver._patch is left
    exactly at its storage's default (a zero-initialised value, since
    nothing here writes it yet - @see peano4.grid.constructLoadStoreComputeFlag()'s
    "CreateDummy" behaviour for a submodel nothing loads) rather than
    something half-computed.

    """

    def __init__(self, solver, guard="true"):
        """!

        solver: exahype2.api.Solver
          Has to have solver._patch and solver._patch_with_halo set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch is updated.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            result = f"""
  if ({self.guard}) {{
    //TODO: Get time information from cell label or equivalent
    //double timeStamp = fineGridCell{self._solver._patch_with_halo.name}.getTimeStamp();
    //double timeStepSize = fineGridCell{self._solver._patch_with_halo.name}.getTimeStepSize();
    double timeStamp = 0.0;
    double timeStepSize = 0.1;
    ::exahype2::CellData patchData(fineGridCell{self._solver._patch_with_halo.name}.value, marker.x(), marker.h(), timeStamp, timeStepSize, fineGridCell{self._solver._patch.name}.value);
    ::{"::".join(self._solver.numerical_scheme.kernel_namespace)}::{self._solver.compute_kernel_call}
  }}
"""
        return result

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
