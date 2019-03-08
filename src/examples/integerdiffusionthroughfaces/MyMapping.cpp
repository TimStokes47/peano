#include "MyMapping.h"
#include "FaceData.h"
#include "CellData.h"


tarch::logging::Log   examples::integerdiffusionthroughfaces::MyMapping::_log( "examples::integerdiffusionthroughfaces::MyMapping" );



void examples::integerdiffusionthroughfaces::MyMapping::beginTraversal() {

}


void examples::integerdiffusionthroughfaces::MyMapping::endTraversal() {

}


void examples::integerdiffusionthroughfaces::MyMapping::createPersistentFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  FaceData&                                    data
) {
  logTraceIn( "createPersistentFace(...)" );
  data.value = 22;
  logTraceOut( "createPersistentFace(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::createHangingFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  FaceData&                                    data
) {
  logTraceIn( "createHangingFace(...)" );
  data.value = 22;
  logTraceOut( "createHangingFace(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyPersistentFace(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  FaceData&                                    data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::destroyHangingFace(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  FaceData&                                    data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchFaceFirstTime(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  FaceData&                                    data
) {
  logTraceInWith4Arguments( "touchFaceFirstTime(...)", x, h, normal, data.toString() );
  data.oldValue = data.value;
  data.value    = 0;
  logTraceOut( "touchFaceFirstTime(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::touchFaceLastTime(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  FaceData&                                    data
) {
  logTraceIn( "touchFaceLastTime(...)" );
  logTraceOut( "touchFaceLastTime(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::createCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data
) {
  logTraceIn( "createCell(...)" );
  data.value = 22;
  logTraceOut( "createCell(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data
) {
  logTraceInWith1Argument( "touchCellFirstTime(...)", data.toString() );
  const int oldValue = data.value;
  data.value = oldValue-1;
  logTraceOutWith1Argument( "touchCellFirstTime(...)", data.toString() );
}


void examples::integerdiffusionthroughfaces::MyMapping::touchCellLastTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data
) {
  logTraceInWith1Argument( "touchCellLastTime(...)", data.toString() );
  logTraceOutWith1Argument( "touchCellLastTime(...)", data.toString() );
}

