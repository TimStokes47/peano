# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Boundary conditions and mesh refinement of the block-structured numerical schemes

The terms, action sets and helpers in this package are used by both the Finite Volume
schemes (exahype2.api.fv) and the fourth-order finite differences with
Runge-Kutta time stepping (exahype2.api.rkfd4). Both schemes realise their
boundary conditions through the same C++ routines in
exahype2::blockstructured.

"""
from .DirichletBoundaryConditions import DirichletBoundaryConditions
from .SommerfeldBoundaryConditions import SommerfeldBoundaryConditions
from .DynamicMeshRefinement import DynamicMeshRefinement
from .DynamicMeshRefinementWithGradients import DynamicMeshRefinementWithGradients
from .create_dynamic_mesh_refinement_action_set import create_dynamic_mesh_refinement_action_set
from .ConsumeRefinementRequests import ConsumeRefinementRequests
from .AdaptMeshAndBalanceLoad import AdaptMeshAndBalanceLoad
from . import BoundaryConditions
from .LoadCheckpoint import LoadCheckpoint
