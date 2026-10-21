# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Fourth-order Finite Differences with explicit Runge-Kutta time stepping
over block-structured meshes

The package is the successor of the legacy exahype2.solvers.rkfd.fd4. It
lives parallel to @ref exahype2.api.fv and does not depend on it: grid
construction, plotting and all action sets are its own. The Runge-Kutta
coefficients come from @ref exahype2.api.rungekutta, which does not depend
on the discretisation in space.

Each mesh cell hosts a patch of patch_size^d cell-centred grid cells. The
fourth-order stencils (plus the optional Kreiss-Oliger dissipation) reach
three grid cells into the neighbouring patches, so each patch is surrounded
by a halo of three grid cells, which is reconstructed from the faces.

The numerical scheme is @ref
exahype2.api.rkfd4.CentralDifferencesWithRungeKutta.CentralDifferencesWithRungeKutta.

@todo The task graph of the Runge-Kutta stages is not there yet.

"""
from .CentralDifferencesWithRungeKutta import CentralDifferencesWithRungeKutta
from .SetupSimulationOverRegularGrid import SetupSimulationOverRegularGrid
from .PlotAsVTKFile import PlotAsVTKFile
from . import actionsets
