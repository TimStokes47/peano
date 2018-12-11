#include "Spacetree.h"
#include "PeanoCurve.h"
#include "TraversalObserver.h"


#include "peano4/utils/Loop.h"
#include "peano4/parallel/Node.h"
#include "peano4/parallel/SpacetreeSet.h"


tarch::logging::Log  peano4::grid::Spacetree::_log( "peano4::grid::Spacetree" );


peano4::grid::Spacetree::Spacetree(
  const tarch::la::Vector<Dimensions,double>&  offset,
  const tarch::la::Vector<Dimensions,double>&  width
):
  _id(0),
  _root() {
  _root.setLevel( 0 );
  _root.setX( offset );
  _root.setH( width );
  _root.setInverted( false );
  _root.setEvenFlags( 0 );

  for (int i=0; i<DimensionsTimesTwo; i++) {
    _root.setAccessNumber( i,0 );
  }

  peano4::parallel::Node::getInstance().registerId(_id);

  logInfo( "Spacetree(...)", "create spacetree with " << offset << "x" << width << " and id " << _id);
}


bool peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(GridVertex  vertex) const {
  if (vertex.getState()==GridVertex::State::HangingVertex) {
	return false;
  }
  else {
	bool result = false;
	for(int i=0; i<TwoPowerD; i++) {
      result |= vertex.getAdjacentRanks(i)==_id;
	}
	return result;
  }
}


bool peano4::grid::Spacetree::isSpacetreeNodeLocal(
  GridVertex            vertices[TwoPowerD]
) const {
  bool isLocal = true;
  dfor2(k)
    isLocal &= (
      (vertices[kScalar].getState()==GridVertex::State::HangingVertex)
      or
      ( vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1)==_id )
	);
  enddforx
  return isLocal;
}


void peano4::grid::Spacetree::traverse(TraversalObserver& observer, peano4::parallel::SpacetreeSet& spacetreeSet) {
  logTraceIn( "traverse(TraversalObserver,SpacetreeSet)" );

  // we need to backup them as the traversal removes the entries from the
  // splitting container while it gives out cells
  std::vector<int> newIds;
  _splitting.insert( _splitTriggered.begin(), _splitTriggered.end() );
  for (auto p: _splitting) {
    newIds.push_back( p.first );
  }
  _splitTriggered.clear();

  traverse(observer);

  assertion( _splitting.empty() );
  for (auto p: newIds) {
	Spacetree newTree(*this);
	assertion( p!=_id );
	newTree._id = p;
	spacetreeSet.addSpacetree(newTree);
  }

  logTraceOut( "traverse(TraversalObserver,SpacetreeSet)" );
}


void peano4::grid::Spacetree::traverse(TraversalObserver& observer) {
  logTraceIn( "traverse(TraversalObserver)" );

  clearStatistics();

  observer.beginTraversal();

  const bool isFirstTraversal = _vertexStack[0].empty() and _vertexStack[1].empty();
  GridVertex vertices[TwoPowerD];
  dfor2(k)
    tarch::la::Vector<Dimensions,double> x = _root.getX() + k.convertScalar<double>();
    vertices[kScalar] = GridVertex(
      isFirstTraversal ? GridVertex::State::Refining
    		           : GridVertex::State::Refined,     // const State& state
       tarch::la::Vector<TwoPowerD,int>(_id),            // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
	   true                                              // antecessor of refined vertex
       #ifdef PeanoDebug
       ,
       x,                                                // const tarch::la::Vector<Dimensions,double>& x
       0                                                 // level
       #endif
    );
    logInfo( "traverse()", "create " << vertices[kScalar].toString() );
  enddforx

  descend(_root,vertices,observer);

  _root.setInverted( not _root.getInverted() );

  observer.endTraversal();

  logTraceOut( "traverse(TraversalObserver)" );
}


bool peano4::grid::Spacetree::isSpacetreeNodeRefined(GridVertex  vertices[TwoPowerD]) {
  bool result = false;
  dfor2(k)
    result |= vertices[kScalar].getState() == GridVertex::State::Refining;
    result |= vertices[kScalar].getState() == GridVertex::State::Refined;
  enddforx
  return result;
}


