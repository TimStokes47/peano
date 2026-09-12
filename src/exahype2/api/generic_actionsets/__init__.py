# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Solver-agnostic peano4.api.solversteps.ActionSet realisations - action sets
that only ever look at the mesh itself (cell/vertex geometry), never at any
solver-specific data, and are therefore reusable across discretisations.
Contrast with e.g. exahype2.api.fv.actionsets, whose action sets are tied
to a concrete Finite Volume solver.

"""

from .ConstructRegularGrid import ConstructRegularGrid
