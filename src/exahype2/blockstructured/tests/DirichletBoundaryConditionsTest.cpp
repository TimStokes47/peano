// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "DirichletBoundaryConditionsTest.h"

#include "exahype2/blockstructured/DirichletBoundaryConditions.h"
#include "peano4/datamanagement/FaceMarker.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "tarch/la/ScalarOperations.h"
#include "tarch/tests/TestMacros.h"

#include <vector>


namespace {
  constexpr int    NumberOfGridCellsPerAxis = 3;
  constexpr int    Overlap                  = 2;
  constexpr int    Unknowns                 = 3;
  constexpr double TimeStamp                = 0.25;
  constexpr double Untouched                = -1234.0;
  constexpr double CellSize                 = 1.0 / 3.0;
  constexpr double GridCellSize = CellSize / NumberOfGridCellsPerAxis;

  int sizeOfFaceData() {
    int result = 2 * Overlap * Unknowns;
    for (int d = 1; d < Dimensions; d++) {
      result *= NumberOfGridCellsPerAxis;
    }
    return result;
  }

  /**
   * Position of entry index within the face data. Inverse of
   * toolbox::blockstructured::serialiseVoxelIndexInOverlap(): axis normal
   * has 2*Overlap entries, all other axes NumberOfGridCellsPerAxis, and
   * axis 0 runs fastest.
   */
  tarch::la::Vector<Dimensions, int> decodeOverlapCell(int index, int normal) {
    tarch::la::Vector<Dimensions, int> result;
    for (int d = 0; d < Dimensions; d++) {
      const int entriesAlongAxis = d == normal ? 2 * Overlap
                                               : NumberOfGridCellsPerAxis;
      result(d)                  = index % entriesAlongAxis;
      index /= entriesAlongAxis;
    }
    return result;
  }

  /**
   * Marker of face faceNumber of the cell with centre cellCentre and size
   * CellSize.
   */
  peano4::datamanagement::FaceMarker createMarker(
    const tarch::la::Vector<Dimensions, double>& cellCentre,
    int                                          faceNumber
  ) {
    peano4::grid::GridTraversalEvent event;
    event.setX(cellCentre);
    event.setH(tarch::la::Vector<Dimensions, double>(CellSize));
    return peano4::datamanagement::FaceMarker(event, faceNumber);
  }

  /**
   * Apply the boundary conditions over the unit square/cube to a face whose
   * entries all hold Untouched. The functor stores x(normal), x of the next
   * axis, and t.
   */
  std::vector<double> applyToFace(
    const peano4::datamanagement::FaceMarker& marker,
    const std::bitset<Dimensions>&            periodicBC
  ) {
    std::vector<double> faceData(sizeOfFaceData(), Untouched);
    const int           normal = marker.getSelectedFaceNumber() % Dimensions;
    // Capture normal by value: with a by-reference capture, icpx 2024.2 at
    // -O2 made the functor read a wrong normal (g++ and -O0 were fine)
    ::exahype2::blockstructured::applyDirichletBoundaryConditions(
      [normal](
        const tarch::la::Vector<Dimensions, double>& x,
        double                                       t,
        double*                                      Q
      ) -> void {
        Q[0] = x(normal);
        Q[1] = x((normal + 1) % Dimensions);
        Q[2] = t;
      },
      marker,
      tarch::la::Vector<Dimensions, double>(0.0),
      tarch::la::Vector<Dimensions, double>(1.0),
      periodicBC,
      NumberOfGridCellsPerAxis,
      Overlap,
      Unknowns,
      TimeStamp,
      faceData.data()
    );
    return faceData;
  }
} // namespace


exahype2::blockstructured::tests::DirichletBoundaryConditionsTest::
  DirichletBoundaryConditionsTest():
  TestCase("exahype2::blockstructured::tests::DirichletBoundaryConditionsTest"
  ) {}


void exahype2::blockstructured::tests::DirichletBoundaryConditionsTest::run() {
  testMethod(testLowerBoundary);
  testMethod(testUpperBoundary);
  testMethod(testPeriodicBoundaryIsSkipped);
  testMethod(testInteriorFaceIsSkipped);
}


