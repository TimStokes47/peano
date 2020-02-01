#include "JacobiUpdate.h"


tarch::logging::Log examples::algebraicmg::mappings::JacobiUpdate::_log( "examples::algebraicmg::mappings::JacobiUpdate");


examples::algebraicmg::mappings::JacobiUpdate::JacobiUpdate(int treeNumber) {
// @todo Please implement
}


examples::algebraicmg::mappings::JacobiUpdate::~JacobiUpdate() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::mappings::JacobiUpdate::getGridControlEvents() {
// @todo Please implement
return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::mappings::JacobiUpdate::beginTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::endTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::createPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::destroyPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::createHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::destroyHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::touchVertexFirstTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
  fineGridVertexMG.setRes(0.0);
  fineGridVertexMG.setDiag(0.0);
}


void examples::algebraicmg::mappings::JacobiUpdate::touchVertexLastTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
  const double omega = 0.7;
  if (fineGridVertexMG.getVertexType()==examples::algebraicmg::vertexdata::MG::VertexType::Inside) {
    assertion3( fineGridVertexMG.getDiag()>0.0, fineGridVertexMG.toString(), center, h );
    fineGridVertexMG.setU(
      fineGridVertexMG.getU()
      +
      omega * fineGridVertexMG.getRes() / fineGridVertexMG.getDiag()
    );
  }
}


void examples::algebraicmg::mappings::JacobiUpdate::touchCellFirstTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


void examples::algebraicmg::mappings::JacobiUpdate::touchCellLastTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


