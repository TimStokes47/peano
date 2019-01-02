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
  _spacetreeState( SpacetreeState::Running ),
  _root(),
  _masterId(-1),
  _joinTriggered(NoJoin),
  _joining(NoJoin) {
  _root.setLevel( 0 );
  _root.setX( offset );
  _root.setH( width );
  _root.setInverted( false );
  _root.setEvenFlags( 0 );

  for (int i=0; i<DimensionsTimesTwo; i++) {
    _root.setAccessNumber( i,0 );
  }

  peano4::parallel::Node::getInstance().registerId(_id);

  clearStatistics();

  logInfo( "Spacetree(...)", "create spacetree with " << offset << "x" << width << " and id " << _id);
}


peano4::grid::Spacetree::Spacetree(
  const Spacetree& copy,
  int              id
):
  _id(id),
  _spacetreeState( SpacetreeState::NewFromSplit ),
  _root( copy._root ),
  _statistics(),
  _masterId(copy._id),
  _splitTriggered(),
  _splitting(),
  _joinTriggered(NoJoin),
  _joining(NoJoin) {
  assertion1( _splitTriggered.count(_id)==0, _id );

  for (auto& p: copy._vertexStack ) {
    if ( PeanoCurve::isInOutStack(p.first) ) {
      _vertexStack.insert( std::pair< int, peano4::stacks::GridVertexStack >(p.first,peano4::stacks::GridVertexStack()) );
      _vertexStack[p.first].clone(p.second);
	}
  }

  clearStatistics();

  logInfo( "Spacetree(...)", "created spacetree " << _id << " from tree " << copy._id );
}


bool peano4::grid::Spacetree::isVertexAdjacentToLocalSpacetree(
  GridVertex  vertex,
  bool        splittingIsConsideredLocal,
  bool        joiningIsConsideredLocal
) const {
  if (vertex.getState()==GridVertex::State::HangingVertex) {
	return false;
  }
  else {
	bool result = false;
	for(int i=0; i<TwoPowerD; i++) {
      assertion( _splitTriggered.count( vertex.getAdjacentRanks(i) )<=1 );
      assertion( _splitting.count( vertex.getAdjacentRanks(i) )<=1 );

      result |= vertex.getAdjacentRanks(i)==_id;

      result |= _splitTriggered.count( vertex.getAdjacentRanks(i) )==1;
      result |= (splittingIsConsideredLocal and _splitting.count( vertex.getAdjacentRanks(i) )==1);

      // No need for such a check: If we join into the master, we do notify all neighbouring
      // ranks about the updated id, but we do not alter the id locally yet. This is done on
      // the receiver side.
      //result |= (_spacetreeState==SpacetreeState::Joining and vertex.getAdjacentRanks(i)==_masterId );
      result |= (joiningIsConsideredLocal and _joining==vertex.getAdjacentRanks(i) and _joining!=NoJoin);
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
      or
      ( _splitting.count( vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1) )==1 )
      or
      ( _joining==vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1) and _joining!=NoJoin )
	);
  enddforx
  return isLocal;
}


void peano4::grid::Spacetree::traverse(TraversalObserver& observer, peano4::parallel::SpacetreeSet& spacetreeSet) {
  logTraceIn( "traverse(TraversalObserver,SpacetreeSet)" );

  assertion( _joinTriggered==NoJoin or _splitTriggered.empty() );
  assertion( _joinTriggered==NoJoin or _splitting.empty() );
  assertion( _joining==NoJoin       or _splitTriggered.empty() );
  assertion( _joining==NoJoin       or _splitting.empty() );

  logDebug(
    "traverse(TraversalObserver,SpacetreeSet)",
	_splitTriggered.size() << " tree split triggered and " <<
	_splitting.size() << " splitting trees on tree " << _id
  );

  if (_spacetreeState!=SpacetreeState::Joined) {
    traverse(observer);
  }


  for (auto p: _splitTriggered) {
	assertion( p.first!=_id );
	Spacetree newTree(*this, p.first);
	spacetreeSet.addSpacetree(std::move(newTree));
  }
  _splitting.clear();
  _splitting.insert( _splitTriggered.begin(), _splitTriggered.end() );
  _splitTriggered.clear();

  _joining       = _joinTriggered;
  _joinTriggered = NoJoin;

  switch (_spacetreeState) {
    case SpacetreeState::NewFromSplit:
      _spacetreeState = SpacetreeState::Running;
      break;
    case SpacetreeState::JoinTriggered:
      _spacetreeState = SpacetreeState::Joining;
      break;
    case SpacetreeState::Joining:
      _spacetreeState = SpacetreeState::Joined;
      break;
    case SpacetreeState::Running:
      break;
    case SpacetreeState::Joined:
      break;
  }

  logTraceOut( "traverse(TraversalObserver,SpacetreeSet)" );
}


