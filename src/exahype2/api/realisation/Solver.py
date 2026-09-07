# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from __future__ import annotations

import peano4.api.output

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

    It also owns the output (generated files) that its Solver.SolverGenerator
    (and the other per-solver generators) produce, held as a standalone
    peano4.api.output.Output. Generators therefore add their artefacts here
    rather than reaching into the Peano 4 project directly; it is the
    exahype2 Project's generator that later folds this output into the
    generated Peano 4 project's own output.

    """

    def __init__(self, solver: SolverSpecification):
        if not isinstance(solver, SolverSpecification):
            raise TypeError(
                "exahype2.api.realisation.Solver expects exactly one exahype2.api.Solver instance, but got {}".format(
                    type(solver)
                )
            )
        self._solver = solver
        self._output = peano4.api.output.Output()

    @property
    def solver(self) -> SolverSpecification:
        return self._solver

    @property
    def output(self) -> peano4.api.output.Output:
        return self._output
