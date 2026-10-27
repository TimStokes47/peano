#include "InterpolationTest.h"
#include "toolbox/blockstructured/Interpolation.h"

#include "../Interpolation.h"
#include "../Restriction.h"
#include <algorithm>
#include "peano4/grid/GridTraversalEvent.h"
#include "peano4/utils/Loop.h"
#include "peano4/datamanagement/FaceMarker.h"
#include "peano4/datamanagement/CellMarker.h"
#include <cmath>
#include "../Enumeration.h"
#include <vector>


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


toolbox::blockstructured::tests::InterpolationTest::InterpolationTest():
  TestCase("toolbox::blockstructured::tests::InterpolationTest") {
}


void toolbox::blockstructured::tests::InterpolationTest::run() {
  testMethod(testRestrictHaloLayer_AoS_averaging);
  testMethod(testRestrictCellForBreakingDam);
  testMethod(testInterpolateCellDataAssociatedToVolumesIntoOverlappingCell_linear);
  testMethod(testHangingFacesForAnyOverlap);
  testMethod(testCellInterpolation);
}


void toolbox::blockstructured::tests::InterpolationTest::testCellInterpolation() {
  constexpr int DoFs     = 6;
  constexpr int Unknowns = 2;
  const int     patchSize = Dimensions == 2 ? DoFs * DoFs : DoFs * DoFs * DoFs;
  // Linear field over the coarse cell, evaluated at the volume centres
  auto field = [](const tarch::la::Vector<Dimensions, double>& x) -> double {
    double result = 1.0;
    for (int d = 0; d < Dimensions; d++) {
      result += (d + 1) * 10.0 * x(d);
    }
    return result;
  };
  std::vector<double> coarse(patchSize * Unknowns);
  dfor(coarseVolume, DoFs) {
    tarch::la::Vector<Dimensions, double> x;
    for (int d = 0; d < Dimensions; d++) {
      x(d) = (coarseVolume(d) + 0.5) / DoFs;
    }
    const int index = peano4::utils::dLinearised(coarseVolume, DoFs);
    coarse[index * Unknowns]     = field(x);
    coarse[index * Unknowns + 1] = -3.0;
  }

  dfor3(child) {
    peano4::grid::GridTraversalEvent event;
    for (int d = 0; d < Dimensions; d++) {
      event.setRelativePositionToFather(d, child(d));
    }
    peano4::datamanagement::CellMarker marker(event);
    std::vector<double> linear(patchSize * Unknowns, -1.0);
    std::vector<double> constant(patchSize * Unknowns, -1.0);
    toolbox::blockstructured::interpolateCell_AoS_linear_with_constant_extrapolation(marker, DoFs, Unknowns, coarse.data(), linear.data());
    toolbox::blockstructured::interpolateCell_AoS_piecewise_constant(marker, DoFs, Unknowns, coarse.data(), constant.data());
    dfor(fineVolume, DoFs) {
      tarch::la::Vector<Dimensions, double> x;
      tarch::la::Vector<Dimensions, double> clamped;
      tarch::la::Vector<Dimensions, double> containingCoarseCentre;
      for (int d = 0; d < Dimensions; d++) {
        x(d)                      = (child(d) + (fineVolume(d) + 0.5) / DoFs) / 3.0;
        clamped(d)                = std::min(std::max(x(d), 0.5 / DoFs), 1.0 - 0.5 / DoFs);
        containingCoarseCentre(d) = (std::floor(x(d) * DoFs) + 0.5) / DoFs;
      }
      const int    index           = peano4::utils::dLinearised(fineVolume, DoFs);
      const double linearValue     = linear[index * Unknowns];
      const double constantValue   = constant[index * Unknowns];
      const double linearExpected   = field(clamped);
      const double constantExpected = field(containingCoarseCentre);
      validateNumericalEqualsWithParams2(linearValue, linearExpected, child, fineVolume);
      validateNumericalEqualsWithParams2(constantValue, constantExpected, child, fineVolume);
      const double secondUnknown = linear[index * Unknowns + 1];
      validateNumericalEqualsWithParams2(secondUnknown, -3.0, child, fineVolume);
    }
  }
  enddforx
}


