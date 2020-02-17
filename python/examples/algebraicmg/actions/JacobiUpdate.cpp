#include "JacobiUpdate.h"
#include "SetupScenario.h"


tarch::logging::Log examples::algebraicmg::actions::JacobiUpdate::_log( "examples::algebraicmg::actions::JacobiUpdate");


examples::algebraicmg::actions::JacobiUpdate::JacobiUpdate(int treeNumber) {
// @todo Please implement
}


examples::algebraicmg::actions::JacobiUpdate::~JacobiUpdate() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::actions::JacobiUpdate::getGridControlEvents() {
// @todo Please implement
return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::actions::JacobiUpdate::beginTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::endTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::createPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::destroyPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::createHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::destroyHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::touchVertexFirstTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::touchVertexLastTime(
  const tarch::la::Vector<Dimensions,double>& center,
  const tarch::la::Vector<Dimensions,double>& h,
  examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG
) {
  const double omega = 0.7;

  if (
    fineGridVertexMG.getVertexType()==examples::algebraicmg::vertexdata::MG::VertexType::Inside
	and
	fineGridVertexMG.getDiag()>0.0
  ) {
    logDebug( "touchVertexLastTime(...)", "update " << fineGridVertexMG.toString() )
    assertion3( fineGridVertexMG.getDiag()>0.0, fineGridVertexMG.toString(), center, h );
    fineGridVertexMG.setU(
      fineGridVertexMG.getU()
      +
      omega * fineGridVertexMG.getRes() / fineGridVertexMG.getDiag()
    );
  }
  else if (
    fineGridVertexMG.getVertexType()==examples::algebraicmg::vertexdata::MG::VertexType::Boundary
  ) {
    fineGridVertexMG.setU( SetupScenario::getSolution(center) );
  }
}


void examples::algebraicmg::actions::JacobiUpdate::touchCellFirstTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


void examples::algebraicmg::actions::JacobiUpdate::touchCellLastTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


