#include "PatchUtils.h"

#include "tarch/Assertions.h"



tarch::la::Vector<Dimensions,double>  exahype2::getVolumeSize(
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          numberOfVolumesPerAxisInPatch
) {
  tarch::la::Vector<Dimensions,double> volumeSize;
  for (int d=0; d<Dimensions; d++) {
    assertion2( numberOfVolumesPerAxisInPatch>=1, h, numberOfVolumesPerAxisInPatch );
    volumeSize(d) = h(d)/numberOfVolumesPerAxisInPatch;
  }
  return volumeSize;
}


tarch::la::Vector<Dimensions,double>  exahype2::getVolumeCentre(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          numberOfVolumesPerAxisInPatch,
  const tarch::la::Vector<Dimensions,int>&     index
) {
  tarch::la::Vector<Dimensions,double> volumeSize = getVolumeSize(h,numberOfVolumesPerAxisInPatch);
  return x - 0.5 * h + 0.5 * volumeSize + tarch::la::multiplyComponents( index.convertScalar<double>(), volumeSize);
}
