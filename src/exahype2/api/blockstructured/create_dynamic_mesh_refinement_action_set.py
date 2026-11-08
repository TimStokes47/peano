# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from ..AdaptivityCriterion import get_adaptivity_criterion
from ..GradientRefinement import GradientRefinement
from ..RefineErasePointwisely import RefineErasePointwisely
from .DynamicMeshRefinement import DynamicMeshRefinement
from .DynamicMeshRefinementWithGradients import DynamicMeshRefinementWithGradients


def create_dynamic_mesh_refinement_action_set(
    solver,
    patch_size: int,
    guard: str,
    min_volume_size: float = None,
    max_volume_size: float = None,
    hand_events_to_peano: bool = False,
    veto_erase: bool = False,
):
    """!

    Create the action set which evaluates the solver's adaptivity criterion

    The numerical schemes and the setup do not evaluate any criterion if the
    solver has none, i.e. they then add no action set at all. Otherwise, the
    criterion's type determines the action set:

    - exahype2.api.RefineErasePointwisely yields an
      exahype2.api.blockstructured.DynamicMeshRefinement;
    - exahype2.api.GradientRefinement yields an
      exahype2.api.blockstructured.DynamicMeshRefinementWithGradients.

    @param solver: exahype2.api.Solver
      The solver whose criterion is evaluated. It may have at most one
      adaptivity criterion (@ref exahype2.api.AdaptivityCriterion).

    @param patch_size: int
      Number of volumes per axis within one patch.

    @param guard: str
      C++ boolean expression, evaluated per cell, guarding whether the
      criterion is evaluated for that cell.

    @param min_volume_size: float
      See exahype2.api.blockstructured.DynamicMeshRefinement.

    @param max_volume_size: float
      See exahype2.api.blockstructured.DynamicMeshRefinement.

    @param hand_events_to_peano: bool
      See exahype2.api.blockstructured.DynamicMeshRefinement.

    @param veto_erase: bool
      See exahype2.api.blockstructured.DynamicMeshRefinement.

    @return The action set, or None if the solver has no adaptivity
      criterion.

    """
    criterion = get_adaptivity_criterion(solver)
    if criterion is None:
        return None
    if solver.auxiliary_variables:
        raise NotImplementedError(
            f"Solver {solver.name}: dynamic mesh refinement is not supported for solvers with auxiliary variables yet."
        )
    if isinstance(criterion, RefineErasePointwisely):
        action_set_type = DynamicMeshRefinement
    elif isinstance(criterion, GradientRefinement):
        action_set_type = DynamicMeshRefinementWithGradients
    else:
        raise TypeError(
            f"No action set evaluates adaptivity criteria of type {type(criterion).__name__}"
        )
    return action_set_type(
        solver,
        patch_size,
        guard=guard,
        min_volume_size=min_volume_size,
        max_volume_size=max_volume_size,
        hand_events_to_peano=hand_events_to_peano,
        veto_erase=veto_erase,
    )
