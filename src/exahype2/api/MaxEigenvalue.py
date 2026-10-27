# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class MaxEigenvalue(PDETerm):
    """!

    The maximum absolute eigenvalue of the flux Jacobian in a given
    direction, i.e. the spectral radius used by Rusanov-type numerical
    fluxes and by the CFL-driven admissible time step size.

    See python/exahype2/solvers/fv/riemann/kernels.py's eigenvalues(), which
    returns the full eigenvalue vector L rather than just its maximum - this
    new API deliberately asks for the reduced quantity solvers actually
    need for their numerical flux, and reduces the legacy signature the
    same way Flux does:

    @code
    virtual void maxEigenvalue(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      int                                          normal,
      double* __restrict__                         lambdaMax // lambdaMax[1]
    );
    @endcode

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "int normal, "
        "double* __restrict__ lambdaMax"
    )

    NAME = "maxEigenvalue"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        implementation: str
          A C++ snippet that becomes maxEigenvalue()'s body (the empty
          string is a valid, generated no-op body), or None (the default)
          to leave maxEigenvalue() pure virtual for you to implement by
          hand. Don't add this term to a solver at all if it needs no
          eigenvalue estimate.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        MaxEigenvalue-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - MaxEigenvalue.SIGNATURE is picked up automatically. Declared
        explicitly (rather than left to be inherited implicitly) for the
        same reason as Flux.Generator: the obvious place to add
        maxEigenvalue-specific generation behaviour later, and so
        max_eigenvalue_term.generator reports its type as
        MaxEigenvalue.Generator rather than the more generic PDETerm.Generator.

        """
