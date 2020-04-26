#include "BoundaryConditions.h"

#include "../PatchUtils.h"


#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"


void exahype2::fv::applyBoundaryConditions(
  std::function< void(
    double                                       Qinside[],
    double                                       Qoutside[],
    const tarch::la::Vector<Dimensions,double>&  faceCentre,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    int                                          normal
  ) >   boundaryCondition,
  const tarch::la::Vector<Dimensions,double>&  faceCentre,
  const tarch::la::Vector<Dimensions,double>&  patchSize,
  double                                       t,
  double                                       dt,
  int                                          numberOfVolumesPerAxisInPatch,
  int                                          unknowns,
  int                                          faceNumber,
  double                                       Q[]
) {
  auto serialisePatchIndex = [&](tarch::la::Vector<Dimensions,int> overlapCell) {{
    int base   = 1;
    int result = 0;
    for (int d=0; d<Dimensions; d++) {{
      result += overlapCell(d) * base;
      if (d==faceNumber % Dimensions) {{
        base *= 2;
      }}
      else {{
        base *= numberOfVolumesPerAxisInPatch;
      }}
    }}
    return result;
  }};

  tarch::la::Vector<Dimensions,double> volumeH = exahype2::getVolumeSize(patchSize, numberOfVolumesPerAxisInPatch);

  dfore(volume,numberOfVolumesPerAxisInPatch,faceNumber % Dimensions,0) {
    tarch::la::Vector<Dimensions,int> insideVolume = volume;
    tarch::la::Vector<Dimensions,int> outsideVolume = volume;

    if (faceNumber<Dimensions) {
      insideVolume(faceNumber % Dimensions)  = 1;
      outsideVolume(faceNumber % Dimensions) = 0;
    }
    else {
      insideVolume(faceNumber % Dimensions)  = 0;
      outsideVolume(faceNumber % Dimensions) = 1;
    }

    int insideVolumeSerialised  = serialisePatchIndex(insideVolume);
    int outsideVolumeSerialised = serialisePatchIndex(outsideVolume);

    boundaryCondition(
      Q + insideVolumeSerialised * unknowns,
      Q + outsideVolumeSerialised * unknowns,
      faceCentre, volumeH, t, dt, faceNumber
    );
  }
}


