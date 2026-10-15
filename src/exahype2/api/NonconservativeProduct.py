# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class NonconservativeProduct(PDETerm):
    """!

    The non-conservative product B(Q) applied to a jump deltaQ in a given
    direction, i.e. BTimesDeltaQ = B(Q) * deltaQ

    The signature matches ::exahype2::fv::rusanov::NonconservativeProductFunctor,
    i.e. what the Rusanov kernels hand over:

    @code
    virtual void nonconservativeProduct(
      const double* __restrict__                   Q,          // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const double* __restrict__                   deltaQ,     // deltaQ[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      const tarch::la::Vector<Dimensions, double>& volumeSize,
      double                                       t,
      double                                       dt,
      int                                          normal,
      double* __restrict__                         BTimesDeltaQ // BTimesDeltaQ[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    The kernels evaluate the term on each face of a volume: Q is the average
    of the two adjacent volumes, deltaQ the difference right minus left (or
    top minus bottom, ...) along normal. Both include the auxiliary
    variables, so a term can use the jump of a material parameter, e.g. of
    a bathymetry. The output only holds unknowns entries.

    The size of the volume is called volumeSize rather than h - as it is in
    the legacy API - as h frequently is the name of an unknown, e.g. the
    water height of the shallow water equations.

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const double* __restrict__ deltaQ, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "const tarch::la::Vector<Dimensions, double>& volumeSize, "
        "double t, "
        "double dt, "
        "int normal, "
        "double* __restrict__ BTimesDeltaQ"
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
