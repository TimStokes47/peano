# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from typing import Sequence

from ..PDETerm import PDETerm


class SommerfeldBoundaryConditions(PDETerm):
    """!

    Sommerfeld (radiative, outgoing) boundary conditions: waves leave the
    domain with little reflection, and the solution approaches a far-field
    solution towards infinity

    The term is an alternative to exahype2.api.DirichletBoundaryConditions.
    @ref exahype2::blockstructured::applySommerfeldBoundaryConditions()
    documents the conditions, their discretisation, and how block-structured
    schemes realise them. The user specifies the far-field solution
    @f$ u_\\infty @f$:

    @code
    virtual void sommerfeldFarFieldSolution(
      const tarch::la::Vector<Dimensions, double>& x,
      double* __restrict__                         Q // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
    );
    @endcode

    Only the first NUMBER_OF_UNKNOWNS entries of Q are read, i.e. the
    implementation does not have to set the auxiliary variables.

    The wave speed is the solver's maximum eigenvalue along the face normal,
    i.e. the solver has to hold an exahype2.api.MaxEigenvalue term. The wave
    speed does not need any solver state, so the boundary conditions use the
    stateless variant of the term if the solver holds one, and the variant
    with state otherwise (@see
    exahype2.api.blockstructured.BoundaryConditions.get_max_eigenvalue_term_for_sommerfeld()). The ghost cells are initialised with the initial condition
    (exahype2.api.InitPointWisely) before the first time step.

    Different to Dirichlet conditions, there is no separate boundary task
    in the time step. The Sommerfeld update replaces the roll over of the
    face data instead (@see
    exahype2.api.fv.actionsets.RollOverUpdatedFaceWithSommerfeldBoundaryConditions and
    exahype2.api.rkfd4.actionsets.RollOverUpdatedFaceWithSommerfeldBoundaryConditions).
    Runge-Kutta schemes accept the term only if the time stamps of their
    stages do not decrease.

    A solver needs a boundary condition term if and only if at least one
    axis of the domain is not periodic (@see
    exahype2.api.blockstructured.BoundaryConditions.get_boundary_conditions()).

    Only the non-stateless variant is supported, i.e. the term has to be
    added with stateless=False.

    """

    SIGNATURE = (
        "const tarch::la::Vector<Dimensions, double>& x, " "double* __restrict__ Q"
    )

    NAME = "sommerfeldFarFieldSolution"

    def __init__(
        self,
        implementation: str = None,
        origin: Sequence[float] = None,
        stateless: bool = False,
    ):
        """!

        @param implementation: str
          A C++ snippet that becomes sommerfeldFarFieldSolution()'s body, or
          None (the default) to leave sommerfeldFarFieldSolution() pure
          virtual for you to implement by hand.

        @param origin: Sequence[float]
          Centre of the system, i.e. the point the outgoing spherical waves
          emanate from. One entry per dimension. None (the default) is the
          origin of the coordinate system.

        @param stateless: bool
          Has to be False, @see the class documentation.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )
        self.origin = None if origin is None else [float(x) for x in origin]

    def get_origin(self, dimensions: int):
        """!

        The origin with one entry per dimension

        @param dimensions: int
          Number of dimensions of the domain.

        @return List[float]

        """
        if self.origin is None:
            return [0.0] * dimensions
        if len(self.origin) != dimensions:
            raise ValueError(
                f"The origin {self.origin} of the Sommerfeld boundary conditions has "
                f"{len(self.origin)} entries, but the domain has {dimensions} dimensions."
            )
        return list(self.origin)

    class Generator(PDETerm.Generator):
        """!

        SommerfeldBoundaryConditions-specific code generation

        Reuses PDETerm.Generator as-is - SommerfeldBoundaryConditions.SIGNATURE
        is picked up automatically. Declared explicitly for the same reason
        as Flux.Generator.

        """