void peano4::grid::Spacetree::refineState(const AutomatonState& coarseGrid, AutomatonState fineGridStates[ThreePowerD], tarch::la::Vector<Dimensions,int>  fineGridStatesPosition, int  axis ) {
  if (axis == -1) {
    int arrayIndex       = peano4::utils::dLinearised(fineGridStatesPosition,3);
	fineGridStates[arrayIndex] = coarseGrid;
	fineGridStates[arrayIndex].setLevel( coarseGrid.getLevel()+1 );
  }
  else {
    assertion( axis >= 0 );
    assertion( axis < Dimensions );

    AutomatonState firstCell  = coarseGrid;
    AutomatonState secondCell = coarseGrid;
    AutomatonState thirdCell  = coarseGrid;

    if (PeanoCurve::isTraversePositiveAlongAxis(coarseGrid,axis)) {
      PeanoCurve::setExitFace( firstCell, axis );
    }
    else {
      PeanoCurve::setEntryFace( firstCell, axis );
    }
    firstCell.setH(axis, coarseGrid.getH(axis)/3.0 );
    fineGridStatesPosition(axis) = 0;
    firstCell.setX(axis,coarseGrid.getX(axis));
    refineState(firstCell,fineGridStates,fineGridStatesPosition,axis-1);

    PeanoCurve::invertEvenFlag( secondCell, axis );
    PeanoCurve::setEntryFace(   secondCell, axis );
    PeanoCurve::setExitFace(    secondCell, axis );
    secondCell.setH(axis, coarseGrid.getH(axis)/3.0 );
    fineGridStatesPosition(axis)= 1;
    secondCell.setX(axis,coarseGrid.getX(axis) + coarseGrid.getH(axis)/3.0);
    refineState(secondCell,fineGridStates,fineGridStatesPosition,axis-1);

    if (PeanoCurve::isTraversePositiveAlongAxis(coarseGrid,axis)) {
      PeanoCurve::setEntryFace( thirdCell, axis );
    }
    else {
      PeanoCurve::setExitFace( thirdCell, axis );
    }
    thirdCell.setH(axis, coarseGrid.getH(axis)/3.0 );
    fineGridStatesPosition(axis) = 2;
    thirdCell.setX( axis, coarseGrid.getX(axis) + 2.0 * coarseGrid.getH(axis)/3.0 );
    refineState(thirdCell,fineGridStates,fineGridStatesPosition,axis-1);
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

  logTraceInWith2Arguments( "getVertexType(...)", position, dimension );
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

  logTraceOutWith1Argument( "getVertexType(...)", toString(result) );
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
    case VertexType::Delete:
      return "delete";
  }
  return "<undef>";
}


void peano4::grid::Spacetree::updateVertexAfterLoad(
  GridVertex&                               vertex,
  GridVertex                                fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&  position
) {
  logTraceInWith1Argument( "updateVertexAfterLoad(GridVertex&)", vertex.toString() );

  if (
    vertex.getState()==GridVertex::State::RefinementTriggered
	and
	isVertexAdjacentToLocalSpacetree(vertex)
  ) {
    vertex.setState( GridVertex::State::Refining );
  }
  else if (
    vertex.getState()==GridVertex::State::EraseTriggered
	and
	isVertexAdjacentToLocalSpacetree(vertex)
  ) {
	assertion1( not vertex.getIsAntecessorOfRefinedVertex(), vertex.toString() );
    vertex.setState( GridVertex::State::Erasing );
  }

  // has to be here. Don't want to interfere with state splitting
  if (
    not isVertexAdjacentToLocalSpacetree(vertex)
    and
	not vertex.getIsAntecessorOfRefinedVertex()
	and
	vertex.getState()==GridVertex::State::Refined
  ) {
	logInfo( "updateVertexAfterLoad(GridVertex&)", "would like to erase " << vertex.toString() << " in spacetree " << _id );
    vertex.setState( GridVertex::State::EraseTriggered );
  }

  vertex.setIsAntecessorOfRefinedVertex(false);

  logTraceOutWith1Argument( "updateVertexAfterLoad(GridVertex&)", vertex.toString() );
}


