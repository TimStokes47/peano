#include "SpacetreeTest.h"
#include "../Spacetree.h"
#include "../GridTraversalEvent.h"
#include "../TraversalObserver.h"

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
    vertices[kScalar].setState( GridVertex::State::Refined );
    vertices[kScalar].setAdjacentRanks( tarch::la::Vector<TwoPowerD,int>(peano4::grid::Spacetree::InvalidRank) );
    vertices[kScalar].setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(true);
    vertices[kScalar].setIsAntecessorOfRefinedVertexInCurrentTreeSweep(true);
    vertices[kScalar].setX(k.convertScalar<double>());
    vertices[kScalar].setLevel(0);
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


void peano4::grid::tests::SpacetreeTest::testCreateNeighbourExchangeLists() {
  #if Dimensions==2
  Spacetree tree( {0.0,0.0}, {1.0,1.0} );

  GridTraversalEvent event;
  GridVertex vertices[TwoPowerD];

  vertices[0].setState( GridVertex::State::Unrefined );
  vertices[0].setAdjacentRanks( { Spacetree::InvalidRank, 1, Spacetree::InvalidRank, 0 } );
  vertices[0].setBackupOfAdjacentRanks( { Spacetree::InvalidRank, 1, Spacetree::InvalidRank, 0 } );
  vertices[1].setState( GridVertex::State::Unrefined );
  vertices[1].setAdjacentRanks( { 1, 1, 0, 1 } );
  vertices[1].setBackupOfAdjacentRanks( { 1, 1, 0, 1 } );
  vertices[2].setState( GridVertex::State::Unrefined );
  vertices[2].setAdjacentRanks( { Spacetree::InvalidRank, 0, Spacetree::InvalidRank, 0 } );
  vertices[2].setBackupOfAdjacentRanks( { Spacetree::InvalidRank, 0, Spacetree::InvalidRank, 0 } );
  vertices[3].setState( GridVertex::State::Unrefined );
  vertices[3].setAdjacentRanks( { 0, 1, 0, 0 } );
  vertices[3].setBackupOfAdjacentRanks( { 0, 1, 0, 0 } );

  // enter cell; no load balancing going on
  tree.createNeighbourExchangeLists( vertices, event, true );

  validateEqualsWithParams1( event.getExchangeFaceData(0), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(1), 1,                         event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(2), 1,                         event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(3), TraversalObserver::NoData, event.toString() );

  vertices[0].setState( GridVertex::State::Unrefined );
  vertices[0].setAdjacentRanks( { Spacetree::InvalidRank, 0, Spacetree::InvalidRank, 1 } );
  vertices[0].setBackupOfAdjacentRanks( { Spacetree::InvalidRank, 0, Spacetree::InvalidRank, 1 } );
  vertices[1].setState( GridVertex::State::Unrefined );
  vertices[1].setAdjacentRanks( { 0, 0, 1, 0 } );
  vertices[1].setBackupOfAdjacentRanks( { 0, 0, 1, 0 } );
  vertices[2].setState( GridVertex::State::Unrefined );
  vertices[2].setAdjacentRanks( { Spacetree::InvalidRank, 1, Spacetree::InvalidRank, 1 } );
  vertices[2].setBackupOfAdjacentRanks( { Spacetree::InvalidRank, 1, Spacetree::InvalidRank, 1 } );
  vertices[3].setState( GridVertex::State::Unrefined );
  vertices[3].setAdjacentRanks( { 1, 0, 1, 1 } );
  vertices[3].setBackupOfAdjacentRanks( { 1, 0, 1, 1 } );

  // enter cell; just triggered the split
  tree._splitTriggered.insert( std::pair<int,int>(1,1) );
  tree.createNeighbourExchangeLists( vertices, event, true );

  validateEqualsWithParams1( event.getExchangeFaceData(0), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(1), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(2), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(3), TraversalObserver::NoData, event.toString() );

  tree.createNeighbourExchangeLists( vertices, event, false );

  validateEqualsWithParams1( event.getExchangeFaceData(0), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(1), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(2), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(3), TraversalObserver::NoData, event.toString() );

  // enter cell; just triggered the split
  tree._splitTriggered.clear();
  tree._splitting.insert( 1 );
  tree.createNeighbourExchangeLists( vertices, event, true );

  validateEqualsWithParams1( event.getExchangeFaceData(0), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(1), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(2), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(3), TraversalObserver::NoData, event.toString() );

  tree.createNeighbourExchangeLists( vertices, event, false );

  validateEqualsWithParams1( event.getExchangeFaceData(0), TraversalObserver::NoData, event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(1), 1,                         event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(2), 1,                         event.toString() );
  validateEqualsWithParams1( event.getExchangeFaceData(3), TraversalObserver::NoData, event.toString() );

#endif
}


void peano4::grid::tests::SpacetreeTest::run() {
  testMethod( testRestrictToCoarseGrid );
  testMethod( testCreateLeaveCellTraversalEvent );
  testMethod( testCreateNeighbourExchangeLists );
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
