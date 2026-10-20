# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from .PDETerm import PDETerm


class DirichletBoundaryConditions(PDETerm):
    """!

    Dirichlet boundary conditions: given a position x outside of the domain
    and a time stamp t, write the prescribed solution Q there.

    @code
    virtual void dirichletBoundaryConditions(
      const tarch::la::Vector<Dimensions, double>& x,
      double                                       t,
      double* __restrict__                         Q // Q[{{NUMBER_OF_UNKNOWNS}}+{{NUMBER_OF_AUXILIARY_VARIABLES}}]
    );
    @endcode

    Different to the legacy boundaryConditions() (@see
    python/exahype2/solvers/fv/SingleSweepAbstract.template.h), the term is
    not handed the solution inside the domain, the face's normal or the
    mesh size. It is parameterised through position and time only.

    A Finite Volume scheme such as exahype2.api.fv.RusanovWithExplicitEuler
    evaluates the term at the centres of the ghost volumes, i.e. of the
    volumes of the halo layer which lie outside of the domain, and stores
    the result in the face data its reconstruction reads from (@see
    exahype2.api.fv.actionsets.ApplyDirichletBoundaryConditions). t is the
    time stamp of the solution that the time step is about to advance.

    A solver needs this term if and only if at least one axis of the domain
    is not periodic (@see exahype2.api.Project.set_global_simulation_parameters()).
    If all axes are periodic, the term is never invoked, and the time step
    does not contain any boundary handling at all.

    Only the non-stateless variant is supported at the moment, i.e. the
    term has to be added with stateless=False.

    """

    SIGNATURE = (
        "const tarch::la::Vector<Dimensions, double>& x, "
        "double t, "
        "double* __restrict__ Q"
    )

    NAME = "dirichletBoundaryConditions"

    def __init__(
        self,
        implementation: str = None,
        stateless: bool = False,
    ):
        """!

        @param implementation: str
          A C++ snippet that becomes dirichletBoundaryConditions()'s body,
          or None (the default) to leave dirichletBoundaryConditions() pure
          virtual for you to implement by hand.

        @param stateless: bool
          Has to be False at the moment, @see the class documentation.

        """
        super().__init__(
            name=self.NAME,
            implementation=implementation,
            stateless=stateless,
        )

    class Generator(PDETerm.Generator):
        """!

        DirichletBoundaryConditions-specific code generation

        Reuses PDETerm.Generator as-is - DirichletBoundaryConditions.SIGNATURE
        is picked up automatically. Declared explicitly for the same reason
        as Flux.Generator.

        """
