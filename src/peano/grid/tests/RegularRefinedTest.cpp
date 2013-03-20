#include "peano/grid/tests/RegularRefinedTest.h"
#include "peano/grid/nodes/RegularRefined.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(peano::grid::tests::RegularRefinedTest)



#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


tarch::logging::Log peano::grid::tests::RegularRefinedTest::_log( "peano::grid::tests::RegularRefinedTest" );


peano::grid::tests::RegularRefinedTest::RegularRefinedTest():
  TestCase( "peano::grid::tests::RegularRefinedTest" ) {
}


peano::grid::tests::RegularRefinedTest::~RegularRefinedTest() {
}


void peano::grid::tests::RegularRefinedTest::run() {
  logTraceIn( "run() ");
  testMethod( test2DComputePositionRelativeToNextCoarserLevelFromFineGridVertexPosition );
  testMethod( test2DComputePositionRelativeToNextCoarserLevelFromFineGridCellPosition );
  testMethod( testIsVertexOnLevel1DataDecompositionBoundaryAlgorithm );
  logTraceOut( "run() ");
}



void peano::grid::tests::RegularRefinedTest::setUp() {
}


void peano::grid::tests::RegularRefinedTest::testIsVertexOnLevel1DataDecompositionBoundaryAlgorithm() {
  logTraceIn( "testIsVertexOnLevel1DataDecompositionBoundaryAlgorithm()" );
  #ifdef Dim2
  peano::grid::UnrolledLevelEnumerator enumerator(
    0.1, //  coarsestGridCellSize,
    0.0, // domainOffset,
    1, // coarseGridEnumeratorsLevel,
    4, // totalHeightOfSubtree,
    2  // relativeLevelOfThisEnumerator
  );

  validateEquals( enumerator.getCellsPerAxis(),     9 );
  validateEquals( enumerator.getVerticesPerAxis(), 10 );

  tarch::la::Vector<DIMENSIONS,int>  vertexPosition(0);

  std::bitset<THREE_POWER_D>  forkedSubtrees;

  forkedSubtrees = 0;

  vertexPosition = 0,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 1,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 2,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 3,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 0,1;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 1,1;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 1,2;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );


  forkedSubtrees = 0;
  forkedSubtrees[1]= true;
  forkedSubtrees[2]= true;
  forkedSubtrees[4]= true;
  forkedSubtrees[5]= true;

  vertexPosition = 0,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 1,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 2,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 3,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 4,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 5,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 6,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 7,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 8,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 9,0;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 0,1;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 3,1;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 0,3;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 3,3;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 6,3;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 9,3;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 0,6;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 3,6;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 6,6;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 9,6;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), true );
  vertexPosition = 0,9;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 3,9;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 6,9;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  vertexPosition = 9,9;
  validateEquals( peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(vertexPosition, enumerator, forkedSubtrees), false );
  #endif
  logTraceOut( "testIsVertexOnLevel1DataDecompositionBoundaryAlgorithm()" );
}


void peano::grid::tests::RegularRefinedTest::test2DComputePositionRelativeToNextCoarserLevelFromFineGridVertexPosition() {
  logTraceIn( "test2DComputePositionRelativeToNextCoarserLevelFromFineGridVertexPosition()" );
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int> fineGridPosition;
  tarch::la::Vector<DIMENSIONS,int> offsetOfCoarseGridEnumerator;
  tarch::la::Vector<DIMENSIONS,int> fineGridPositionRelativeToCoarserCell;

  fineGridPosition = 0,0;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridVertexPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 0 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 0 );

  fineGridPosition = 3,1;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridVertexPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 0 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 3 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 1 );

  fineGridPosition = 4,2;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridVertexPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 1 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 2 );

  fineGridPosition = 6,3;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridVertexPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 1 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 3 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 3 );

  fineGridPosition = 7,4;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridVertexPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 2 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 1 );
  #endif
  logTraceOut( "test2DComputePositionRelativeToNextCoarserLevelFromFineGridVertexPosition()" );
}


void peano::grid::tests::RegularRefinedTest::test2DComputePositionRelativeToNextCoarserLevelFromFineGridCellPosition() {
  logTraceIn( "test2DComputePositionRelativeToNextCoarserLevelFromFineGridCellPosition()" );
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int> fineGridPosition;
  tarch::la::Vector<DIMENSIONS,int> offsetOfCoarseGridEnumerator;
  tarch::la::Vector<DIMENSIONS,int> fineGridPositionRelativeToCoarserCell;

  fineGridPosition = 0,0;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridCellPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 0 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 0 );

  fineGridPosition = 3,1;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridCellPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 1 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 1 );

  fineGridPosition = 4,2;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridCellPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 1 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 2 );

  fineGridPosition = 6,3;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridCellPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 2 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 0 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 0 );

  fineGridPosition = 7,4;
  peano::grid::nodes::computePositionRelativeToNextCoarserLevelFromFineGridCellPosition(
    fineGridPosition,
    offsetOfCoarseGridEnumerator,
    fineGridPositionRelativeToCoarserCell
  );
  validateEquals( offsetOfCoarseGridEnumerator(0), 2 );
  validateEquals( offsetOfCoarseGridEnumerator(1), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(0), 1 );
  validateEquals( fineGridPositionRelativeToCoarserCell(1), 1 );
  #endif
  logTraceOut( "test2DComputePositionRelativeToNextCoarserLevelFromFineGridCellPosition()" );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif

