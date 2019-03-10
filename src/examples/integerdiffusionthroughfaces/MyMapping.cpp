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
  const tarch::la::Vector<Dimensions,double>&  normal,
  FaceData&                                    data
) {
  logTraceIn( "createPersistentFace(...)" );
  data.value = 0;
  logTraceOut( "createPersistentFace(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::createHangingFace(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  normal,
  FaceData&                                    data
) {
  logTraceIn( "createHangingFace(...)" );
  data.value = 0;
  logTraceOut( "createHangingFace(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyPersistentFace(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  normal,
  FaceData&                                    data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::destroyHangingFace(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  normal,
  FaceData&                                    data
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchFaceFirstTime(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  const tarch::la::Vector<Dimensions,double>&  normal,
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
  const tarch::la::Vector<Dimensions,double>&  normal,
  FaceData&                                    data
) {
  logTraceIn( "touchFaceLastTime(...)" );
  logTraceOut( "touchFaceLastTime(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::createCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces
) {
  logTraceIn( "createCell(...)" );
  data.value = 0;
  logTraceOut( "createCell(...)" );
}


void examples::integerdiffusionthroughfaces::MyMapping::destroyCell(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces
) {

}


void examples::integerdiffusionthroughfaces::MyMapping::touchCellFirstTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces
) {
  logTraceInWith1Argument( "touchCellFirstTime(...)", data.toString() );
  const int oldValue = data.value;

  if (
    center(0) > 0.2 and center (0) <0.4 and
    center(1) > 0.2 and center (1) <0.4
  ) {
    data.value = 30;
  }
  else {
    data.value = oldValue-1;
    for (int i=0; i<TwoTimesD; i++) {
      data.value = std::max( data.value, faces(i).oldValue/2 );
    }
  }

  for (int i=0; i<TwoTimesD; i++) {
    faces(i).value += data.value;
  }

  logTraceOutWith1Argument( "touchCellFirstTime(...)", data.toString() );
}


void examples::integerdiffusionthroughfaces::MyMapping::touchCellLastTime(
  const tarch::la::Vector<Dimensions,double>&  center,
  const tarch::la::Vector<Dimensions,double>&  h,
  CellData&                                    data,
  Faces&                                       faces
) {
  logTraceInWith1Argument( "touchCellLastTime(...)", data.toString() );
  logTraceOutWith1Argument( "touchCellLastTime(...)", data.toString() );
}

