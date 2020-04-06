#include "PatchUtils.h"

#include "tarch/Assertions.h"



tarch::la::Vector<Dimensions,double>  exahype2::getVolumeSize(
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,int>&     numberOfVolumesPerPatch
) {
  tarch::la::Vector<Dimensions,double> volumeSize;
  for (int d=0; d<Dimensions; d++) {
    assertion2( numberOfVolumesPerPatch(d)>=1, h, numberOfVolumesPerPatch );
    volumeSize(d) = h(d)/numberOfVolumesPerPatch(d);
  }
  return volumeSize;
}


tarch::la::Vector<Dimensions,double>  exahype2::getVolumeCentre(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,int>&     numberOfVolumesPerPatch,
  const tarch::la::Vector<Dimensions,int>&     index
) {
  tarch::la::Vector<Dimensions,double> volumeSize = getVolumeSize(h,numberOfVolumesPerPatch);
  return x - 0.5 * h + 0.5 * volumeSize + tarch::la::multiplyComponents( index.convertScalar<double>(), h);
}