void peano4::grid::Spacetree::traverse(TraversalObserver& observer) {
  logTraceIn( "traverse(TraversalObserver)" );

  clearStatistics();

  _gridControlEvents = observer.getGridControlEvents();
  observer.beginTraversal();

  const bool isFirstTraversal = _vertexStack[0].empty() and _vertexStack[1].empty();
  GridVertex vertices[TwoPowerD];
  dfor2(k)
    tarch::la::Vector<Dimensions,double> x = _root.getX() + k.convertScalar<double>();
    vertices[kScalar] = GridVertex(
      isFirstTraversal ? GridVertex::State::Refining
    		           : GridVertex::State::Refined,     // const State& state
       tarch::la::Vector<TwoPowerD,int>(InvalidRank),            // const tarch::la::Vector<TwoPowerD,int>& adjacentRanks
	   true,
	   true                                              // antecessor of refined vertex
       #ifdef PeanoDebug
       ,
       x,                                                // const tarch::la::Vector<Dimensions,double>& x
       0                                                 // level
       #endif
    );
    vertices[kScalar].setAdjacentRanks(TwoPowerD-1-kScalar,_id);
    logDebug( "traverse()", "create " << vertices[kScalar].toString() );
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
    result |= vertices[kScalar].getState() == GridVertex::State::EraseTriggered;
    result |= vertices[kScalar].getState() == GridVertex::State::Erasing;
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


std::string peano4::grid::Spacetree::toString( SpacetreeState state ) {
  switch (state) {
    case SpacetreeState::NewFromSplit:
      return "new-from-split";
    case SpacetreeState::Running:
      return "running";
    case SpacetreeState::JoinTriggered:
      return "join-triggered";
    case SpacetreeState::Joining:
      return "joining";
  }
  return "<undef>";
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

  receiveAndMergeVertexIfAdjacentToDomainBoundary( vertex );


  vertex.setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep( vertex.getIsAntecessorOfRefinedVertexInCurrentTreeSweep() );
  vertex.setIsAntecessorOfRefinedVertexInCurrentTreeSweep( false );
  vertex.setNumberOfAdjacentRefinedLocalCells(0);


  if (
    vertex.getState()==GridVertex::State::RefinementTriggered
  ) {
    if ( isVertexAdjacentToLocalSpacetree(vertex,true,true) ) {
      vertex.setState( GridVertex::State::Refining );
      _statistics.setStationarySweeps( 0 );
    }
    else {
      vertex.setState( GridVertex::State::Unrefined );
      // Could be set by number of adjacent cells, e.g.
      // Anyway, don't reset stationary counter as this might
      // stop the tree from merging into its father
      //_statistics.setStationarySweeps( 0 );
    }
  }
  else if (
    vertex.getState()==GridVertex::State::EraseTriggered
  ) {
	if ( vertex.getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() ) {
      logDebug( "updateVertexAfterLoad(...)", "vertex " << vertex.toString() << " may not be erased as it is father or further refined vertices. Unroll flag" );
      vertex.setState( GridVertex::State::Refined );
 	}
	else if ( isVertexAdjacentToLocalSpacetree(vertex,true,true) ){
      vertex.setState( GridVertex::State::Erasing );
      _statistics.setStationarySweeps( 0 );
	}
	else {
      vertex.setState( GridVertex::State::Refined );
      _statistics.setStationarySweeps( 0 );
	}
  }


  // has to be here. Don't want to interfere with state splitting
  if (
    not isVertexAdjacentToLocalSpacetree(vertex,true,true)
    and
	not vertex.getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep()
	and
	vertex.getState()==GridVertex::State::Refined
  ) {
	logDebug( "updateVertexAfterLoad(GridVertex&)", "would like to erase " << vertex.toString() << " in spacetree " << _id );
    vertex.setState( GridVertex::State::EraseTriggered );
  }

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
  logTraceInWith2Arguments( "updateVertexBeforeStore(GridVertex&)", vertex.toString(), _id );

  if (
    vertex.getNumberOfAdjacentRefinedLocalCells()==TwoPowerD
	and
	vertex.getState()==GridVertex::State::Unrefined
  ) {
	vertex.setState( GridVertex::State::RefinementTriggered );
	logInfo( "updateVertexBeforeStore(...)", "have to post-refine vertex " << vertex.toString() );
  }

  bool restrictIsAntecessorOfRefinedVertex = vertex.getIsAntecessorOfRefinedVertexInCurrentTreeSweep();

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
    restrictIsAntecessorOfRefinedVertex = true;
  }
  else if (vertex.getState()==GridVertex::State::Erasing) {
    vertex.setState( GridVertex::State::Unrefined );
  }

  if (vertex.getState()==GridVertex::State::Refined) {
    if (isVertexAdjacentToLocalSpacetree(vertex,true,true)) {
      _statistics.setNumberOfRefinedVertices( _statistics.getNumberOfRefinedVertices()+1 );
    }
    restrictIsAntecessorOfRefinedVertex = true;
  }
  if (vertex.getState()==GridVertex::State::Unrefined) {
    if (isVertexAdjacentToLocalSpacetree(vertex,true,true)) {
      _statistics.setNumberOfUnrefinedVertices( _statistics.getNumberOfUnrefinedVertices()+1 );
    }
  }

  if (restrictIsAntecessorOfRefinedVertex) {
	dfor2(k)
      if (restrictToCoarseGrid(k,fineVertexPositionWithinPatch)) {
        coarseGridVertices[kScalar].setIsAntecessorOfRefinedVertexInCurrentTreeSweep(true);
      }
	enddforx
  }

  sendOutVertexIfAdjacentToDomainBoundary( vertex );

  logTraceOutWith2Arguments( "updateVertexBeforeStore()", vertex.toString(), _id );
}



