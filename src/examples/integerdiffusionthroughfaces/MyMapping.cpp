#include "MyMapping.h"


void examples::integerdiffusionthroughfaces::MyMapping::createPersistentFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {
  data = 22;
}


void examples::integerdiffusionthroughfaces::MyMapping::createHangingFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {
  data = 22;
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyPersistentFace(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::destroyHangingFace(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchFaceFirstTime(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchFaceLastTime(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::createCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {
  data = 22;
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::enterCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::leaveCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {

}

