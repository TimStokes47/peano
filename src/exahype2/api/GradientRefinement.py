# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .AdaptivityCriterion import AdaptivityCriterion


class GradientRefinement(AdaptivityCriterion):
    """!

    Gradient-based refinement criterion: given a one-sided difference
    quotient of the solution along one coordinate axis, decide whether the
    mesh around this point should be refined, erased (coarsened) or kept.

    @code
    virtual void refineEraseWithGradient(
      const double* __restrict__                  gradient, // gradient[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      int                                         normal,
      ::exahype2::RefinementCommand* __restrict__ refinementCommand
    );
    @endcode

    The action set which evaluates the criterion calls the term 2*Dimensions
    times per volume: For each coordinate axis normal, it passes the left
    difference quotient @f$ (Q_j - Q_{j-1})/h @f$ and the right one
    @f$ (Q_{j+1} - Q_j)/h @f$. Each call starts from Keep. The volume asks
    for a refinement if any of the calls says Refine, and it may be erased
    only if all of the calls say Erase.

    Volumes at the patch boundary have no neighbour on one side. For them,
    the difference quotient towards the patch's interior is passed in both
    calls (@ref exahype2::fv::computeOneSidedGradient()).

    gradient holds the gradient of the unknowns. The output
    semantics are those of every adaptivity criterion
    (@ref exahype2.api.AdaptivityCriterion).

    """

    SIGNATURE = (
        "const double* __restrict__ gradient, "
        "int normal, "
        "::exahype2::RefinementCommand* __restrict__ refinementCommand"
    )

    NAME = "refineEraseWithGradient"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        @param implementation: str
          A C++ snippet that becomes refineEraseWithGradient()'s body, or
          None (the default) to leave refineEraseWithGradient() pure virtual
          for you to implement by hand. The empty string is a valid body: it
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

        GradientRefinement-specific code generation

        Reuses PDETerm.Generator: GradientRefinement.SIGNATURE is picked up
        automatically. Only the stub for the user's solver differs, as the
        term is not handed any position x which the stub could trace.

        """

        def generate_stub_definition(self, qualified_class_name: str) -> str:
            """!
            Same as PDETerm.Generator.generate_stub_definition(), but traces
            normal only.
            """
            if self.term.implementation is not None or self.term.stateless:
                return ""
            method_name = f"{self.term.name}(...)"
            return (
                f"void {qualified_class_name}::{self.term.name}(\n"
                f"  {self.signature}\n"
                f") {{\n"
                f'  logTraceInWith1Argument("{method_name}", normal);\n'
                f'  logWarning("{method_name}", "not implemented yet");\n'
                f"  // @todo implement\n"
                f'  logTraceOut("{method_name}");\n'
                f"}}\n"
            )
