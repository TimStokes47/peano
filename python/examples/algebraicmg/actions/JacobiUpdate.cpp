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


void examples::algebraicmg::actions::JacobiUpdate::beginTraversal() {}

void examples::algebraicmg::actions::JacobiUpdate::endTraversal() {}

void examples::algebraicmg::actions::JacobiUpdate::createPersistentVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::JacobiUpdate::destroyPersistentVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::JacobiUpdate::createHangingVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::JacobiUpdate::destroyHangingVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::JacobiUpdate::touchVertexFirstTime(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}


void examples::algebraicmg::actions::JacobiUpdate::touchVertexLastTime(
  const peano4::datamanagement::VertexMarker& marker,
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
    assertion2( fineGridVertexMG.getDiag()>0.0, fineGridVertexMG.toString(), marker.toString() );
    fineGridVertexMG.setU(
      fineGridVertexMG.getU()
      +
      omega * fineGridVertexMG.getRes() / fineGridVertexMG.getDiag()
    );
  }
  else if (
    fineGridVertexMG.getVertexType()==examples::algebraicmg::vertexdata::MG::VertexType::Boundary
  ) {
    fineGridVertexMG.setU( SetupScenario::getSolution(marker.x()) );
  }
}


void examples::algebraicmg::actions::JacobiUpdate::touchCellFirstTime(
      const peano4::datamanagement::CellMarker& marker,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}


void examples::algebraicmg::actions::JacobiUpdate::touchCellLastTime(
      const peano4::datamanagement::CellMarker& marker,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}


