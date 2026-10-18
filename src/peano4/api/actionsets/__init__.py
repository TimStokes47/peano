# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

This is a set of generic action sets that multiple Peano users or
extensions built on top of Peano might find useful.

Solver-agnostic peano4.api.solversteps.ActionSet realisations - action sets
that only ever look at the mesh itself (cell/vertex geometry), never at any
solver-specific data, and are therefore reusable across discretisations and
extensions (ExaHyPE, ExaParty, MGHyPE, ...). Contrast with e.g.
exahype2.api.fv.actionsets, whose action sets are tied to a concrete Finite
Volume solver.

"""

from .ConstructRegularGrid import ConstructRegularGrid