peano4::grid::GridVertex peano4::grid::Spacetree::createHangingVertex(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  const tarch::la::Vector<Dimensions,int>&     vertexPositionWithin3x3Patch
) const {
  logTraceInWith3Arguments( "createHangingVertex(...)", x, level, vertexPositionWithin3x3Patch );

  GridVertex result(
    GridVertex::State::HangingVertex,
	getAdjacentRanksForNewVertex(coarseGridVertices,vertexPositionWithin3x3Patch),
	false,
    false                                       // antecessor of refined vertex
    #ifdef PeanoDebug
    ,
    x,                                          // const tarch::la::Vector<Dimensions,double>& x
    level                                       // level
    #endif
  );

  logTraceOutWith1Argument( "createHangingVertex(...)", result.toString() );
  return result;
}


tarch::la::Vector<TwoPowerD,int> peano4::grid::Spacetree::getAdjacentRanksForNewVertex(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     vertexPositionWithin3x3Patch
) const {
  tarch::la::Vector<TwoPowerD,int> adjacentRanks(InvalidRank);

  dfor2(k)
    std::bitset<Dimensions> vertexToInherit;
    std::bitset<Dimensions> indexToInherit;
    for ( int d=0; d<Dimensions; d++ ) {
      if (vertexPositionWithin3x3Patch(d)<=1) {
        vertexToInherit.set(d,false);
        indexToInherit.set(d,k(d)>0 or vertexPositionWithin3x3Patch(d)>0);
      }
      else {
        vertexToInherit.set(d,true);
        indexToInherit.set(d,not (k(d)<1 or vertexPositionWithin3x3Patch(d)<3) );
      }
    }
    logDebug( "createNewPersistentVertex(...)", "inherit " << indexToInherit.to_ulong() << "th index from coarse vertex " << vertexToInherit << " into local index " << kScalar );
    adjacentRanks(kScalar) = coarseGridVertices[ vertexToInherit.to_ulong() ].getAdjacentRanks(indexToInherit.to_ulong());
  enddforx

  return adjacentRanks;
}


peano4::grid::GridVertex peano4::grid::Spacetree::createNewPersistentVertex(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  const tarch::la::Vector<Dimensions,int>&     vertexPositionWithin3x3Patch
) const {
  logTraceInWith3Arguments( "createNewPersistentVertex(...)", x, level, vertexPositionWithin3x3Patch );

  GridVertex result(
    GridVertex::State::Unrefined,
	getAdjacentRanksForNewVertex(coarseGridVertices,vertexPositionWithin3x3Patch),
	false,
    false                                       // antecessor of refined vertex
    #ifdef PeanoDebug
    ,
    x,                                          // const tarch::la::Vector<Dimensions,double>& x
    level                                       // level
    #endif
  );

  logTraceOutWith1Argument( "createNewPersistentVertex(...)", result.toString() );
  return result;
}


