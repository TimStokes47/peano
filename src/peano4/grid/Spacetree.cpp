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


void peano4::grid::Spacetree::refineState(const AutomatonState& coarseGrid, AutomatonState fineGrid[ThreePowerD], tarch::la::Vector<Dimensions,int>  fineGridPosition, int  axis ) {
  if (axis == -1) {
    int arrayIndex       = peano4::utils::dLinearised(fineGridPosition,3);
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


tarch::la::Vector<Dimensions,int> peano4::grid::Spacetree::convertToIntegerVector( const std::bitset<Dimensions>& in ) {
  tarch::la::Vector<Dimensions,int> result;
  for (int d=0; d<Dimensions; d++) {
	result[d] = in[d] ? 1 : 0;
  }
  return result;
}


peano4::grid::Spacetree::VertexType peano4::grid::Spacetree::getVertexType(
  GridVertex                         coarseGridVertices[TwoPowerD],
  tarch::la::Vector<Dimensions,int>  position,
  int                                dimension
) {
  if (dimension<0) {
    switch (coarseGridVertices[ peano4::utils::dLinearised(position,2) ].getState()) {
      case GridVertex::State::HangingVertex:
    	return VertexType::Hanging;
      case GridVertex::State::Unrefined:
      	return VertexType::Hanging;
      case GridVertex::State::Refined:
      	return VertexType::Persistent;
      case GridVertex::State::RefinementTriggered:
      	return VertexType::Hanging;
      case GridVertex::State::Refining:
      	return VertexType::New;
      case GridVertex::State::EraseTriggered:
      	return VertexType::Persistent;
      case GridVertex::State::Erasing:
      	return VertexType::Delete;
    }
    assertion3(false, peano4::utils::dLinearised(position,2), position, coarseGridVertices[ peano4::utils::dLinearised(position,2) ].toString() );
  }
  else if ( position(dimension)==0 ) {
    position(dimension)=0;
    return getVertexType(coarseGridVertices,position,dimension-1);
  }
  else if ( position(dimension)==3 ) {
    position(dimension)=1;
    return getVertexType(coarseGridVertices,position,dimension-1);
  }

  position(dimension)=0;
  peano4::grid::Spacetree::VertexType lhs = getVertexType(coarseGridVertices,position,dimension-1);

  position(dimension)=1;
  peano4::grid::Spacetree::VertexType rhs = getVertexType(coarseGridVertices,position,dimension-1);

  VertexType result = lhs;
  if (
    lhs==VertexType::New and rhs==VertexType::Hanging or lhs==VertexType::Hanging and rhs==VertexType::New
  ) {
	result = VertexType::New;
  }
  if (
    lhs==VertexType::New and rhs==VertexType::Persistent or lhs==VertexType::Persistent and rhs==VertexType::New
  ) {
	result = VertexType::Persistent;
  }
  if (
    lhs==VertexType::New and rhs==VertexType::Delete or lhs==VertexType::Delete and rhs==VertexType::New
  ) {
	result = VertexType::Persistent;
  }
  if (
    lhs==VertexType::Hanging and rhs==VertexType::Persistent or lhs==VertexType::Persistent and rhs==VertexType::Hanging
  ) {
	result = VertexType::Persistent;
  }
  if (
    lhs==VertexType::Hanging and rhs==VertexType::Delete or lhs==VertexType::Delete and rhs==VertexType::Hanging
  ) {
	result = VertexType::Delete;
  }
  if (
    lhs==VertexType::Persistent and rhs==VertexType::Delete or lhs==VertexType::Delete and rhs==VertexType::Persistent
  ) {
	result = VertexType::Persistent;
  }

  return result;
}


std::string peano4::grid::Spacetree::toString( VertexType type ) {
  switch (type) {
    case VertexType::New:
      return "new";
    case VertexType::Hanging:
      return "hanging";
    case VertexType::Persistent:
      return "persistent";
  }
  return "<undef>";
}


void peano4::grid::Spacetree::loadVertices(
  const AutomatonState&                        fineGridState,
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
) {
  logTraceInWith1Argument( "loadVertices(...)", fineGridState.toString() );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           localVertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> localVertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(localVertexIndex);

    #ifdef PeanoDebug
    const tarch::la::Vector<Dimensions,double> x = fineGridState.getX()
      + tarch::la::multiplyComponents(
          convertToIntegerVector(localVertexIndex).convertScalar<double>(),
		  fineGridState.getH()
	    );
    #endif

    VertexType type        = getVertexType(coarseGridVertices,localVertexPositionWithinPatch);
    const int  stackNumber = PeanoCurve::getReadStackNumber(fineGridState,localVertexIndex);
    if (
      stackNumber>=1 and type==VertexType::New
	  or
      stackNumber>=1 and type==VertexType::Hanging
	) {
      type = VertexType::Persistent;
    }

    switch (type) {
      case VertexType::New:
    	logDebug( "loadVertices(...)", "stack no=" << stackNumber );
    	fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ] = GridVertex(
          GridVertex::State::Unrefined,
          tarch::la::Vector<TwoPowerD,int>(0)         // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
          #ifdef PeanoDebug
          ,
          x,                                          // const tarch::la::Vector<Dimensions,double>& x
		  fineGridState.getLevel()                    // level
          #endif
		);
    	break;
      case VertexType::Hanging:
      	fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ] = GridVertex(
          GridVertex::State::HangingVertex,
          tarch::la::Vector<TwoPowerD,int>(0)         // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
          #ifdef PeanoDebug
          ,
          x,                                          // const tarch::la::Vector<Dimensions,double>& x
		  fineGridState.getLevel()                    // level
          #endif
  		);
      	break;
      case VertexType::Persistent:
      case VertexType::Delete:
        {
          assertion3( not _vertexStack[stackNumber].empty(), stackNumber, localVertexIndex, localVertexPositionWithinPatch );
          logDebug( "readVertices(...)", "read vertex from stack " << stackNumber );
          fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ]  = _vertexStack[stackNumber].back();
          _vertexStack[stackNumber].pop_back();
        }
        break;
    }

    logDebug(
      "loadVertices(...)",
	  "handle " << toString(type) << " vertex " << localVertexIndex << " at " << localVertexPositionWithinPatch << ": " <<
	  fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString()
	);

    assertionNumericalEquals3(
      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].getX(), x,
      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString(),
	  fineGridState.toString(), localVertexIndex
	);
    assertionEquals3(
      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].getLevel(), fineGridState.getLevel(),
      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString(),
	  fineGridState.toString(), localVertexIndex
	);
  }

  logTraceOutWith1Argument( "loadVertices(...)", fineGridState.toString() );
}


