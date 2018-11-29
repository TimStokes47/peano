#include "Spacetree.h"
#include "PeanoCurve.h"


#include "peano4/utils/Loop.h"


tarch::logging::Log  peano4::grid::Spacetree::_log( "peano4::grid::Spacetree" );


peano4::grid::Spacetree::Spacetree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width):
  _root() {
  _root.setLevel( 0 );
  _root.setX( offset );
  _root.setH( width );
  _root.setInverted( false );
  _root.setEvenFlags( 0 );
  for (int i=0; i<DimensionsTimesTwo; i++)
    _root.setAccessNumber( i,0 );
  logInfo( "Spacetree(...)", "create spacetree with " << offset << "x" << width );
}


peano4::grid::Spacetree peano4::grid::Spacetree::createTrivialTree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width) {
  Spacetree result(offset,width);
  return result;
}


void peano4::grid::Spacetree::traverse() {
  logTraceIn( "traverse()" );

  const bool isFirstTraversal = _vertexStack[0].empty() and _vertexStack[1].empty();
  GridVertex vertices[TwoPowerD];
  dfor2(k)
    tarch::la::Vector<Dimensions,double> x = _root.getX() + k.convertScalar<double>();
    vertices[kScalar] = GridVertex(
      isFirstTraversal ? GridVertex::State::Refining
    		           : GridVertex::State::Refined,     // const State& state
       tarch::la::Vector<TwoPowerD,int>(0)         // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
       #ifdef PeanoDebug
       ,
       x,                                          // const tarch::la::Vector<Dimensions,double>& x
       0                                           // level
       #endif
    );
    logInfo( "createTrivialTree(...)", "create " << vertices[kScalar].toString() );
  enddforx

  descend(_root,vertices);

  _root.setInverted( not _root.getInverted() );

  logTraceOut( "traverse()" );
}


bool peano4::grid::Spacetree::isSpacetreeNodeRefined(GridVertex  vertices[TwoPowerD]) {
  bool result = false;
  dfor2(k)
    result |= vertices[kScalar].getState() == GridVertex::State::Refining;
    result |= vertices[kScalar].getState() == GridVertex::State::Refined;
  enddforx
  return result;
}


void peano4::grid::Spacetree::refineState(const AutomatonState& coarseGrid, AutomatonState fineGrid[ThreePowerD] ) {
  logTraceInWith1Argument( "refineState(...)", coarseGrid.toString() );

  refineState(coarseGrid,fineGrid,tarch::la::Vector<Dimensions,int>(0),Dimensions-1);

  logTraceOutWith1Argument( "refineState(...)", coarseGrid.toString() );
}


void peano4::grid::Spacetree::refineState(const AutomatonState& coarseGrid, AutomatonState fineGrid[ThreePowerD], tarch::la::Vector<Dimensions,int>  fineGridPosition, int  axis ) {
  if (axis == -1) {
    int arrayIndex       = PeanoCurve::lineariseCellIndex(fineGridPosition);
	fineGrid[arrayIndex] = coarseGrid;
	fineGrid[arrayIndex].setLevel( coarseGrid.getLevel()+1 );
	fineGrid[arrayIndex].setH( coarseGrid.getH()/3.0 );
  }
  else {
    assertion( axis >= 0 );
    assertion( axis < Dimensions );

    AutomatonState firstCell  = coarseGrid;
    AutomatonState secondCell = coarseGrid;
    AutomatonState thirdCell  = coarseGrid;

    PeanoCurve::setExitFace( firstCell, axis );
    fineGridPosition(axis) = PeanoCurve::isTraversePositiveAlongAxis(coarseGrid,axis)? 0:2;
    firstCell.setX(axis,coarseGrid.getX(axis) + PeanoCurve::isTraversePositiveAlongAxis(coarseGrid,axis) ? 0.0 : 2.0 * coarseGrid.getH(axis)/3.0);
    refineState(firstCell,fineGrid,fineGridPosition,axis-1);

    PeanoCurve::invertEvenFlag( secondCell, axis );
    PeanoCurve::setEntryFace(   secondCell, axis );
    PeanoCurve::setExitFace(    secondCell, axis );
    fineGridPosition(axis)= 1;
    secondCell.setX(axis,coarseGrid.getX(axis) + coarseGrid.getH(axis)/3.0);
    refineState(secondCell,fineGrid,fineGridPosition,axis-1);

    PeanoCurve::setEntryFace( thirdCell, axis );
    fineGridPosition(axis) = PeanoCurve::isTraversePositiveAlongAxis(coarseGrid,axis)? 2:0;
    thirdCell.setX(axis,coarseGrid.getX(axis) + PeanoCurve::isTraversePositiveAlongAxis(coarseGrid,axis) ? 2.0 * coarseGrid.getH(axis)/3.0 : 0.0);
    refineState(thirdCell,fineGrid,fineGridPosition,axis-1);
  }
}


void peano4::grid::Spacetree::loadVertices(const AutomatonState& state, GridVertex  vertices[TwoPowerD]) {
  logTraceInWith1Argument( "loadVertices(...)", state.toString() );

  logTraceOutWith1Argument( "loadVertices(...)", state.toString() );
}


void peano4::grid::Spacetree::storeVertices(const AutomatonState& state, GridVertex  vertices[TwoPowerD]) {

}


void peano4::grid::Spacetree::descend(
  const AutomatonState& state,
  GridVertex            vertices[TwoPowerD]
) {
  assertion( isSpacetreeNodeRefined(vertices) );
  logTraceInWith1Argument( "descend(...)", state.toString() );

  peano4::utils::LoopDirection loopDirection = PeanoCurve::getLoopDirection(state);

  AutomatonState fineGrid[ThreePowerD];
  refineState(state, fineGrid );

  zfor(k,3,loopDirection)
    // load cell's vertices
    GridVertex fineGridVertices[TwoPowerD];
    loadVertices(fineGrid[PeanoCurve::lineariseCellIndex(k)],fineGridVertices);

    // enter cell event
    // if (isSpacetreeNodeRefined(vertices)) {
    // leave Cell

    storeVertices(fineGrid[PeanoCurve::lineariseCellIndex(k)],fineGridVertices);
  endzfor

  logTraceOut( "descend(...)" );
}