void peano4::grid::Spacetree::loadVertices(
  const AutomatonState&                        fineGridStatesState,
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
) {
  logTraceInWith3Arguments( "loadVertices(...)", fineGridStatesState.toString(), cellPositionWithin3x3Patch, _id );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridStatesState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> vertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(vertexIndex);

    #ifdef PeanoDebug
    const tarch::la::Vector<Dimensions,double> x = fineGridStatesState.getX()
      + tarch::la::multiplyComponents(
          convertToIntegerVector(vertexIndex).convertScalar<double>(),
		  fineGridStatesState.getH()
	    );
    #endif

    VertexType type        = getVertexType(coarseGridVertices,vertexPositionWithinPatch);
    const int  stackNumber = PeanoCurve::getReadStackNumber(fineGridStatesState,vertexIndex);

    // reset to persistent, as new vertex already has been generated
    if ( not PeanoCurve::isInOutStack(stackNumber) and type==VertexType::New ) {
      type = VertexType::Persistent;
      logDebug(
        "loadVertices(...)",
		"reset stack flag for local vertex " << vertexPositionWithinPatch << " from new to persistent" );
    }

    switch (type) {
      case VertexType::New:
    	logDebug( "loadVertices(...)", "stack no=" << stackNumber );
        if ( PeanoCurve::isInOutStack(stackNumber) ) {
          fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = createNewPersistentVertex(coarseGridVertices,x,fineGridStatesState.getLevel(),vertexPositionWithinPatch);
        }
    	break;
      case VertexType::Hanging:
      	fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = createHangingVertex(coarseGridVertices,x,fineGridStatesState.getLevel(),vertexPositionWithinPatch);
      	break;
      case VertexType::Persistent:
      case VertexType::Delete:
        {
          logDebug( "readVertices(...)", "read vertex from stack " << stackNumber );
          assertion3( not _vertexStack[stackNumber].empty(), stackNumber, vertexIndex, vertexPositionWithinPatch );
          fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ]  = _vertexStack[stackNumber].pop();
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
            updateVertexAfterLoad(
	          fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],
			  coarseGridVertices,
			  vertexPositionWithinPatch
			);
          }
        }
        break;
    }

    logDebug(
      "loadVertices(...)",
	  "handle " << toString(type) << " vertex " << vertexIndex << " at " << vertexPositionWithinPatch << ": " <<
	  fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString()
	);

    assertionNumericalEquals3(
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getX(), x,
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString(),
	  fineGridStatesState.toString(), vertexIndex
	);
    assertionEquals3(
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getLevel(), fineGridStatesState.getLevel(),
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString(),
	  fineGridStatesState.toString(), vertexIndex
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
    const std::bitset<Dimensions>           vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> vertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(vertexIndex);

    const int   stackNumber = PeanoCurve::getWriteStackNumber(fineGridStatesState,vertexIndex);
    VertexType  type        = getVertexType(coarseGridVertices,vertexPositionWithinPatch);

    switch (type) {
      case VertexType::New:
      case VertexType::Persistent:
        {
      	  logDebug(
            "storeVertices(...)",
			"write vertex " << fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString() << " to stack " << stackNumber
		  );
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
            updateVertexBeforeStore(
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],
			  coarseGridVertices,
			  vertexPositionWithinPatch
            );
          }
      	  _vertexStack[stackNumber].push( fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] );
        }
        break;
      case VertexType::Hanging:
    	logDebug( "storeVertices(...)", "discard vertex " << fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString() );
    	break;
      case VertexType::Delete:
      	logDebug( "storeVertices(...)", "delete vertex " << fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString() );
        if ( !PeanoCurve::isInOutStack(stackNumber) ) {
       	  _vertexStack[stackNumber].push( fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] );
        }
        break;
    }
  }

  logTraceOutWith1Argument( "storeVertices(...)", fineGridStatesState.toString() );
}


std::set<int>  peano4::grid::Spacetree::getAdjacentDomainIds( const GridVertex& vertex ) const {
  std::set<int> neighbourIds;
  for (int i=0; i<TwoPowerD; i++) {
    if (
      vertex.getAdjacentRanks(i)!=_id
      and
      vertex.getAdjacentRanks(i)!=InvalidRank
      and
      not _splitting.count(vertex.getAdjacentRanks(i))==1
    ) {
      neighbourIds.insert( vertex.getAdjacentRanks(i) );
    }
  }
  return neighbourIds;
}


