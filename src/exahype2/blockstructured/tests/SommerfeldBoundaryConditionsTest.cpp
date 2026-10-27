// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "SommerfeldBoundaryConditionsTest.h"

#include "exahype2/blockstructured/SommerfeldBoundaryConditions.h"
#include "peano4/datamanagement/FaceMarker.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "tarch/la/ScalarOperations.h"
#include "tarch/tests/TestMacros.h"
#include "toolbox/blockstructured/Enumeration.h"

#include <cmath>
#include <functional>
#include <vector>


namespace {
  constexpr int    NumberOfGridCellsPerAxis = 3;
  constexpr int    Overlap                  = 3;
  constexpr int    Unknowns                 = 2;
  constexpr int    AuxiliaryVariables       = 1;
  constexpr int    Doubles                  = Unknowns + AuxiliaryVariables;
  constexpr double Untouched                = -1234.0;
  constexpr double CellSize                 = 2.0 / 3.0;
  constexpr double GridCellSize = CellSize / NumberOfGridCellsPerAxis;

  using WaveSpeed = std::function<double(
    const double*                                Q,
    const tarch::la::Vector<Dimensions, double>& x,
    int                                          normal
  )>;

  using FarFieldSolution = std::function<
    void(const tarch::la::Vector<Dimensions, double>& x, double* Q)>;

  /**
   * Number of grid cells within the face data
   */
  int numberOfOverlapCells() {
    int result = 2 * Overlap;
    for (int d = 1; d < Dimensions; d++) {
      result *= NumberOfGridCellsPerAxis;
    }
    return result;
  }

