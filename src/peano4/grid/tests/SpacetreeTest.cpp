#include "SpacetreeTest.h"
#include "../Spacetree.h"

#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"

#include "tarch/la/Vector.h"


tarch::logging::Log peano4::grid::tests::SpacetreeTest::_log("peano4::grid::tests::SpacetreeTest");


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


peano4::grid::tests::SpacetreeTest::SpacetreeTest():
  TestCase( "peano4::grid::tests::SpacetreeTest" ) {
}


void peano4::grid::tests::SpacetreeTest::testRestrictToCoarseGrid() {
  #if Dimensions==2
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
  #endif
}


void peano4::grid::tests::SpacetreeTest::testCreateLeaveCellTraversalEvent() {
  GridVertex vertices[TwoPowerD];
  dfor2(k)
    tarch::la::Vector<Dimensions,double> x = k.convertScalar<double>();
    vertices[kScalar] = GridVertex(
       GridVertex::State::Refined,     // const State& state
       tarch::la::Vector<TwoPowerD,int>(peano4::grid::Spacetree::InvalidRank),            // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
	   true,
	   true                                              // antecessor of refined vertex
       #ifdef PeanoDebug
       ,
       x,                                                // const tarch::la::Vector<Dimensions,double>& x
       0                                                 // level
       #endif
    );
  }

  // AutomatonState(const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& inverted, const std::bitset<Dimensions>& evenFlags, const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber);
  AutomatonState  state;
  state.setLevel( 0 );
  state.setX( 0.0 );
  state.setH( 1.0 );
  state.setInverted( false );
  state.setEvenFlags( 0 );

/*
  observer.enterCell(createLeaveCellTraversalEvent(
    vertices, fineGridVertices, fineGridStates[peano4::utils::dLinearised(k,3)], k
  ));
*/
}


void peano4::grid::tests::SpacetreeTest::run() {
  testMethod( testRestrictToCoarseGrid );
  testMethod( testCreateLeaveCellTraversalEvent );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
