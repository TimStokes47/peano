# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.toolbox.blockstructured.api.ReconstructPatchAndApplyFunctor import (
    ReconstructedArrayMemoryLocation,
    ReconstructPatchAndApplyFunctor,
)


class ReconstructPatchWithHalo(ReconstructPatchAndApplyFunctor):
    """!

    Reconstruct solver._patch_with_halo from solver._patch and its
    surrounding halo (solver._patch_overlap_current_time_step), so
    exahype2.api.fv.actionsets.UpdateCell has a halo-augmented patch to
    update from.

    Realised on top of peano4.toolbox.blockstructured.api's
    ReconstructPatchAndApplyFunctor, which already knows how to gather a
    cell patch and its face-associated overlaps into one contiguous,
    temporary oldQWithHalo array (@see its own docstring) - this class only
    supplies the functor_implementation that copies that temporary array
    into solver._patch_with_halo's own accessor, since the toolbox class by
    itself has no notion of a persisted "patch with halo" cell submodel of
    its own (it only ever exposes the reconstructed data as a local, scratch
    C++ array to whatever functor you hand it).

    @todo Ported and drastically slimmed down from the legacy
    python/exahype2/solvers/fv/actionsets/ReconstructPatchWithHalo.py: the
    legacy version additionally read/wrote a per-cell CellLabel (time
    stamp, time step size, "has updated" flag) and dispatched into
    solver-generated compute kernels
    (repositories::<solver>.cellCanUseStatelessPDETerms(...) and friends) -
    this actually *solved* the PDE here. Neither the cell label nor that
    kernel dispatch machinery exist in the new exahype2.api yet (@see
    exahype2.api.fv.RusanovWithExplicitEuler.__create_data_structures()),
    so none of that is ported here - this class currently only reconstructs
    the halo; it does not (yet) call any Riemann solver.

    """

    def __init__(self, solver):
        """!

        solver: exahype2.api.Solver
          Has to have solver._patch, solver._patch_overlap_current_time_step
          and solver._patch_with_halo set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        """
        patch_with_halo_dofs_per_axis = solver._patch_with_halo.dim[0]
        number_of_values_2d = solver._patch_with_halo.no_of_unknowns * patch_with_halo_dofs_per_axis**2
        number_of_values_3d = number_of_values_2d * patch_with_halo_dofs_per_axis

        super().__init__(
            patch=solver._patch,
            patch_overlap=solver._patch_overlap_current_time_step,
            functor_implementation=f"""
    #if Dimensions==2
    std::copy(oldQWithHalo, oldQWithHalo + {number_of_values_2d}, fineGridCell{solver._patch_with_halo.name}.value);
    #elif Dimensions==3
    std::copy(oldQWithHalo, oldQWithHalo + {number_of_values_3d}, fineGridCell{solver._patch_with_halo.name}.value);
    #endif
""",
            reconstructed_array_memory_location=ReconstructedArrayMemoryLocation.Heap,
            guard="not marker.willBeRefined() and not marker.hasBeenRefined()",
            add_assertions_to_halo_exchange=False,
        )
        self._solver = solver

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def get_includes(self):
        return (
            super().get_includes()
            + """
#include <algorithm>
"""
            + self._solver._get_default_includes()
            + self._solver.user_solver_includes
        )
