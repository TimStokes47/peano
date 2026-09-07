# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class Flux(PDETerm):
    """!

    The conservative flux F(Q) of a hyperbolic PDE

    See python/exahype2/solvers/fv/riemann/kernels.py, which declares this
    term's generated method as

    @code
    virtual void flux(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      const tarch::la::Vector<Dimensions, double>& h,
      double                                       t,
      double                                       dt,
      int                                          normal,
      double* __restrict__                         F // F[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    Q holds unknowns+auxiliary_variables entries, whereas the output F only
    holds unknowns entries: only the actual unknowns are subject to the
    hyperbolic formulation, auxiliary variables are not.

    Unlike most other PDE terms, flux() also gets a static, GPU-offloadable
    overload (`static GPUCallableMethod void flux(...)`, no trailing normal
    -dependent virtual dispatch) whenever the solver's PDE terms are
    stateless - kernels.py emits it guarded by
    `FLUX_IMPLEMENTATION!="<user-defined>" and STATELESS_PDE_TERMS`. This is
    solver-wide (a solver's `pde_terms_without_state` flag, not a per-term
    setting), but flux is one of the terms that actually takes advantage of
    it once enabled, unlike e.g. eigenvalues().

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "const tarch::la::Vector<Dimensions, double>& h, "
        "double t, double dt, int normal, "
        "double* __restrict__ F"
    )

    NAME = "flux"
    
    def __init__(self, 
                 implementation: str  = None,
                 stateless:      bool = False,
                 ):
        """!

        implementation: str
          One of PDETerm.USER_DEFINED/NONE/EMPTY, or a C++ snippet that
          becomes flux()'s body. Defaults to PDETerm.NONE, i.e. the PDE has
          no flux contribution until you actively enable one.

        """
        super().__init__(name=self.NAME,
                         implementation=implementation,
                         stateless=stateless,
                         )

    class Generator(PDETerm.Generator):
        """!

        Flux-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - Flux.SIGNATURE is picked up automatically, and flux is
        generated with a GPU-offloadable static overload precisely when
        term.stateless is set, matching the class docstring above. Declared
        explicitly (rather than left to be inherited implicitly) so that
        Flux.Generator is the obvious place to add flux-specific generation
        behaviour later, and so flux_term.generator reports its type as
        Flux.Generator rather than the more generic PDETerm.Generator.

        """