void peano4::grid::Spacetree::storeVertices(
  const AutomatonState&                        fineGridState,
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
) {
  logTraceInWith1Argument( "storeVertices(...)", fineGridState.toString() );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           localVertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> localVertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(localVertexIndex);

    const int   stackNumber = PeanoCurve::getWriteStackNumber(fineGridState,localVertexIndex);
    VertexType  type        = getVertexType(coarseGridVertices,localVertexPositionWithinPatch);
    if (
      stackNumber>=1 and type==VertexType::New
	  or
      stackNumber>=1 and type==VertexType::Hanging
	) {
      type = VertexType::Persistent;
    }

    switch (type) {
      case VertexType::New:
      case VertexType::Persistent:
        {
      	  logDebug(
            "storeVertices(...)",
			"write vertex " << fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString() << " to stack " << stackNumber
		  );
      	  _vertexStack[stackNumber].push_back( fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ] );
        }
        break;
      case VertexType::Hanging:
    	logDebug( "storeVertices(...)", "discard vertex " << fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString() );
    	break;
      case VertexType::Delete:
      	logDebug( "storeVertices(...)", "delete vertex " << fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString() );
        break;
    }
  }

  logTraceOutWith1Argument( "storeVertices(...)", fineGridState.toString() );
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

  zfor3(k,loopDirection)
    // load cell's vertices
    GridVertex fineGridVertices[TwoPowerD];
    loadVertices(fineGrid[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices, k);

    // enter cell event
    // if (isSpacetreeNodeRefined(vertices)) {
    // leave Cell

    storeVertices(fineGrid[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices, k);
  endzfor

  logTraceOut( "descend(...)" );
}