void toolbox::blockstructured::tests::InterpolationTest::testHangingFacesForAnyOverlap() {
  constexpr int DoFs = 4;
  constexpr int Unknowns = 2;
  for (int overlap = 1; overlap <= 3; overlap++) {
    const int faceSize = 2 * overlap * (Dimensions == 2 ? DoFs : DoFs * DoFs);
    for (int faceNumber = 0; faceNumber < 2 * Dimensions; faceNumber++) {
      const int normal = faceNumber % Dimensions;
      const int tangential = (normal + 1) % Dimensions;
      std::vector<double> coarseConstant(faceSize * Unknowns, 7.0);
      std::vector<double> coarseLinear(faceSize * Unknowns, 0.0);
      dfore(tangentialIndex, DoFs, normal, 0) {
        for (int layer = 0; layer < 2 * overlap; layer++) {
          tarch::la::Vector<Dimensions, int> volume = tangentialIndex;
          volume(normal) = layer;
          const int index = serialiseVoxelIndexInOverlap(volume, DoFs, overlap, normal);
          for (int j = 0; j < Unknowns; j++) {
            coarseLinear[index * Unknowns + j] = tangentialIndex(tangential) + 0.5;
          }
        }
      }

      std::vector<double> coarseRestricted(faceSize * Unknowns, 0.0);
      dfor3(subface) {
        if (subface(normal) != 0) {
          continue;
        }
        peano4::grid::GridTraversalEvent event;
        for (int d = 0; d < Dimensions; d++) {
          event.setRelativePositionToFather(d, subface(d));
        }
        peano4::datamanagement::FaceMarker marker(event, faceNumber);

        std::vector<double> fine(faceSize * Unknowns, -1.0);
        toolbox::blockstructured::interpolateHaloLayer_AoS_piecewise_constant(marker, DoFs, overlap, Unknowns, coarseConstant.data(), fine.data());
        for (auto value : fine) {
          validateNumericalEqualsWithParams3(value, 7.0, overlap, faceNumber, subface);
        }
        toolbox::blockstructured::interpolateHaloLayer_AoS_linear_with_constant_extrapolation(marker, DoFs, overlap, Unknowns, coarseConstant.data(), fine.data());
        for (auto value : fine) {
          validateNumericalEqualsWithParams3(value, 7.0, overlap, faceNumber, subface);
        }

        toolbox::blockstructured::interpolateHaloLayer_AoS_linear_with_constant_extrapolation(marker, DoFs, overlap, Unknowns, coarseLinear.data(), fine.data());
        dfore(tangentialIndex, DoFs, normal, 0) {
          // Centre of the fine volume in coarse volume units
          const double position = (tangentialIndex(tangential) + subface(tangential) * DoFs + 0.5) / 3.0;
          const double expected = std::min(std::max(position, 0.5), DoFs - 0.5);
          for (int layer = 0; layer < 2 * overlap; layer++) {
            tarch::la::Vector<Dimensions, int> volume = tangentialIndex;
            volume(normal) = layer;
            const int index = serialiseVoxelIndexInOverlap(volume, DoFs, overlap, normal);
            validateNumericalEqualsWithParams4(fine[index * Unknowns], expected, overlap, faceNumber, subface, volume);
          }
        }

        std::vector<double> fineConstant(faceSize * Unknowns, 2.0);
        toolbox::blockstructured::restrictInnerHalfOfHaloLayer_AoS_averaging(marker, DoFs, overlap, Unknowns, fineConstant.data(), coarseRestricted.data());
      }
      enddforx

      // The inner half of the fine face is the half of the fine cell. For the
      // left face of a cell, this is the right half of the face.
      const bool innerIsRightHalf = faceNumber < Dimensions;
      dfore(tangentialIndex, DoFs, normal, 0) {
        for (int layer = 0; layer < 2 * overlap; layer++) {
          tarch::la::Vector<Dimensions, int> volume = tangentialIndex;
          volume(normal) = layer;
          const int index = serialiseVoxelIndexInOverlap(volume, DoFs, overlap, normal);
          const bool inInnerHalf = innerIsRightHalf ? layer >= overlap : layer < overlap;
          const double expected = inInnerHalf ? 2.0 : 0.0;
          for (int j = 0; j < Unknowns; j++) {
            const double restricted = coarseRestricted[index * Unknowns + j];
            validateNumericalEqualsWithParams3(restricted, expected, overlap, faceNumber, volume);
          }
        }
      }
    }
  }
}


void toolbox::blockstructured::tests::InterpolationTest::testInterpolateCellDataAssociatedToVolumesIntoOverlappingCell_linear() {
  #if Dimensions==3
  const int NumberOfUnknowns = 3;
  const int SourcePatchSize = 4;
  const int SourceHaloSize = 3;
  const int DestinationPatchSize = 16;
  const int DestinationHaloSize = 1;

  int SourceSize = (SourcePatchSize+2*SourceHaloSize) * (SourcePatchSize+2*SourceHaloSize) * (SourcePatchSize+2*SourceHaloSize) * NumberOfUnknowns;
  double* sourceData = new double[ SourceSize ];
  for (int i=0; i<SourceSize; i++) sourceData[i] = 1.0;

  int DestinationSize = (DestinationPatchSize+2*DestinationHaloSize) * (DestinationPatchSize+2*DestinationHaloSize) * (DestinationPatchSize+2*DestinationHaloSize) * NumberOfUnknowns;
  double* destinationData = new double[ DestinationSize ];

  interpolateCellDataAssociatedToVolumesIntoOverlappingCell_linear(
    SourcePatchSize,
    DestinationPatchSize,
    SourceHaloSize,
    DestinationHaloSize,
    NumberOfUnknowns,
    sourceData,
    destinationData,
    ::peano4::utils::LoopPlacement::SpreadOut
  );

  for (int i=0; i<DestinationSize; i++) {
    validateNumericalEqualsWithParams1( destinationData[i], 1.0, i );
  }

  delete[] sourceData;
  delete[] destinationData;
  #endif
}


