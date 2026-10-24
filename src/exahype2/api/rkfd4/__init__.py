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
fourth-order stencils reach two grid cells into the neighbouring patches,
so each patch is surrounded by a halo of two grid cells, which is
reconstructed from the faces. The scheme has no artificial dissipation.
@ref exagrype.api.FD4WithKODissipation adds a Kreiss-Oliger dissipation on
top, with a halo of three grid cells.

The numerical scheme is @ref
exahype2.api.rkfd4.CentralDifferencesWithRungeKutta.CentralDifferencesWithRungeKutta.
Its DSLHyPE kernels reside in @ref exahype2.api.rkfd4.FD4Kernels.

"""
from .CentralDifferencesWithRungeKutta import CentralDifferencesWithRungeKutta
from .SetupSimulationOverRegularGrid import SetupSimulationOverRegularGrid
from .PlotAsVTKFile import PlotAsVTKFile
from . import actionsets