bool peano4::grid::Spacetree::restrictToCoarseGrid(
  const tarch::la::Vector<Dimensions,int>&  coarseVertexPosition,
  const tarch::la::Vector<Dimensions,int>&  fineVertexPosition
) {
  bool result = true;
  for (int d=0; d<Dimensions; d++) {
	assertion2( coarseVertexPosition(d)>=0, coarseVertexPosition, fineVertexPosition );
	assertion2( coarseVertexPosition(d)<=1, coarseVertexPosition, fineVertexPosition );

	assertion2( fineVertexPosition(d)>=0, coarseVertexPosition, fineVertexPosition );
	assertion2( fineVertexPosition(d)<=3, coarseVertexPosition, fineVertexPosition );

    result &=
      (
        (coarseVertexPosition(d)==1 and fineVertexPosition(d)>0)
		or
		(coarseVertexPosition(d)==0 and fineVertexPosition(d)<3)
      );
  }
  return result;
}


void peano4::grid::Spacetree::updateVertexBeforeStore(
  GridVertex&                               vertex,
  GridVertex                                coarseGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&  fineVertexPositionWithinPatch
) {
  logTraceInWith1Argument( "updateVertexBeforeStore(GridVertex&)", vertex.toString() );

  bool restrictIsAntecessorOfRefinedVertex = false;

  if (vertex.getState()==GridVertex::State::RefinementTriggered) {
    _statistics.setNumberOfRefiningVertices( _statistics.getNumberOfRefiningVertices()+1 );
    restrictIsAntecessorOfRefinedVertex = true;
  }
  else if (vertex.getState()==GridVertex::State::Refining) {
    vertex.setState( GridVertex::State::Refined );
    restrictIsAntecessorOfRefinedVertex = true;
  }
  else if (vertex.getState()==GridVertex::State::EraseTriggered) {
    _statistics.setNumberOfErasingVertices( _statistics.getNumberOfErasingVertices()+1 );
  }
  else if (vertex.getState()==GridVertex::State::Erasing) {
    vertex.setState( GridVertex::State::Unrefined );
  }

  if (vertex.getState()==GridVertex::State::Refined) {
    _statistics.setNumberOfRefinedVertices( _statistics.getNumberOfRefinedVertices()+1 );
    restrictIsAntecessorOfRefinedVertex = true;
  }
  if (vertex.getState()==GridVertex::State::Unrefined) {
    _statistics.setNumberOfUnrefinedVertices( _statistics.getNumberOfUnrefinedVertices()+1 );
  }

  if (restrictIsAntecessorOfRefinedVertex) {
	dfor2(k)
      if (restrictToCoarseGrid(k,fineVertexPositionWithinPatch)) {
        coarseGridVertices[kScalar].setIsAntecessorOfRefinedVertex(true);
      }
	enddforx
  }

  logTraceOutWith1Argument( "updateVertexBeforeStore()", vertex.toString() );
}


peano4::grid::GridVertex peano4::grid::Spacetree::createNewPersistentVertex(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level
) {
  logTraceInWith2Arguments( "createNewPersistentVerte(...)", x, level );

  tarch::la::Vector<TwoPowerD,int> adjacentRanks(-1);
  dfor2(k)
    adjacentRanks(kScalar) = coarseGridVertices[ kScalar ].getAdjacentRanks(TwoPowerD-1-kScalar);
  enddforx

  GridVertex result(
    GridVertex::State::Unrefined,
    adjacentRanks,
    false                                       // antecessor of refined vertex
    #ifdef PeanoDebug
    ,
    x,                                          // const tarch::la::Vector<Dimensions,double>& x
    level                                       // level
    #endif
  );

  logTraceOutWith1Argument( "createNewPersistentVerte(...)", result.toString() );
  return result;
}


