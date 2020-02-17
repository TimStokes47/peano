#include "ComputeResidualWithGeometricOperators.h"
#include "SetupScenario.h"


#include "tarch/la/Matrix.h"


#include "toolbox/finiteelements/ElementMatrix.h"
#include "toolbox/finiteelements/StencilFactory.h"


tarch::logging::Log examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::_log( "examples::algebraicmg::actions::ComputeResidualWithGeometricOperators");


examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::ComputeResidualWithGeometricOperators(int treeNumber) {
}


examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::~ComputeResidualWithGeometricOperators() {
// @todo Please implement
}


std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::getGridControlEvents() {
// @todo Please implement
return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::beginTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::endTraversal(
      ) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::createPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::destroyPersistentVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::createHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::destroyHangingVertex(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchVertexFirstTime(
  const tarch::la::Vector<Dimensions,double>& center,
  const tarch::la::Vector<Dimensions,double>& h,
  examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG
) {
  fineGridVertexMG.setRes(0.0);
  fineGridVertexMG.setDiag(0.0);
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchVertexLastTime(
      const tarch::la::Vector<Dimensions,double>& center,
      const tarch::la::Vector<Dimensions,double>& h,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {
// @todo Please implement
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchCellFirstTime(
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  peano4::datamanagement::CellWrapper<void> fineGridCell,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
  peano4::datamanagement::CellWrapper<void> coarseGridCell
) {
  if (not fineGridCell.isRefined()) {
    tarch::la::Vector<TwoPowerD, double> u;
    tarch::la::Vector<TwoPowerD, double> r;

    // gather solution values
    for (int i=0; i<TwoPowerD; i++) {
      u(i) = fineGridVerticesMG(i).getU();
    }

    const double epsilon = SetupScenario::getEpsilon(fineGridCell.centre());

    // Holds the stiffness matrix for the Laplacian. Not the most sophisticated
    // version. Obviously, we could compute it once for h=1.0 and then just
    // rescale it. But in this code, I prefer to have everything in one place.
    tarch::la::Matrix<TwoPowerD,TwoPowerD,double> localStiffnessMatrix =
      toolbox::finiteelements::getElementWiseAssemblyMatrix(
        toolbox::finiteelements::getLaplacian(fineGridCell.h())
      );

    // compute residual contribution. Mind the minus sign here that
    // results from the residual's definition: r = b-Au
    r = - epsilon * localStiffnessMatrix * u;

    // scatter residual contributions
    for (int i=0; i<TwoPowerD; i++) {
      fineGridVerticesMG(i).setRes(  fineGridVerticesMG(i).getRes() + r(i) );
      fineGridVerticesMG(i).setDiag( fineGridVerticesMG(i).getDiag() + localStiffnessMatrix(i,i) );
    }
  }
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchCellLastTime(
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> fineGridCell,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG,
      peano4::datamanagement::CellWrapper<void> coarseGridCell) {
// @todo Please implement
}


