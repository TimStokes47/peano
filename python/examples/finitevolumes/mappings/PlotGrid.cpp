#include "PlotGrid.h"
#include "Constants.h"

#include "peano4/utils/Loop.h"


tarch::logging::Log examples::finitevolumes::mappings::PlotGrid::_log( "examples::finitevolumes::mappings::PlotGrid");


examples::finitevolumes::mappings::PlotGrid::PlotGrid(int treeNumber) {
// @todo Please implement
}


examples::finitevolumes::mappings::PlotGrid::~PlotGrid() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::finitevolumes::mappings::PlotGrid::getGridControlEvents() {
  std::vector< peano4::grid::GridControlEvent > result;

  result.push_back(peano4::grid::GridControlEvent(
    peano4::grid::GridControlEvent::RefinementControl::Refine,
    tarch::la::Vector<Dimensions,double>(0.0),
    tarch::la::Vector<Dimensions,double>(1.0),
    tarch::la::Vector<Dimensions,double>(0.1)
  ));

  return result;
}


void examples::finitevolumes::mappings::PlotGrid::beginTraversal(
      ) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::endTraversal(
      ) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::createPersistentFace(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      const tarch::la::Vector<Dimensions,double>& normal,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::destroyPersistentFace(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      const tarch::la::Vector<Dimensions,double>& normal,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::createHangingFace(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      const tarch::la::Vector<Dimensions,double>& normal,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::destroyHangingFace(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      const tarch::la::Vector<Dimensions,double>& normal,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::touchFaceFirstTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      const tarch::la::Vector<Dimensions,double>& normal,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::touchFaceLastTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      const tarch::la::Vector<Dimensions,double>& normal,
      examples::finitevolumes::facedata::Q& fineGridFaceQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::createCell(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      examples::finitevolumes::celldata::Q& fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
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


void examples::finitevolumes::mappings::PlotGrid::destroyCell(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      examples::finitevolumes::celldata::Q& fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::touchCellFirstTime(
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


void examples::finitevolumes::mappings::PlotGrid::touchCellLastTime(
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> fineGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> fineGridCellQ,
      peano4::datamanagement::FaceEnumerator<examples::finitevolumes::facedata::Q> coarseGridFacesQ,
      peano4::datamanagement::CellWrapper<examples::finitevolumes::celldata::Q> coarseGridCellQ
) {
// @todo Please implement
}


