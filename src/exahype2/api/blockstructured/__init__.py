# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Boundary conditions of the block-structured numerical schemes

The terms and helpers in this package are used by both the Finite Volume
schemes (exahype2.api.fv) and the fourth-order finite differences with
Runge-Kutta time stepping (exahype2.api.rkfd4). Both schemes realise their
boundary conditions through the same C++ routines in
exahype2::blockstructured.

"""
from .DirichletBoundaryConditions import DirichletBoundaryConditions
from .SommerfeldBoundaryConditions import SommerfeldBoundaryConditions
from . import BoundaryConditions