void toolbox::blockstructured::tests::InterpolationTest::testRestrictCellForBreakingDam() {
  #if Dimensions==2 and PeanoDebug>0
  peano4::grid::GridTraversalEvent    dummyEvent;
  peano4::datamanagement::CellMarker  cellMarker(dummyEvent);
  cellMarker.setRelativePositionWithinFatherCell( 0,0 );
  cellMarker.setRelativePositionWithinFatherCell( 1,0 );
  double fineGridValues[] =
      {
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.0, 0.0, 0.0, 0.0, 0.0,

        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.0, 0.0, 0.0, 0.0, 0.0,

        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.0, 0.0, 0.0, 0.0, 0.0,

        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.1, 0.2, 0.3, 0.4, 0.5,
        0.0, 0.0, 0.0, 0.0, 0.0
      };

  double coarseGridValues[4*4*5];

  std::fill_n(coarseGridValues,4*4*5,0.0);

  ::toolbox::blockstructured::restrictCell_AoS_averaging(
      cellMarker,
      4,
      5,
      fineGridValues,
      coarseGridValues
  );

  validateNumericalEquals( coarseGridValues[0*5+0], 0.1 );
  validateNumericalEquals( coarseGridValues[0*5+1], 0.2 );
  validateNumericalEquals( coarseGridValues[0*5+2], 0.3 );
  validateNumericalEquals( coarseGridValues[0*5+3], 0.4 );
  validateNumericalEquals( coarseGridValues[0*5+4], 0.5 );

  validateNumericalEquals( coarseGridValues[1*5+0], 0.0 );
  validateNumericalEquals( coarseGridValues[1*5+1], 0.0 );
  validateNumericalEquals( coarseGridValues[1*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[1*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[1*5+4], 0.0 );

  validateNumericalEquals( coarseGridValues[2*5+0], 0.0 );
  validateNumericalEquals( coarseGridValues[2*5+1], 0.0 );
  validateNumericalEquals( coarseGridValues[2*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[2*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[2*5+4], 0.0 );

  validateNumericalEquals( coarseGridValues[4*5+0], 0.1/3.0 );
  validateNumericalEquals( coarseGridValues[4*5+1], 0.2/3.0 );
  validateNumericalEquals( coarseGridValues[4*5+2], 0.3/3.0 );
  validateNumericalEquals( coarseGridValues[4*5+3], 0.4/3.0 );
  validateNumericalEquals( coarseGridValues[4*5+4], 0.5/3.0 );


  #endif
}


void toolbox::blockstructured::tests::InterpolationTest::testRestrictHaloLayer_AoS_averaging() {
  #if Dimensions==2
  peano4::grid::GridTraversalEvent    dummyEvent;
  dummyEvent.setRelativePositionToFather(0,0);
  dummyEvent.setRelativePositionToFather(1,0);

  peano4::datamanagement::FaceMarker  faceMarker(dummyEvent);
  double fineGridValues[] =
      {
       0.100047, 0.000933957, 0,0,0.00493399,
       0.100045, 0.000870917, 0,0,0.0045799,
       0.100039, 0.000801622, 0,0,0.00422816,
       0.100045, 0.00081321,  0,0,0.0042177,
       0.100047, 0.000933957, 0,0,0.00493399,
       0.100045, 0.000870917, 2.71051e-20,0,0.0045799,
       0.100039, 0.000801622, 0,0,0.00422816,
       0.100045,0.00081321,0,0,0.0042177
      };
  double coarseGridValues[] =
      {
        0,0,0,0,0,
        0,0,0,0,0,
        0,0,0,0,0,
        0,0,0,0,0,
        0,0,0,0,0,
        0,0,0,0,0,
        0,0,0,0,0,
        0,0,0,0,0
      };

  toolbox::blockstructured::restrictHaloLayer_AoS_averaging(
    faceMarker,
    4, // int                                       numberOfDoFsPerAxisInPatch,
    1, // int                                       overlap,
    5, // int                                       unknowns,
    fineGridValues, coarseGridValues
  );

  validateNumericalEquals( coarseGridValues[0*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[0*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[1*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[1*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[2*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[2*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[3*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[3*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[4*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[4*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[5*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[5*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[6*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[6*5+3], 0.0 );
  validateNumericalEquals( coarseGridValues[7*5+2], 0.0 );
  validateNumericalEquals( coarseGridValues[7*5+3], 0.0 );

  #endif
}

#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