void peano4::grid::Spacetree::receiveAndMergeVertexIfAdjacentToDomainBoundary( GridVertex& vertex ) {
  if (
    _spacetreeState==SpacetreeState::NewFromSplit
	 and
	 isVertexAdjacentToLocalSpacetree(vertex,true,true)
  ) {
	vertex = peano4::parallel::Node::getInstance().getVertexSynchronously(_masterId,_id);
	assertion2( vertex.getState() != GridVertex::Refining, vertex.toString(), _id );
	assertion2( vertex.getState() != GridVertex::Erasing,  vertex.toString(), _id );
  }


  if (
    (_spacetreeState==SpacetreeState::Running or _spacetreeState==SpacetreeState::JoinTriggered)
	and
	// @todo mjuss false sein, denn waehrend wir mergen ist ein Vertex ja noch net local
	isVertexAdjacentToLocalSpacetree(vertex,true,false)
  ) {
    std::set<int> neighbourIds = getAdjacentDomainIds(vertex);
    for (auto neighbour: neighbourIds) {
      assertion1( neighbour>=0, neighbour );
      const int  inStack  = peano4::parallel::Node::getInstance().getInputStackNumberOfBoundaryExchange(neighbour);

      assertion6(
        not _vertexStack[ inStack ].empty(),
		vertex.toString(), _id, inStack,
		_joinTriggered, _joining, neighbour
      );

      GridVertex inVertex = _vertexStack[ inStack ].pop();

      assertion4( vertex.getState()!=GridVertex::State::HangingVertex,   inVertex.toString(), vertex.toString(), _id, neighbour );
      assertion4( inVertex.getState()!=GridVertex::State::HangingVertex, inVertex.toString(), vertex.toString(), _id, neighbour );
      assertionVectorNumericalEquals4( vertex.getX(), inVertex.getX(),   inVertex.toString(), vertex.toString(), _id, neighbour );
      assertionEquals4( vertex.getLevel(), inVertex.getLevel(),          inVertex.toString(), vertex.toString(), _id, neighbour );

      //
      // Merge adjacency lists. The neighbour owns some entries
      //
      for (int i=0; i<TwoPowerD; i++) {
        if (vertex.getAdjacentRanks(i)==neighbour and neighbour!=InvalidRank) {
          vertex.setAdjacentRanks(i, inVertex.getAdjacentRanks(i));
        }
      }

      //
      // Merge helper flags
      //
      vertex.setIsAntecessorOfRefinedVertexInCurrentTreeSweep( vertex.getIsAntecessorOfRefinedVertexInCurrentTreeSweep() | inVertex.getIsAntecessorOfRefinedVertexInCurrentTreeSweep() );

      //
      // Update refinement flags
      //
      assertion3( vertex.getState() != GridVertex::State::Erasing,  vertex.toString(), inVertex.toString(), _id );
      assertion3( vertex.getState() != GridVertex::State::Refining, vertex.toString(), inVertex.toString(), _id );

      const bool OnlyNeighbourHasTriggeredRefinement =
        inVertex.getState()==GridVertex::State::RefinementTriggered &&
        vertex.getState()==GridVertex::State::Unrefined;
      const bool OnlyNeighbourHasTriggeredErase =
        inVertex.getState()==GridVertex::State::EraseTriggered &&
        vertex.getState()==GridVertex::State::Refined;
      const bool OnlyLocalHasTriggeredRefinement =
        vertex.getState()==GridVertex::State::RefinementTriggered &&
        inVertex.getState()==GridVertex::State::Unrefined;
      const bool OnlyLocalHasTriggeredErase =
        vertex.getState()==GridVertex::State::EraseTriggered &&
        inVertex.getState()==GridVertex::State::Refined;

      if (OnlyNeighbourHasTriggeredRefinement || OnlyLocalHasTriggeredRefinement) {
        vertex.setState( GridVertex::State::RefinementTriggered );
        logDebug( "receiveAndMergeVertexIfAdjacentToDomainBoundary( GridVertex )", "set state to " << vertex.toString() << " on tree " << _id << " due to merge with neighbour" );
      }
      else if (OnlyNeighbourHasTriggeredErase || OnlyLocalHasTriggeredErase) {
        vertex.setState( GridVertex::State::EraseTriggered );
        logDebug( "receiveAndMergeVertexIfAdjacentToDomainBoundary( GridVertex )", "set state to " << vertex.toString() << " on tree " << _id << " due to merge with neighbour" );
      }
      else {
        assertionEquals4( vertex.getState(), inVertex.getState(),   inVertex.toString(), vertex.toString(), _id, neighbour );
      }
    }
  }

  // @todo Reihenfolge: Muss danach kommen, denn net dass wir erst reinmergen, indices umsetzen, dadurch wird
  // vertex local und dann versuchen wir auch noch zu empfangen
  if (
    _joining!=NoJoin and
    tarch::la::contains( vertex.getAdjacentRanks(), _joining)
  ) {
    logDebug(
      "receiveAndMergeVertexIfAdjacentToDomainBoundary(GridVertex)",
      "receive updated copy of vertex " << vertex.toString() << " from worker tree " << _joining << " synchronously on tree " << _id
    );
    GridVertex inVertex = peano4::parallel::Node::getInstance().getVertexSynchronously(_joining,_id);
    assertionVectorNumericalEquals4( vertex.getX(), inVertex.getX(),   inVertex.toString(), vertex.toString(), _id, _joining );
    assertionEquals4( vertex.getLevel(), inVertex.getLevel(),          inVertex.toString(), vertex.toString(), _id, _joining );
    vertex = inVertex;
    assertion2( vertex.getState() != GridVertex::Refining,   vertex.toString(), _id );
    assertion2( vertex.getState() != GridVertex::Erasing,    vertex.toString(), _id );
    assertion2( vertex.getState() == GridVertex::Unrefined,  vertex.toString(), _id );
    // alter ranks, i.e. replace joining id with this tree's id
    for (int i=0; i<TwoPowerD; i++) {
      if ( vertex.getAdjacentRanks(i)==_joining) {
        vertex.setAdjacentRanks(i,_id);
      }
    }
    logDebug(
      "receiveAndMergeVertexIfAdjacentToDomainBoundary(GridVertex)",
      "new vertex is " << vertex.toString()
    );
  }

  if (
    not _splitting.empty()
    and
    isVertexAdjacentToLocalSpacetree(vertex,true,true)
  ) {
	std::set<int> targetIds;
	for (int i=0; i<TwoPowerD; i++) {
      if ( _splitting.count( vertex.getAdjacentRanks(i) )>0 ) {
    	targetIds.insert( vertex.getAdjacentRanks(i) );
      }
	}
	for (auto p: targetIds ) {
      peano4::parallel::Node::getInstance().sendVertexSynchronously(vertex,_id,p);
	}
  }

  if (
    _spacetreeState==SpacetreeState::Joining
	 and
	 isVertexAdjacentToLocalSpacetree(vertex,true,true)
  ) {
    logDebug(
      "receiveAndMergeVertexIfAdjacentToDomainBoundary(GridVertex)",
      "send vertex " << vertex.toString() << " on tree " << _id << " synchronously to tree " << _masterId
    );
    peano4::parallel::Node::getInstance().sendVertexSynchronously(vertex,_id,_masterId);
  }
}


