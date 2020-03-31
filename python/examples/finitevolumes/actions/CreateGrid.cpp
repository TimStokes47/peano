#include "CreateGrid.h"


tarch::logging::Log examples::finitevolumes::actions::CreateGrid::_log( "examples::finitevolumes::actions::CreateGrid");


examples::finitevolumes::actions::CreateGrid::CreateGrid(int treeNumber) {
// @todo Please implement
}


examples::finitevolumes::actions::CreateGrid::~CreateGrid() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::finitevolumes::actions::CreateGrid::getGridControlEvents() {
// @todo Please implement
return std::vector< peano4::grid::GridControlEvent >();
}


void examples::finitevolumes::actions::CreateGrid::beginTraversal(
      ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "beginTraversal()" );
// @todo Please implement
// logTraceOut( "beginTraversal()" );
}


void examples::finitevolumes::actions::CreateGrid::endTraversal(
      ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "endTraversal()" );
// @todo Please implement
// logTraceOut( "endTraversal()" );
}


void examples::finitevolumes::actions::CreateGrid::createPersistentFace(
      const peano4::datamanagement::FaceMarker& marker,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "createPersistentFace()" );
// @todo Please implement
// logTraceOut( "createPersistentFace()" );
}


void examples::finitevolumes::actions::CreateGrid::destroyPersistentFace(
      const peano4::datamanagement::FaceMarker& marker,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "destroyPersistentFace()" );
// @todo Please implement
// logTraceOut( "destroyPersistentFace()" );
}


void examples::finitevolumes::actions::CreateGrid::createHangingFace(
      const peano4::datamanagement::FaceMarker& marker,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "createHangingFace()" );
// @todo Please implement
// logTraceOut( "createHangingFace()" );
}


void examples::finitevolumes::actions::CreateGrid::destroyHangingFace(
      const peano4::datamanagement::FaceMarker& marker,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "destroyHangingFace()" );
// @todo Please implement
// logTraceOut( "destroyHangingFace()" );
}


void examples::finitevolumes::actions::CreateGrid::touchFaceFirstTime(
      const peano4::datamanagement::FaceMarker& marker,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "touchFaceFirstTime()" );
// @todo Please implement
// logTraceOut( "touchFaceFirstTime()" );
}


void examples::finitevolumes::actions::CreateGrid::touchFaceLastTime(
      const peano4::datamanagement::FaceMarker& marker,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "touchFaceLastTime()" );
// @todo Please implement
// logTraceOut( "touchFaceLastTime()" );
}


void examples::finitevolumes::actions::CreateGrid::createCell(
  const peano4::datamanagement::CellMarker& marker,
  peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
  examples::finitevolumes::celldata::Q& fineGridCellQ,
  peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
  examples::finitevolumes::celldata::Q& coarseGridCellQ
) {
  const double subcellWidth = h(0) / PatchSize;
  dfor(k,PatchSize) {
    const tarch::la::Vector<Dimensions,double> subcellCentre =
      center - 0.5*h + k.convertScalar<double>() * subcellWidth + subcellWidth;
    int dofIndex = peano4::utils::dLinearised(k,PatchSize) * NumberOfUnknownsPerCell;
    double initialValue = 0.01;
    if (
      tarch::la::norm2( subcellCentre-tarch::la::Vector<Dimensions,double>(0.5) ) < 0.3
    ) {
      initialValue = 1.0;
    }
    for (int i=0; i<NumberOfUnknownsPerCell; i++) {
      fineGridCellQ.value[dofIndex] = 0.0;
      dofIndex++;
    }
    fineGridCellQ.value[dofIndex-1] = initialValue;
  }
}


void examples::finitevolumes::actions::CreateGrid::destroyCell(
      const peano4::datamanagement::CellMarker& marker,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      examples::finitevolumes::celldata::Q& fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "destroyCell()" );
// @todo Please implement
// logTraceOut( "destroyCell()" );
}


void examples::finitevolumes::actions::CreateGrid::touchCellFirstTime(
      const peano4::datamanagement::CellMarker& marker,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      examples::finitevolumes::celldata::Q& fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "touchCellFirstTime()" );
// @todo Please implement
// logTraceOut( "touchCellFirstTime()" );
}


void examples::finitevolumes::actions::CreateGrid::touchCellLastTime(
      const peano4::datamanagement::CellMarker& marker,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      examples::finitevolumes::celldata::Q& fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      examples::finitevolumes::celldata::Q& coarseGridCellQ) {

// Feel free to comment in and to add arguments if you want to trace them.
// Ensure the logTraceOut is activated, too, if you switch on traceIn.     
// logTraceIn( "touchCellLastTime()" );
// @todo Please implement
// logTraceOut( "touchCellLastTime()" );
}


