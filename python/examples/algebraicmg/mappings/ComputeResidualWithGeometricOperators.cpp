#include "ComputeResidualWithGeometricOperators.h"
#include "SetupScenario.h"


#include "toolbox/finiteelements/ElementMatrix.h"
#include "toolbox/finiteelements/StencilFactory.h"


tarch::logging::Log examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::_log( "examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators");


examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::ComputeResidualWithGeometricOperators(int treeNumber) {
  _localStiffnessMatrixOneIntegrationPoint =
    toolbox::finiteelements::getElementWiseAssemblyMatrix(
      toolbox::finiteelements::getLaplacian(1.0)
    );
  logDebug( "ComputeResidualWithGeometricOperators(...)", "local element matrix " << _localStiffnessMatrixOneIntegrationPoint );
  logDebug( "ComputeResidualWithGeometricOperators(...)", "constructed from stencil " << toolbox::finiteelements::getLaplacian(1.0));
}


examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::~ComputeResidualWithGeometricOperators() {
}


std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::getGridControlEvents() {
  return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::beginTraversal() {
}


void examples::algebraicmg::mappings::ComputeResidualWithGeometricOperators::endTraversal() {
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
	// @todo Nur auf dem Feingitter
  tarch::la::Vector<TwoPowerD, double> u;
  tarch::la::Vector<TwoPowerD, double> r;

  // gather solution values
  for (int i=0; i<TwoPowerD; i++) {
    u(i) = fineGridVerticesMG(i).getU();
  }

  if ( fineGridCellA.data().entries.empty() ) {
    tarch::la::Matrix<TwoPowerD,TwoPowerD,double>  localStiffnessMatrix =
      toolbox::finiteelements::getPoissonMatrixWithJumpingCoefficient(
        fineGridCellA.centre(), fineGridCellA.h(), 1,
		[](const tarch::la::Vector<Dimensions,double>& x) -> double {
          return SetupScenario::getEpsilon(x);
		}
	  );

    for (int row=0; row<TwoPowerD; row++)
    for (int col=0; col<TwoPowerD; col++) {
/*
      // Seems to be too restrictive, i.e. fails in the digit eight or nine.

      assertionNumericalEquals(
        localStiffnessMatrix(row,col),
		tarch::la::pow(fineGridCellA.h()(0), (double)(Dimensions-2))
        * SetupScenario::getEpsilon(fineGridCellA.centre())
        * _localStiffnessMatrixOneIntegrationPoint(row,col)
      );
*/
      fineGridCellA.data().entries.push_back( localStiffnessMatrix(row,col) );
    }


/*
    const double scaling = tarch::la::pow(fineGridCellA.h()(0), (double)(Dimensions-2))
	                       * SetupScenario::getEpsilon(fineGridCellA.centre());
    for (int row=0; row<TwoPowerD; row++)
    for (int col=0; col<TwoPowerD; col++) {
      fineGridCellA.data().entries.push_back( scaling * _localStiffnessMatrixOneIntegrationPoint(row,col) );
    }
*/
  }

  tarch::la::Matrix<TwoPowerD,TwoPowerD,double>  localStiffnessMatrix;
  for (int row=0; row<TwoPowerD; row++)
  for (int col=0; col<TwoPowerD; col++) {
    localStiffnessMatrix(row,col) = fineGridCellA.data().entries[ row*TwoPowerD + col ];
  }

  // compute residual contribution. Mind the minus sign here that
  // results from the residual's definition: r = b-Au
  const double scaling = tarch::la::pow(fineGridCellA.h()(0), (double)(Dimensions-2));
  r = - scaling * _localStiffnessMatrixOneIntegrationPoint * u;

  // scatter residual contributions
  for (int i=0; i<TwoPowerD; i++) {
    fineGridVerticesMG(i).setRes(  fineGridVerticesMG(i).getRes() + r(i) );
    fineGridVerticesMG(i).setDiag( fineGridVerticesMG(i).getDiag() + scaling * _localStiffnessMatrixOneIntegrationPoint(i,i) );
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

