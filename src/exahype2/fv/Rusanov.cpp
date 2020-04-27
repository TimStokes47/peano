#include "Rusanov.h"
#include "../PatchUtils.h"

#include "tarch/Assertions.h"

#include "peano4/utils/Loop.h"


namespace {
  /**
   * Helper routine that I need in the log statements
   */
  std::string plotVolume(
    double Q[],
    int    unknowns
  ) {
    std::string result = "(" + std::to_string(Q[0]);
    for (int i=1; i<unknowns; i++) result += "," + std::to_string(Q[i]);
    result += ")";
    return result;
  };

  void splitRiemann1d(
    std::function< void(
            double                                       Q[],
            const tarch::la::Vector<Dimensions,double>&  faceCentre,
            const tarch::la::Vector<Dimensions,double>&  volumeH,
            double                                       t,
            double                                       dt,
            int                                          normal,
            double                                       F[]
    ) >   flux,
    std::function< void(
            double                                       Q[],
            const tarch::la::Vector<Dimensions,double>&  faceCentre,
            const tarch::la::Vector<Dimensions,double>&  volumeH,
            double                                       t,
            double                                       dt,
            int                                          normal,
            double                                       lambdas[]
    ) >   eigenvalues,
    double QL[],
    double QR[],
    const tarch::la::Vector<Dimensions,double>&  x,
    double                                       dx,
    double                                       t,
    double                                       dt,
    int                                          normal,
    int                                          unknowns,
    double                                       F[]
  ) {
    assertion(normal>=0);
    assertion(normal<Dimensions);

    double FL[unknowns];
    double FR[unknowns];
    flux(QL,x,dx,t,dt,normal,FL);
    flux(QR,x,dx,t,dt,normal,FR);

    double lambdas[unknowns];
    double lambdaMax = 0.0;

    eigenvalues(QL,x,dx,t,dt,normal,lambdas);
    for (int unknown=0; unknown<unknowns; unknown++) {
      assertion(lambdas[unknown]==lambdas[unknown]);
      assertion(lambdas[unknown]>=0.0);
      lambdaMax = std::max(lambdaMax,lambdas[unknown]);
    }
    eigenvalues(QR,x,dx,t,dt,normal % Dimensions,lambdas);
    for (int unknown=0; unknown<unknowns; unknown++) {
      assertion(lambdas[unknown]==lambdas[unknown]);
      assertion(lambdas[unknown]>=0.0);
      lambdaMax = std::max(lambdaMax,lambdas[unknown]);
    }

    for (int unknown=0; unknown<unknowns; unknown++) {
      F[unknown] = 0.5 * FL[unknown] + 0.5 * FR[unknown] - 0.5 * lambdaMax * (QR[unknown] - QL[unknown]);
    }
  };
}


void exahype2::fv::applyRusanovToPatch(
  std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       F[]
  ) >   flux,
  std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       lambdas[]
  ) >   eigenvalues,
  const tarch::la::Vector<Dimensions,double>&  patchCentre,
  const tarch::la::Vector<Dimensions,double>&  patchSize,
  double                                       t,
  double                                       dt,
  int                                          numberOfVolumesPerAxisInPatch,
  int                                          unknowns,
  double                                       Qin[],
  double                                       Qout[]
) {
  static tarch::logging::Log _log( "exahype2::fv" );
  logTraceInWith6Arguments( "applyRusanovToPatch(...)", patchCentre, patchSize, t, dt, numberOfVolumesPerAxisInPatch, unknowns );

  assertion( dt>=tarch::la::NUMERICAL_ZERO_DIFFERENCE );

  tarch::la::Vector<Dimensions,double> volumeH = exahype2::getVolumeSize(patchSize, numberOfVolumesPerAxisInPatch);

  dfor(cell,numberOfVolumesPerAxisInPatch) {
    tarch::la::Vector<Dimensions,int> currentVoxel = cell + tarch::la::Vector<Dimensions,int>(1);
    int currentVoxelSerialised = peano4::utils::dLinearised(currentVoxel,numberOfVolumesPerAxisInPatch + 2);
    logDebug( "applyRusanovToPatch(...)", "handle volume " << cell << " in destination patch, i.e. source volume " << currentVoxel << ": " << plotVolume(Qin + currentVoxelSerialised*unknowns, unknowns) << " [" << currentVoxelSerialised << "]" );

    double accumulatedNumericalFlux[unknowns];
    for (int i=0; i<unknowns; i++) {
      accumulatedNumericalFlux[i] = 0.0;
    }
    double numericalFlux[unknowns]; // helper in/out variable
    for (int d=0; d<Dimensions; d++) {
      tarch::la::Vector<Dimensions,int>    neighbourVolume = currentVoxel;
      tarch::la::Vector<Dimensions,double> x              = exahype2::getVolumeCentre(patchCentre, patchSize, numberOfVolumesPerAxisInPatch, cell);

      neighbourVolume(d) -= 1;
      int neighbourVolumeSerialised = peano4::utils::dLinearised(neighbourVolume,numberOfVolumesPerAxisInPatch + 2);
      x(d) -= 0.5 * volumeH(d);
      assertion(neighbourVolume(d)>=0);

      logDebug( "applyRusanovToPatch(...)", "handle neighbour " << neighbourVolume << ": " << plotVolume(Qin + neighbourVolumeSerialised*unknowns, unknowns) << " [" << neighbourVolumeSerialised << "]");
      splitRiemann1d(
        flux, eigenvalues,
        Qin + neighbourVolumeSerialised*unknowns,
        Qin + currentVoxelSerialised*unknowns,
        x, volumeH(d), t, dt, d, unknowns,
        numericalFlux
      );
      for (int unknown=0; unknown<unknowns; unknown++) {
        accumulatedNumericalFlux[unknown] += numericalFlux[unknown];
        assertion(  numericalFlux[unknown]==numericalFlux[unknown] );
        assertion(  accumulatedNumericalFlux[unknown]==accumulatedNumericalFlux[unknown] );
      }

      neighbourVolume(d) += 2;
      neighbourVolumeSerialised = peano4::utils::dLinearised(neighbourVolume,numberOfVolumesPerAxisInPatch + 2);
      x(d) += 1.0 * volumeH(d);

      logDebug( "applyRusanovToPatch(...)", "handle neighbour " << neighbourVolume << ": " << plotVolume(Qin + neighbourVolumeSerialised*unknowns, unknowns) << " [" << neighbourVolumeSerialised << "]" );
      splitRiemann1d(
        flux, eigenvalues,
        Qin + neighbourVolumeSerialised*unknowns,
        Qin + currentVoxelSerialised*unknowns,
        x, volumeH(d), t, dt, d, unknowns,
        numericalFlux
      );      for (int unknown=0; unknown<unknowns; unknown++) {
        accumulatedNumericalFlux[unknown] -= numericalFlux[unknown];
        assertion(  numericalFlux[unknown]==numericalFlux[unknown] );
        assertion(  accumulatedNumericalFlux[unknown]==accumulatedNumericalFlux[unknown] );
      }
    }

    int destinationVoxelSerialised = peano4::utils::dLinearised(cell,numberOfVolumesPerAxisInPatch);

    for (int unknown=0; unknown<unknowns; unknown++) {
      Qout[ destinationVoxelSerialised*unknowns+unknown ] += dt / volumeH(0) * accumulatedNumericalFlux[unknown];
    }
  }

  logTraceOut( "applyRusanovToPatch(...)" );
}