void peano4::grid::Spacetree::loadVertices(
  const AutomatonState&                        fineGridStatesState,
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
) {
  logTraceInWith2Arguments( "loadVertices(...)", fineGridStatesState.toString(), cellPositionWithin3x3Patch );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridStatesState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           localVertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> localVertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(localVertexIndex);

    #ifdef PeanoDebug
    const tarch::la::Vector<Dimensions,double> x = fineGridStatesState.getX()
      + tarch::la::multiplyComponents(
          convertToIntegerVector(localVertexIndex).convertScalar<double>(),
		  fineGridStatesState.getH()
	    );
    #endif

    VertexType type        = getVertexType(coarseGridVertices,localVertexPositionWithinPatch);
    const int  stackNumber = PeanoCurve::getReadStackNumber(fineGridStatesState,localVertexIndex);

    // reset to persistent, as new vertex already has been generated
    if ( not PeanoCurve::isInOutStack(stackNumber) and type==VertexType::New ) {
      type = VertexType::Persistent;
      logDebug(
        "loadVertices(...)",
		"reset stack flag for local vertex " << localVertexPositionWithinPatch << " from new to persistent" );
    }

    switch (type) {
      case VertexType::New:
    	logDebug( "loadVertices(...)", "stack no=" << stackNumber );
    	fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ] = createNewPersistentVertex(coarseGridVertices,x,fineGridStatesState.getLevel());
        updateVertexAfterLoad(
          fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ],
		  coarseGridVertices,
		  localVertexPositionWithinPatch
        );
    	break;
      case VertexType::Hanging:
      	fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ] = GridVertex(
          GridVertex::State::HangingVertex,
          tarch::la::Vector<TwoPowerD,int>(0),        // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
		  false                                       // antecessor of refined vertex
          #ifdef PeanoDebug
          ,
          x,                                          // const tarch::la::Vector<Dimensions,double>& x
		  fineGridStatesState.getLevel()              // level
          #endif
  		);
      	break;
      case VertexType::Persistent:
      case VertexType::Delete:
        {
          logDebug( "readVertices(...)", "read vertex from stack " << stackNumber );
          assertion3( not _vertexStack[stackNumber].empty(), stackNumber, localVertexIndex, localVertexPositionWithinPatch );
          fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ]  = _vertexStack[stackNumber].pop();
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
            updateVertexAfterLoad(
	          fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ],
			  coarseGridVertices,
			  localVertexPositionWithinPatch
			);
          }
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
	  fineGridStatesState.toString(), localVertexIndex
	);
    assertionEquals3(
      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].getLevel(), fineGridStatesState.getLevel(),
      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString(),
	  fineGridStatesState.toString(), localVertexIndex
	);
  }

  logTraceOutWith1Argument( "loadVertices(...)", fineGridStatesState.toString() );
}


void peano4::grid::Spacetree::storeVertices(
  const AutomatonState&                        fineGridStatesState,
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
) {
  logTraceInWith1Argument( "storeVertices(...)", fineGridStatesState.toString() );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridStatesState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           localVertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> localVertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(localVertexIndex);

    const int   stackNumber = PeanoCurve::getWriteStackNumber(fineGridStatesState,localVertexIndex);
    VertexType  type        = getVertexType(coarseGridVertices,localVertexPositionWithinPatch);

    switch (type) {
      case VertexType::New:
      case VertexType::Persistent:
        {
      	  logDebug(
            "storeVertices(...)",
			"write vertex " << fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].toString() << " to stack " << stackNumber
		  );
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
        	    // @todo Raus
        	    if (
        	      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].getX(0)<0.4 and
        	      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].getX(1)<0.4 and
        		  fineGridStatesState.getLevel()<4 and fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].getState()==GridVertex::State::Unrefined and
        		  stackNumber<=1
        		  and
        		  isVertexAdjacentToLocalSpacetree(fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ])
        		) {
        	      fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ].setState( GridVertex::State::RefinementTriggered );
        	    }

            updateVertexBeforeStore(
              fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ],
			  coarseGridVertices,
			  localVertexPositionWithinPatch
            );
          }
      	  _vertexStack[stackNumber].push( fineGridVertices[ peano4::utils::dLinearised(localVertexIndex) ] );
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

  logTraceOutWith1Argument( "storeVertices(...)", fineGridStatesState.toString() );
}


void peano4::grid::Spacetree::clearStatistics() {
  _statistics.setNumberOfRefinedVertices( 0 );
  _statistics.setNumberOfUnrefinedVertices( 0 );
  _statistics.setNumberOfErasingVertices( 0 );
  _statistics.setNumberOfRefiningVertices( 0 );
}


