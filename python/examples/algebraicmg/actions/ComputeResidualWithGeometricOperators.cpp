#include "ComputeResidualWithGeometricOperators.h"
#include "SetupScenario.h"


#include "tarch/la/Matrix.h"


#include "toolbox/finiteelements/ElementMatrix.h"
#include "toolbox/finiteelements/StencilFactory.h"


tarch::logging::Log examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::_log( "examples::algebraicmg::actions::ComputeResidualWithGeometricOperators");


examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::ComputeResidualWithGeometricOperators(int treeNumber) {}

examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::~ComputeResidualWithGeometricOperators() {}

std::vector< peano4::grid::GridControlEvent > examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::getGridControlEvents() {
  return std::vector< peano4::grid::GridControlEvent >();
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::beginTraversal() {}

void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::endTraversal() {}

void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::createPersistentVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::destroyPersistentVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::createHangingVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}

void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::destroyHangingVertex(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchVertexFirstTime(
  const peano4::datamanagement::VertexMarker& marker,
  examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG
) {
  fineGridVertexMG.setRes(0.0);
  fineGridVertexMG.setDiag(0.0);
}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchVertexLastTime(
      const peano4::datamanagement::VertexMarker& marker,
      examples::algebraicmg::vertexdata::MG& fineGridVertexMG,
      peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}


void examples::algebraicmg::actions::ComputeResidualWithGeometricOperators::touchCellFirstTime(
  const peano4::datamanagement::CellMarker& marker,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG
) {
  if (not marker.isRefined()) {
    tarch::la::Vector<TwoPowerD, double> u;
    tarch::la::Vector<TwoPowerD, double> r;

    // gather solution values
    for (int i=0; i<TwoPowerD; i++) {
      u(i) = fineGridVerticesMG(i).getU();
    }

    const double epsilon = SetupScenario::getEpsilon(marker.x());

    // Holds the stiffness matrix for the Laplacian. Not the most sophisticated
    // version. Obviously, we could compute it once for h=1.0 and then just
    // rescale it. But in this code, I prefer to have everything in one place.
    tarch::la::Matrix<TwoPowerD,TwoPowerD,double> localStiffnessMatrix =
      toolbox::finiteelements::getElementWiseAssemblyMatrix(
        toolbox::finiteelements::getLaplacian(marker.h())
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
  const peano4::datamanagement::CellMarker& marker,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> fineGridVerticesMG,
  peano4::datamanagement::VertexEnumerator<examples::algebraicmg::vertexdata::MG> coarseGridVerticesMG) {}
