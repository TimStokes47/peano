#include "MyMapping.h"


tarch::logging::Log   examples::integerdiffusionthroughfaces::MyMapping::_log( "examples::integerdiffusionthroughfaces::MyMapping" );



void examples::integerdiffusionthroughfaces::MyMapping::beginTraversal() {

}


void examples::integerdiffusionthroughfaces::MyMapping::endTraversal() {

}


void examples::integerdiffusionthroughfaces::MyMapping::createPersistentFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {
  logTraceIn( "createPersistentFace(...)" );
  data = 22;
  logTraceOut( "createPersistentFace(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::createHangingFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {
  logTraceIn( "createHangingFace(...)" );
  data = 22;
  logTraceOut( "createHangingFace(...)" );
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
  logTraceInWith4Arguments( "touchFaceFirstTime(...)", x, h, normal, data );
  logTraceOut( "touchFaceFirstTime(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::touchFaceLastTime(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int&                                         data
) {
  logTraceIn( "touchFaceLastTime(...)" );
  logTraceOut( "touchFaceLastTime(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::createCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {
  logTraceIn( "createCell(...)" );
  data = 22;
  logTraceOut( "createCell(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {
  logTraceIn( "touchCellFirstTime(...)" );
  logTraceOut( "touchCellFirstTime(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::touchCellLastTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int&                                         data
) {
  logTraceIn( "touchCellLastTime(...)" );
  logTraceOut( "touchCellLastTime(...)" );
}

