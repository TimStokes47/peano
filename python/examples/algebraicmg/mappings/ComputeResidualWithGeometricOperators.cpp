#include "ComputeResidualWithGeometricOperators.h"


#include "toolbox/finiteelements/ElementMatrix.h"
#include "toolbox/finiteelements/StencilFactory.h"


tarch::logging::Log examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::_log( "examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators");


examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::ComputeResidualWithGeometricOperators(int treeNumber) {
  _localStiffnessMatrix =
    toolbox::finiteelements::getElementWiseAssemblyMatrix(
      toolbox::finiteelements::getLaplacian(1.0)
    );
  logDebug( "ComputeResidualWithGeometricOperators(...)", "local element matrix " << _localStiffnessMatrix );
  logDebug( "ComputeResidualWithGeometricOperators(...)", "constructed from stencil " << toolbox::finiteelements::getLaplacian(1.0));
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
	      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
	      peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA
) {
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::destroyPersistentVertex(
	      const tarch::la::Vector<Dimensions,double>& center,
	      const tarch::la::Vector<Dimensions,double>& h,
	      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
	      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
	      peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA) {
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::createHangingVertex(
	      const tarch::la::Vector<Dimensions,double>& center,
	      const tarch::la::Vector<Dimensions,double>& h,
	      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
	      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
	      peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA) {
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::destroyHangingVertex(
	      const tarch::la::Vector<Dimensions,double>& center,
	      const tarch::la::Vector<Dimensions,double>& h,
	      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
	      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
	      peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA) {
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchVertexFirstTime(
	      const tarch::la::Vector<Dimensions,double>& center,
	      const tarch::la::Vector<Dimensions,double>& h,
	      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
	      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
	      peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA) {
  fineGridVertexMG.setRes(0.0);
  fineGridVertexMG.setDiag(0.0);
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchVertexLastTime(
	      const tarch::la::Vector<Dimensions,double>& center,
	      const tarch::la::Vector<Dimensions,double>& h,
	      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
	      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
	      peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA
) {
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchCellFirstTime(
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> fineGridCellA,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
  peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA
) {
  tarch::la::Vector<TwoPowerD, double> u;
  tarch::la::Vector<TwoPowerD, double> r;

  // gather solution values
  for (int i=0; i<TwoPowerD; i++) {
    u(i) = fineGridVerticesMG(i).getU();
  }

  // compute residual contribution. Mind the minus sign here that
  // results from the residual's definition: r = b-Au
  const double scaling = tarch::la::pow(fineGridCellA.h()(0), (double)(Dimensions-2));
  r = - scaling * _localStiffnessMatrix * u;

  // scatter residual contributions
  for (int i=0; i<TwoPowerD; i++) {
    fineGridVerticesMG(i).setRes(  fineGridVerticesMG(i).getRes() + r(i) );
    fineGridVerticesMG(i).setDiag( fineGridVerticesMG(i).getDiag() + scaling * _localStiffnessMatrix(i,i) );
  }
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::touchCellLastTime(
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> fineGridCellA,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
  peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA
) {
}



void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::createCell(
  const tarch::la::Vector<Dimensions,double>& center,
  const tarch::la::Vector<Dimensions,double>& h,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  examples::algebraicmg::celldata::A& fineGridCellA,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
  peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA
) {}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::destroyCell(
  const tarch::la::Vector<Dimensions,double>& center,
  const tarch::la::Vector<Dimensions,double>& h,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  examples::algebraicmg::celldata::A& fineGridCellA,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
  peano4::datamanagement::CellWrapper<examples::algebraicmg::celldata::A> coarseGridCellA
) {}

