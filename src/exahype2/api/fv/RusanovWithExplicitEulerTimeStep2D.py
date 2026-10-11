# The following parameters shoule be set when calling Parser().parse(...)

# Parameters used to generate non stateless code (set in parser)
"""
flux: "const fluxFunctor&",                   
nonconservativeProduct: "const nonconservativeProductFunctor&", 
sourceTerm: "const sourceFunctor&",
maxEigenvalue: "const maxEigenvalueFunctor&",   
"""

# Parameter added for loop parallelisation (set in parser)
"""
loopParallelism: "peano4::utils::LoopPlacement"
"""

def timeStepWithRusanov(patchData: "::exahype2::CellData<double, double>&") -> "void":
    N = Integer("patchData.numberOfCells")
    t = DataBlock([[0, N]], "patchData.t", "double")
    dt = DataBlock([[0, N]], "patchData.dt", "double")
    h = DataBlock([[0, N]], "patchData.cellSize", "tarch::la::Vector<2, double>")
    x = DataBlock([[0, N]], "patchData.cellCentre", "tarch::la::Vector<2, double>")
    maxEigenvalues = DataBlock([[0, N]], "patchData.maxEigenvalue", "double")

    input = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [-H, NV + H], [-H, NV + H], [0, N]], "patchData.QIn")
    output = DataBlock([[0, NumberOfUnknowns + NumberOfAuxiliaryVariables], [0, NV], [0, NV], [0, N]], "patchData.QOut")
    output = input[:, 0:NV, 0:NV]

    if source:
       source = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV], [0, N]], eval("sourceTerm", input[:, 0:NV, 0:NV], x, h, t, dt))
       output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + dt * source

    maxEigenvaluesX = DataBlock([[0, 1], [-H, NV + H], [-H, NV + H], [0, N]], eval("maxEigenvalue", input, x, 0))
    maxEigenvaluesY = DataBlock([[0, 1], [-H, NV + H], [-H, NV + H], [0, N]], eval("maxEigenvalue", input, x, 1))

    lambdaLeft = max(maxEigenvaluesX[0:1, -1:NV-1, 0:NV], maxEigenvaluesX[0:1, 0:NV, 0:NV])
    lambdaRight = max(maxEigenvaluesX[0:1, 0:NV, 0:NV], maxEigenvaluesX[0:1, 1:NV + 1, 0:NV])
    lambdaBottom = max(maxEigenvaluesY[0:1, 0:NV, -1:NV-1], maxEigenvaluesY[0:1, 0:NV, 0:NV])
    lambdaTop = max(maxEigenvaluesY[0:1, 0:NV, 0:NV], maxEigenvaluesY[0:1, 0:NV, 1:NV + 1])

    fluxLeft = -1.0 * lambdaLeft * (input[0:NumberOfUnknowns, 0:NV, 0:NV] - input[0:NumberOfUnknowns, -1:NV-1, 0:NV])
    fluxRight = -1.0 * lambdaRight * (input[0:NumberOfUnknowns, 1:NV+1, 0:NV] - input[0:NumberOfUnknowns, 0:NV, 0:NV])
    output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + dt / h(0) * (fluxLeft - fluxRight) * NV * 0.5

    fluxBottom = -1.0 * lambdaBottom * (input[0:NumberOfUnknowns, 0:NV, 0:NV] - input[0:NumberOfUnknowns, 0:NV, -1:NV-1])
    fluxTop = -1.0 * lambdaTop * (input[0:NumberOfUnknowns, 0:NV, 1:NV+1] - input[0:NumberOfUnknowns, 0:NV, 0:NV])
    output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + dt / h(1) * (fluxBottom - fluxTop) * NV * 0.5

    if flux:
      flux_x = DataBlock([[0, NumberOfUnknowns], [-H, NV + H], [0, NV], [0, N]], eval("flux", input[:, -1:NV+1, 0:NV], x, 0))
      flux_y = DataBlock([[0, NumberOfUnknowns], [0, NV], [-H, NV + H], [0, N]], eval("flux", input[:, 0:NV, -1:NV+1], x, 1))
      
      output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + 0.5 * NV * dt / h(0) *  (flux_x[:, -1:NV-1, 0:NV] - flux_x[:, 1:NV+1, 0:NV])
      output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + 0.5 * NV * dt / h(1) * (flux_y[:, 0:NV, -1:NV-1] - flux_y[:, 0:NV, 1:NV+1])

    if nonconservativeProduct:
       average_x = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, -1:NV, 0:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV + 1, 0:NV])
       delta_x = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV + H, 0:NV] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, -H:NV, 0:NV]
       nonconservative_x = DataBlock([[0, NumberOfUnknowns], [0, NV + H], [0, NV], [0, N]], eval("nonconservativeProduct", average_x, delta_x, x, h, t, dt, 0))
       output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + 0.5 * NV * dt / h(0) * (-nonconservative_x[:, 0:NV, 0:NV] - nonconservative_x[:, 1:NV+1, 0:NV])
       
       average_y = 0.5 * (input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, -1:NV] + input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV + 1])
       delta_y = input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, 0:NV + H] - input[0:NumberOfUnknowns + NumberOfAuxiliaryVariables, 0:NV, -H:NV]
       nonconservative_y = DataBlock([[0, NumberOfUnknowns], [0, NV], [0, NV + H], [0, N]], eval("nonconservativeProduct", average_y, delta_y, x, h, t, dt, 1))
       output[0:NumberOfUnknowns, :, :] = output[0:NumberOfUnknowns, :, :] + 0.5 * NV * dt / h(1) * (-nonconservative_y[:, 0:NV, 0:NV] - nonconservative_y[:, 0:NV, 1:NV+1])

    if maxEigenvalue:
      maxEigenvaluesX2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output, x, 0))
      maxEigenvaluesY2 = DataBlock([[0, 1], [0, NV], [0, NV], [0, N]], eval("maxEigenvalue", output, x, 1))
      maxEigenvaluesPerVolume = max(maxEigenvaluesX2, maxEigenvaluesY2)
      maxEigenvalues = max(maxEigenvaluesPerVolume)