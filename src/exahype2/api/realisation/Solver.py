# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

from ..Solver import Solver as SolverSpecification


class Solver:
    """!

    Realisation of a solver specification

    An exahype2.api.Solver is a plain specification of a solver (name,
    unknowns, PDE terms, ...). This class is the counterpart that turns
    exactly one such specification into something concrete. Every instance
    is therefore tied to exactly one exahype2.api.Solver for its whole
    lifetime; it does not aggregate or switch between several
    specifications.

    """

    def __init__(self, solver: SolverSpecification):
        if not isinstance(solver, SolverSpecification):
            raise TypeError(
                "exahype2.api.realisation.Solver expects exactly one exahype2.api.Solver instance, but got {}".format(
                    type(solver)
                )
            )
        self._solver = solver

    @property
    def solver(self) -> SolverSpecification:
        return self._solver
