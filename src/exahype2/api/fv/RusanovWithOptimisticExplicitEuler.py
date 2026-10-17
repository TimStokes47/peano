# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os
import re

import peano4.api
import peano4.api.datamodel
import peano4.api.dastgen2
import toolbox.blockstructured.api
import exahype2.api
from exahype2.api import Target
import exahype2.api.dslhype as dslhype
 
from ..Solver import Solver
from .actionsets import HandleBoundary
from .actionsets import ProjectPatchOntoFaces
from .actionsets import RollOverUpdatedFace
from .actionsets import UpdateCell
from .actionsets import ComputeGlobalCFLCondition


## DSLHyPE time step kernels on the core and on the shell of a patch
#
# Variants of exahype2.api.fv.timeStepWithRusanov2D/3D that update only the
# core or only the shell of the patch
# (@ref exahype2.api.fv.RusanovWithOptimisticExplicitEuler.RusanovWithOptimisticExplicitEuler).
# They are never executed as Python, but parsed by DSLHyPE and hence follow
# the conventions documented with timeStepWithRusanov2D in
# RusanovWithExplicitEuler.py. Like the original kernels, they assume that the
# Rusanov stencil reaches one volume in each direction. All volumes between H
# and NV-H along each axis are core volumes, so the shell is H volumes thick.
#
# - The core kernels read the input only within [H-1,NV-H+1) along each axis.
#   For H=1, they hence do not touch the halo and can run before the halo
#   data has arrived. They write the output and the maximum eigenvalue of the
#   core only.
# - The shell kernels tile the shell into rectangular strips (2d) or slabs
#   (3d) and run the update once per strip with its own set of temporary
#   variables, as the DSL has no loops. They write the output of the shell
#   only. However, they reduce the maximum eigenvalue over the whole new
#   patch, so they have to run after the core kernel has written its output.
#
# The 3d kernels test for the PDE terms by the same names as
# timeStepWithRusanov2D, i.e. source, flux, nonconservativeProduct and
# maxEigenvalue.


