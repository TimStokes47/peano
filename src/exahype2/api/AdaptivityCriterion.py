# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class AdaptivityCriterion(PDETerm):
    """!

    Base class of all the PDE terms which decide where to refine or coarsen
    the mesh

    An adaptivity criterion is a PDE term which writes its decision into the
    output argument refinementCommand. It points to one
    ::exahype2::RefinementCommand, i.e. Keep, Refine or Erase (see
    exahype2/RefinementControl.h). The caller initialises it with Keep before
    the call, so an implementation only has to set it if it wants to refine
    or erase. In particular, the empty string as implementation yields a
    criterion which always keeps the mesh.

    The subclasses differ in what they are handed:

    - exahype2.api.RefineErasePointwisely is handed the solution in one
      volume.
    - exahype2.api.GradientRefinement is handed a one-sided difference
      quotient of the solution along one coordinate axis.

    Only if a solver has an adaptivity criterion do the numerical schemes
    and the setup evaluate it, i.e. add the action sets which collect refine
    and erase requests (@ref exahype2.api.blockstructured.create_dynamic_mesh_refinement_action_set.create_dynamic_mesh_refinement_action_set()).
    A solver may have at most one adaptivity criterion.

    The class itself defines no signature, i.e. you always add one of its
    subclasses to a solver.

    """

    def __init__(
        self,
        name: str,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        @param name: str
          The C++ method name of the criterion.

        @param implementation: str
          A C++ snippet that becomes the method's body, or None (the default)
          to leave the method pure virtual for you to implement by hand. The
          empty string is a valid body: it leaves *refinementCommand
          untouched, i.e. at Keep.

        @param stateless: bool
          Generate a static, GPU-offloadable method instead of a virtual
          one. Requires an implementation.

        """
        super().__init__(
            name=name,
            implementation=implementation,
            stateless=stateless,
        )


def get_adaptivity_criterion(solver) -> AdaptivityCriterion:
    """!

    The solver's adaptivity criterion, or None if it has none

    Raises a ValueError if the solver has more than one adaptivity
    criterion.

    @param solver: exahype2.api.Solver
      The solver to inspect.

    """
    criteria = [
        term for term in solver.pde_terms if isinstance(term, AdaptivityCriterion)
    ]
    if len(criteria) > 1:
        raise ValueError(
            f"Solver {solver.name} has {len(criteria)} adaptivity criteria "
            f"({', '.join(term.name for term in criteria)}), but supports at most one"
        )
    return criteria[0] if criteria else None
