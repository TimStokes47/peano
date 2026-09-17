# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class NonconservativeProduct(PDETerm):
    """!

    The non-conservative product B(Q) applied to the input in a given
    direction, i.e. BTimesQ = B(Q) * Q.

    See python/exahype2/solvers/fv/riemann/kernels.py, whose
    nonconservativeProduct() multiplies B(Q) with an actual jump deltaQ
    (BTimesDeltaQ = B(Q) * deltaQ) rather than with Q itself. This new API
    reduces the signature the same way Flux does and, since there is no
    deltaQ parameter (yet), applies B(Q) to Q instead - deltaQ and the
    BTimesDeltaQ naming return once PDE terms are tied to real C code
    snippets with proper signatures:

    @code
    virtual void nonconservativeProduct(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      int                                          normal,
      double* __restrict__                         BTimesQ // BTimesQ[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "int normal, "
        "double* __restrict__ BTimesQ"
    )

    NAME = "nonconservativeProduct"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        implementation: str
          A C++ snippet that becomes nonconservativeProduct()'s body (the
          empty string is a valid, generated no-op body), or None (the
          default) to leave nonconservativeProduct() pure virtual for you to
          implement by hand. Don't add this term to a solver at all if the
          PDE has no non-conservative contribution.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        NonconservativeProduct-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - NonconservativeProduct.SIGNATURE is picked up automatically.
        Declared explicitly (rather than left to be inherited implicitly)
        for the same reason as Flux.Generator: the obvious place to add
        nonconservativeProduct-specific generation behaviour later, and so
        ncp_term.generator reports its type as
        NonconservativeProduct.Generator rather than the more generic
        PDETerm.Generator.

        """
