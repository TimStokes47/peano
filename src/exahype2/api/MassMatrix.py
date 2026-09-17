# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class MassMatrix(PDETerm):
    """!

    The (possibly Q-dependent) mass matrix M(Q) multiplying the input in a
    given direction, i.e. MTimesQ = M(Q) * Q. There is no direct legacy
    equivalent named "mass matrix" - the closest relative is aderdg's
    multiplyMaterialParameterMatrix() in
    python/exahype2/solvers/aderdg/AbstractSolverDeclarations.py, which is
    also normal-dependent, so this term follows the same reduced-signature
    pattern as Flux/MaxEigenvalue:

    @code
    virtual void massMatrix(
      const double* __restrict__                   Q, // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
      const tarch::la::Vector<Dimensions, double>& x,
      int                                          normal,
      double* __restrict__                         MTimesQ // MTimesQ[{{NUMBER_OF_UNKNOWNS}}]
    );
    @endcode

    """

    SIGNATURE = (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "int normal, "
        "double* __restrict__ MTimesQ"
    )

    NAME = "massMatrix"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        implementation: str
          A C++ snippet that becomes massMatrix()'s body (the empty string
          is a valid, generated no-op body), or None (the default) to leave
          massMatrix() pure virtual for you to implement by hand. Don't add
          this term to a solver at all if the PDE has an (implicit) identity
          mass matrix.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        MassMatrix-specific code generation

        Reuses PDETerm.Generator's declaration/definition/GPU-offload logic
        as-is - MassMatrix.SIGNATURE is picked up automatically. Declared
        explicitly (rather than left to be inherited implicitly) for the
        same reason as Flux.Generator: the obvious place to add
        massMatrix-specific generation behaviour later, and so
        mass_matrix_term.generator reports its type as MassMatrix.Generator
        rather than the more generic PDETerm.Generator.

        """