  /**
   * Position of grid cell index within the face data. Inverse of
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
   * Signed distance of an overlap cell from the face: 0 is the first cell
   * inside the domain, -1 the ghost cell adjacent to the face, -2 the next
   * ghost cell, and so forth.
   */
  int layerOfOverlapCell(
    const tarch::la::Vector<Dimensions, int>& overlapCell,
    int                                       normal,
    bool                                      isLowerBoundary
  ) {
    return isLowerBoundary
             ? overlapCell(normal) - Overlap
             : Overlap - 1 - overlapCell(normal);
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
   * Centre of the cell in the corner of the domain which is adjacent to
   * the lower or upper boundary along normal and to the lower boundary
   * along all other axes.
   */
  tarch::la::Vector<Dimensions, double> cornerCellCentre(
    int  normal,
    bool isLowerBoundary
  ) {
    tarch::la::Vector<Dimensions, double> result(-1.0 + CellSize / 2.0);
    if (not isLowerBoundary) {
      result(normal) = 1.0 - CellSize / 2.0;
    }
    return result;
  }

  /**
   * Apply the boundary conditions over @f$ [-1,1]^d @f$ around the origin.
   */
  void apply(
    WaveSpeed                                 waveSpeed,
    FarFieldSolution                          farFieldSolution,
    const peano4::datamanagement::FaceMarker& marker,
    const std::bitset<Dimensions>&            periodicBC,
    double                                    timeStepSize,
    const std::vector<double>&                faceDataOld,
    std::vector<double>&                      faceDataNew
  ) {
    ::exahype2::blockstructured::applySommerfeldBoundaryConditions(
      waveSpeed,
      farFieldSolution,
      marker,
      tarch::la::Vector<Dimensions, double>(-1.0),
      tarch::la::Vector<Dimensions, double>(2.0),
      periodicBC,
      NumberOfGridCellsPerAxis,
      Overlap,
      Unknowns,
      AuxiliaryVariables,
      tarch::la::Vector<Dimensions, double>(0.0),
      timeStepSize,
      faceDataOld.data(),
      faceDataNew.data()
    );
  }
} // namespace


exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::
  SommerfeldBoundaryConditionsTest():
  TestCase("exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest"
  ) {}


void exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::run() {
  testMethod(testFarFieldIsPreserved);
  testMethod(testBoxSchemeOnAxis);
  testMethod(testLowerAndUpperFacesAreSymmetric);
  testMethod(testZeroTimeStepSizeCopiesGhostCells);
  testMethod(testInteriorAndPeriodicFacesAreSkipped);
}


void exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::
  testFarFieldIsPreserved() {
  constexpr double FarField0    = 0.5;
  constexpr double FarField1    = -2.0;
  constexpr double OldAuxiliary = 3.0;
  constexpr double NewAuxiliary = 7.0;
  constexpr double TimeStepSize = 0.01;

  for (int normal = 0; normal < Dimensions; normal++) {
    for (bool isLowerBoundary : {true, false}) {
      std::vector<double> faceDataOld(numberOfOverlapCells() * Doubles);
      std::vector<double> faceDataNew(numberOfOverlapCells() * Doubles);
      for (int cell = 0; cell < numberOfOverlapCells(); cell++) {
        const bool isGhost = layerOfOverlapCell(
                               decodeOverlapCell(cell, normal),
                               normal,
                               isLowerBoundary
                             )
                             < 0;
        faceDataOld[cell * Doubles + 0] = FarField0;
        faceDataOld[cell * Doubles + 1] = FarField1;
        faceDataOld[cell * Doubles + 2] = OldAuxiliary;
        faceDataNew[cell * Doubles + 0] = isGhost ? Untouched : FarField0;
        faceDataNew[cell * Doubles + 1] = isGhost ? Untouched : FarField1;
        faceDataNew[cell * Doubles + 2] = isGhost ? Untouched : NewAuxiliary;
      }

      apply(
        [](
          const double*                                Q,
          const tarch::la::Vector<Dimensions, double>& x,
          int                                          normal
        ) -> double { return 1.3; },
        [](const tarch::la::Vector<Dimensions, double>& x, double* Q) -> void {
          Q[0] = FarField0;
          Q[1] = FarField1;
        },
        createMarker(
          cornerCellCentre(normal, isLowerBoundary),
          isLowerBoundary ? normal : normal + Dimensions
        ),
        0,
        TimeStepSize,
        faceDataOld,
        faceDataNew
      );

      for (int cell = 0; cell < numberOfOverlapCells(); cell++) {
        const tarch::la::Vector<Dimensions, int>
          overlapCell = decodeOverlapCell(cell, normal);
        validateWithParams4(
          tarch::la::equals(faceDataNew[cell * Doubles + 0], FarField0),
          normal,
          isLowerBoundary,
          overlapCell,
          faceDataNew[cell * Doubles + 0]
        );
        validateWithParams4(
          tarch::la::equals(faceDataNew[cell * Doubles + 1], FarField1),
          normal,
          isLowerBoundary,
          overlapCell,
          faceDataNew[cell * Doubles + 1]
        );
        validateWithParams4(
          tarch::la::equals(faceDataNew[cell * Doubles + 2], NewAuxiliary),
          normal,
          isLowerBoundary,
          overlapCell,
          faceDataNew[cell * Doubles + 2]
        );
      }
    }
  }
}


void exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::
  testBoxSchemeOnAxis() {
  constexpr double OldGhost     = 0.3;
  constexpr double OldInside    = 0.7;
  constexpr double NewInside    = 0.6;
  constexpr double FarField     = 0.1;
  constexpr double Speed        = 2.0;
  constexpr double TimeStepSize = 0.01;

  for (int normal = 0; normal < Dimensions; normal++) {
    // Cell adjacent to the upper boundary along normal whose centre lies on
    // the axis through the origin. Its middle grid cell lies on this axis
    tarch::la::Vector<Dimensions, double> cellCentre(0.0);
    cellCentre(normal) = 1.0 - CellSize / 2.0;

    std::vector<double> faceDataOld(numberOfOverlapCells() * Doubles, 0.4);
    std::vector<double> faceDataNew(numberOfOverlapCells() * Doubles, 0.5);

    tarch::la::Vector<Dimensions, int> ghostCell(NumberOfGridCellsPerAxis / 2);
    tarch::la::Vector<Dimensions, int> insideCell(NumberOfGridCellsPerAxis / 2);
    ghostCell(normal)  = Overlap;
    insideCell(normal) = Overlap - 1;
    const int ghostIndex
      = toolbox::blockstructured::serialiseVoxelIndexInOverlap(
          ghostCell,
          NumberOfGridCellsPerAxis,
          Overlap,
          normal
        )
        * Doubles;
    const int insideIndex
      = toolbox::blockstructured::serialiseVoxelIndexInOverlap(
          insideCell,
          NumberOfGridCellsPerAxis,
          Overlap,
          normal
        )
        * Doubles;
    faceDataOld[ghostIndex]  = OldGhost;
    faceDataOld[insideIndex] = OldInside;
    faceDataNew[insideIndex] = NewInside;

    apply(
      [](
        const double*                                Q,
        const tarch::la::Vector<Dimensions, double>& x,
        int                                          normal
      ) -> double { return Speed; },
      [](const tarch::la::Vector<Dimensions, double>& x, double* Q) -> void {
        Q[0] = FarField;
        Q[1] = FarField;
      },
      createMarker(cellCentre, normal + Dimensions),
      0,
      TimeStepSize,
      faceDataOld,
      faceDataNew
    );

    // On the axis, r=|x_n| is the distance of the ghost cell's centre from
    // the origin
    const double r        = 1.0 + 0.5 * GridCellSize;
    const double sigma    = Speed / GridCellSize;
    const double newGhost = faceDataNew[ghostIndex];
    const double residual
      = (newGhost - OldGhost + NewInside - OldInside) / TimeStepSize
        + sigma * (newGhost - NewInside + OldGhost - OldInside)
        + Speed / r * (OldGhost + OldInside - 2.0 * FarField);
    validateWithParams3(std::abs(residual) < 1e-10, normal, newGhost, residual);
  }
}


void exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::
  testLowerAndUpperFacesAreSymmetric() {
  constexpr double TimeStepSize = 0.02;

  for (int normal = 0; normal < Dimensions; normal++) {
    std::vector<double> faceDataNew[2];
    for (bool isLowerBoundary : {true, false}) {
      std::vector<double>  faceDataOld(numberOfOverlapCells() * Doubles);
      std::vector<double>& newData = faceDataNew[isLowerBoundary ? 0 : 1];
      newData.resize(numberOfOverlapCells() * Doubles);

      // The data depends on the distance from the face along the normal and
      // on the tangential position, so it is mirrored along the normal
      for (int cell = 0; cell < numberOfOverlapCells(); cell++) {
        const tarch::la::Vector<Dimensions, int>
                  overlapCell = decodeOverlapCell(cell, normal);
        const int layer       = layerOfOverlapCell(
          overlapCell,
          normal,
          isLowerBoundary
        );
        int tangentialPosition = 0;
        for (int d = 0; d < Dimensions; d++) {
          if (d != normal) {
            tangentialPosition += (d + 1) * overlapCell(d);
          }
        }
        for (int unknown = 0; unknown < Doubles; unknown++) {
          const double value = 0.2 + 0.05 * layer + 0.01 * tangentialPosition
                               + 0.003 * unknown;
          faceDataOld[cell * Doubles + unknown] = value;
          newData[cell * Doubles + unknown]     = layer < 0 ? Untouched
                                                            : value + 0.02;
        }
      }

      apply(
        [](
          const double*                                Q,
          const tarch::la::Vector<Dimensions, double>& x,
          int                                          normal
        ) -> double { return 1.0 + 0.5 * Q[0]; },
        [](const tarch::la::Vector<Dimensions, double>& x, double* Q) -> void {
          Q[0] = 0.1;
          Q[1] = 1.0 / (1.0 + tarch::la::norm2(x));
        },
        createMarker(
          cornerCellCentre(normal, isLowerBoundary),
          isLowerBoundary ? normal : normal + Dimensions
        ),
        0,
        TimeStepSize,
        faceDataOld,
        newData
      );
    }

    for (int lowerCell = 0; lowerCell < numberOfOverlapCells(); lowerCell++) {
      const tarch::la::Vector<Dimensions, int>
                overlapCell = decodeOverlapCell(lowerCell, normal);
      const int layer       = layerOfOverlapCell(overlapCell, normal, true);
      tarch::la::Vector<Dimensions, int> mirroredCell = overlapCell;
      mirroredCell(normal)                            = Overlap - 1 - layer;
      const int upperCell = toolbox::blockstructured::
        serialiseVoxelIndexInOverlap(
          mirroredCell,
          NumberOfGridCellsPerAxis,
          Overlap,
          normal
        );
      for (int unknown = 0; unknown < Doubles; unknown++) {
        const double lowerValue = faceDataNew[0][lowerCell * Doubles + unknown];
        const double upperValue = faceDataNew[1][upperCell * Doubles + unknown];
        validateWithParams5(
          tarch::la::equals(lowerValue, upperValue),
          normal,
          overlapCell,
          unknown,
          lowerValue,
          upperValue
        );
      }
      if (layer < 0) {
        validateWithParams2(
          not tarch::la::equals(faceDataNew[0][lowerCell * Doubles], Untouched),
          normal,
          overlapCell
        );
      }
    }
  }
}


void exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::
  testZeroTimeStepSizeCopiesGhostCells() {
  for (int normal = 0; normal < Dimensions; normal++) {
    for (bool isLowerBoundary : {true, false}) {
      std::vector<double> faceDataOld(numberOfOverlapCells() * Doubles);
      std::vector<double> faceDataNew(numberOfOverlapCells() * Doubles);
      for (int i = 0; i < numberOfOverlapCells() * Doubles; i++) {
        faceDataOld[i] = 0.1 * i;
        faceDataNew[i] = layerOfOverlapCell(
                           decodeOverlapCell(i / Doubles, normal),
                           normal,
                           isLowerBoundary
                         ) < 0
                           ? Untouched
                           : -0.1 * i;
      }

      apply(
        [](
          const double*                                Q,
          const tarch::la::Vector<Dimensions, double>& x,
          int                                          normal
        ) -> double { return 1.0; },
        [](const tarch::la::Vector<Dimensions, double>& x, double* Q) -> void {
          Q[0] = 0.0;
          Q[1] = 0.0;
        },
        createMarker(
          cornerCellCentre(normal, isLowerBoundary),
          isLowerBoundary ? normal : normal + Dimensions
        ),
        0,
        0.0,
        faceDataOld,
        faceDataNew
      );

      for (int i = 0; i < numberOfOverlapCells() * Doubles; i++) {
        const bool isGhost = layerOfOverlapCell(
                               decodeOverlapCell(i / Doubles, normal),
                               normal,
                               isLowerBoundary
                             )
                             < 0;
        validateWithParams4(
          tarch::la::equals(faceDataNew[i], isGhost ? 0.1 * i : -0.1 * i),
          normal,
          isLowerBoundary,
          i,
          faceDataNew[i]
        );
      }
    }
  }
}


void exahype2::blockstructured::tests::SommerfeldBoundaryConditionsTest::
  testInteriorAndPeriodicFacesAreSkipped() {
  for (int normal = 0; normal < Dimensions; normal++) {
    std::bitset<Dimensions> periodicBC;
    periodicBC.set(normal);

    // Faces of the cell in the centre of the domain, and boundary faces
    // along a periodic axis
    const std::vector<
      std::pair<peano4::datamanagement::FaceMarker, std::bitset<Dimensions>>>
      faces = {
        {createMarker(tarch::la::Vector<Dimensions, double>(0.0), normal), 0},
        {createMarker(
           tarch::la::Vector<Dimensions, double>(0.0),
           normal + Dimensions
         ),
         0},
        {createMarker(cornerCellCentre(normal, true), normal), periodicBC},
        {createMarker(cornerCellCentre(normal, false), normal + Dimensions),
         periodicBC}};

    for (const auto& [marker, periodicBCOfFace] : faces) {
      const std::vector<double>
        faceDataOld(numberOfOverlapCells() * Doubles, 1.0);
      std::vector<double>
        faceDataNew(numberOfOverlapCells() * Doubles, Untouched);
      apply(
        [](
          const double*                                Q,
          const tarch::la::Vector<Dimensions, double>& x,
          int                                          normal
        ) -> double { return 1.0; },
        [](const tarch::la::Vector<Dimensions, double>& x, double* Q) -> void {
          Q[0] = 0.0;
          Q[1] = 0.0;
        },
        marker,
        periodicBCOfFace,
        0.1,
        faceDataOld,
        faceDataNew
      );
      for (int i = 0; i < numberOfOverlapCells() * Doubles; i++) {
        validateWithParams3(
          tarch::la::equals(faceDataNew[i], Untouched),
          normal,
          marker.toString(),
          faceDataNew[i]
        );
      }
    }
  }
}