void peano4::grid::Spacetree::updateVertexRanksWithinCell( GridVertex fineGridVertices[TwoPowerD], int newId ) {
  dfor2(k)
    fineGridVertices[kScalar].setAdjacentRanks(TwoPowerD-kScalar-1,newId);
  enddforx
}


void peano4::grid::Spacetree::descend(
  const AutomatonState& state,
  GridVertex            vertices[TwoPowerD],
  TraversalObserver&    observer
) {
  assertion( isSpacetreeNodeRefined(vertices) );
  logTraceInWith1Argument( "descend(...)", state.toString() );

  #if PeanoDebug>=2
  dfor2(k)
    logDebug( "descend(...)", "-" << vertices[kScalar].toString() );
  enddforx
  #endif

  peano4::utils::LoopDirection loopDirection = PeanoCurve::getLoopDirection(state);
  logDebug( "descend(...)", "- direction=" << loopDirection );

  //
  // Construct the 3^d new children states
  //
  AutomatonState fineGridStates[ThreePowerD];
  refineState(state, fineGridStates );

  #if PeanoDebug>=2
  dfor3(k)
    logDebug( "descend(...)", "-" << fineGridStates[kScalar].toString() );
  enddforx
  #endif

  //
  // Loop over children
  //
  zfor3(k,loopDirection)
    //
    // Load cell's vertices
    //
    GridVertex fineGridVertices[TwoPowerD];
    loadVertices(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices, k);

    //
    // Enter cell
    //
    if ( isSpacetreeNodeLocal(fineGridVertices) ) {
      observer.enterCell(
        fineGridStates[peano4::utils::dLinearised(k,3)].getX(),
	    fineGridStates[peano4::utils::dLinearised(k,3)].getH(),
	    isSpacetreeNodeRefined(fineGridVertices)
	  );
    }
    else {
      logDebug( "descend(...)", "node is not local on tree " << _id );
    }

    //
    // DFS
    //
    if (isSpacetreeNodeRefined(fineGridVertices)) {
      if ( isSpacetreeNodeLocal(fineGridVertices) ) {
        _statistics.setNumberOfLocalRefinedCells( _statistics.getNumberOfLocalRefinedCells()+1 );
      }
      else {
        _statistics.setNumberOfRemoteRefinedCells( _statistics.getNumberOfRemoteRefinedCells()+1 );
      }
      descend(
        fineGridStates[peano4::utils::dLinearised(k,3)],
        fineGridVertices,
		observer
      );
    }
    else {
      if ( isSpacetreeNodeLocal(fineGridVertices) ) {
        _statistics.setNumberOfLocalUnrefinedCells( _statistics.getNumberOfLocalUnrefinedCells()+1 );
      }
      else {
        _statistics.setNumberOfRemoteUnrefinedCells( _statistics.getNumberOfRemoteUnrefinedCells()+1 );
      }
    }

    //
    // Leave cell
    //


    //
    // Decompose tree if split requested
    //
    if ( isSpacetreeNodeLocal(fineGridVertices) and !_splitting.empty() ) {
      const int targetSpacetreeId = _splitting.begin()->first;
      updateVertexRanksWithinCell( fineGridVertices, targetSpacetreeId );
      _splitting.begin()->second--;
      if (_splitting.begin()->second==0) {
        _splitting.erase( _splitting.begin() );
      }
    }

    //
    // Store vertices
    //
    storeVertices(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices, k);
  endzfor

  logTraceOut( "descend(...)" );
}


peano4::grid::GridStatistics peano4::grid::Spacetree::getGridStatistics() const {
  return _statistics;
}


void peano4::grid::Spacetree::split(int cells) {
  const int newSpacetreeId = peano4::parallel::Node::getInstance().getNextFreeLocalId();
  assertion( _splitTriggered.count(newSpacetreeId)==0 );
  _splitTriggered.insert( std::pair<int,int>(newSpacetreeId,cells) );
  peano4::parallel::Node::getInstance().registerId(newSpacetreeId);
}