void exahype2::blockstructured::tests::DirichletBoundaryConditionsTest::
  testLowerBoundary() {
  for (int normal = 0; normal < Dimensions; normal++) {
    // Cell in the lower left corner of the domain, face along the lower
    // boundary
    const std::vector<double> faceData = applyToFace(
      createMarker(
        tarch::la::Vector<Dimensions, double>(CellSize / 2.0),
        normal
      ),
      0
    );
    const int nextAxis = (normal + 1) % Dimensions;

    for (int index = 0; index < sizeOfFaceData() / Unknowns; index++) {
      const tarch::la::Vector<Dimensions, int>
        overlapCell = decodeOverlapCell(index, normal);
      if (overlapCell(normal) < Overlap) {
        // Ghost layer 0 is adjacent to the face, i.e. sits at
        // overlapCell(normal)=Overlap-1
        const int    ghostLayer          = Overlap - 1 - overlapCell(normal);
        const double expectedNormalX     = -(ghostLayer + 0.5) * GridCellSize;
        const double expectedTangentialX = (overlapCell(nextAxis) + 0.5)
                                           * GridCellSize;
        validateWithParams3(
          tarch::la::equals(faceData[index * Unknowns + 0], expectedNormalX),
          normal,
          overlapCell,
          faceData[index * Unknowns + 0]
        );
        validateWithParams3(
          tarch::la::
            equals(faceData[index * Unknowns + 1], expectedTangentialX),
          normal,
          overlapCell,
          faceData[index * Unknowns + 1]
        );
        validateWithParams3(
          tarch::la::equals(faceData[index * Unknowns + 2], TimeStamp),
          normal,
          overlapCell,
          faceData[index * Unknowns + 2]
        );
      } else {
        for (int unknown = 0; unknown < Unknowns; unknown++) {
          validateWithParams3(
            tarch::la::equals(faceData[index * Unknowns + unknown], Untouched),
            normal,
            overlapCell,
            faceData[index * Unknowns + unknown]
          );
        }
      }
    }
  }
}


void exahype2::blockstructured::tests::DirichletBoundaryConditionsTest::
  testUpperBoundary() {
  for (int normal = 0; normal < Dimensions; normal++) {
    // Cell in the upper right corner of the domain, face along the upper
    // boundary
    const std::vector<double> faceData = applyToFace(
      createMarker(
        tarch::la::Vector<Dimensions, double>(1.0 - CellSize / 2.0),
        normal + Dimensions
      ),
      0
    );
    const int nextAxis = (normal + 1) % Dimensions;

    for (int index = 0; index < sizeOfFaceData() / Unknowns; index++) {
      const tarch::la::Vector<Dimensions, int>
        overlapCell = decodeOverlapCell(index, normal);
      if (overlapCell(normal) >= Overlap) {
        // Ghost layer 0 is adjacent to the face, i.e. sits at
        // overlapCell(normal)=Overlap
        const int    ghostLayer      = overlapCell(normal) - Overlap;
        const double expectedNormalX = 1.0 + (ghostLayer + 0.5) * GridCellSize;
        const double expectedTangentialX = 1.0 - CellSize
                                           + (overlapCell(nextAxis) + 0.5
                                             ) * GridCellSize;
        validateWithParams3(
          tarch::la::equals(faceData[index * Unknowns + 0], expectedNormalX),
          normal,
          overlapCell,
          faceData[index * Unknowns + 0]
        );
        validateWithParams3(
          tarch::la::
            equals(faceData[index * Unknowns + 1], expectedTangentialX),
          normal,
          overlapCell,
          faceData[index * Unknowns + 1]
        );
        validateWithParams3(
          tarch::la::equals(faceData[index * Unknowns + 2], TimeStamp),
          normal,
          overlapCell,
          faceData[index * Unknowns + 2]
        );
      } else {
        for (int unknown = 0; unknown < Unknowns; unknown++) {
          validateWithParams3(
            tarch::la::equals(faceData[index * Unknowns + unknown], Untouched),
            normal,
            overlapCell,
            faceData[index * Unknowns + unknown]
          );
        }
      }
    }
  }
}


void exahype2::blockstructured::tests::DirichletBoundaryConditionsTest::
  testPeriodicBoundaryIsSkipped() {
  for (int normal = 0; normal < Dimensions; normal++) {
    std::bitset<Dimensions> periodicBC;
    periodicBC.set(normal);

    const auto lowerFaceData = applyToFace(
      createMarker(
        tarch::la::Vector<Dimensions, double>(CellSize / 2.0),
        normal
      ),
      periodicBC
    );
    const auto upperFaceData = applyToFace(
      createMarker(
        tarch::la::Vector<Dimensions, double>(1.0 - CellSize / 2.0),
        normal + Dimensions
      ),
      periodicBC
    );
    for (int i = 0; i < sizeOfFaceData(); i++) {
      validateWithParams3(
        tarch::la::equals(lowerFaceData[i], Untouched),
        normal,
        i,
        lowerFaceData[i]
      );
      validateWithParams3(
        tarch::la::equals(upperFaceData[i], Untouched),
        normal,
        i,
        upperFaceData[i]
      );
    }
  }
}


void exahype2::blockstructured::tests::DirichletBoundaryConditionsTest::
  testInteriorFaceIsSkipped() {
  for (int normal = 0; normal < Dimensions; normal++) {
    // Cell in the centre of the domain: none of its faces is on the boundary
    const tarch::la::Vector<Dimensions, double> centre(0.5);
    const auto lowerFaceData = applyToFace(createMarker(centre, normal), 0);
    const auto upperFaceData = applyToFace(
      createMarker(centre, normal + Dimensions),
      0
    );
    for (int i = 0; i < sizeOfFaceData(); i++) {
      validateWithParams3(
        tarch::la::equals(lowerFaceData[i], Untouched),
        normal,
        i,
        lowerFaceData[i]
      );
      validateWithParams3(
        tarch::la::equals(upperFaceData[i], Untouched),
        normal,
        i,
        upperFaceData[i]
      );
    }
  }
}
