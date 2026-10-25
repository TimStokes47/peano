#include "LopsidedAdvection.h"

#include "tarch/Assertions.h"


void exahype2::fd::addLopsidedAdvection(
  const double* __restrict__                         QWithHalo,
  double* __restrict__                               rightHandSide,
  const tarch::la::Vector<Dimensions, double>&       patchSize,
  int                                                numberOfGridCellsPerPatchPerAxis,
  int                                                haloSize,
  int                                                unknowns,
  int                                                auxiliaryVariables,
  const int*                                         velocity,
  const int*                                         quantities,
  int                                                numberOfQuantities
) {
  assertion1(haloSize >= 3, haloSize);

  const int    n                  = numberOfGridCellsPerPatchPerAxis;
  const int    nWithHalo          = n + 2 * haloSize;
  const int    entriesPerGridCell = unknowns + auxiliaryVariables;
  const double weights[5]         = {-3.0, -10.0, 18.0, -6.0, 1.0};

  int stride[Dimensions];
  int numberOfGridCells = 1;
  stride[0]             = entriesPerGridCell;
  for (int d = 0; d < Dimensions; d++) {
    if (d > 0) {
      stride[d] = stride[d - 1] * nWithHalo;
    }
    numberOfGridCells *= n;
  }

  for (int gridCell = 0; gridCell < numberOfGridCells; gridCell++) {
    int input     = 0;
    int remainder = gridCell;
    for (int d = 0; d < Dimensions; d++) {
      input     += (remainder % n + haloSize) * stride[d];
      remainder /= n;
    }
    const int output = gridCell * unknowns;

    for (int d = 0; d < Dimensions; d++) {
      const double v             = QWithHalo[input + velocity[d]];
      const int    direction     = v > 0.0 ? 1 : -1;
      const double scaling       = v * direction * n / patchSize(d) / 12.0;
      for (int i = 0; i < numberOfQuantities; i++) {
        const int quantity   = quantities[i];
        double    derivative = 0.0;
        for (int shift = -1; shift <= 3; shift++) {
          derivative += weights[shift + 1] * QWithHalo[input + direction * shift * stride[d] + quantity];
        }
        rightHandSide[output + quantity] += scaling * derivative;
      }
    }
  }
}