## Rusanov update of the core of a 2d patch.
def timeStepWithRusanov2DOnCore(patchData: "::exahype2::CellData<double, double>&") -> "void":
    N = Integer("patchData.numberOfCells")
    t = DataBlock([[0, N]], "patchData.t", "double")
    dt = DataBlock([[0, N]], "patchData.dt", "double")
    h = DataBlock([[0, N]], "patchData.cellSize", "tarch::la::Vector<2, double>")
    x = DataBlock([[0, N]], "patchData.cellCentre", "tarch::la::Vector<2, double>")
    maxEigenvalues = DataBlock([[0, N]], "patchData.maxEigenvalue", "double")

    input = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [-H, NV + H], [-H, NV + H], [0, N]], "patchData.QIn")
    output = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [0, NV], [0, NV], [0, N]], "patchData.QOut")

    output[:, H:NV-H, H:NV-H] = input[:, H:NV-H, H:NV-H]

    if source:
       source = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H, NV-H], [0, N]], eval("sourceTerm", input[:, H:NV-H, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + dt * source

    maxEigenvaluesX = DataBlock([[0, 1], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, H-1:NV-H+1, H:NV-H], x, 0))
    maxEigenvaluesY = DataBlock([[0, 1], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, H:NV-H, H-1:NV-H+1], x, 1))

    lambdaLeft = max(maxEigenvaluesX[0:1, H-1:NV-H-1, H:NV-H], maxEigenvaluesX[0:1, H:NV-H, H:NV-H])
    lambdaRight = max(maxEigenvaluesX[0:1, H:NV-H, H:NV-H], maxEigenvaluesX[0:1, H+1:NV-H+1, H:NV-H])
    lambdaBottom = max(maxEigenvaluesY[0:1, H:NV-H, H-1:NV-H-1], maxEigenvaluesY[0:1, H:NV-H, H:NV-H])
    lambdaTop = max(maxEigenvaluesY[0:1, H:NV-H, H:NV-H], maxEigenvaluesY[0:1, H:NV-H, H+1:NV-H+1])

    fluxLeft = -1.0 * lambdaLeft * (input[0:NumberOfUnknowns, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, H-1:NV-H-1, H:NV-H])
    fluxRight = -1.0 * lambdaRight * (input[0:NumberOfUnknowns, H+1:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + dt / h(0) * (fluxLeft - fluxRight) * NV * 0.5

    fluxBottom = -1.0 * lambdaBottom * (input[0:NumberOfUnknowns, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, H:NV-H, H-1:NV-H-1])
    fluxTop = -1.0 * lambdaTop * (input[0:NumberOfUnknowns, H:NV-H, H+1:NV-H+1] - input[0:NumberOfUnknowns, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + dt / h(1) * (fluxBottom - fluxTop) * NV * 0.5

    if flux:
      flux_x = DataBlock([[0, NumberOfUnknowns], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("flux", input[:, H-1:NV-H+1, H:NV-H], x, 0))
      flux_y = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("flux", input[:, H:NV-H, H-1:NV-H+1], x, 1))
      output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_x[:, H-1:NV-H-1, H:NV-H] - flux_x[:, H+1:NV-H+1, H:NV-H])
      output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_y[:, H:NV-H, H-1:NV-H-1] - flux_y[:, H:NV-H, H+1:NV-H+1])

    if nonconservativeProduct:
       average_x = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H-1:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H+1, H:NV-H])
       delta_x = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H-1:NV-H, H:NV-H]
       nonconservative_x = DataBlock([[0, NumberOfUnknowns], [H, NV-H+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_x, delta_x, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_x[:, H:NV-H, H:NV-H] - nonconservative_x[:, H+1:NV-H+1, H:NV-H])

       average_y = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H+1])
       delta_y = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H-1:NV-H]
       nonconservative_y = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_y, delta_y, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_y[:, H:NV-H, H:NV-H] - nonconservative_y[:, H:NV-H, H+1:NV-H+1])

    if maxEigenvalue:
      maxEigenvaluesX2 = DataBlock([[0, 1], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", output[:, H:NV-H, H:NV-H], x, 0))
      maxEigenvaluesY2 = DataBlock([[0, 1], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", output[:, H:NV-H, H:NV-H], x, 1))
      maxEigenvaluesPerVolume = max(maxEigenvaluesX2, maxEigenvaluesY2)
      maxEigenvalues = max(maxEigenvaluesPerVolume)


## Rusanov update of the shell of a 2d patch. Run after timeStepWithRusanov2DOnCore().
def timeStepWithRusanov2DOnShell(patchData: "::exahype2::CellData<double, double>&") -> "void":
    N = Integer("patchData.numberOfCells")
    t = DataBlock([[0, N]], "patchData.t", "double")
    dt = DataBlock([[0, N]], "patchData.dt", "double")
    h = DataBlock([[0, N]], "patchData.cellSize", "tarch::la::Vector<2, double>")
    x = DataBlock([[0, N]], "patchData.cellCentre", "tarch::la::Vector<2, double>")
    maxEigenvalues = DataBlock([[0, N]], "patchData.maxEigenvalue", "double")

    input = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [-H, NV + H], [-H, NV + H], [0, N]], "patchData.QIn")
    output = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [0, NV], [0, NV], [0, N]], "patchData.QOut")

    # Shell part YLow: [0,NV) x [0,H)
    output[:, 0:NV, 0:H] = input[:, 0:NV, 0:H]

    if source:
       sourceYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, H], [0, N]], eval("sourceTerm", input[:, 0:NV, 0:H], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + dt * sourceYLow

    maxEigenvaluesXYLow = DataBlock([[0, 1], [0-1, NV+1], [0, H], [0, N]], eval("maxEigenvalue", input[:, 0-1:NV+1, 0:H], x, 0))
    maxEigenvaluesYYLow = DataBlock([[0, 1], [0, NV], [0-1, H+1], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0-1:H+1], x, 1))

    lambdaLeftYLow = max(maxEigenvaluesXYLow[0:1, 0-1:NV-1, 0:H], maxEigenvaluesXYLow[0:1, 0:NV, 0:H])
    lambdaRightYLow = max(maxEigenvaluesXYLow[0:1, 0:NV, 0:H], maxEigenvaluesXYLow[0:1, 0+1:NV+1, 0:H])
    lambdaBottomYLow = max(maxEigenvaluesYYLow[0:1, 0:NV, 0-1:H-1], maxEigenvaluesYYLow[0:1, 0:NV, 0:H])
    lambdaTopYLow = max(maxEigenvaluesYYLow[0:1, 0:NV, 0:H], maxEigenvaluesYYLow[0:1, 0:NV, 0+1:H+1])

    fluxLeftYLow = -1.0 * lambdaLeftYLow * (input[0:NumberOfUnknowns, 0:NV, 0:H] - input[0:NumberOfUnknowns, 0-1:NV-1, 0:H])
    fluxRightYLow = -1.0 * lambdaRightYLow * (input[0:NumberOfUnknowns, 0+1:NV+1, 0:H] - input[0:NumberOfUnknowns, 0:NV, 0:H])
    output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + dt / h(0) * (fluxLeftYLow - fluxRightYLow) * NV * 0.5

    fluxBottomYLow = -1.0 * lambdaBottomYLow * (input[0:NumberOfUnknowns, 0:NV, 0:H] - input[0:NumberOfUnknowns, 0:NV, 0-1:H-1])
    fluxTopYLow = -1.0 * lambdaTopYLow * (input[0:NumberOfUnknowns, 0:NV, 0+1:H+1] - input[0:NumberOfUnknowns, 0:NV, 0:H])
    output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + dt / h(1) * (fluxBottomYLow - fluxTopYLow) * NV * 0.5

    if flux:
      flux_xYLow = DataBlock([[0, NumberOfUnknowns], [0-1, NV+1], [0, H], [0, N]], eval("flux", input[:, 0-1:NV+1, 0:H], x, 0))
      flux_yYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0-1, H+1], [0, N]], eval("flux", input[:, 0:NV, 0-1:H+1], x, 1))
      output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + 0.5 * NV * dt / h(0) * (flux_xYLow[:, 0-1:NV-1, 0:H] - flux_xYLow[:, 0+1:NV+1, 0:H])
      output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + 0.5 * NV * dt / h(1) * (flux_yYLow[:, 0:NV, 0-1:H-1] - flux_yYLow[:, 0:NV, 0+1:H+1])

    if nonconservativeProduct:
       average_xYLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:H])
       delta_xYLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:H]
       nonconservative_xYLow = DataBlock([[0, NumberOfUnknowns], [0, NV+1], [0, H], [0, N]], eval("nonconservativeProduct", average_xYLow, delta_xYLow, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + 0.5 * NV * dt / h(0) * (-nonconservative_xYLow[:, 0:NV, 0:H] - nonconservative_xYLow[:, 0+1:NV+1, 0:H])

       average_yYLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H+1])
       delta_yYLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:H]
       nonconservative_yYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, H+1], [0, N]], eval("nonconservativeProduct", average_yYLow, delta_yYLow, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:H] + 0.5 * NV * dt / h(1) * (-nonconservative_yYLow[:, 0:NV, 0:H] - nonconservative_yYLow[:, 0:NV, 0+1:H+1])

    # Shell part YHigh: [0,NV) x [NV-H,NV)
    output[:, 0:NV, NV-H:NV] = input[:, 0:NV, NV-H:NV]

    if source:
       sourceYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H, NV], [0, N]], eval("sourceTerm", input[:, 0:NV, NV-H:NV], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + dt * sourceYHigh

    maxEigenvaluesXYHigh = DataBlock([[0, 1], [0-1, NV+1], [NV-H, NV], [0, N]], eval("maxEigenvalue", input[:, 0-1:NV+1, NV-H:NV], x, 0))
    maxEigenvaluesYYHigh = DataBlock([[0, 1], [0, NV], [NV-H-1, NV+1], [0, N]], eval("maxEigenvalue", input[:, 0:NV, NV-H-1:NV+1], x, 1))

    lambdaLeftYHigh = max(maxEigenvaluesXYHigh[0:1, 0-1:NV-1, NV-H:NV], maxEigenvaluesXYHigh[0:1, 0:NV, NV-H:NV])
    lambdaRightYHigh = max(maxEigenvaluesXYHigh[0:1, 0:NV, NV-H:NV], maxEigenvaluesXYHigh[0:1, 0+1:NV+1, NV-H:NV])
    lambdaBottomYHigh = max(maxEigenvaluesYYHigh[0:1, 0:NV, NV-H-1:NV-1], maxEigenvaluesYYHigh[0:1, 0:NV, NV-H:NV])
    lambdaTopYHigh = max(maxEigenvaluesYYHigh[0:1, 0:NV, NV-H:NV], maxEigenvaluesYYHigh[0:1, 0:NV, NV-H+1:NV+1])

    fluxLeftYHigh = -1.0 * lambdaLeftYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns, 0-1:NV-1, NV-H:NV])
    fluxRightYHigh = -1.0 * lambdaRightYHigh * (input[0:NumberOfUnknowns, 0+1:NV+1, NV-H:NV] - input[0:NumberOfUnknowns, 0:NV, NV-H:NV])
    output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + dt / h(0) * (fluxLeftYHigh - fluxRightYHigh) * NV * 0.5

    fluxBottomYHigh = -1.0 * lambdaBottomYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns, 0:NV, NV-H-1:NV-1])
    fluxTopYHigh = -1.0 * lambdaTopYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H+1:NV+1] - input[0:NumberOfUnknowns, 0:NV, NV-H:NV])
    output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + dt / h(1) * (fluxBottomYHigh - fluxTopYHigh) * NV * 0.5

    if flux:
      flux_xYHigh = DataBlock([[0, NumberOfUnknowns], [0-1, NV+1], [NV-H, NV], [0, N]], eval("flux", input[:, 0-1:NV+1, NV-H:NV], x, 0))
      flux_yYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H-1, NV+1], [0, N]], eval("flux", input[:, 0:NV, NV-H-1:NV+1], x, 1))
      output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(0) * (flux_xYHigh[:, 0-1:NV-1, NV-H:NV] - flux_xYHigh[:, 0+1:NV+1, NV-H:NV])
      output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(1) * (flux_yYHigh[:, 0:NV, NV-H-1:NV-1] - flux_yYHigh[:, 0:NV, NV-H+1:NV+1])

    if nonconservativeProduct:
       average_xYHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, NV-H:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, NV-H:NV])
       delta_xYHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, NV-H:NV] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, NV-H:NV]
       nonconservative_xYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV+1], [NV-H, NV], [0, N]], eval("nonconservativeProduct", average_xYHigh, delta_xYHigh, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(0) * (-nonconservative_xYHigh[:, 0:NV, NV-H:NV] - nonconservative_xYHigh[:, 0+1:NV+1, NV-H:NV])

       average_yYHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H-1:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV+1])
       delta_yYHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H-1:NV]
       nonconservative_yYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H, NV+1], [0, N]], eval("nonconservativeProduct", average_yYHigh, delta_yYHigh, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(1) * (-nonconservative_yYHigh[:, 0:NV, NV-H:NV] - nonconservative_yYHigh[:, 0:NV, NV-H+1:NV+1])

    # Shell part XLow: [0,H) x [H,NV-H)
    output[:, 0:H, H:NV-H] = input[:, 0:H, H:NV-H]

    if source:
       sourceXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H, NV-H], [0, N]], eval("sourceTerm", input[:, 0:H, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + dt * sourceXLow

    maxEigenvaluesXXLow = DataBlock([[0, 1], [0-1, H+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0-1:H+1, H:NV-H], x, 0))
    maxEigenvaluesYXLow = DataBlock([[0, 1], [0, H], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, 0:H, H-1:NV-H+1], x, 1))

    lambdaLeftXLow = max(maxEigenvaluesXXLow[0:1, 0-1:H-1, H:NV-H], maxEigenvaluesXXLow[0:1, 0:H, H:NV-H])
    lambdaRightXLow = max(maxEigenvaluesXXLow[0:1, 0:H, H:NV-H], maxEigenvaluesXXLow[0:1, 0+1:H+1, H:NV-H])
    lambdaBottomXLow = max(maxEigenvaluesYXLow[0:1, 0:H, H-1:NV-H-1], maxEigenvaluesYXLow[0:1, 0:H, H:NV-H])
    lambdaTopXLow = max(maxEigenvaluesYXLow[0:1, 0:H, H:NV-H], maxEigenvaluesYXLow[0:1, 0:H, H+1:NV-H+1])

    fluxLeftXLow = -1.0 * lambdaLeftXLow * (input[0:NumberOfUnknowns, 0:H, H:NV-H] - input[0:NumberOfUnknowns, 0-1:H-1, H:NV-H])
    fluxRightXLow = -1.0 * lambdaRightXLow * (input[0:NumberOfUnknowns, 0+1:H+1, H:NV-H] - input[0:NumberOfUnknowns, 0:H, H:NV-H])
    output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + dt / h(0) * (fluxLeftXLow - fluxRightXLow) * NV * 0.5

    fluxBottomXLow = -1.0 * lambdaBottomXLow * (input[0:NumberOfUnknowns, 0:H, H:NV-H] - input[0:NumberOfUnknowns, 0:H, H-1:NV-H-1])
    fluxTopXLow = -1.0 * lambdaTopXLow * (input[0:NumberOfUnknowns, 0:H, H+1:NV-H+1] - input[0:NumberOfUnknowns, 0:H, H:NV-H])
    output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + dt / h(1) * (fluxBottomXLow - fluxTopXLow) * NV * 0.5

    if flux:
      flux_xXLow = DataBlock([[0, NumberOfUnknowns], [0-1, H+1], [H, NV-H], [0, N]], eval("flux", input[:, 0-1:H+1, H:NV-H], x, 0))
      flux_yXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H-1, NV-H+1], [0, N]], eval("flux", input[:, 0:H, H-1:NV-H+1], x, 1))
      output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_xXLow[:, 0-1:H-1, H:NV-H] - flux_xXLow[:, 0+1:H+1, H:NV-H])
      output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_yXLow[:, 0:H, H-1:NV-H-1] - flux_yXLow[:, 0:H, H+1:NV-H+1])

    if nonconservativeProduct:
       average_xXLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H+1, H:NV-H])
       delta_xXLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:H, H:NV-H]
       nonconservative_xXLow = DataBlock([[0, NumberOfUnknowns], [0, H+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_xXLow, delta_xXLow, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_xXLow[:, 0:H, H:NV-H] - nonconservative_xXLow[:, 0+1:H+1, H:NV-H])

       average_yXLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H+1])
       delta_yXLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H-1:NV-H]
       nonconservative_yXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_yXLow, delta_yXLow, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_yXLow[:, 0:H, H:NV-H] - nonconservative_yXLow[:, 0:H, H+1:NV-H+1])

    # Shell part XHigh: [NV-H,NV) x [H,NV-H)
    output[:, NV-H:NV, H:NV-H] = input[:, NV-H:NV, H:NV-H]

    if source:
       sourceXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H, NV-H], [0, N]], eval("sourceTerm", input[:, NV-H:NV, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + dt * sourceXHigh

    maxEigenvaluesXXHigh = DataBlock([[0, 1], [NV-H-1, NV+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, NV-H-1:NV+1, H:NV-H], x, 0))
    maxEigenvaluesYXHigh = DataBlock([[0, 1], [NV-H, NV], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, NV-H:NV, H-1:NV-H+1], x, 1))

    lambdaLeftXHigh = max(maxEigenvaluesXXHigh[0:1, NV-H-1:NV-1, H:NV-H], maxEigenvaluesXXHigh[0:1, NV-H:NV, H:NV-H])
    lambdaRightXHigh = max(maxEigenvaluesXXHigh[0:1, NV-H:NV, H:NV-H], maxEigenvaluesXXHigh[0:1, NV-H+1:NV+1, H:NV-H])
    lambdaBottomXHigh = max(maxEigenvaluesYXHigh[0:1, NV-H:NV, H-1:NV-H-1], maxEigenvaluesYXHigh[0:1, NV-H:NV, H:NV-H])
    lambdaTopXHigh = max(maxEigenvaluesYXHigh[0:1, NV-H:NV, H:NV-H], maxEigenvaluesYXHigh[0:1, NV-H:NV, H+1:NV-H+1])

    fluxLeftXHigh = -1.0 * lambdaLeftXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns, NV-H-1:NV-1, H:NV-H])
    fluxRightXHigh = -1.0 * lambdaRightXHigh * (input[0:NumberOfUnknowns, NV-H+1:NV+1, H:NV-H] - input[0:NumberOfUnknowns, NV-H:NV, H:NV-H])
    output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + dt / h(0) * (fluxLeftXHigh - fluxRightXHigh) * NV * 0.5

    fluxBottomXHigh = -1.0 * lambdaBottomXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns, NV-H:NV, H-1:NV-H-1])
    fluxTopXHigh = -1.0 * lambdaTopXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H+1:NV-H+1] - input[0:NumberOfUnknowns, NV-H:NV, H:NV-H])
    output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + dt / h(1) * (fluxBottomXHigh - fluxTopXHigh) * NV * 0.5

    if flux:
      flux_xXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H-1, NV+1], [H, NV-H], [0, N]], eval("flux", input[:, NV-H-1:NV+1, H:NV-H], x, 0))
      flux_yXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H-1, NV-H+1], [0, N]], eval("flux", input[:, NV-H:NV, H-1:NV-H+1], x, 1))
      output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_xXHigh[:, NV-H-1:NV-1, H:NV-H] - flux_xXHigh[:, NV-H+1:NV+1, H:NV-H])
      output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_yXHigh[:, NV-H:NV, H-1:NV-H-1] - flux_yXHigh[:, NV-H:NV, H+1:NV-H+1])

    if nonconservativeProduct:
       average_xXHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H-1:NV, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV+1, H:NV-H])
       delta_xXHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H-1:NV, H:NV-H]
       nonconservative_xXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_xXHigh, delta_xXHigh, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_xXHigh[:, NV-H:NV, H:NV-H] - nonconservative_xXHigh[:, NV-H+1:NV+1, H:NV-H])

       average_yXHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H+1])
       delta_yXHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H-1:NV-H]
       nonconservative_yXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_yXHigh, delta_yXHigh, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_yXHigh[:, NV-H:NV, H:NV-H] - nonconservative_yXHigh[:, NV-H:NV, H+1:NV-H+1])

    if maxEigenvalue:
      maxEigenvaluesX2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output[:, 0:NV, 0:NV], x, 0))
      maxEigenvaluesY2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output[:, 0:NV, 0:NV], x, 1))
      maxEigenvaluesPerVolume = max(maxEigenvaluesX2, maxEigenvaluesY2)
      maxEigenvalues = max(maxEigenvaluesPerVolume)