void peano4::grid::Spacetree::sendOutVertexIfAdjacentToDomainBoundary( const GridVertex& vertex ) {
  logTraceInWith2Arguments( "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)", vertex.toString(), _id );

  if (
    isVertexAdjacentToLocalSpacetree(vertex,false,true)
	and
	_spacetreeState != SpacetreeState::Joining
  ) {
	std::set<int>  outRanks;
    for (int i=0; i<TwoPowerD; i++) {
      if (
        vertex.getAdjacentRanks(i)!=_id
		and
		vertex.getAdjacentRanks(i)!=InvalidRank
		and
		_splitTriggered.count( vertex.getAdjacentRanks(i) )==0
/*
		and
		_joinTriggered.count( vertex.getAdjacentRanks(i) )==0
*/
      ) {
    	outRanks.insert( vertex.getAdjacentRanks(i) );
      }
    }
    for (auto p: outRanks) {
      //
      // Boundary exchange
      //
      const int stackNo = peano4::parallel::Node::getInstance().getOutputStackNumberOfBoundaryExchange(p);
      if (_spacetreeState==SpacetreeState::JoinTriggered and p!=_masterId) {
    	GridVertex vertexCopy = vertex;
    	for (int i=0; i<TwoPowerD; i++) {
    	  vertexCopy.setAdjacentRanks(i,
            vertexCopy.getAdjacentRanks(i)==_id ? _masterId : vertexCopy.getAdjacentRanks(i)
          );
    	}
        _vertexStack[ stackNo ].push( vertexCopy );
        logDebug(
          "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)",
  		  "deliver vertex " << vertexCopy.toString() <<
		  " instead of vertex " << vertex.toString() << " to tree " << p <<
		  " as we are about to join"
        );
      }
      else {
        _vertexStack[ stackNo ].push( vertex );
        logDebug(
          "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)",
		  "vertex " << vertex.toString() << " on tree " << _id <<
		  " goes to tree " << p << " through stack " << stackNo
        );
      }
    }
  }
  else {
    logDebug(
      "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)",
      "vertex " << vertex.toString() << " on tree " << _id <<
      " is not local"
    );
  }
  logTraceOutWith1Argument( "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)", isVertexAdjacentToLocalSpacetree(vertex,true,true) );
}


