# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class InsertPatchInterior(AbstractFVActionSet):
    """!

    Insert the patch interior into the patch

    Copies solver._patch_interior, i.e. the result of
    exahype2.api.fv.actionsets.UpdateCellInterior, into the patch interior
    of solver._patch. Afterwards, the first entry of solver._max_eigenvalue
    holds the maximum of its first entry (the maximum eigenvalue of the
    shell) and its third entry (the maximum eigenvalue of the patch
    interior), i.e. the maximum eigenvalue of the whole patch (@see
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler).

    """

    def __init__(self, solver, patch_size, overlap, guard):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._patch, solver._patch_interior and
          solver._max_eigenvalue set - @see
          exahype2.api.fv.RusanovWithOptimisticExplicitEuler.

        @param patch_size: int
          Number of Finite Volumes per axis within one patch.

        @param overlap: int
          Thickness of the shell, i.e. the patch interior has
          patch_size-2*overlap Finite Volumes per axis.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch interior is inserted.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.patch_size = patch_size
        self.overlap = overlap
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            unknowns = self._solver.unknowns + self._solver.auxiliary_variables
            interior_size = self.patch_size - 2 * self.overlap
            max_eigenvalue = f"fineGridCell{self._solver._max_eigenvalue.name}"
            result = f"""
  dfor(interiorVolume,{interior_size}) {{
    tarch::la::Vector<Dimensions,int> patchVolume = interiorVolume + tarch::la::Vector<Dimensions,int>({self.overlap});
    int interiorVolumeSerialised = peano4::utils::dLinearised(interiorVolume,{interior_size});
    int patchVolumeSerialised    = peano4::utils::dLinearised(patchVolume,{self.patch_size});
    for (int j=0; j<{unknowns}; j++) {{
      fineGridCell{self._solver._patch.name}.value[patchVolumeSerialised*{unknowns}+j] = fineGridCell{self._solver._patch_interior.name}.value[interiorVolumeSerialised*{unknowns}+j];
    }}
  }}

  {max_eigenvalue}.setMaxEigenvalue( 0, std::max( {max_eigenvalue}.getMaxEigenvalue(0), {max_eigenvalue}.getMaxEigenvalue(2) ) );
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_includes(self):
        return super().get_includes() + """
#include "peano4/utils/Loop.h"
"""

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
