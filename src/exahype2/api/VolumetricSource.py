# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class VolumetricSource(PDETerm):
    """!

    The algebraic, volume-distributed source term S(Q) of a hyperbolic PDE -
    as opposed to PointSource, which contributes only at isolated points.

    See python/exahype2/solvers/fv/riemann/kernels.py's sourceTerm(), which
    is not normal-dependent (a volumetric source has no notion of a face
    direction), so unlike Flux/MaxEigenvalue/MassMatrix/
    NonconservativeProduct this term has no normal parameter to begin with:

    @code
    virtual void volumetricSource(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      double* __restrict__                         S // S[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "double* __restrict__ S"
    )

    NAME = "volumetricSource"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        implementation: str
          A C++ snippet that becomes volumetricSource()'s body (the empty
          string is a valid, generated no-op body), or None (the default)
          to leave volumetricSource() pure virtual for you to implement by
          hand. Don't add this term to a solver at all if the PDE has no
          volumetric source contribution.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        VolumetricSource-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - VolumetricSource.SIGNATURE is picked up automatically.
        Declared explicitly (rather than left to be inherited implicitly)
        for the same reason as Flux.Generator: the obvious place to add
        volumetricSource-specific generation behaviour later, and so
        volumetric_source_term.generator reports its type as
        VolumetricSource.Generator rather than the more generic
        PDETerm.Generator.

        """
