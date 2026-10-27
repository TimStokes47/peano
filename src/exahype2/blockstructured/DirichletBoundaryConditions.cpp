// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "DirichletBoundaryConditions.h"

#include "peano4/utils/Loop.h"
#include "tarch/la/ScalarOperations.h"
#include "tarch/logging/Log.h"
#include "toolbox/blockstructured/Enumeration.h"


namespace {
  tarch::logging::Log _log("exahype2::blockstructured");
} // namespace


void exahype2::blockstructured::applyDirichletBoundaryConditions(
  std::function<
    void(const tarch::la::Vector<Dimensions, double>& x, double t, double* Q)>
                                               boundaryCondition,
  const peano4::datamanagement::FaceMarker&    marker,
  const tarch::la::Vector<Dimensions, double>& domainOffset,
  const tarch::la::Vector<Dimensions, double>& domainSize,
  const std::bitset<Dimensions>&               periodicBC,
  int                                          numberOfGridCellsPerAxis,
  int                                          overlap,
  int                                          unknownsPlusAuxiliaryVariables,
  double                                       t,
  double*                                      faceData
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

  logTraceInWith1Argument(
    "applyDirichletBoundaryConditions(...)",
    marker.toString()
  );

  const tarch::la::Vector<Dimensions, double>
    gridCellH = marker.h() / static_cast<double>(numberOfGridCellsPerAxis);

  dfore(gridCell, numberOfGridCellsPerAxis, normal, 0) {
    for (int layer = 0; layer < overlap; layer++) {
      tarch::la::Vector<Dimensions, int>    overlapCell = gridCell;
      tarch::la::Vector<Dimensions, double> x;
      for (int d = 0; d < Dimensions; d++) {
        x(d
        ) = marker.x()(d) - 0.5 * marker.h()(d)
            + (gridCell(d) + 0.5) * gridCellH(d);
      }
      if (isLowerBoundary) {
        overlapCell(normal) = overlap - 1 - layer;
        x(normal) = marker.x()(normal) - (layer + 0.5) * gridCellH(normal);
      } else {
        overlapCell(normal) = overlap + layer;
        x(normal) = marker.x()(normal) + (layer + 0.5) * gridCellH(normal);
      }
      const int index = toolbox::blockstructured::serialiseVoxelIndexInOverlap(
        overlapCell,
        numberOfGridCellsPerAxis,
        overlap,
        normal
      );
      boundaryCondition(
        x,
        t,
        faceData + index * unknownsPlusAuxiliaryVariables
      );
    }
  }

  logTraceOut("applyDirichletBoundaryConditions(...)");
}