## Rusanov update of the core of a 3d patch.
def timeStepWithRusanov3DOnCore(patchData: "::exahype2::CellData<double, double>&") -> "void":
    N = Integer("patchData.numberOfCells")
    t = DataBlock([[0, N]], "patchData.t", "double")
    dt = DataBlock([[0, N]], "patchData.dt", "double")
    h = DataBlock([[0, N]], "patchData.cellSize", "tarch::la::Vector<3, double>")
    x = DataBlock([[0, N]], "patchData.cellCentre", "tarch::la::Vector<3, double>")
    maxEigenvalues = DataBlock([[0, N]], "patchData.maxEigenvalue", "double")

    input = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [-H, NV + H], [-H, NV + H], [-H, NV + H], [0, N]], "patchData.QIn")
    output = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [0, NV], [0, NV], [0, NV], [0, N]], "patchData.QOut")

    output[:, H:NV-H, H:NV-H, H:NV-H] = input[:, H:NV-H, H:NV-H, H:NV-H]

    if source:
       source = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H, NV-H], [H, NV-H], [0, N]], eval("sourceTerm", input[:, H:NV-H, H:NV-H, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + dt * source

    maxEigenvaluesX = DataBlock([[0, 1], [H-1, NV-H+1], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, H-1:NV-H+1, H:NV-H, H:NV-H], x, 0))
    maxEigenvaluesY = DataBlock([[0, 1], [H, NV-H], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, H:NV-H, H-1:NV-H+1, H:NV-H], x, 1))
    maxEigenvaluesZ = DataBlock([[0, 1], [H, NV-H], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, H:NV-H, H:NV-H, H-1:NV-H+1], x, 2))

    lambdaLeft = max(maxEigenvaluesX[0:1, H-1:NV-H-1, H:NV-H, H:NV-H], maxEigenvaluesX[0:1, H:NV-H, H:NV-H, H:NV-H])
    lambdaRight = max(maxEigenvaluesX[0:1, H:NV-H, H:NV-H, H:NV-H], maxEigenvaluesX[0:1, H+1:NV-H+1, H:NV-H, H:NV-H])
    lambdaBottom = max(maxEigenvaluesY[0:1, H:NV-H, H-1:NV-H-1, H:NV-H], maxEigenvaluesY[0:1, H:NV-H, H:NV-H, H:NV-H])
    lambdaTop = max(maxEigenvaluesY[0:1, H:NV-H, H:NV-H, H:NV-H], maxEigenvaluesY[0:1, H:NV-H, H+1:NV-H+1, H:NV-H])
    lambdaNear = max(maxEigenvaluesZ[0:1, H:NV-H, H:NV-H, H-1:NV-H-1], maxEigenvaluesZ[0:1, H:NV-H, H:NV-H, H:NV-H])
    lambdaFar = max(maxEigenvaluesZ[0:1, H:NV-H, H:NV-H, H:NV-H], maxEigenvaluesZ[0:1, H:NV-H, H:NV-H, H+1:NV-H+1])

    fluxLeft = -1.0 * lambdaLeft * (input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, H-1:NV-H-1, H:NV-H, H:NV-H])
    fluxRight = -1.0 * lambdaRight * (input[0:NumberOfUnknowns, H+1:NV-H+1, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + dt / h(0) * (fluxLeft - fluxRight) * NV * 0.5

    fluxBottom = -1.0 * lambdaBottom * (input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, H:NV-H, H-1:NV-H-1, H:NV-H])
    fluxTop = -1.0 * lambdaTop * (input[0:NumberOfUnknowns, H:NV-H, H+1:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + dt / h(1) * (fluxBottom - fluxTop) * NV * 0.5

    fluxNear = -1.0 * lambdaNear * (input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H-1:NV-H-1])
    fluxFar = -1.0 * lambdaFar * (input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H+1:NV-H+1] - input[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + dt / h(2) * (fluxNear - fluxFar) * NV * 0.5

    if flux:
      flux_x = DataBlock([[0, NumberOfUnknowns], [H-1, NV-H+1], [H, NV-H], [H, NV-H], [0, N]], eval("flux", input[:, H-1:NV-H+1, H:NV-H, H:NV-H], x, 0))
      flux_y = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("flux", input[:, H:NV-H, H-1:NV-H+1, H:NV-H], x, 1))
      flux_z = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("flux", input[:, H:NV-H, H:NV-H, H-1:NV-H+1], x, 2))
      output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_x[:, H-1:NV-H-1, H:NV-H, H:NV-H] - flux_x[:, H+1:NV-H+1, H:NV-H, H:NV-H])
      output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_y[:, H:NV-H, H-1:NV-H-1, H:NV-H] - flux_y[:, H:NV-H, H+1:NV-H+1, H:NV-H])
      output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(2) * (flux_z[:, H:NV-H, H:NV-H, H-1:NV-H-1] - flux_z[:, H:NV-H, H:NV-H, H+1:NV-H+1])

    if nonconservativeProduct:
       average_x = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H-1:NV-H, H:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H+1, H:NV-H, H:NV-H])
       delta_x = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H+1, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H-1:NV-H, H:NV-H, H:NV-H]
       nonconservative_x = DataBlock([[0, NumberOfUnknowns], [H, NV-H+1], [H, NV-H], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_x, delta_x, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_x[:, H:NV-H, H:NV-H, H:NV-H] - nonconservative_x[:, H+1:NV-H+1, H:NV-H, H:NV-H])

       average_y = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H-1:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H+1, H:NV-H])
       delta_y = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H-1:NV-H, H:NV-H]
       nonconservative_y = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H, NV-H+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_y, delta_y, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_y[:, H:NV-H, H:NV-H, H:NV-H] - nonconservative_y[:, H:NV-H, H+1:NV-H+1, H:NV-H])

       average_z = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H, H:NV-H+1])
       delta_z = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, H:NV-H, H:NV-H, H-1:NV-H]
       nonconservative_z = DataBlock([[0, NumberOfUnknowns], [H, NV-H], [H, NV-H], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_z, delta_z, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, H:NV-H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(2) * (-nonconservative_z[:, H:NV-H, H:NV-H, H:NV-H] - nonconservative_z[:, H:NV-H, H:NV-H, H+1:NV-H+1])

    if maxEigenvalue:
      maxEigenvaluesX2 = DataBlock([[0, 1], [H, NV-H], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", output[:, H:NV-H, H:NV-H, H:NV-H], x, 0))
      maxEigenvaluesY2 = DataBlock([[0, 1], [H, NV-H], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", output[:, H:NV-H, H:NV-H, H:NV-H], x, 1))
      maxEigenvaluesZ2 = DataBlock([[0, 1], [H, NV-H], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", output[:, H:NV-H, H:NV-H, H:NV-H], x, 2))
      maxEigenvaluesXY = max(maxEigenvaluesX2, maxEigenvaluesY2)
      maxEigenvaluesPerVolume = max(maxEigenvaluesXY, maxEigenvaluesZ2)
      maxEigenvalues = max(maxEigenvaluesPerVolume)


## Rusanov update of the shell of a 3d patch. Run after timeStepWithRusanov3DOnCore().
def timeStepWithRusanov3DOnShell(patchData: "::exahype2::CellData<double, double>&") -> "void":
    N = Integer("patchData.numberOfCells")
    t = DataBlock([[0, N]], "patchData.t", "double")
    dt = DataBlock([[0, N]], "patchData.dt", "double")
    h = DataBlock([[0, N]], "patchData.cellSize", "tarch::la::Vector<3, double>")
    x = DataBlock([[0, N]], "patchData.cellCentre", "tarch::la::Vector<3, double>")
    maxEigenvalues = DataBlock([[0, N]], "patchData.maxEigenvalue", "double")

    input = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [-H, NV + H], [-H, NV + H], [-H, NV + H], [0, N]], "patchData.QIn")
    output = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [0, NV], [0, NV], [0, NV], [0, N]], "patchData.QOut")

    # Shell part ZLow: [0,NV) x [0,NV) x [0,H)
    output[:, 0:NV, 0:NV, 0:H] = input[:, 0:NV, 0:NV, 0:H]

    if source:
       sourceZLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [0, H], [0, N]], eval("sourceTerm", input[:, 0:NV, 0:NV, 0:H], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + dt * sourceZLow

    maxEigenvaluesXZLow = DataBlock([[0, 1], [0-1, NV+1], [0, NV], [0, H], [0, N]], eval("maxEigenvalue", input[:, 0-1:NV+1, 0:NV, 0:H], x, 0))
    maxEigenvaluesYZLow = DataBlock([[0, 1], [0, NV], [0-1, NV+1], [0, H], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0-1:NV+1, 0:H], x, 1))
    maxEigenvaluesZZLow = DataBlock([[0, 1], [0, NV], [0, NV], [0-1, H+1], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0:NV, 0-1:H+1], x, 2))

    lambdaLeftZLow = max(maxEigenvaluesXZLow[0:1, 0-1:NV-1, 0:NV, 0:H], maxEigenvaluesXZLow[0:1, 0:NV, 0:NV, 0:H])
    lambdaRightZLow = max(maxEigenvaluesXZLow[0:1, 0:NV, 0:NV, 0:H], maxEigenvaluesXZLow[0:1, 0+1:NV+1, 0:NV, 0:H])
    lambdaBottomZLow = max(maxEigenvaluesYZLow[0:1, 0:NV, 0-1:NV-1, 0:H], maxEigenvaluesYZLow[0:1, 0:NV, 0:NV, 0:H])
    lambdaTopZLow = max(maxEigenvaluesYZLow[0:1, 0:NV, 0:NV, 0:H], maxEigenvaluesYZLow[0:1, 0:NV, 0+1:NV+1, 0:H])
    lambdaNearZLow = max(maxEigenvaluesZZLow[0:1, 0:NV, 0:NV, 0-1:H-1], maxEigenvaluesZZLow[0:1, 0:NV, 0:NV, 0:H])
    lambdaFarZLow = max(maxEigenvaluesZZLow[0:1, 0:NV, 0:NV, 0:H], maxEigenvaluesZZLow[0:1, 0:NV, 0:NV, 0+1:H+1])

    fluxLeftZLow = -1.0 * lambdaLeftZLow * (input[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] - input[0:NumberOfUnknowns, 0-1:NV-1, 0:NV, 0:H])
    fluxRightZLow = -1.0 * lambdaRightZLow * (input[0:NumberOfUnknowns, 0+1:NV+1, 0:NV, 0:H] - input[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H])
    output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + dt / h(0) * (fluxLeftZLow - fluxRightZLow) * NV * 0.5

    fluxBottomZLow = -1.0 * lambdaBottomZLow * (input[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] - input[0:NumberOfUnknowns, 0:NV, 0-1:NV-1, 0:H])
    fluxTopZLow = -1.0 * lambdaTopZLow * (input[0:NumberOfUnknowns, 0:NV, 0+1:NV+1, 0:H] - input[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H])
    output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + dt / h(1) * (fluxBottomZLow - fluxTopZLow) * NV * 0.5

    fluxNearZLow = -1.0 * lambdaNearZLow * (input[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] - input[0:NumberOfUnknowns, 0:NV, 0:NV, 0-1:H-1])
    fluxFarZLow = -1.0 * lambdaFarZLow * (input[0:NumberOfUnknowns, 0:NV, 0:NV, 0+1:H+1] - input[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H])
    output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + dt / h(2) * (fluxNearZLow - fluxFarZLow) * NV * 0.5

    if flux:
      flux_xZLow = DataBlock([[0, NumberOfUnknowns], [0-1, NV+1], [0, NV], [0, H], [0, N]], eval("flux", input[:, 0-1:NV+1, 0:NV, 0:H], x, 0))
      flux_yZLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0-1, NV+1], [0, H], [0, N]], eval("flux", input[:, 0:NV, 0-1:NV+1, 0:H], x, 1))
      flux_zZLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [0-1, H+1], [0, N]], eval("flux", input[:, 0:NV, 0:NV, 0-1:H+1], x, 2))
      output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + 0.5 * NV * dt / h(0) * (flux_xZLow[:, 0-1:NV-1, 0:NV, 0:H] - flux_xZLow[:, 0+1:NV+1, 0:NV, 0:H])
      output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + 0.5 * NV * dt / h(1) * (flux_yZLow[:, 0:NV, 0-1:NV-1, 0:H] - flux_yZLow[:, 0:NV, 0+1:NV+1, 0:H])
      output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + 0.5 * NV * dt / h(2) * (flux_zZLow[:, 0:NV, 0:NV, 0-1:H-1] - flux_zZLow[:, 0:NV, 0:NV, 0+1:H+1])

    if nonconservativeProduct:
       average_xZLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:NV, 0:H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:NV, 0:H])
       delta_xZLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:NV, 0:H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:NV, 0:H]
       nonconservative_xZLow = DataBlock([[0, NumberOfUnknowns], [0, NV+1], [0, NV], [0, H], [0, N]], eval("nonconservativeProduct", average_xZLow, delta_xZLow, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + 0.5 * NV * dt / h(0) * (-nonconservative_xZLow[:, 0:NV, 0:NV, 0:H] - nonconservative_xZLow[:, 0+1:NV+1, 0:NV, 0:H])

       average_yZLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:NV, 0:H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV+1, 0:H])
       delta_yZLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV+1, 0:H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:NV, 0:H]
       nonconservative_yZLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV+1], [0, H], [0, N]], eval("nonconservativeProduct", average_yZLow, delta_yZLow, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + 0.5 * NV * dt / h(1) * (-nonconservative_yZLow[:, 0:NV, 0:NV, 0:H] - nonconservative_yZLow[:, 0:NV, 0+1:NV+1, 0:H])

       average_zZLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, 0-1:H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, 0:H+1])
       delta_zZLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, 0:H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, 0-1:H]
       nonconservative_zZLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [0, H+1], [0, N]], eval("nonconservativeProduct", average_zZLow, delta_zZLow, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] = output[0:NumberOfUnknowns, 0:NV, 0:NV, 0:H] + 0.5 * NV * dt / h(2) * (-nonconservative_zZLow[:, 0:NV, 0:NV, 0:H] - nonconservative_zZLow[:, 0:NV, 0:NV, 0+1:H+1])

    # Shell part ZHigh: [0,NV) x [0,NV) x [NV-H,NV)
    output[:, 0:NV, 0:NV, NV-H:NV] = input[:, 0:NV, 0:NV, NV-H:NV]

    if source:
       sourceZHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [NV-H, NV], [0, N]], eval("sourceTerm", input[:, 0:NV, 0:NV, NV-H:NV], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + dt * sourceZHigh

    maxEigenvaluesXZHigh = DataBlock([[0, 1], [0-1, NV+1], [0, NV], [NV-H, NV], [0, N]], eval("maxEigenvalue", input[:, 0-1:NV+1, 0:NV, NV-H:NV], x, 0))
    maxEigenvaluesYZHigh = DataBlock([[0, 1], [0, NV], [0-1, NV+1], [NV-H, NV], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0-1:NV+1, NV-H:NV], x, 1))
    maxEigenvaluesZZHigh = DataBlock([[0, 1], [0, NV], [0, NV], [NV-H-1, NV+1], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0:NV, NV-H-1:NV+1], x, 2))

    lambdaLeftZHigh = max(maxEigenvaluesXZHigh[0:1, 0-1:NV-1, 0:NV, NV-H:NV], maxEigenvaluesXZHigh[0:1, 0:NV, 0:NV, NV-H:NV])
    lambdaRightZHigh = max(maxEigenvaluesXZHigh[0:1, 0:NV, 0:NV, NV-H:NV], maxEigenvaluesXZHigh[0:1, 0+1:NV+1, 0:NV, NV-H:NV])
    lambdaBottomZHigh = max(maxEigenvaluesYZHigh[0:1, 0:NV, 0-1:NV-1, NV-H:NV], maxEigenvaluesYZHigh[0:1, 0:NV, 0:NV, NV-H:NV])
    lambdaTopZHigh = max(maxEigenvaluesYZHigh[0:1, 0:NV, 0:NV, NV-H:NV], maxEigenvaluesYZHigh[0:1, 0:NV, 0+1:NV+1, NV-H:NV])
    lambdaNearZHigh = max(maxEigenvaluesZZHigh[0:1, 0:NV, 0:NV, NV-H-1:NV-1], maxEigenvaluesZZHigh[0:1, 0:NV, 0:NV, NV-H:NV])
    lambdaFarZHigh = max(maxEigenvaluesZZHigh[0:1, 0:NV, 0:NV, NV-H:NV], maxEigenvaluesZZHigh[0:1, 0:NV, 0:NV, NV-H+1:NV+1])

    fluxLeftZHigh = -1.0 * lambdaLeftZHigh * (input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns, 0-1:NV-1, 0:NV, NV-H:NV])
    fluxRightZHigh = -1.0 * lambdaRightZHigh * (input[0:NumberOfUnknowns, 0+1:NV+1, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV])
    output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + dt / h(0) * (fluxLeftZHigh - fluxRightZHigh) * NV * 0.5

    fluxBottomZHigh = -1.0 * lambdaBottomZHigh * (input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns, 0:NV, 0-1:NV-1, NV-H:NV])
    fluxTopZHigh = -1.0 * lambdaTopZHigh * (input[0:NumberOfUnknowns, 0:NV, 0+1:NV+1, NV-H:NV] - input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV])
    output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + dt / h(1) * (fluxBottomZHigh - fluxTopZHigh) * NV * 0.5

    fluxNearZHigh = -1.0 * lambdaNearZHigh * (input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H-1:NV-1])
    fluxFarZHigh = -1.0 * lambdaFarZHigh * (input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H+1:NV+1] - input[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV])
    output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + dt / h(2) * (fluxNearZHigh - fluxFarZHigh) * NV * 0.5

    if flux:
      flux_xZHigh = DataBlock([[0, NumberOfUnknowns], [0-1, NV+1], [0, NV], [NV-H, NV], [0, N]], eval("flux", input[:, 0-1:NV+1, 0:NV, NV-H:NV], x, 0))
      flux_yZHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [0-1, NV+1], [NV-H, NV], [0, N]], eval("flux", input[:, 0:NV, 0-1:NV+1, NV-H:NV], x, 1))
      flux_zZHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [NV-H-1, NV+1], [0, N]], eval("flux", input[:, 0:NV, 0:NV, NV-H-1:NV+1], x, 2))
      output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(0) * (flux_xZHigh[:, 0-1:NV-1, 0:NV, NV-H:NV] - flux_xZHigh[:, 0+1:NV+1, 0:NV, NV-H:NV])
      output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(1) * (flux_yZHigh[:, 0:NV, 0-1:NV-1, NV-H:NV] - flux_yZHigh[:, 0:NV, 0+1:NV+1, NV-H:NV])
      output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(2) * (flux_zZHigh[:, 0:NV, 0:NV, NV-H-1:NV-1] - flux_zZHigh[:, 0:NV, 0:NV, NV-H+1:NV+1])

    if nonconservativeProduct:
       average_xZHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:NV, NV-H:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:NV, NV-H:NV])
       delta_xZHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:NV, NV-H:NV] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:NV, NV-H:NV]
       nonconservative_xZHigh = DataBlock([[0, NumberOfUnknowns], [0, NV+1], [0, NV], [NV-H, NV], [0, N]], eval("nonconservativeProduct", average_xZHigh, delta_xZHigh, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(0) * (-nonconservative_xZHigh[:, 0:NV, 0:NV, NV-H:NV] - nonconservative_xZHigh[:, 0+1:NV+1, 0:NV, NV-H:NV])

       average_yZHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:NV, NV-H:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV+1, NV-H:NV])
       delta_yZHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV+1, NV-H:NV] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:NV, NV-H:NV]
       nonconservative_yZHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV+1], [NV-H, NV], [0, N]], eval("nonconservativeProduct", average_yZHigh, delta_yZHigh, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(1) * (-nonconservative_yZHigh[:, 0:NV, 0:NV, NV-H:NV] - nonconservative_yZHigh[:, 0:NV, 0+1:NV+1, NV-H:NV])

       average_zZHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, NV-H-1:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, NV-H:NV+1])
       delta_zZHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, NV-H:NV+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV, NV-H-1:NV]
       nonconservative_zZHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [NV-H, NV+1], [0, N]], eval("nonconservativeProduct", average_zZHigh, delta_zZHigh, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] = output[0:NumberOfUnknowns, 0:NV, 0:NV, NV-H:NV] + 0.5 * NV * dt / h(2) * (-nonconservative_zZHigh[:, 0:NV, 0:NV, NV-H:NV] - nonconservative_zZHigh[:, 0:NV, 0:NV, NV-H+1:NV+1])

    # Shell part YLow: [0,NV) x [0,H) x [H,NV-H)
    output[:, 0:NV, 0:H, H:NV-H] = input[:, 0:NV, 0:H, H:NV-H]

    if source:
       sourceYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, H], [H, NV-H], [0, N]], eval("sourceTerm", input[:, 0:NV, 0:H, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + dt * sourceYLow

    maxEigenvaluesXYLow = DataBlock([[0, 1], [0-1, NV+1], [0, H], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0-1:NV+1, 0:H, H:NV-H], x, 0))
    maxEigenvaluesYYLow = DataBlock([[0, 1], [0, NV], [0-1, H+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0-1:H+1, H:NV-H], x, 1))
    maxEigenvaluesZYLow = DataBlock([[0, 1], [0, NV], [0, H], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, 0:NV, 0:H, H-1:NV-H+1], x, 2))

    lambdaLeftYLow = max(maxEigenvaluesXYLow[0:1, 0-1:NV-1, 0:H, H:NV-H], maxEigenvaluesXYLow[0:1, 0:NV, 0:H, H:NV-H])
    lambdaRightYLow = max(maxEigenvaluesXYLow[0:1, 0:NV, 0:H, H:NV-H], maxEigenvaluesXYLow[0:1, 0+1:NV+1, 0:H, H:NV-H])
    lambdaBottomYLow = max(maxEigenvaluesYYLow[0:1, 0:NV, 0-1:H-1, H:NV-H], maxEigenvaluesYYLow[0:1, 0:NV, 0:H, H:NV-H])
    lambdaTopYLow = max(maxEigenvaluesYYLow[0:1, 0:NV, 0:H, H:NV-H], maxEigenvaluesYYLow[0:1, 0:NV, 0+1:H+1, H:NV-H])
    lambdaNearYLow = max(maxEigenvaluesZYLow[0:1, 0:NV, 0:H, H-1:NV-H-1], maxEigenvaluesZYLow[0:1, 0:NV, 0:H, H:NV-H])
    lambdaFarYLow = max(maxEigenvaluesZYLow[0:1, 0:NV, 0:H, H:NV-H], maxEigenvaluesZYLow[0:1, 0:NV, 0:H, H+1:NV-H+1])

    fluxLeftYLow = -1.0 * lambdaLeftYLow * (input[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] - input[0:NumberOfUnknowns, 0-1:NV-1, 0:H, H:NV-H])
    fluxRightYLow = -1.0 * lambdaRightYLow * (input[0:NumberOfUnknowns, 0+1:NV+1, 0:H, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H])
    output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + dt / h(0) * (fluxLeftYLow - fluxRightYLow) * NV * 0.5

    fluxBottomYLow = -1.0 * lambdaBottomYLow * (input[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, 0-1:H-1, H:NV-H])
    fluxTopYLow = -1.0 * lambdaTopYLow * (input[0:NumberOfUnknowns, 0:NV, 0+1:H+1, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H])
    output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + dt / h(1) * (fluxBottomYLow - fluxTopYLow) * NV * 0.5

    fluxNearYLow = -1.0 * lambdaNearYLow * (input[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, 0:H, H-1:NV-H-1])
    fluxFarYLow = -1.0 * lambdaFarYLow * (input[0:NumberOfUnknowns, 0:NV, 0:H, H+1:NV-H+1] - input[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H])
    output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + dt / h(2) * (fluxNearYLow - fluxFarYLow) * NV * 0.5

    if flux:
      flux_xYLow = DataBlock([[0, NumberOfUnknowns], [0-1, NV+1], [0, H], [H, NV-H], [0, N]], eval("flux", input[:, 0-1:NV+1, 0:H, H:NV-H], x, 0))
      flux_yYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0-1, H+1], [H, NV-H], [0, N]], eval("flux", input[:, 0:NV, 0-1:H+1, H:NV-H], x, 1))
      flux_zYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, H], [H-1, NV-H+1], [0, N]], eval("flux", input[:, 0:NV, 0:H, H-1:NV-H+1], x, 2))
      output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_xYLow[:, 0-1:NV-1, 0:H, H:NV-H] - flux_xYLow[:, 0+1:NV+1, 0:H, H:NV-H])
      output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_yYLow[:, 0:NV, 0-1:H-1, H:NV-H] - flux_yYLow[:, 0:NV, 0+1:H+1, H:NV-H])
      output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + 0.5 * NV * dt / h(2) * (flux_zYLow[:, 0:NV, 0:H, H-1:NV-H-1] - flux_zYLow[:, 0:NV, 0:H, H+1:NV-H+1])

    if nonconservativeProduct:
       average_xYLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:H, H:NV-H])
       delta_xYLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, 0:H, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, 0:H, H:NV-H]
       nonconservative_xYLow = DataBlock([[0, NumberOfUnknowns], [0, NV+1], [0, H], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_xYLow, delta_xYLow, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_xYLow[:, 0:NV, 0:H, H:NV-H] - nonconservative_xYLow[:, 0+1:NV+1, 0:H, H:NV-H])

       average_yYLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H+1, H:NV-H])
       delta_yYLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0-1:H, H:NV-H]
       nonconservative_yYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, H+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_yYLow, delta_yYLow, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_yYLow[:, 0:NV, 0:H, H:NV-H] - nonconservative_yYLow[:, 0:NV, 0+1:H+1, H:NV-H])

       average_zYLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H, H:NV-H+1])
       delta_zYLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:H, H-1:NV-H]
       nonconservative_zYLow = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, H], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_zYLow, delta_zYLow, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, 0:H, H:NV-H] + 0.5 * NV * dt / h(2) * (-nonconservative_zYLow[:, 0:NV, 0:H, H:NV-H] - nonconservative_zYLow[:, 0:NV, 0:H, H+1:NV-H+1])

    # Shell part YHigh: [0,NV) x [NV-H,NV) x [H,NV-H)
    output[:, 0:NV, NV-H:NV, H:NV-H] = input[:, 0:NV, NV-H:NV, H:NV-H]

    if source:
       sourceYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H, NV], [H, NV-H], [0, N]], eval("sourceTerm", input[:, 0:NV, NV-H:NV, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + dt * sourceYHigh

    maxEigenvaluesXYHigh = DataBlock([[0, 1], [0-1, NV+1], [NV-H, NV], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0-1:NV+1, NV-H:NV, H:NV-H], x, 0))
    maxEigenvaluesYYHigh = DataBlock([[0, 1], [0, NV], [NV-H-1, NV+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0:NV, NV-H-1:NV+1, H:NV-H], x, 1))
    maxEigenvaluesZYHigh = DataBlock([[0, 1], [0, NV], [NV-H, NV], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, 0:NV, NV-H:NV, H-1:NV-H+1], x, 2))

    lambdaLeftYHigh = max(maxEigenvaluesXYHigh[0:1, 0-1:NV-1, NV-H:NV, H:NV-H], maxEigenvaluesXYHigh[0:1, 0:NV, NV-H:NV, H:NV-H])
    lambdaRightYHigh = max(maxEigenvaluesXYHigh[0:1, 0:NV, NV-H:NV, H:NV-H], maxEigenvaluesXYHigh[0:1, 0+1:NV+1, NV-H:NV, H:NV-H])
    lambdaBottomYHigh = max(maxEigenvaluesYYHigh[0:1, 0:NV, NV-H-1:NV-1, H:NV-H], maxEigenvaluesYYHigh[0:1, 0:NV, NV-H:NV, H:NV-H])
    lambdaTopYHigh = max(maxEigenvaluesYYHigh[0:1, 0:NV, NV-H:NV, H:NV-H], maxEigenvaluesYYHigh[0:1, 0:NV, NV-H+1:NV+1, H:NV-H])
    lambdaNearYHigh = max(maxEigenvaluesZYHigh[0:1, 0:NV, NV-H:NV, H-1:NV-H-1], maxEigenvaluesZYHigh[0:1, 0:NV, NV-H:NV, H:NV-H])
    lambdaFarYHigh = max(maxEigenvaluesZYHigh[0:1, 0:NV, NV-H:NV, H:NV-H], maxEigenvaluesZYHigh[0:1, 0:NV, NV-H:NV, H+1:NV-H+1])

    fluxLeftYHigh = -1.0 * lambdaLeftYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns, 0-1:NV-1, NV-H:NV, H:NV-H])
    fluxRightYHigh = -1.0 * lambdaRightYHigh * (input[0:NumberOfUnknowns, 0+1:NV+1, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H])
    output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + dt / h(0) * (fluxLeftYHigh - fluxRightYHigh) * NV * 0.5

    fluxBottomYHigh = -1.0 * lambdaBottomYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, NV-H-1:NV-1, H:NV-H])
    fluxTopYHigh = -1.0 * lambdaTopYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H+1:NV+1, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H])
    output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + dt / h(1) * (fluxBottomYHigh - fluxTopYHigh) * NV * 0.5

    fluxNearYHigh = -1.0 * lambdaNearYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H-1:NV-H-1])
    fluxFarYHigh = -1.0 * lambdaFarYHigh * (input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H+1:NV-H+1] - input[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H])
    output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + dt / h(2) * (fluxNearYHigh - fluxFarYHigh) * NV * 0.5

    if flux:
      flux_xYHigh = DataBlock([[0, NumberOfUnknowns], [0-1, NV+1], [NV-H, NV], [H, NV-H], [0, N]], eval("flux", input[:, 0-1:NV+1, NV-H:NV, H:NV-H], x, 0))
      flux_yYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H-1, NV+1], [H, NV-H], [0, N]], eval("flux", input[:, 0:NV, NV-H-1:NV+1, H:NV-H], x, 1))
      flux_zYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H, NV], [H-1, NV-H+1], [0, N]], eval("flux", input[:, 0:NV, NV-H:NV, H-1:NV-H+1], x, 2))
      output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_xYHigh[:, 0-1:NV-1, NV-H:NV, H:NV-H] - flux_xYHigh[:, 0+1:NV+1, NV-H:NV, H:NV-H])
      output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_yYHigh[:, 0:NV, NV-H-1:NV-1, H:NV-H] - flux_yYHigh[:, 0:NV, NV-H+1:NV+1, H:NV-H])
      output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(2) * (flux_zYHigh[:, 0:NV, NV-H:NV, H-1:NV-H-1] - flux_zYHigh[:, 0:NV, NV-H:NV, H+1:NV-H+1])

    if nonconservativeProduct:
       average_xYHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, NV-H:NV, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, NV-H:NV, H:NV-H])
       delta_xYHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV+1, NV-H:NV, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:NV, NV-H:NV, H:NV-H]
       nonconservative_xYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV+1], [NV-H, NV], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_xYHigh, delta_xYHigh, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_xYHigh[:, 0:NV, NV-H:NV, H:NV-H] - nonconservative_xYHigh[:, 0+1:NV+1, NV-H:NV, H:NV-H])

       average_yYHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H-1:NV, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV+1, H:NV-H])
       delta_yYHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H-1:NV, H:NV-H]
       nonconservative_yYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H, NV+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_yYHigh, delta_yYHigh, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_yYHigh[:, 0:NV, NV-H:NV, H:NV-H] - nonconservative_yYHigh[:, 0:NV, NV-H+1:NV+1, H:NV-H])

       average_zYHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV, H:NV-H+1])
       delta_zYHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, NV-H:NV, H-1:NV-H]
       nonconservative_zYHigh = DataBlock([[0, NumberOfUnknowns], [0, NV], [NV-H, NV], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_zYHigh, delta_zYHigh, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] = output[0:NumberOfUnknowns, 0:NV, NV-H:NV, H:NV-H] + 0.5 * NV * dt / h(2) * (-nonconservative_zYHigh[:, 0:NV, NV-H:NV, H:NV-H] - nonconservative_zYHigh[:, 0:NV, NV-H:NV, H+1:NV-H+1])

    # Shell part XLow: [0,H) x [H,NV-H) x [H,NV-H)
    output[:, 0:H, H:NV-H, H:NV-H] = input[:, 0:H, H:NV-H, H:NV-H]

    if source:
       sourceXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H, NV-H], [H, NV-H], [0, N]], eval("sourceTerm", input[:, 0:H, H:NV-H, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + dt * sourceXLow

    maxEigenvaluesXXLow = DataBlock([[0, 1], [0-1, H+1], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0-1:H+1, H:NV-H, H:NV-H], x, 0))
    maxEigenvaluesYXLow = DataBlock([[0, 1], [0, H], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, 0:H, H-1:NV-H+1, H:NV-H], x, 1))
    maxEigenvaluesZXLow = DataBlock([[0, 1], [0, H], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, 0:H, H:NV-H, H-1:NV-H+1], x, 2))

    lambdaLeftXLow = max(maxEigenvaluesXXLow[0:1, 0-1:H-1, H:NV-H, H:NV-H], maxEigenvaluesXXLow[0:1, 0:H, H:NV-H, H:NV-H])
    lambdaRightXLow = max(maxEigenvaluesXXLow[0:1, 0:H, H:NV-H, H:NV-H], maxEigenvaluesXXLow[0:1, 0+1:H+1, H:NV-H, H:NV-H])
    lambdaBottomXLow = max(maxEigenvaluesYXLow[0:1, 0:H, H-1:NV-H-1, H:NV-H], maxEigenvaluesYXLow[0:1, 0:H, H:NV-H, H:NV-H])
    lambdaTopXLow = max(maxEigenvaluesYXLow[0:1, 0:H, H:NV-H, H:NV-H], maxEigenvaluesYXLow[0:1, 0:H, H+1:NV-H+1, H:NV-H])
    lambdaNearXLow = max(maxEigenvaluesZXLow[0:1, 0:H, H:NV-H, H-1:NV-H-1], maxEigenvaluesZXLow[0:1, 0:H, H:NV-H, H:NV-H])
    lambdaFarXLow = max(maxEigenvaluesZXLow[0:1, 0:H, H:NV-H, H:NV-H], maxEigenvaluesZXLow[0:1, 0:H, H:NV-H, H+1:NV-H+1])

    fluxLeftXLow = -1.0 * lambdaLeftXLow * (input[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, 0-1:H-1, H:NV-H, H:NV-H])
    fluxRightXLow = -1.0 * lambdaRightXLow * (input[0:NumberOfUnknowns, 0+1:H+1, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + dt / h(0) * (fluxLeftXLow - fluxRightXLow) * NV * 0.5

    fluxBottomXLow = -1.0 * lambdaBottomXLow * (input[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, 0:H, H-1:NV-H-1, H:NV-H])
    fluxTopXLow = -1.0 * lambdaTopXLow * (input[0:NumberOfUnknowns, 0:H, H+1:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + dt / h(1) * (fluxBottomXLow - fluxTopXLow) * NV * 0.5

    fluxNearXLow = -1.0 * lambdaNearXLow * (input[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, 0:H, H:NV-H, H-1:NV-H-1])
    fluxFarXLow = -1.0 * lambdaFarXLow * (input[0:NumberOfUnknowns, 0:H, H:NV-H, H+1:NV-H+1] - input[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + dt / h(2) * (fluxNearXLow - fluxFarXLow) * NV * 0.5

    if flux:
      flux_xXLow = DataBlock([[0, NumberOfUnknowns], [0-1, H+1], [H, NV-H], [H, NV-H], [0, N]], eval("flux", input[:, 0-1:H+1, H:NV-H, H:NV-H], x, 0))
      flux_yXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("flux", input[:, 0:H, H-1:NV-H+1, H:NV-H], x, 1))
      flux_zXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("flux", input[:, 0:H, H:NV-H, H-1:NV-H+1], x, 2))
      output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_xXLow[:, 0-1:H-1, H:NV-H, H:NV-H] - flux_xXLow[:, 0+1:H+1, H:NV-H, H:NV-H])
      output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_yXLow[:, 0:H, H-1:NV-H-1, H:NV-H] - flux_yXLow[:, 0:H, H+1:NV-H+1, H:NV-H])
      output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(2) * (flux_zXLow[:, 0:H, H:NV-H, H-1:NV-H-1] - flux_zXLow[:, 0:H, H:NV-H, H+1:NV-H+1])

    if nonconservativeProduct:
       average_xXLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:H, H:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H+1, H:NV-H, H:NV-H])
       delta_xXLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H+1, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0-1:H, H:NV-H, H:NV-H]
       nonconservative_xXLow = DataBlock([[0, NumberOfUnknowns], [0, H+1], [H, NV-H], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_xXLow, delta_xXLow, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_xXLow[:, 0:H, H:NV-H, H:NV-H] - nonconservative_xXLow[:, 0+1:H+1, H:NV-H, H:NV-H])

       average_yXLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H-1:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H+1, H:NV-H])
       delta_yXLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H-1:NV-H, H:NV-H]
       nonconservative_yXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H, NV-H+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_yXLow, delta_yXLow, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_yXLow[:, 0:H, H:NV-H, H:NV-H] - nonconservative_yXLow[:, 0:H, H+1:NV-H+1, H:NV-H])

       average_zXLow = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H, H:NV-H+1])
       delta_zXLow = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:H, H:NV-H, H-1:NV-H]
       nonconservative_zXLow = DataBlock([[0, NumberOfUnknowns], [0, H], [H, NV-H], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_zXLow, delta_zXLow, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, 0:H, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(2) * (-nonconservative_zXLow[:, 0:H, H:NV-H, H:NV-H] - nonconservative_zXLow[:, 0:H, H:NV-H, H+1:NV-H+1])

    # Shell part XHigh: [NV-H,NV) x [H,NV-H) x [H,NV-H)
    output[:, NV-H:NV, H:NV-H, H:NV-H] = input[:, NV-H:NV, H:NV-H, H:NV-H]

    if source:
       sourceXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H, NV-H], [H, NV-H], [0, N]], eval("sourceTerm", input[:, NV-H:NV, H:NV-H, H:NV-H], x, h, t, dt))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + dt * sourceXHigh

    maxEigenvaluesXXHigh = DataBlock([[0, 1], [NV-H-1, NV+1], [H, NV-H], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, NV-H-1:NV+1, H:NV-H, H:NV-H], x, 0))
    maxEigenvaluesYXHigh = DataBlock([[0, 1], [NV-H, NV], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("maxEigenvalue", input[:, NV-H:NV, H-1:NV-H+1, H:NV-H], x, 1))
    maxEigenvaluesZXHigh = DataBlock([[0, 1], [NV-H, NV], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("maxEigenvalue", input[:, NV-H:NV, H:NV-H, H-1:NV-H+1], x, 2))

    lambdaLeftXHigh = max(maxEigenvaluesXXHigh[0:1, NV-H-1:NV-1, H:NV-H, H:NV-H], maxEigenvaluesXXHigh[0:1, NV-H:NV, H:NV-H, H:NV-H])
    lambdaRightXHigh = max(maxEigenvaluesXXHigh[0:1, NV-H:NV, H:NV-H, H:NV-H], maxEigenvaluesXXHigh[0:1, NV-H+1:NV+1, H:NV-H, H:NV-H])
    lambdaBottomXHigh = max(maxEigenvaluesYXHigh[0:1, NV-H:NV, H-1:NV-H-1, H:NV-H], maxEigenvaluesYXHigh[0:1, NV-H:NV, H:NV-H, H:NV-H])
    lambdaTopXHigh = max(maxEigenvaluesYXHigh[0:1, NV-H:NV, H:NV-H, H:NV-H], maxEigenvaluesYXHigh[0:1, NV-H:NV, H+1:NV-H+1, H:NV-H])
    lambdaNearXHigh = max(maxEigenvaluesZXHigh[0:1, NV-H:NV, H:NV-H, H-1:NV-H-1], maxEigenvaluesZXHigh[0:1, NV-H:NV, H:NV-H, H:NV-H])
    lambdaFarXHigh = max(maxEigenvaluesZXHigh[0:1, NV-H:NV, H:NV-H, H:NV-H], maxEigenvaluesZXHigh[0:1, NV-H:NV, H:NV-H, H+1:NV-H+1])

    fluxLeftXHigh = -1.0 * lambdaLeftXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, NV-H-1:NV-1, H:NV-H, H:NV-H])
    fluxRightXHigh = -1.0 * lambdaRightXHigh * (input[0:NumberOfUnknowns, NV-H+1:NV+1, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + dt / h(0) * (fluxLeftXHigh - fluxRightXHigh) * NV * 0.5

    fluxBottomXHigh = -1.0 * lambdaBottomXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, NV-H:NV, H-1:NV-H-1, H:NV-H])
    fluxTopXHigh = -1.0 * lambdaTopXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H+1:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + dt / h(1) * (fluxBottomXHigh - fluxTopXHigh) * NV * 0.5

    fluxNearXHigh = -1.0 * lambdaNearXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H-1:NV-H-1])
    fluxFarXHigh = -1.0 * lambdaFarXHigh * (input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H+1:NV-H+1] - input[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H])
    output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + dt / h(2) * (fluxNearXHigh - fluxFarXHigh) * NV * 0.5

    if flux:
      flux_xXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H-1, NV+1], [H, NV-H], [H, NV-H], [0, N]], eval("flux", input[:, NV-H-1:NV+1, H:NV-H, H:NV-H], x, 0))
      flux_yXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H-1, NV-H+1], [H, NV-H], [0, N]], eval("flux", input[:, NV-H:NV, H-1:NV-H+1, H:NV-H], x, 1))
      flux_zXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H, NV-H], [H-1, NV-H+1], [0, N]], eval("flux", input[:, NV-H:NV, H:NV-H, H-1:NV-H+1], x, 2))
      output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (flux_xXHigh[:, NV-H-1:NV-1, H:NV-H, H:NV-H] - flux_xXHigh[:, NV-H+1:NV+1, H:NV-H, H:NV-H])
      output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (flux_yXHigh[:, NV-H:NV, H-1:NV-H-1, H:NV-H] - flux_yXHigh[:, NV-H:NV, H+1:NV-H+1, H:NV-H])
      output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(2) * (flux_zXHigh[:, NV-H:NV, H:NV-H, H-1:NV-H-1] - flux_zXHigh[:, NV-H:NV, H:NV-H, H+1:NV-H+1])

    if nonconservativeProduct:
       average_xXHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H-1:NV, H:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV+1, H:NV-H, H:NV-H])
       delta_xXHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV+1, H:NV-H, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H-1:NV, H:NV-H, H:NV-H]
       nonconservative_xXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV+1], [H, NV-H], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_xXHigh, delta_xXHigh, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(0) * (-nonconservative_xXHigh[:, NV-H:NV, H:NV-H, H:NV-H] - nonconservative_xXHigh[:, NV-H+1:NV+1, H:NV-H, H:NV-H])

       average_yXHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H-1:NV-H, H:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H+1, H:NV-H])
       delta_yXHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H+1, H:NV-H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H-1:NV-H, H:NV-H]
       nonconservative_yXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H, NV-H+1], [H, NV-H], [0, N]], eval("nonconservativeProduct", average_yXHigh, delta_yXHigh, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(1) * (-nonconservative_yXHigh[:, NV-H:NV, H:NV-H, H:NV-H] - nonconservative_yXHigh[:, NV-H:NV, H+1:NV-H+1, H:NV-H])

       average_zXHigh = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H, H-1:NV-H] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H, H:NV-H+1])
       delta_zXHigh = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H, H:NV-H+1] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, NV-H:NV, H:NV-H, H-1:NV-H]
       nonconservative_zXHigh = DataBlock([[0, NumberOfUnknowns], [NV-H, NV], [H, NV-H], [H, NV-H+1], [0, N]], eval("nonconservativeProduct", average_zXHigh, delta_zXHigh, x, h, t, dt, 2))
       output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] = output[0:NumberOfUnknowns, NV-H:NV, H:NV-H, H:NV-H] + 0.5 * NV * dt / h(2) * (-nonconservative_zXHigh[:, NV-H:NV, H:NV-H, H:NV-H] - nonconservative_zXHigh[:, NV-H:NV, H:NV-H, H+1:NV-H+1])

    if maxEigenvalue:
      maxEigenvaluesX2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output[:, 0:NV, 0:NV, 0:NV], x, 0))
      maxEigenvaluesY2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output[:, 0:NV, 0:NV, 0:NV], x, 1))
      maxEigenvaluesZ2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output[:, 0:NV, 0:NV, 0:NV], x, 2))
      maxEigenvaluesXY = max(maxEigenvaluesX2, maxEigenvaluesY2)
      maxEigenvaluesPerVolume = max(maxEigenvaluesXY, maxEigenvaluesZ2)
      maxEigenvalues = max(maxEigenvaluesPerVolume)


