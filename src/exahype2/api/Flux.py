# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class Flux(PDETerm):
    """!

    The conservative flux F(Q) of a hyperbolic PDE

    See python/exahype2/solvers/fv/riemann/kernels.py for the legacy
    equivalent, which additionally threads h/t/dt through the signature.
    This new API's flux() is, for now, deliberately reduced to

    @code
    virtual void flux(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      int                                          normal,
      double* __restrict__                         F // F[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    h/t/dt will return once PDE terms are tied to real C code snippets with
    proper signatures; until then every term merely gets position, normal
    (where physically meaningful) and its input/output.

    Q holds the unknowns, and so does the output F. Auxiliary variables are
    not subject to the hyperbolic formulation. If the solver has any, a pointer to
    each set follows right behind Q (@ref exahype2.api.AuxiliaryVariables).

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "int normal, "
        "double* __restrict__ F"
    )

    NAME = "flux"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        implementation: str
          A C++ snippet that becomes flux()'s body (the empty string is a
          valid, generated no-op body), or None (the default) to leave
          flux() pure virtual for you to implement by hand. Don't add this
          term to a solver at all if the PDE has no flux contribution.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        Flux-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - Flux.SIGNATURE is picked up automatically. Declared
        explicitly (rather than left to be inherited implicitly) so that
        Flux.Generator is the obvious place to add flux-specific generation
        behaviour later, and so flux_term.generator reports its type as
        Flux.Generator rather than the more generic PDETerm.Generator.

        """