void peano4::grid::Spacetree::clearStatistics() {
  _statistics.setNumberOfRefinedVertices( 0 );
  _statistics.setNumberOfUnrefinedVertices( 0 );
  _statistics.setNumberOfErasingVertices( 0 );
  _statistics.setNumberOfRefiningVertices( 0 );

  _statistics.setNumberOfLocalUnrefinedCells( 0 );
  _statistics.setNumberOfRemoteUnrefinedCells( 0 );
  _statistics.setNumberOfLocalRefinedCells( 0 );
  _statistics.setNumberOfRemoteRefinedCells( 0 );

  _statistics.setStationarySweeps( _statistics.getStationarySweeps()+1 );
  if (_statistics.getStationarySweeps()>NumberOfStationarySweepsToWaitAtLeastTillJoin+1) {
    _statistics.setStationarySweeps( NumberOfStationarySweepsToWaitAtLeastTillJoin+1 );
  }
}


void peano4::grid::Spacetree::incrementNumberOfAdjacentRefinedLocalCells(GridVertex  vertices[TwoPowerD]) {
  dfor2(k)
    vertices[kScalar].setNumberOfAdjacentRefinedLocalCells( vertices[kScalar].getNumberOfAdjacentRefinedLocalCells()+1 );
  enddforx
}


void peano4::grid::Spacetree::updateVertexRanksWithinCell( GridVertex fineGridVertices[TwoPowerD], int newId ) {
  dfor2(k)
    fineGridVertices[kScalar].setAdjacentRanks(TwoPowerD-kScalar-1,newId);
  enddforx
}



void peano4::grid::Spacetree::evaluateGridControlEvents(
  const AutomatonState& state,
  GridVertex            coarseGridVertices[TwoPowerD],
  GridVertex            fineGridVertices[TwoPowerD]
) const {
  logTraceInWith1Argument( "evaluateGridControlEvents(...)", state.toString() );

  bool mayChangeGrid = true;
  for (int i=0; i<TwoPowerD; i++) {
    mayChangeGrid &= (
      coarseGridVertices[i].getState()==GridVertex::State::HangingVertex
	  or
      coarseGridVertices[i].getState()==GridVertex::State::Unrefined
	  or
      coarseGridVertices[i].getState()==GridVertex::State::Refined
    );
  }

  bool refine = false;
  bool erase  = false;
  if (mayChangeGrid) {
    for (auto p: _gridControlEvents) {
      if (
        tarch::la::allGreater( state.getX() + state.getH(), p.getOffset() )
        and
        tarch::la::allSmaller( state.getX(), p.getOffset()+p.getWidth() )
        and
        p.getRefinementControl()==GridControlEvent::RefinementControl::Refine
        and
 	    tarch::la::allGreaterEquals( state.getH(), p.getH() )
	  ) {
        erase  = false;
        refine = true;
      }

      if (
        tarch::la::allGreaterEquals( state.getX(), p.getOffset() )
        and
        tarch::la::allSmallerEquals( state.getX() + state.getH(), p.getOffset()+p.getWidth() )
        and
        p.getRefinementControl()==GridControlEvent::RefinementControl::Erase
        and
   	    tarch::la::allSmaller( state.getH(), p.getH()*3.0 )
  	  ) {
        erase  = true;
        refine = false;
      }
    }
  }

  if (refine) {
    for (int i=0; i<TwoPowerD; i++) {
      if (
		isVertexAdjacentToLocalSpacetree( fineGridVertices[i], true, true )
        and
	    fineGridVertices[i].getState()==GridVertex::State::Unrefined
		//and
		//not fineGridVertices[i].getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep()
      ) {
  	    fineGridVertices[i].setState( GridVertex::State::RefinementTriggered );
      }
    }
  }

  if (erase) {
    for (int i=0; i<TwoPowerD; i++) {
      if (
		isVertexAdjacentToLocalSpacetree( fineGridVertices[i], true, true )
        and
	    fineGridVertices[i].getState()==GridVertex::State::Refined
		and
		not fineGridVertices[i].getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep()
      ) {
  	    fineGridVertices[i].setState( GridVertex::State::EraseTriggered );
      }
    }
  }

  logInfo( "evaluateGridControlEvents(...)", "state=" << state.toString() << ", erase=" << erase );
  logTraceOutWith3Arguments( "evaluateGridControlEvents(...)", state.toString(), refine, erase );
}


