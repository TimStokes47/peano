#include "SetupScenario.h"


tarch::logging::Log examples::algebraicmg::mappings::SetupScenario::_log( "examples::algebraicmg::mappings::SetupScenario");


examples::algebraicmg::mappings::SetupScenario::SetupScenario(int treeNumber) {
// @todo Please implement
}


examples::algebraicmg::mappings::SetupScenario::~SetupScenario() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::mappings::SetupScenario::getGridControlEvents() {
// @todo Please implement
return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::mappings::SetupScenario::beginTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::endTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::createPersistentVertex(
  const tarch::la::Vector<Dimensions,double>& center,
  const tarch::la::Vector<Dimensions,double>& h,
  examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG
) {
}


void examples::algebraicmg::mappings::SetupScenario::destroyPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::createHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::destroyHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::touchVertexFirstTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::touchVertexLastTime(
  const tarch::la::Vector<Dimensions,double>& center,
  const tarch::la::Vector<Dimensions,double>& h,
  examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG
) {
  if (
    tarch::la::oneEquals(center,0.0)
    or
    tarch::la::oneEquals(center,1.0)
  ) {
    fineGridVertexMG.setVertexType( examples::algebraicmg::vertexdata::MG::VertexType::Boundary );
    fineGridVertexMG.setU( 4.0 );
    fineGridVertexMG.setRhs( 0.0 );
  }
  else {
    fineGridVertexMG.setVertexType( examples::algebraicmg::vertexdata::MG::VertexType::Inside );
    fineGridVertexMG.setU( -1.0 );
    fineGridVertexMG.setRhs( 0.0 );
  }
}


void examples::algebraicmg::mappings::SetupScenario::touchCellFirstTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


void examples::algebraicmg::mappings::SetupScenario::touchCellLastTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


