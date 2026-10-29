# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .AdaptivityCriterion import AdaptivityCriterion


class RefineErasePointwisely(AdaptivityCriterion):
    """!

    Point-wise refinement criterion: given the solution Q at a single degree
    of freedom, its position x and the time stamp t, decide whether the mesh
    around this point should be refined, erased (coarsened) or kept.

    @code
    virtual void refineErasePointwisely(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      double                                       t,
      ::exahype2::RefinementCommand* __restrict__  refinementCommand
    );
    @endcode

    Like all the other PDE terms, the generated method returns void and
    writes its outcome into an output argument. The output semantics are
    those of every adaptivity criterion (@ref exahype2.api.AdaptivityCriterion).

    Different to the legacy refinementCriterion(Q, x, h, t), the term is not
    handed the mesh size. Bounds on the mesh size are the job of the grid
    construction, not of the physics.

    Q holds unknowns+auxiliary_variables entries.

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "double t, "
        "::exahype2::RefinementCommand* __restrict__ refinementCommand"
    )

    NAME = "refineErasePointwisely"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        @param implementation: str
          A C++ snippet that becomes refineErasePointwisely()'s body, or None
          (the default) to leave refineErasePointwisely() pure virtual for
          you to implement by hand. The empty string is a valid body: it
          leaves *refinementCommand untouched, i.e. at Keep.

        @param stateless: bool
          Generate a static, GPU-offloadable method instead of a virtual
          one. Requires an implementation.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(AdaptivityCriterion.Generator):
        """!

        RefineErasePointwisely-specific code generation

        Reuses PDETerm.Generator as-is: RefineErasePointwisely.SIGNATURE is
        picked up automatically. Declared explicitly for the same reason as
        Flux.Generator, i.e. as the place for term-specific generation
        behaviour later on.

        """