void peano4::grid::Spacetree::descend(
  const AutomatonState& state,
  GridVertex            vertices[TwoPowerD],
  TraversalObserver&    observer
) {
  assertion( isSpacetreeNodeRefined(vertices) );
  logTraceInWith2Arguments( "descend(...)", state.toString(), _id );

  #if PeanoDebug>=2
  dfor2(k)
    logDebug( "descend(...)", "-" << vertices[kScalar].toString() );
  enddforx
  #endif

  peano4::utils::LoopDirection loopDirection = PeanoCurve::getLoopDirection(state);

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
    // Mesh refinement
    //
    evaluateGridControlEvents(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices);

    //
    // Enter cell
    //
    if ( isSpacetreeNodeLocal(fineGridVertices) ) {
      observer.enterCell(
        fineGridStates[peano4::utils::dLinearised(k,3)].getX(),
	    fineGridStates[peano4::utils::dLinearised(k,3)].getH(),
	    isSpacetreeNodeRefined(fineGridVertices),
		_id
	  );
      if ( not isSpacetreeNodeLocal(vertices) ) {
        updateVerticesAroundForkedCell(fineGridVertices);
      }
    }
    else {
      logDebug( "descend(...)", "node is not local on tree " << _id );
      if ( isSpacetreeNodeLocal(vertices) ) {
        updateVerticesAroundForkedCell(fineGridVertices);
      }
    }

    //
    // DFS
    //
    if (isSpacetreeNodeRefined(fineGridVertices)) {
      if ( isSpacetreeNodeLocal(fineGridVertices) ) {
        _statistics.setNumberOfLocalRefinedCells( _statistics.getNumberOfLocalRefinedCells()+1 );
        incrementNumberOfAdjacentRefinedLocalCells( fineGridVertices );
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
    if ( isSpacetreeNodeLocal(fineGridVertices) ) {
      observer.leaveCell(
        fineGridStates[peano4::utils::dLinearised(k,3)].getX(),
	    fineGridStates[peano4::utils::dLinearised(k,3)].getH(),
	    isSpacetreeNodeRefined(fineGridVertices),
		_id
	  );
    }


    // For a discussion of admissible splits, see split()
    if (
      not _splitTriggered.empty() and
      isSpacetreeNodeLocal(fineGridVertices) and
      isSpacetreeNodeLocal(vertices)
	) {
      int targetSpacetreeId = -1;
      for (auto& p: _splitTriggered) {
    	if (p.second>0) {
          p.second--;
          targetSpacetreeId = p.first;
          break;
    	}
      }
      // still some splits left
      if ( targetSpacetreeId>=0 ) {
    	logDebug( "descend(...)", "deploy cell " << fineGridStates[peano4::utils::dLinearised(k,3)].toString() << " to tree " << targetSpacetreeId );
        updateVertexRanksWithinCell( fineGridVertices, targetSpacetreeId );
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


void peano4::grid::Spacetree::updateVerticesAroundForkedCell(
  GridVertex            fineGridVertices[TwoPowerD]
) const {
  dfor2(k)
    fineGridVertices[kScalar].setIsAntecessorOfRefinedVertexInCurrentTreeSweep(true);
  enddforx
}


void peano4::grid::Spacetree::split(int cells) {
  assertion( _joinTriggered==NoJoin );
  assertion( _joining==NoJoin );

  const int newSpacetreeId = peano4::parallel::Node::getInstance().getNextFreeLocalId();
  assertion1( _splitTriggered.count(newSpacetreeId)==0, newSpacetreeId );
  _splitTriggered.insert( std::pair<int,int>(newSpacetreeId,cells) );
  peano4::parallel::Node::getInstance().registerId( newSpacetreeId );
}


std::string peano4::grid::Spacetree::toString() const {
  std::ostringstream msg;
  msg << "(" << toString(_spacetreeState)
	  << "," << _statistics.toString();
  if (_joinTriggered!=NoJoin) {
	msg << ",join-triggered-width=" << _joinTriggered;
  }
  else {
	msg << ",no-join-triggered-width";
  }
  if (_joining!=NoJoin) {
	msg << ",joining-width=" << _joinTriggered;
  }
  else {
	msg << ",not-joining";
  }
  msg << ")";
  return msg.str();
}


bool peano4::grid::Spacetree::mayJoinWithMaster() const {
  return _statistics.getNumberOfLocalRefinedCells()==0
     and _statistics.getNumberOfRefiningVertices()==0
	 and _spacetreeState==SpacetreeState::Running
	 and _masterId>0
     and _statistics.getStationarySweeps()>NumberOfStationarySweepsToWaitAtLeastTillJoin
	 and _joinTriggered==NoJoin
	 and _joining==NoJoin
	 and _splitTriggered.empty()
	 and _splitting.empty();
}


void peano4::grid::Spacetree::joinWithMaster() {
  assertion1( mayJoinWithMaster(), _id);
  _spacetreeState = SpacetreeState::JoinTriggered;
  assertion( _splitTriggered.empty() );
  assertion( _splitting.empty() );
}


void peano4::grid::Spacetree::joinWithWorker(int id) {
  logInfo( "joinWithWorker(int)", "add tree " << id << " to tree " << _id );
  _joinTriggered = id;
  assertion( _splitTriggered.empty() );
  assertion( _splitting.empty() );
}
