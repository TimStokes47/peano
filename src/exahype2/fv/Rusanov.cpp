#include "Rusanov.h"
#include "../PatchUtils.h"

#include "tarch/Assertions.h"

#include "peano4/utils/Loop.h"


void exahype2::fv::applyRusanovToPatch(
  std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        const tarch::la::Vector<Dimensions,double>&  t,
        const tarch::la::Vector<Dimensions,double>&  dt,
        int                                          normal,
        double                                       F[]
  ) >   flux,
  std::function< void(
        double                                       Q[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        const tarch::la::Vector<Dimensions,double>&  t,
        const tarch::la::Vector<Dimensions,double>&  dt,
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

  auto plotVolume = [&unknowns](double Q[]) -> std::string {
    std::string result = "(" + std::to_string(Q[0]);
    for (int i=1; i<unknowns; i++) result += "," + std::to_string(Q[i]);
    result += ")";
    return result;
  };

  auto splitRiemann1d = [&flux, &eigenvalues, &unknowns](double QL[], double QR[], const tarch::la::Vector<Dimensions,double>& x, double dx, double t, double dt, int normal, double F[]) -> void {
    double averageQ[unknowns];
    for (int unknown=0; unknown<unknowns; unknown++) {{
      averageQ[unknown] = 0.5 * (QL[unknown] + QR[unknown]);
      assertion4(averageQ[unknown]==averageQ[unknown], x, dx, dt, normal);
    }}

    double averageF[unknowns];
    double lambdas[unknowns];
    flux(averageQ,x,dx,t,dt,normal % Dimensions,averageF);

    double lambdaMax = 0.0;
    eigenvalues(QL,x,dx,t,dt,normal % Dimensions,lambdas);
    for (int unknown=0; unknown<unknowns; unknown++) {{
      assertion(lambdas[unknown]==lambdas[unknown]);
      lambdaMax = std::max(lambdaMax,lambdas[unknown]);
    }}
    eigenvalues(QR,x,dx,t,dt,normal % Dimensions,lambdas);
    for (int unknown=0; unknown<unknowns; unknown++) {{
      assertion(lambdas[unknown]==lambdas[unknown]);
      lambdaMax = std::max(lambdaMax,lambdas[unknown]);
    }}

    for (int unknown=0; unknown<unknowns; unknown++) {
      assertion( QR[unknown]==QR[unknown] );
      assertion( QL[unknown]==QL[unknown] );
      F[unknown] = averageF[unknown] - 0.5 * lambdaMax * (QR[unknown] - QL[unknown]);
      assertion9( F[unknown]==F[unknown], averageF[unknown], lambdas[unknown], QR[unknown], QL[unknown], unknown, x, dx, dt, normal );
    }
  };

  assertion( dt>=tarch::la::NUMERICAL_ZERO_DIFFERENCE );

  tarch::la::Vector<Dimensions,double> volumeH = exahype2::getVolumeSize(patchSize, numberOfVolumesPerAxisInPatch);

  dfor(cell,numberOfVolumesPerAxisInPatch) {
    tarch::la::Vector<Dimensions,int> currentVoxel = cell + tarch::la::Vector<Dimensions,int>(1);
    int currentVoxelSerialised = peano4::utils::dLinearised(currentVoxel,numberOfVolumesPerAxisInPatch + 2);
    logDebug( "applyRusanovToPatch(...)", "handle volume " << cell << " in destination patch, i.e. source volume " << currentVoxel << ": " << plotVolume(Qin + currentVoxelSerialised*unknowns) << " [" << currentVoxelSerialised << "]" );

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

      logDebug( "applyRusanovToPatch(...)", "handle neighbour " << neighbourVolume << ": " << plotVolume(Qin + neighbourVolumeSerialised*unknowns) << " [" << neighbourVolumeSerialised << "]");
      splitRiemann1d(
        Qin + neighbourVolumeSerialised*unknowns,
        Qin + currentVoxelSerialised*unknowns,
        x, volumeH(d), t, dt, d,
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

      logDebug( "applyRusanovToPatch(...)", "handle neighbour " << neighbourVolume << ": " << plotVolume(Qin + neighbourVolumeSerialised*unknowns) << " [" << neighbourVolumeSerialised << "]" );
      splitRiemann1d(
        Qin + currentVoxelSerialised*unknowns,
        Qin + neighbourVolumeSerialised*unknowns,
        x, volumeH(d), t, dt, d + Dimensions,
        numericalFlux
      );
      for (int unknown=0; unknown<unknowns; unknown++) {
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

