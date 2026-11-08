# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class PointSource(PDETerm):
    """!

    A source term that contributes only at isolated points (e.g. a seismic
    point source), as opposed to VolumetricSource, which is distributed over
    the whole domain.

    See python/exahype2/solvers/aderdg/AbstractSolverDeclarations.py's
    pointSource(), which additionally takes t, dt and an index n selecting
    which of possibly several point sources is being evaluated - dropped
    here the same way h/t/dt are dropped from Flux, and n returns once PDE
    terms are tied to real C code snippets with proper signatures:

    @code
    virtual void pointSource(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      double* __restrict__                         forceVector // forceVector[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "double* __restrict__ forceVector"
    )

    NAME = "pointSource"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        implementation: str
          A C++ snippet that becomes pointSource()'s body (the empty string
          is a valid, generated no-op body), or None (the default) to leave
          pointSource() pure virtual for you to implement by hand. Don't add
          this term to a solver at all if the PDE has no point source
          contribution.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        PointSource-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - PointSource.SIGNATURE is picked up automatically. Declared
        explicitly (rather than left to be inherited implicitly) for the
        same reason as Flux.Generator: the obvious place to add
        pointSource-specific generation behaviour later, and so
        point_source_term.generator reports its type as
        PointSource.Generator rather than the more generic PDETerm.Generator.

        """
