#include "ComputeResidualWithGeometricOperators.h"


#include "toolbox/finiteelements/ElementMatrix.h"
#include "toolbox/finiteelements/StencilFactory.h"


tarch::logging::Log examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::_log( "examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators");


examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::ComputeResidualWithGeometricOperators(int treeNumber) {
  _localStiffnessMatrix =
    toolbox::finiteelements::getElementWiseAssemblyMatrix(
      toolbox::finiteelements::getLaplacian(1.0)
    );
}


examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::~ComputeResidualWithGeometricOperators() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::getGridControlEvents() {
// @todo Please implement
return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::beginTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::endTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::createPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::destroyPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::createHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::destroyHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchVertexFirstTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchVertexLastTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchCellFirstTime(
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  peano4::datamanagement::CellWrapper<void> fineGridCell,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
  peano4::datamanagement::CellWrapper<void> coarseGridCell
) {
  tarch::la::Vector<TwoPowerD, double> u;
  tarch::la::Vector<TwoPowerD, double> r;

  // gather solution values
  for (int i=0; i<TwoPowerD; i++) {
    u(i) = fineGridVerticesMG(i).getU();
  }

  r = tarch::la::volume(fineGridCell.h()) * _localStiffnessMatrix * u;

  // scatter residual contributions
  for (int i=0; i<TwoPowerD; i++) {
    fineGridVerticesMG(i).setRes(  fineGridVerticesMG(i).getRes() + r(i) );
    fineGridVerticesMG(i).setDiag( fineGridVerticesMG(i).getDiag() + tarch::la::volume(fineGridCell.h()) * _localStiffnessMatrix(i,i) );
  }
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchCellLastTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


