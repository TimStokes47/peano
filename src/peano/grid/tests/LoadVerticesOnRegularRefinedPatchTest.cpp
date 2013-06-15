#include "peano/grid/tests/LoadVerticesOnRegularRefinedPatchTest.h"
#include "peano/grid/nodes/tasks/LoadVerticesOnRegularRefinedPatch.h"
#include "peano/grid/nodes/Constants.h"

#include "peano/utils/Globals.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest)


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif



tarch::logging::Log peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::_log( "peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest" );


peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::LoadVerticesOnRegularRefinedPatchTest():
  tarch::tests::TestCase( "peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest" ) {
}


peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::~LoadVerticesOnRegularRefinedPatchTest() {
}


void peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::run() {
  logTraceIn( "run() ");
  testMethod( test2D );
  testMethod( test3D );
  logTraceOut( "run() ");
}


void peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::setUp() {
}


void peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::test2D() {
#ifdef Dim2
  validate(false);
#endif
}


void peano::grid::tests::LoadVerticesOnRegularRefinedPatchTest::test3D() {
  #ifdef Dim3

  const std::bitset<THREE_POWER_D> forkedSubtrees =
    1 * 64 + 1 * 128 + 0 * 256 +
    1 *  8 + 1 *  16 + 0 *  32 +
    0 *  1 + 0 *   2 + 0 *   4;

  tarch::la::Vector<DIMENSIONS,int>  vertexPosition;

  tarch::la::assignList( vertexPosition ) = 0, 0, 0;
  validateWithParams2( !peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree( vertexPosition, 9, forkedSubtrees ), vertexPosition, forkedSubtrees );

  tarch::la::assignList( vertexPosition ) = 5, 0, 0;
  validateWithParams2( !peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree( vertexPosition, 9, forkedSubtrees ), vertexPosition, forkedSubtrees );

  tarch::la::assignList( vertexPosition ) = 5, 2, 0;
  validateWithParams2( !peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree( vertexPosition, 9, forkedSubtrees ), vertexPosition, forkedSubtrees );

  tarch::la::assignList( vertexPosition ) = 5, 3, 0;
  validateWithParams2(  peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree( vertexPosition, 9, forkedSubtrees ), vertexPosition, forkedSubtrees );

  tarch::la::assignList( vertexPosition ) = 5, 4, 0;
  validateWithParams2(  peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree( vertexPosition, 9, forkedSubtrees ), vertexPosition, forkedSubtrees );

  tarch::la::assignList( vertexPosition ) = 5, 4, 1;
  validateWithParams2( !peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree( vertexPosition, 9, forkedSubtrees ), vertexPosition, forkedSubtrees );

  #endif
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