void exahype2::fv::applyRusanovToPatch_FaceLoops2d(
  std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       F[]
  ) >   flux,
  std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal,
        double                                       lambdas[]
  ) >   eigenvalues,
  const tarch::la::Vector<Dimensions,double>&  patchCentre,
  const tarch::la::Vector<Dimensions,double>&  patchSize,
  double                                       t,
  double                                       dt,
  int                                          numberOfVolumesPerAxisInPatch,
  int                                          unknowns,
  double                                       Qin[],
  double                                       Qout[]
) {
  static tarch::logging::Log _log( "exahype2::fv" );
  logTraceInWith6Arguments( "applyRusanovToPatch_FaceLoops2d(...)", patchCentre, patchSize, t, dt, numberOfVolumesPerAxisInPatch, unknowns );

  assertion( dt>=tarch::la::NUMERICAL_ZERO_DIFFERENCE );

  tarch::la::Vector<Dimensions,double> volumeH = exahype2::getVolumeSize(patchSize, numberOfVolumesPerAxisInPatch);

  double numericalFlux[unknowns]; // helper out variable

  for (int y=0; y<numberOfVolumesPerAxisInPatch; y++)
  for (int x=0; x<=numberOfVolumesPerAxisInPatch; x++) {
    const int leftVoxelInPreimage  =  x
                                   + (y+1) * (2 + numberOfVolumesPerAxisInPatch);
    const int rightVoxelInPreimage =  x+1
                                   + (y+1) * (2 + numberOfVolumesPerAxisInPatch);

    const int leftVoxelInImage     = x-1
                                   + y * numberOfVolumesPerAxisInPatch;
    const int rightVoxelInImage    = x
                                   + y * numberOfVolumesPerAxisInPatch;
    splitRiemann1d(
      flux, eigenvalues,
      Qin + leftVoxelInPreimage*unknowns,
      Qin + rightVoxelInPreimage*unknowns,
      x, volumeH(0), t, dt, 0, //  last argument = normal
      unknowns,
      numericalFlux
    );

    for (int unknown=0; unknown<unknowns; unknown++) {
      if (x>0) {
        Qout[ leftVoxelInImage*unknowns+unknown ] -= dt / volumeH(0) * numericalFlux[unknown];
      }
      if (x<numberOfVolumesPerAxisInPatch) {
        Qout[ rightVoxelInImage*unknowns+unknown ] += dt / volumeH(0) * numericalFlux[unknown];
      }
    }
  }

  for (int y=0; y<=numberOfVolumesPerAxisInPatch; y++)
  for (int x=0; x<numberOfVolumesPerAxisInPatch; x++) {
    const int lowerVoxelInPreimage =  x+1
                                   +  y * (2 + numberOfVolumesPerAxisInPatch);
    const int upperVoxelInPreimage =  x+1
                                   + (y+1) * (2 + numberOfVolumesPerAxisInPatch);

    const int lowerVoxelInImage    = x
                                   + (y-1) * numberOfVolumesPerAxisInPatch;
    const int upperVoxelInImage    = x
                                   + y * numberOfVolumesPerAxisInPatch;
    splitRiemann1d(
      flux, eigenvalues,
      Qin + lowerVoxelInPreimage*unknowns,
      Qin + upperVoxelInPreimage*unknowns,
      x, volumeH(0), t, dt, 1, //  last argument = normal
      unknowns,
      numericalFlux
    );

    for (int unknown=0; unknown<unknowns; unknown++) {
      if (y>0) {
        Qout[ lowerVoxelInImage*unknowns+unknown ] -= dt / volumeH(0) * numericalFlux[unknown];
      }
      if (y<numberOfVolumesPerAxisInPatch) {
        Qout[ upperVoxelInImage*unknowns+unknown ] += dt / volumeH(0) * numericalFlux[unknown];
      }
    }
  }

  logTraceOut( "applyRusanovToPatch_FaceLoops2d(...)" );
}