class RusanovWithOptimisticExplicitEuler(Solver.NumericalScheme):
    """!

    Finite Volumes with a Rusanov Riemann solver and optimistic explicit Euler time
    stepping, realised over a block-structured mesh. 
    
    ## Design idea
    
    The finite volume solver operates over a block-structured mesh, where each 
    cell of the mesh holds a patch of finite volumes. This solver's fundamental
    idea is that it splits up the patch into three classes of volumes. Assume
    we have a NxNxN patch of volumes, where N is the patch_size, and this patch
    is surrounded by a halo of size H. So we have in total a (N+2H)x(N+2H)x(N+2H)
    patch that feeds into the time stepping as reconstructed patch.
    We now distinguish three classes of volumes:
    
    - The halo cells
    - The shell cells, which is the outer layer of the interior volumes within 
      the NxNxN arrangement.
    - The core cells, which are the remaining volumes in the interior of the 
      patch, so effectively a (N-2H)x(N-2H)x(N-2H) patch.
      
    The sketch below shows the three classes for a 2d patch with N=10 and H=1.
    It is generated by
    src/exahype2/api/fv/rusanov-with-optimistic-explicit-euler-illustrations.py.

    @image html rusanov-with-optimistic-explicit-euler-volume-classes.png

    For the new solver, we now define two new update kernels:
    
    - timeStepWithRusanov2DOnCore which is timeStepWithRusanov2D but acts only on 
      the core of a patch.
    - timeStepWithRusanov2DOnShell which works only on the shell.
    
    The 3D variants follow equivalently.
    
      
      
      
    """

    def __init__(self, solver: Solver, patch_size: int, overlap: int = 1, cfl_constant: float = 0.9):
        """!

        The constructor memorises the attributes. However, it does not do this
        within this class, but it adds the important properties straight to the
        solver, so anybody else working against this solver knows about the
        properties introduced by the numerical scheme.

        @see @ref __create_data_structures()

        patch_size: int
          Number of Finite Volumes per axis within one patch (i.e. within
          one mesh cell).

        overlap: int
          Number of Finite Volumes the halo layer reconstructed around a
          patch - from the neighbouring patches' faces - is wide.

        cfl_constant: float
          Safety factor the admissible time step size (@see
          exahype2.api.fv.actionsets.ComputeGlobalCFLCondition,
          which this is passed straight through to) is scaled down by,
          relative to the theoretical CFL=1 stability limit
          h / (Dimensions * lambda_max). 0.9 is a
          conservative default for an explicit-Euler Finite Volume scheme;
          this codebase has no prior precedent value to match (grep for
          "cfl" outside this class finds only exaparty's unrelated SPH
          solvers, which default to 0.1).

        """
        super().__init__(solver)
        self.patch_size = patch_size
        self.overlap = overlap
        # Set in construct_output(): the generated kernels live in the project
        # namespace, as their names are solver-specific
        self.kernel_namespace = None
        self.cfl_constant = cfl_constant
        self.__create_data_structures()

    @property
    def kernel_function_name(self) -> str:
        """!
        Name of the generated time step kernel. It carries the solver name,
        as do the kernels/<SolverName>_rusanov*.h files holding it, so two
        solvers' kernels do not clash.
        """
        return self._solver.name + "_timeStepWithRusanov"

    def __create_data_structures(self):
        """!

        Create the mesh data structures (a Finite Volume patch per cell,
        plus the face-based overlap patches used to reconstruct its halo)
        this numerical scheme requires.

        Ported from the legacy exahype2.solvers.fv.FV.create_data_structures():
        one @ref peano4.api.datamodel.Patch of patch_size^d Finite Volumes
        per mesh cell (_patch), plus a halo-augmented copy of it
        (_patch_with_halo, (patch_size+2*overlap)^d, @see
        toolbox.blockstructured.api.ReconstructPatchWithHalo) and two overlap
        patches of width 2*overlap attached to the mesh faces:

        - _patch_overlap_current_time_step: the persistent halo data
          toolbox.blockstructured.api.ReconstructPatchWithHalo reads from to
          rebuild _patch_with_halo (@see construct_output(), which hands it
          _patch/_patch_overlap_current_time_step/_patch_with_halo directly
          rather than the whole solver).
        - _patch_overlap_update: the projection of the just-updated _patch
          accumulated by the current sweep (@see
          exahype2.api.fv.actionsets.ProjectPatchOntoFaces), rolled over
          into _patch_overlap_current_time_step by
          exahype2.api.fv.actionsets.RollOverUpdatedFace once the update is
          complete.
        - _max_eigenvalue: one double per cell, named after the solver
          followed by MaxEigenvalue (e.g. EulerFVSmoothMaxEigenvalue),
          which holds the maximum eigenvalue of
          the patch after its last update
          (@see exahype2.api.fv.actionsets.UpdateCell).
          exahype2.api.fv.actionsets.ComputeGlobalCFLCondition reduces these
          values into the next time step size. It is modelled via DaStGen2
          as an array of cardinality one. The array is a
          peano4.api.dastgen2.Peano4SmartPointerDoubleArray, i.e. the value
          resides on the heap.

        All five are assigned onto self._solver (the shared
        exahype2.api.Solver instance both this numerical scheme and its
        surrounding solver's Solver.GridConstruction/Solver.Plotter
        counterparts hold a reference to as self._solver), not onto self,
        since the remaining FV action sets that need them - @see
        exahype2.api.fv.actionsets.HandleBoundary,
        exahype2.api.fv.actionsets.UpdateCell,
        exahype2.api.fv.actionsets.ComputeGlobalCFLCondition,
        exahype2.api.fv.actionsets.AdaptivityCriterion - are handed that
        shared solver and look the patches up as solver._patch /
        solver._patch_overlap_*.

        Storage is hardwired to smart pointers (peano4.api.datamodel's
        PatchToDoubleArrayWithSmartPointer), matching the legacy default
        (Storage.SmartPointers for both cell and face data) - the legacy
        Storage enum's other two backends (CallStack, Heap) are not
        exposed here.

        Two pieces of the legacy method are deliberately not ported yet:
        - The cell/face label creation
          (exahype2.grid.create_cell_label()/create_face_label()) - that
          module has not been ported to the new API.
        - The load/store/compute guards
          (FV._provide_cell_data_to_compute_kernels_default_guard() and
          friends) - they are keyed off the legacy SolverState state
          machine, which the task-graph architecture has no equivalent of:
          a task only ever runs within its own phase's task graph in the
          first place, so "are we still constructing the grid" does not
          need to be re-checked at runtime the same way.

        """
        total_unknowns = self._solver.unknowns + self._solver.auxiliary_variables
        identifier = self._solver._unknown_identifier()

        self._solver._patch = peano4.api.datamodel.Patch(
            (self.patch_size, self.patch_size, self.patch_size),
            total_unknowns,
            identifier,
        )
        self._solver._patch_with_halo = peano4.api.datamodel.Patch(
            (self.patch_size+2*self.overlap, self.patch_size+2*self.overlap, self.patch_size+2*self.overlap),
            total_unknowns,
            identifier + "WithHalo",
        )
        self._solver._patch_overlap_current_time_step = peano4.api.datamodel.Patch(
            (2 * self.overlap, self.patch_size, self.patch_size),
            total_unknowns,
            identifier + "CurrentTimeStep",
        )
        self._solver._patch_overlap_update = peano4.api.datamodel.Patch(
            (2 * self.overlap, self.patch_size, self.patch_size),
            total_unknowns,
            identifier + "Update",
        )
        self._solver._max_eigenvalue = peano4.api.datamodel.DaStGen2(self._solver.name + "MaxEigenvalue")
        self._solver._max_eigenvalue.data.add_attribute(
            peano4.api.dastgen2.Peano4SmartPointerDoubleArray("maxEigenvalue", 1)
        )


    def __add_data_to_repository(self, peano_project):
        """!
        
        Consult @ref page_exahype_programmers_guide_new_solver for some information
        how this routine fits to the typical workflow when we introduce a new
        solver.
        
        """
        peano4.api.Logger().log("""
Patch data
----------
{}
Patch overlap data
------------------
{}
{}""".format( str(self._solver._patch), str(self._solver._patch_overlap_current_time_step), str(self._solver._patch_overlap_update) ) )

        peano_project.datamodel.add_cell(self._solver._patch)
        peano_project.datamodel.add_cell(self._solver._patch_with_halo)
        peano_project.datamodel.add_face(self._solver._patch_overlap_current_time_step)
        peano_project.datamodel.add_face(self._solver._patch_overlap_update)
        peano_project.datamodel.add_cell(self._solver._max_eigenvalue)

        # The task graph compiler only uses, loads and stores data where the
        # solver's grid selector holds, too (@see peano4.api.datamodel.DoF).
        for patch in (
            self._solver._patch,
            self._solver._patch_with_halo,
            self._solver._patch_overlap_current_time_step,
            self._solver._patch_overlap_update,
            self._solver._max_eigenvalue,
        ):
            patch.grid_selector = self._solver.grid_selector

        self._solver._patch.generator =peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch, "double"
        )
        self._solver._patch_with_halo.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_with_halo, "double"
        )
        self._solver._patch_overlap_current_time_step.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_overlap_current_time_step, "double"
        )
        self._solver._patch_overlap_update.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_overlap_update, "double"
        )
        
        self._solver._patch_overlap_current_time_step.generator.merge_method_definition = (
            toolbox.blockstructured.api.get_face_merge_implementation(
                self._solver._patch_overlap_current_time_step
            )
        )
        self._solver._patch_overlap_current_time_step.generator.includes += """
#include "peano4/utils/Loop.h"
"""

        # Only faces selected by the solver's grid selector hold valid data,
        # so only those are exchanged.
        self._solver._patch_overlap_current_time_step.generator.send_condition = self._solver.grid_selector.createFaceGuard()
        self._solver._patch_overlap_current_time_step.generator.receive_and_merge_condition = self._solver.grid_selector.createFaceGuard()

        self._solver._patch_overlap_update.generator.send_condition = "false"
        self._solver._patch_overlap_update.generator.receive_and_merge_condition = "false"

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         time_step):
        """!

        """
        peano4.api.Logger().log("Create time stepping task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        self.__add_data_to_repository(peano_project)

        self.kernel_namespace = peano_project.namespace
        # Construct the call first, as it validates the PDE terms' variants
        self._solver.compute_kernel_call = self.__construct_kernel_call(Target.Serial, False)
        self.__create_time_step_functions(peano_project.directory)
        
        reconstruction_task         = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Reconstruct and apply BCs", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        update_cell_task            = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Update cell", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task       = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        compute_cfl_condition_task  = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Compute global CFL condition", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        
        time_step.add_task(reconstruction_task)
        time_step.add_task(update_cell_task)
        time_step.add_task(project_on_faces_task)
        time_step.add_task(compute_cfl_condition_task)

        ## Build the reconstruction tasks befilling the patch_with_halo, which is logically temporary
        reconstruction_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        reconstruction_task.use_cell(self._solver._patch_with_halo, peano4.api.taskgraph.Task.DataAccess.WRITE)
        reconstruction_task.use_face(self._solver._patch_overlap_current_time_step, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        reconstruction_task.add_action_set(
            toolbox.blockstructured.api.ReconstructPatchWithHalo(
                patch=self._solver._patch,
                patch_overlap=self._solver._patch_overlap_current_time_step,
                reconstructed_patch=self._solver._patch_with_halo,
                guard=self._solver.grid_selector.createCellGuard(),
                add_assertions_to_halo_exchange=False,
                additional_includes=self._solver._get_default_includes() + self._solver.user_solver_includes,
            )
        )
        reconstruction_task.add_action_set( HandleBoundary(self._solver, guard=self._solver.grid_selector.createFaceGuard()) )

        ## Do the actual time step update
        update_cell_task.add_dependency(reconstruction_task)
        update_cell_task.use_cell(self._solver._patch_with_halo, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        update_cell_task.use_cell(self._solver._patch,           peano4.api.taskgraph.Task.DataAccess.WRITE)
        update_cell_task.use_cell(self._solver._max_eigenvalue,  peano4.api.taskgraph.Task.DataAccess.WRITE)
        update_cell_task.add_action_set(
            UpdateCell(
                self._solver,
                guard=self._solver.grid_selector.createCellGuard(),
            )
        )

        ## Reduce the cells' maximum eigenvalues into the next time step size.
        ## Runs in parallel to the projection onto the faces
        compute_cfl_condition_task.add_dependency(update_cell_task)
        compute_cfl_condition_task.use_cell(self._solver._max_eigenvalue, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        compute_cfl_condition_task.add_action_set(
            ComputeGlobalCFLCondition(
                self._solver,
                self.cfl_constant,
                self.patch_size,
                guard=self._solver.grid_selector.createCellGuard(),
            )
        )
        
        ## Next project the new data onto the faces. Also roll over the solution
        project_on_faces_task.add_dependency(update_cell_task)
        project_on_faces_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        project_on_faces_task.use_face(self._solver._patch_overlap_update, peano4.api.taskgraph.Task.DataAccess.TEMPORARY)
        project_on_faces_task.use_face(self._solver._patch_overlap_current_time_step, peano4.api.taskgraph.Task.DataAccess.WRITE)
        project_on_faces_task.add_action_set(
            ProjectPatchOntoFaces(
                self._solver._patch,
                self._solver._patch_overlap_update,
                guard=self._solver.grid_selector.createCellGuard(),
                includes=self._solver._get_default_includes() + self._solver.user_solver_includes,
            )
        )
        project_on_faces_task.add_action_set(
            RollOverUpdatedFace(
                self._solver._patch_overlap_current_time_step,
                self._solver._patch_overlap_update,
                guard=self._solver.grid_selector.createFaceGuard(),
                includes=self._solver._get_default_includes() + self._solver.user_solver_includes,
            )
        )
        
        if peano4.api.Logger().is_visualisation:
            time_step.visualise(title="Time stepping task graph for {}".format(self._solver.name))
        if peano4.api.Logger().is_text:
            time_step.log_as_text(title="Time stepping task graph for {}".format(self._solver.name))

    def __create_time_step_functions(self, output_directory):
        """!
        
        Calling this function is delayed until all PDE term information is provided.
        In this way we can avoid generating code that will not be used. The following
        information is required to translate a Python function using DSLHyPE:
        - template_parameters: the variables that are used within the Python function
          to represent external information
        - functor_parameters: the PDE terms that can be used by the solver
        - function: the Python function itself

        TODO: Currently the MLIR types are passed manually here, this should ideally
        be inferred

        """

        template_parameters = [
            dslhype.SyntaxTree.Argument("NV", dslhype.SyntaxTree.TInteger()),
            dslhype.SyntaxTree.Argument("H", dslhype.SyntaxTree.TInteger()),
            dslhype.SyntaxTree.Argument("NumberOfUnknowns", dslhype.SyntaxTree.TInteger()),
            dslhype.SyntaxTree.Argument("NumberOfAuxiliaryVariables", dslhype.SyntaxTree.TInteger())
        ]
    
        functor_parameters = [
            dslhype.SyntaxTree.Argument("flux", dslhype.SyntaxTree.TCustom("const ::exahype2::fv::rusanov::FluxFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()"),
            dslhype.SyntaxTree.Argument("nonconservativeProduct", dslhype.SyntaxTree.TCustom("const ::exahype2::fv::rusanov::NonconservativeProductFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()"),
            dslhype.SyntaxTree.Argument("sourceTerm", dslhype.SyntaxTree.TCustom("const ::exahype2::fv::rusanov::SourceFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, !llvm.ptr) -> ()"),
            dslhype.SyntaxTree.Argument("maxEigenvalue", dslhype.SyntaxTree.TCustom("const ::exahype2::fv::rusanov::MaxEigenvalueFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()")
        ]

        time_step_dsl_function_2d = exahype2.api.fv.timeStepWithRusanov2D
        time_step_dsl_function_3d = exahype2.api.fv.timeStepWithRusanov3D
        dslhype.translate_dslhype_function(
            function = time_step_dsl_function_2d,
            function_namespace = self.kernel_namespace,
            template_parameters=template_parameters,
            functor_parameters=functor_parameters,
            pde_terms=self._solver.pde_terms,
            output_file_name=self._solver.name + "_rusanov2d",
            target=dslhype.Target.Serial,
            stateless=False,
            function_name=self.kernel_function_name,
            output_directory=output_directory,
        )
        dslhype.translate_dslhype_function(
            function = time_step_dsl_function_3d,
            function_namespace = self.kernel_namespace,
            template_parameters=template_parameters,
            functor_parameters=functor_parameters,
            pde_terms=self._solver.pde_terms,
            output_file_name=self._solver.name + "_rusanov3d",
            target=dslhype.Target.Serial,
            stateless=False,
            function_name=self.kernel_function_name,
            output_directory=output_directory,
        )
        file = open(os.path.join(output_directory, "kernels", f"{self._solver.name}_rusanov.h"), "w")
        file.write(f"""#if Dimensions == 2
    #include "{self._solver.name}_rusanov2d.h"
#else
    #include "{self._solver.name}_rusanov3d.h"
#endif""")
        file.close()

    def __construct_kernel_call(self, target: Target, stateless: bool):
        """!

        This function constructs the calls to the compute kernels based on the
        application PDE terms and the version chosen.

        """
        available_terms = ["flux", "nonconservativeProduct", "source", "maxEigenvalue"]

        if stateless:
            return f"""::{"::".join(self.kernel_namespace)}::{self.kernel_function_name}<
        {self.patch_size},
        {self.overlap},
        {self._solver.unknowns},
        {self._solver.auxiliary_variables}
    >(
        patchData,
        {self._solver.name}
    );"""
        else:
            # Variants per term the kernel takes: {name: {stateless: PDETerm}}
            variants = {}
            for application_term in self._solver.pde_terms:
                if application_term.NAME in available_terms:
                    variants.setdefault(application_term.NAME, {})[application_term.stateless] = application_term

            terms_with_both_variants = [name for name in variants if len(variants[name]) == 2]
            terms_without_stateless_variant = [name for name in variants if True not in variants[name]]
            if terms_with_both_variants and terms_without_stateless_variant:
                raise Exception(
                    f"Solver {self._solver.name}: the PDE terms {terms_with_both_variants} are "
                    f"registered both with and without state. In this case, the generated code "
                    f"asks cellCanUseStatelessPDETerms() per cell before it invokes the kernel: "
                    f"if it returns true, the kernel is called with the stateless variants of all "
                    f"PDE terms, otherwise with the variants with state. However, the PDE terms "
                    f"{terms_without_stateless_variant} have no stateless variant, so a kernel "
                    f"call would mix stateless and stateful routines. Either add stateless "
                    f"variants of {terms_without_stateless_variant}, or register "
                    f"{terms_with_both_variants} only once."
                )

            def kernel_call(stateless: bool) -> str:
                pde_term_calls = []
                for term in available_terms:
                    if term not in variants:
                        continue
                    # A term registered only once is used in either case
                    application_term = variants[term].get(stateless, next(iter(variants[term].values())))
                    # Commas inside template brackets, e.g. Vector<Dimensions, double>,
                    # do not separate parameters
                    arguments = [
                        parameter.split()[-1]
                        for parameter in re.split(r",(?![^<]*>)", application_term.SIGNATURE)
                    ]
                    # Stateless terms are static and take a dummy tag (@see PDETerm)
                    if application_term.stateless:
                        arguments.append("::exahype2::Solver::Offloadable::Yes")
                    pde_term_calls.append(f"""
    [&](
        {application_term.SIGNATURE}
    )->void {{
        repositories::{self._solver.get_name_of_global_instance()}.{application_term.name}({", ".join(arguments)});
    }}""")
                return f"""::{"::".join(self.kernel_namespace)}::{self.kernel_function_name}<
    {self.patch_size},
    {self.overlap},
    {self._solver.unknowns},
    {self._solver.auxiliary_variables}
>(
    patchData,
{",\n".join(pde_term_calls)}
);"""

            if not terms_with_both_variants:
                return kernel_call(stateless=False)
            return f"""if (repositories::{self._solver.get_name_of_global_instance()}.cellCanUseStatelessPDETerms(
    patchData.cellCentre[0],
    patchData.cellSize[0],
    patchData.t[0],
    patchData.dt[0]
)) {{
{kernel_call(stateless=True)}
}} else {{
{kernel_call(stateless=False)}
}}"""