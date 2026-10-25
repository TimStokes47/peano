// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "SommerfeldBoundaryConditions.h"

#include "peano4/utils/Loop.h"
#include "tarch/la/ScalarOperations.h"
#include "tarch/logging/Log.h"
#include "toolbox/blockstructured/Enumeration.h"

#include <cmath>
#include <vector>


namespace {
  tarch::logging::Log _log("exahype2::blockstructured");
} // namespace


void exahype2::blockstructured::applySommerfeldBoundaryConditions(
  std::function<double(
    const double*                                Q,
    const tarch::la::Vector<Dimensions, double>& x,
    int                                          normal
  )> waveSpeed,
  std::function<void(const tarch::la::Vector<Dimensions, double>& x, double* Q)>
                                               farFieldSolution,
  const peano4::datamanagement::FaceMarker&    marker,
  const tarch::la::Vector<Dimensions, double>& domainOffset,
  const tarch::la::Vector<Dimensions, double>& domainSize,
  const std::bitset<Dimensions>&               periodicBC,
  int                                          numberOfGridCellsPerAxis,
  int                                          overlap,
  int                                          numberOfUnknowns,
  int                                          numberOfAuxiliaryVariables,
  const tarch::la::Vector<Dimensions, double>& origin,
  double                                       timeStepSize,
  const double*                                faceDataOld,
  double*                                      faceDataNew
) {
  const int  normal          = marker.getSelectedFaceNumber() % Dimensions;
  const bool isLowerBoundary = tarch::la::equals(
    marker.x()(normal),
    domainOffset(normal)
  );
  const bool isUpperBoundary = tarch::la::equals(
    marker.x()(normal),
    domainOffset(normal) + domainSize(normal)
  );

  if (periodicBC[normal] or not(isLowerBoundary or isUpperBoundary)) {
    return;
  }

  logTraceInWith2Arguments(
    "applySommerfeldBoundaryConditions(...)",
    marker.toString(),
    timeStepSize
  );

  const int unknownsPlusAuxiliaryVariables = numberOfUnknowns
                                             + numberOfAuxiliaryVariables;
  const tarch::la::Vector<Dimensions, double>
    gridCellH = marker.h() / static_cast<double>(numberOfGridCellsPerAxis);
  const bool copyGhostCells = tarch::la::equals(timeStepSize, 0.0);

  std::vector<double> farField(unknownsPlusAuxiliaryVariables);

  dfore(gridCell, numberOfGridCellsPerAxis, normal, 0) {
    for (int layer = 0; layer < overlap; layer++) {
      tarch::la::Vector<Dimensions, int>    outsideCell = gridCell;
      tarch::la::Vector<Dimensions, int>    insideCell  = gridCell;
      tarch::la::Vector<Dimensions, double> x;
      for (int d = 0; d < Dimensions; d++) {
        x(d
        ) = marker.x()(d) - 0.5 * marker.h()(d)
            + (gridCell(d) + 0.5) * gridCellH(d);
      }
      if (isLowerBoundary) {
        outsideCell(normal) = overlap - 1 - layer;
        insideCell(normal)  = overlap - layer;
        x(normal) = marker.x()(normal) - (layer + 0.5) * gridCellH(normal);
      } else {
        outsideCell(normal) = overlap + layer;
        insideCell(normal)  = overlap - 1 + layer;
        x(normal) = marker.x()(normal) + (layer + 0.5) * gridCellH(normal);
      }

      const int outsideIndex
        = toolbox::blockstructured::serialiseVoxelIndexInOverlap(
            outsideCell,
            numberOfGridCellsPerAxis,
            overlap,
            normal
          )
          * unknownsPlusAuxiliaryVariables;
      const int insideIndex
        = toolbox::blockstructured::serialiseVoxelIndexInOverlap(
            insideCell,
            numberOfGridCellsPerAxis,
            overlap,
            normal
          )
          * unknownsPlusAuxiliaryVariables;

      const double* outsideOld = faceDataOld + outsideIndex;
      const double* insideOld  = faceDataOld + insideIndex;
      const double* insideNew  = faceDataNew + insideIndex;
      double*       outsideNew = faceDataNew + outsideIndex;

      if (copyGhostCells) {
        for (int i = 0; i < unknownsPlusAuxiliaryVariables; i++) {
          outsideNew[i] = outsideOld[i];
        }
        continue;
      }

      farFieldSolution(x, farField.data());

      const double r     = tarch::la::norm2(x - origin);
      const double v     = waveSpeed(outsideOld, x, normal);
      const double sigma = v / gridCellH(normal) * r
                           / std::abs(x(normal) - origin(normal));
      const double sigmaDt = sigma * timeStepSize;
      const double damping = timeStepSize * v / r;

      for (int i = 0; i < numberOfUnknowns; i++) {
        outsideNew[i] = ((1.0 - sigmaDt) * outsideOld[i]
                         + (1.0 + sigmaDt) * insideOld[i]
                         - (1.0 - sigmaDt) * insideNew[i]
                         - damping
                             * (outsideOld[i] + insideOld[i] - 2.0 * farField[i])
                        )
                        / (1.0 + sigmaDt);
      }
      for (int i = numberOfUnknowns; i < unknownsPlusAuxiliaryVariables; i++) {
        outsideNew[i] = insideNew[i];
      }
    }
  }

  logTraceOut("applySommerfeldBoundaryConditions(...)");
}
