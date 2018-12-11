#include "SpacetreeTest.h"
#include "../Spacetree.h"
#include "tarch/la/Vector.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(peano4::grid::tests::SpacetreeTest)


tarch::logging::Log peano4::grid::tests::SpacetreeTest::_log("peano4::grid::tests::SpacetreeTest");


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


void peano4::grid::tests::SpacetreeTest::testRestrictToCoarseGrid() {
  tarch::la::Vector<Dimensions,int>  positionInPatch;
  tarch::la::Vector<Dimensions,int>  coarsePosition;

  positionInPatch = 0,0;
  coarsePosition = 0,0;
  validateWithParams2( Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );

  positionInPatch = 0,3;
  coarsePosition = 0,1;
  validateWithParams2( Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );

  positionInPatch = 3,3;
  coarsePosition = 1,1;
  validateWithParams2( Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );

  positionInPatch = 2,2;
  coarsePosition = 1,1;
  validateWithParams2( Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );

  positionInPatch = 2,3;
  coarsePosition = 1,1;
  validateWithParams2( Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );

  positionInPatch = 0,2;
  coarsePosition = 1,1;
  validateWithParams2( !Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );

  positionInPatch = 0,2;
  coarsePosition = 0,0;
  validateWithParams2( Spacetree::restrictToCoarseGrid(coarsePosition,positionInPatch), coarsePosition, positionInPatch );
}


void peano4::grid::tests::SpacetreeTest::run() {
  testMethod( testRestrictToCoarseGrid );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
