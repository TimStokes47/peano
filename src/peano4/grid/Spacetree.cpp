#include "Spacetree.h"
#include "PeanoCurve.h"
#include "TraversalObserver.h"
#include "GridTraversalEvent.h"


#include "peano4/utils/Loop.h"
#include "peano4/parallel/Node.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "tarch/mpi/Rank.h"


tarch::logging::Log  peano4::grid::Spacetree::_log( "peano4::grid::Spacetree" );
const int peano4::grid::Spacetree::InvalidRank(-1);
const int peano4::grid::Spacetree::NumberOfStationarySweepsToWaitAtLeastTillJoin(4);


peano4::grid::Spacetree::Spacetree(
  const tarch::la::Vector<Dimensions,double>&  offset,
  const tarch::la::Vector<Dimensions,double>&  width
):
  _id(0),
  _spacetreeState( SpacetreeState::NewRoot ),
  _root(),
  _masterId(-1) {
  _root.setLevel( 0 );
  _root.setX( offset );
  _root.setH( width );
  _root.setInverted( false );
  _root.setEvenFlags( 0 );

  for (int i=0; i<DimensionsTimesTwo; i++) {
    _root.setAccessNumber( i,0 );
  }

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
  _splitting() {
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


peano4::grid::Spacetree::Spacetree( Spacetree&& other ):
  _id(other._id),
  _spacetreeState( other._spacetreeState ),
  _root( std::move(other._root) ),
  _statistics(),
  _masterId(other._masterId),
  _splitTriggered(),
  _splitting(),
  _joinTriggered(),
  _joining(),
  _vertexStack( std::move(other._vertexStack)) {
}


peano4::grid::Spacetree::~Spacetree() {
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
      result |= (joiningIsConsideredLocal and _joining.count( vertex.getAdjacentRanks(i) )==1);
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
      (
        vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1)==_id
        and
        _splitting.count( vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1) )==0
      )
	  or
      (_joining.count( vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1) )==1)
	);
    isLocal &= _spacetreeState!=SpacetreeState::Joining;
  enddforx

  return isLocal;
}


void peano4::grid::Spacetree::traverse(TraversalObserver& observer, peano4::parallel::SpacetreeSet& spacetreeSet) {
  logTraceIn( "traverse(TraversalObserver,SpacetreeSet)" );

  assertion( _joinTriggered.empty() or _splitTriggered.empty() );
  assertion( _joinTriggered.empty() or _splitting.empty() );
  assertion( _joining.empty()       or _splitTriggered.empty() );
  assertion( _joining.empty()       or _splitting.empty() );

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
    spacetreeSet.addSpacetree(*this,p.first);
  }
  _splitting.clear();
  _splitting.insert( _splitTriggered.begin(), _splitTriggered.end() );
  _splitTriggered.clear();

  _joining.clear();
  _joining.insert( _joinTriggered.begin(), _joinTriggered.end() );
  _joinTriggered.clear();

  switch (_spacetreeState) {
    case SpacetreeState::NewRoot:
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
  logDebug( "traverse(TraversalObserver&)", "got " << _gridControlEvents.size() << " grid control event(s)" );

  #if !defined(SharedMemoryParallelisation) and !defined(Parallel)
  observer.beginTraversal();
  #endif

  _splittedCells.clear();

  _coarseningHasBeenVetoed = false;

  const bool isFirstTraversal = _spacetreeState==SpacetreeState::NewRoot;
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
    if (_id==0) {
      vertices[kScalar].setAdjacentRanks(TwoPowerD-1-kScalar,_id);
    }
    logDebug( "traverse()", "create " << vertices[kScalar].toString() );
  enddforx

  descend(_root,vertices,observer);

  _root.setInverted( not _root.getInverted() );

  #if !defined(SharedMemoryParallelisation) and !defined(Parallel)
  observer.endTraversal();
  #endif

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


peano4::grid::Spacetree::CellType peano4::grid::Spacetree::getCellType(
  GridVertex  vertices[TwoPowerD]
) {
  bool allVerticesAreDelete = true;
  bool allVerticesAreNew    = true;

  for (int i=0; i<TwoPowerD; i++) {
    switch (vertices[ i ].getState()) {
      case GridVertex::State::HangingVertex:
    	break;
      case GridVertex::State::Erasing:
      case GridVertex::State::EraseTriggered:
      case GridVertex::State::RefinementTriggered:
      case GridVertex::State::Refining:
      case GridVertex::State::Refined:
      case GridVertex::State::Unrefined:
        allVerticesAreDelete = false;
    	allVerticesAreNew    = false;
    	break;
      case GridVertex::State::New:
        allVerticesAreDelete = false;
      	break;
      case GridVertex::State::Delete:
       	allVerticesAreNew    = false;
       	break;
    }
  }

  assertion( not (allVerticesAreDelete and allVerticesAreNew) );

  if (allVerticesAreDelete) {
	 return CellType::Delete;
  }
  else if (allVerticesAreNew) {
	 return CellType::New;
  }
  else return CellType::Persistent;
}


peano4::grid::Spacetree::FaceType peano4::grid::Spacetree::getFaceType(
  GridVertex                         coarseGridVertices[TwoPowerD],
  int                                faceNumber
) {
  FaceType  result = FaceType::Persistent;

  bool allVerticesAreNew      = true;
  bool allVerticesAreHanging  = true;
  bool allVerticesAreDelete   = true;

  const int normal = faceNumber % Dimensions;
  for (int i=0; i<TwoPowerD; i++) {
	std::bitset<Dimensions> studiedVertex = i;
    studiedVertex.set(normal,faceNumber>=Dimensions);
    switch (coarseGridVertices[ studiedVertex.to_ulong() ].getState()) {
      case GridVertex::State::HangingVertex:
        break;
      case GridVertex::State::New:
    	allVerticesAreDelete  = false;
    	allVerticesAreHanging = false;
        break;
      case GridVertex::State::RefinementTriggered:
      case GridVertex::State::Unrefined:
      case GridVertex::State::Refined:
      case GridVertex::State::EraseTriggered:
      case GridVertex::State::Erasing:
      case GridVertex::State::Refining:
    	allVerticesAreNew     = false;
    	allVerticesAreDelete  = false;
    	allVerticesAreHanging = false;
    	break;
      case GridVertex::State::Delete:
    	allVerticesAreNew     = false;
    	allVerticesAreHanging = false;
    	break;
    }
  }

  if ( allVerticesAreHanging ) {
	return FaceType::Hanging;
  }
  else if ( allVerticesAreNew ) {
	assertion( not allVerticesAreHanging );
	assertion( not allVerticesAreDelete );
	return FaceType::New;
  }
  else if ( allVerticesAreDelete ) {
	assertion( not allVerticesAreHanging );
	assertion( not allVerticesAreNew );
	return FaceType::Delete;
  }
  else return FaceType::Persistent;
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
      case GridVertex::State::Delete:
      case GridVertex::State::New:
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
    case SpacetreeState::NewRoot:
      return "new-root";
    case SpacetreeState::NewFromSplit:
      return "new-from-split";
    case SpacetreeState::Running:
      return "running";
    case SpacetreeState::JoinTriggered:
      return "join-triggered";
    case SpacetreeState::Joining:
      return "joining";
    case SpacetreeState::Joined:
      return "joined";
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


std::string peano4::grid::Spacetree::toString( FaceType type ) {
  switch (type) {
    case FaceType::New:
      return "new";
    case FaceType::Hanging:
      return "hanging";
    case FaceType::Persistent:
      return "persistent";
    case FaceType::Delete:
      return "delete";
  }
  return "<undef>";
}


std::string peano4::grid::Spacetree::toString( CellType type ) {
  switch (type) {
    case CellType::New:
      return "new";
    case CellType::Persistent:
      return "persistent";
    case CellType::Delete:
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
      logDebug( "updateVertexAfterLoad(...)", "vertex " << vertex.toString() << " may not be erased on tree " << _id << " as it is father of further refined vertices. Unroll flag" );
	  vertex.setState( GridVertex::State::Refined );
	  _coarseningHasBeenVetoed = true;
 	}
	else {
      logDebug( "updateVertexAfterLoad(...)", "erase vertex " << vertex.toString() << " outside of domain on tree " << _id );
      vertex.setState( GridVertex::State::Erasing );
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
  logTraceInWith2Arguments( "updateVertexBeforeStore()", vertex.toString(), _id );

  if (
	vertex.getState()==GridVertex::State::New
  ) {
	vertex.setState( GridVertex::State::Unrefined );
  }

  if (
    vertex.getNumberOfAdjacentRefinedLocalCells()==TwoPowerD
	and
	vertex.getState()==GridVertex::State::Unrefined
  ) {
	vertex.setState( GridVertex::State::RefinementTriggered );
	logDebug( "updateVertexBeforeStore(...)", "have to post-refine vertex " << vertex.toString() );
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

  if ( not isVertexAdjacentToLocalSpacetree(vertex,false,false)) {
	std::set<int> children = peano4::parallel::Node::getInstance().getChildren(_id);
	for (int i=0; i<TwoPowerD; i++) {
	  if (
			  /*
        vertex.getAdjacentRanks(i)!=_id
		and
        vertex.getAdjacentRanks(i)!=_masterId
		and
*/
		children.count( vertex.getAdjacentRanks(i) )==0
      ) {
        vertex.setAdjacentRanks(i,InvalidRank);
	  }
	}
  }

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
    GridVertex::State::New,
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
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch,
  TraversalObserver&                           observer
) {
  logTraceInWith3Arguments( "loadVertices(...)", fineGridStatesState.toString(), cellPositionWithin3x3Patch, _id );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridStatesState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> vertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(vertexIndex);

    const tarch::la::Vector<Dimensions,double> x = fineGridStatesState.getX()
      + tarch::la::multiplyComponents(
          convertToIntegerVector(vertexIndex).convertScalar<double>(),
		  fineGridStatesState.getH()
	    );

    VertexType type        = getVertexType(coarseGridVertices,vertexPositionWithinPatch);
    const int  stackNumber = PeanoCurve::getVertexReadStackNumber(fineGridStatesState,vertexIndex);

    // reset to persistent, as new vertex already has been generated
    if ( not PeanoCurve::isInOutStack(stackNumber) and type==VertexType::New ) {
      type = VertexType::Persistent;
      logDebug(
        "loadVertices(...)",
		"reset stack flag for local vertex " << vertexPositionWithinPatch << " from new/hanging to persistent" );
    }

    switch (type) {
      case VertexType::New:
    	logDebug( "loadVertices(...)", "stack no=" << stackNumber );
        assertion( PeanoCurve::isInOutStack(stackNumber) );
        fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = createNewPersistentVertex(coarseGridVertices,x,fineGridStatesState.getLevel(),vertexPositionWithinPatch);
    	break;
      case VertexType::Hanging:
      	fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = createHangingVertex(coarseGridVertices,x,fineGridStatesState.getLevel(),vertexPositionWithinPatch);
      	break;
      case VertexType::Persistent:
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
            fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].setState(GridVertex::Delete);
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
  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch,
  TraversalObserver&                           observer
) {
  logTraceInWith1Argument( "storeVertices(...)", fineGridStatesState.toString() );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(fineGridStatesState);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const tarch::la::Vector<Dimensions,int> vertexPositionWithinPatch = cellPositionWithin3x3Patch + convertToIntegerVector(vertexIndex);

    const tarch::la::Vector<Dimensions,double> x = fineGridStatesState.getX()
      + tarch::la::multiplyComponents(
          convertToIntegerVector(vertexIndex).convertScalar<double>(),
		  fineGridStatesState.getH()
	    );

    const int   stackNumber = PeanoCurve::getVertexWriteStackNumber(fineGridStatesState,vertexIndex);
    VertexType  type        = getVertexType(coarseGridVertices,vertexPositionWithinPatch);

    if ( not PeanoCurve::isInOutStack(stackNumber) and type==VertexType::Delete) {
      type = VertexType::Persistent;
      logDebug(
        "storeVertices(...)",
		"reset stack flag for local vertex " << vertexPositionWithinPatch << " from new/hanging to persistent" );
    }

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
        assertion( PeanoCurve::isInOutStack(stackNumber) );
        break;
    }
  }

  logTraceOutWith1Argument( "storeVertices(...)", fineGridStatesState.toString() );
}


std::set<int>  peano4::grid::Spacetree::getAdjacentDomainIds( const GridVertex& vertex, bool calledByReceivingProcess ) const {
  const bool isLocalVertex =
	  calledByReceivingProcess ?
      isVertexAdjacentToLocalSpacetree(vertex,true,false) :
      isVertexAdjacentToLocalSpacetree(vertex,false,true);

  std::set<int> neighbourIds;
  for (int i=0; i<TwoPowerD; i++) {
    const bool mandatoryCriteria =
		      vertex.getAdjacentRanks(i)!=_id
		      and
		      vertex.getAdjacentRanks(i)!=InvalidRank;

    // I should not try to receive anything from a node that we just are
    // splitting into.
    const bool receiverCriteria = (_spacetreeState!=SpacetreeState::NewFromSplit or _masterId!=vertex.getAdjacentRanks(i))
    		                  and (_splitting.count(vertex.getAdjacentRanks(i))==0);
    // I should not send out anything to a node that I will split into in
    // the next sweep
    const bool senderCriteria   = (_joining.count( vertex.getAdjacentRanks(i)) ==0 )
    		                  and (_splitTriggered.count(vertex.getAdjacentRanks(i))==0);

    const bool insertCriteria = calledByReceivingProcess ?
      (mandatoryCriteria and isLocalVertex and receiverCriteria) :
      (mandatoryCriteria and isLocalVertex and senderCriteria);

    if (insertCriteria) {
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
	assertion5(
      not _vertexStack[ peano4::parallel::Node::getInstance().getInputStackNumberForSplitMergeDataExchange(_masterId) ].empty(),
	  peano4::parallel::Node::getInstance().getInputStackNumberForSplitMergeDataExchange(_masterId), _masterId, vertex.toString(), _id,
	  toString()
	);
    GridVertex inVertex = _vertexStack[ peano4::parallel::Node::getInstance().getInputStackNumberForSplitMergeDataExchange(_masterId) ].pop();
    assertion2( vertex.getState() != GridVertex::Refining, vertex.toString(), _id );
    assertion2( vertex.getState() != GridVertex::Erasing,  vertex.toString(), _id );
  }

  if ( _spacetreeState!=SpacetreeState::NewFromSplit ) {
    std::set<int> neighbourIds = getAdjacentDomainIds(vertex,true);

    for (auto neighbour: neighbourIds) {
      assertion1( neighbour>=0, neighbour );
      const int  inStack  = peano4::parallel::Node::getInstance().getInputStackNumberOfBoundaryExchange(neighbour);

      assertion5(
        not _vertexStack[ inStack ].empty(),
		vertex.toString(), inStack, neighbour, _id,
		toString()
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
        (vertex.getState()==GridVertex::State::Unrefined or vertex.getState()==GridVertex::State::New or vertex.getState()==GridVertex::State::Delete);
      const bool OnlyLocalHasTriggeredRefinement =
        vertex.getState()==GridVertex::State::RefinementTriggered &&
        (inVertex.getState()==GridVertex::State::Unrefined or inVertex.getState()==GridVertex::State::New or inVertex.getState()==GridVertex::State::Delete);
      const bool OnlyNeighbourHasTriggeredErase =
        inVertex.getState()==GridVertex::State::EraseTriggered &&
        vertex.getState()==GridVertex::State::Refined;
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
    not _joining.empty()
  ) {
	std::set<int> targetIds;
	for (int i=0; i<TwoPowerD; i++) {
      if ( _joining.count( vertex.getAdjacentRanks(i) )>0 ) {
    	targetIds.insert( vertex.getAdjacentRanks(i) );
      }
	}
	for (auto p: targetIds ) {
      logDebug(
        "receiveAndMergeVertexIfAdjacentToDomainBoundary(GridVertex)",
        "receive updated copy of vertex " << vertex.toString() << " from worker tree " << p << " synchronously on tree " << _id
      );
      GridVertex inVertex = _vertexStack[ peano4::parallel::Node::getInstance().getInputStackNumberForSplitMergeDataExchange(p) ].pop();
      assertionVectorNumericalEquals4( vertex.getX(), inVertex.getX(),   inVertex.toString(), vertex.toString(), _id, p );
      assertionEquals4( vertex.getLevel(), inVertex.getLevel(),          inVertex.toString(), vertex.toString(), _id, p );
      vertex = inVertex;
      assertion2( vertex.getState() != GridVertex::Refining,   vertex.toString(), _id );
      assertion2( vertex.getState() != GridVertex::Erasing,    vertex.toString(), _id );
      assertion2( vertex.getState() == GridVertex::Unrefined,  vertex.toString(), _id );

      // alter ranks, i.e. replace joining id with this tree's id
      for (int i=0; i<TwoPowerD; i++) {
        if ( vertex.getAdjacentRanks(i)==p) {
          vertex.setAdjacentRanks(i,_id);
        }
      }
      logDebug(
        "receiveAndMergeVertexIfAdjacentToDomainBoundary(GridVertex)",
        "new vertex is " << vertex.toString()
      );
    }
  }

  if ( not _splitting.empty() and isVertexAdjacentToLocalSpacetree(vertex,true,true) ) {
    std::set<int> targetIds;
    for (int i=0; i<TwoPowerD; i++) {
      if ( _splitting.count( vertex.getAdjacentRanks(i) )>0 ) {
        targetIds.insert( vertex.getAdjacentRanks(i) );
      }
    }
    for (auto p: targetIds ) {
      _vertexStack[ peano4::parallel::Node::getInstance().getOutputStackNumberForSplitMergeDataExchange(p) ].push(vertex);
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
    _vertexStack[ peano4::parallel::Node::getInstance().getOutputStackNumberForSplitMergeDataExchange(_id) ].push(vertex);
  }
}


void peano4::grid::Spacetree::sendOutVertexIfAdjacentToDomainBoundary( const GridVertex& vertex ) {
  logTraceInWith2Arguments( "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)", vertex.toString(), _id );

  if (
	_spacetreeState != SpacetreeState::Joining
  ) {
    std::set<int> outRanks = getAdjacentDomainIds(vertex,false);

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
  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    _statistics.setNumberOfRefinedVertices( TwoPowerD );
    _statistics.setNumberOfLocalRefinedCells( 1 );
    _statistics.setNumberOfRemoteRefinedCells( 0 );
  }
  else {
    _statistics.setNumberOfRefinedVertices( 0 );
    _statistics.setNumberOfLocalRefinedCells( 0 );
    _statistics.setNumberOfRemoteRefinedCells( 1 );
  }

  _statistics.setNumberOfUnrefinedVertices( 0 );
  _statistics.setNumberOfErasingVertices( 0 );
  _statistics.setNumberOfRefiningVertices( 0 );

  _statistics.setNumberOfLocalUnrefinedCells( 0 );
  _statistics.setNumberOfRemoteUnrefinedCells( 0 );

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
) {
  logTraceInWith1Argument( "evaluateGridControlEvents(...)", state.toString() );

  bool mayChangeGrid = true;
  for (int i=0; i<TwoPowerD; i++) {
    // excluding refinement triggered and refining ensures that we have no immediate
	// refine anymore. This is important for the adjacency lists. We have to allow
	// erase triggered, as the grid control events are evaluated top-down, i.e. data
	// might be set.
    mayChangeGrid &= (
      coarseGridVertices[i].getState()==GridVertex::State::HangingVertex
	  or
      coarseGridVertices[i].getState()==GridVertex::State::Unrefined
	  or
      coarseGridVertices[i].getState()==GridVertex::State::Refined
	  or
      coarseGridVertices[i].getState()==GridVertex::State::EraseTriggered
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

      //Es gibt viel zu viele Zellen in toto! Da wird net sauber weggeloescht ausserhalb der Domaene

      if (
        tarch::la::allGreaterEquals( state.getX(), p.getOffset() )
        and
        tarch::la::allSmallerEquals( state.getX() + state.getH(), p.getOffset()+p.getWidth() )
        and
        p.getRefinementControl()==GridControlEvent::RefinementControl::Erase
        and
   	    tarch::la::allSmaller( state.getH(), p.getH() )
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
      ) {
  	    fineGridVertices[i].setState( GridVertex::State::EraseTriggered );
  	    if (not isSpacetreeNodeLocal(coarseGridVertices)) {
          logDebug( "evaluateGridControlEvents(...)", "emergency flag set on tree " << _id << ": seems to be unable to erase because of domain decomposition");
  	    }
      }
    }
  }

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
    loadVertices(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices, k, observer);

    //
    // Mesh refinement
    // Enter cell
    //
    if ( isSpacetreeNodeLocal(fineGridVertices) ) {
      if ( not isSpacetreeNodeLocal(vertices) ) {
        updateVerticesAroundForkedCell(vertices);
      }
      evaluateGridControlEvents(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices);

      observer.enterCell(createEnterCellTraversalEvent(
        fineGridVertices,fineGridStates[peano4::utils::dLinearised(k,3)]
      ));
    }
    else {
      if ( isSpacetreeNodeLocal(vertices) ) {
        updateVerticesAroundForkedCell(vertices);
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
      observer.leaveCell(createLeaveCellTraversalEvent(
        fineGridVertices,fineGridStates[peano4::utils::dLinearised(k,3)]
      ));
    }

    splitOrMoveNode(
      vertices,
	  fineGridVertices
    );


    //
    // Store vertices
    //
    storeVertices(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices, k, observer);
  endzfor

  logTraceOut( "descend(...)" );
}


peano4::grid::GridTraversalEvent peano4::grid::Spacetree::createEnterCellTraversalEvent(
  GridVertex                                   fineGridVertices[TwoPowerD],
  const AutomatonState&                        state
) const {
  logTraceInWith1Argument( "createEnterCellTraversalEvent(...)", state.toString() );
  GridTraversalEvent  event;

  event.setX( state.getX() + state.getH()*0.5 );
  event.setH( state.getH() );
  event.setIsRefined( isSpacetreeNodeRefined(fineGridVertices) );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(state);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>  vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const int  stackNumber = PeanoCurve::getVertexReadStackNumber(state,vertexIndex);

    switch ( fineGridVertices[vertexIndex.to_ulong()].getState() ) {
      case GridVertex::HangingVertex:
   	    event.setVertexDataFrom(i,TraversalObserver::CreateOrDestroyHangingGridEntity);
    	break;
      case GridVertex::New:
        {
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
            event.setVertexDataFrom(i,TraversalObserver::CreateOrDestroyPersistentGridEntity);
          }
          else {
            event.setVertexDataFrom(i,stackNumber);
          }
        }
    	break;
      case GridVertex::Unrefined:
      case GridVertex::Refined:
      case GridVertex::RefinementTriggered:
      case GridVertex::Refining:
      case GridVertex::EraseTriggered:
      case GridVertex::Erasing:
      case GridVertex::Delete:
        event.setVertexDataFrom(i,stackNumber);
    	break;
    }
    event.setVertexDataTo(i,vertexIndex.to_ulong());
  }


  for (int i=0; i<2*Dimensions; i++) {
    int        faceIndex   = PeanoCurve::getFaceNumberAlongCurve(state,i);
    FaceType   type        = getFaceType(fineGridVertices,faceIndex);
    const int  stackNumber = PeanoCurve::getFaceReadStackNumber(state,faceIndex);

    switch (type) {
        case FaceType::New:
        {
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
            event.setFaceDataFrom(i,TraversalObserver::CreateOrDestroyPersistentGridEntity);
          }
          else {
            event.setFaceDataFrom(i,stackNumber);
          }
        }
    	break;
      case FaceType::Hanging:
   	    event.setFaceDataFrom(i,TraversalObserver::CreateOrDestroyHangingGridEntity);
      	break;
      case FaceType::Persistent:
      case FaceType::Delete:
        event.setFaceDataFrom(i,stackNumber);
        break;
    }
    event.setFaceDataTo(i,faceIndex);
  }

  {
	CellType   type        = getCellType(fineGridVertices);
    const int  stackNumber = PeanoCurve::getCellReadStackNumber(state);

    switch (type) {
      case CellType::New:
        event.setCellData(TraversalObserver::CreateOrDestroyPersistentGridEntity);
    	break;
      case CellType::Persistent:
      case CellType::Delete:
        event.setCellData(stackNumber);
    	break;
    }
  }

  logTraceOutWith2Arguments( "createEnterCellTraversalEvent(...)", state.toString(), event.toString() );
  return event;
}


peano4::grid::GridTraversalEvent peano4::grid::Spacetree::createLeaveCellTraversalEvent(
  GridVertex              fineGridVertices[TwoPowerD],
  const AutomatonState&   state
) const {
  logTraceInWith1Argument( "createLeaveCellTraversalEvent(...)", state.toString() );
  GridTraversalEvent  event;

  event.setX( state.getX() + state.getH()*0.5 );
  event.setH( state.getH() );
  event.setIsRefined( isSpacetreeNodeRefined(fineGridVertices) );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(state);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const int   stackNumber = PeanoCurve::getVertexWriteStackNumber(state,vertexIndex);

    event.setVertexDataFrom(i,vertexIndex.to_ulong());
    switch ( fineGridVertices[vertexIndex.to_ulong()].getState() ) {
      case GridVertex::HangingVertex:
        event.setVertexDataTo(i,TraversalObserver::CreateOrDestroyHangingGridEntity);
      case GridVertex::New:
      case GridVertex::Unrefined:
      case GridVertex::Refined:
      case GridVertex::RefinementTriggered:
      case GridVertex::Refining:
      case GridVertex::EraseTriggered:
      case GridVertex::Erasing:
        event.setVertexDataTo(i,stackNumber);
      case GridVertex::Delete:
        {
          if ( PeanoCurve::isInOutStack(stackNumber) ) {
            event.setVertexDataTo(i,TraversalObserver::CreateOrDestroyPersistentGridEntity);
          }
          else {
            event.setVertexDataTo(i,stackNumber);
          }
        }
        break;
    }
  }

  for (int i=0; i<2*Dimensions; i++) {
    int        faceIndex   = PeanoCurve::getFaceNumberAlongCurve(state,i);
    FaceType   type        = getFaceType(fineGridVertices,faceIndex);
    const int  stackNumber = PeanoCurve::getFaceWriteStackNumber(state,faceIndex);

    event.setFaceDataFrom(i,faceIndex);

    switch (type) {
      case FaceType::Hanging:
        event.setFaceDataTo(i,TraversalObserver::CreateOrDestroyHangingGridEntity);
    	break;
      case FaceType::New:
      case FaceType::Persistent:
        event.setFaceDataTo(i,stackNumber);
        break;
      case FaceType::Delete:
  	    if ( PeanoCurve::isInOutStack(stackNumber) ) {
          event.setFaceDataTo(i,TraversalObserver::CreateOrDestroyPersistentGridEntity);
  	    }
  	    else {
          event.setFaceDataTo(i,stackNumber);
  	    }
        break;
    }
  }

  {
	CellType   type        = getCellType(fineGridVertices);
    const int  stackNumber = PeanoCurve::getCellWriteStackNumber(state);

    switch (type) {
      case CellType::New:
      case CellType::Persistent:
        event.setCellData(stackNumber);
      	break;
      case CellType::Delete:
        event.setCellData(TraversalObserver::CreateOrDestroyPersistentGridEntity);
      	break;
    }
  }

  logTraceOutWith2Arguments( "createLeaveCellTraversalEvent(...)", state.toString(), event.toString() );
  return event;
}


void peano4::grid::Spacetree::splitOrMoveNode(
  GridVertex  vertices[TwoPowerD],
  GridVertex  fineGridVertices[TwoPowerD]
) {
  if (_splitTriggered.empty()) return;

  bool isSplitCandidate =
      isSpacetreeNodeLocal(fineGridVertices) and
	  isSpacetreeNodeLocal(vertices);

  if (isSpacetreeNodeRefined(fineGridVertices)) {
    assertion1( _splittedCells.size()>=ThreePowerD, _splittedCells.size() );

    int reducedMarker = _splittedCells.back();
    for (int i=0; i<ThreePowerD; i++) {
      int topMarker = _splittedCells.back();
      _splittedCells.pop_back();
      if (topMarker!=reducedMarker) {
        reducedMarker = -1;
      }
    }

    if (reducedMarker>=0 and isSplitCandidate) {
      updateVertexRanksWithinCell( fineGridVertices, reducedMarker );
    }
    else {
      reducedMarker = -1;
    }

    _splittedCells.push_back(reducedMarker);
  }
  else { // not refined
    int targetSpacetreeId = -1;
    for (auto& p: _splitTriggered) {
      if (p.second>0) {
        targetSpacetreeId = p.first;
        break;
      }
    }

    if (isSplitCandidate and targetSpacetreeId>=0) {
      updateVertexRanksWithinCell( fineGridVertices, targetSpacetreeId );
      for (auto& p: _splitTriggered) {
        if (p.first==targetSpacetreeId) {
	      p.second--;
          break;
	    }
      }
	  _splittedCells.push_back(targetSpacetreeId);
	}
	else {
	  _splittedCells.push_back(-1);
	}
  }
}


peano4::grid::GridStatistics peano4::grid::Spacetree::getGridStatistics() const {
  return _statistics;
}


void peano4::grid::Spacetree::updateVerticesAroundForkedCell(
  GridVertex            coarseGridVertices[TwoPowerD]
) const {
  dfor2(k)
    coarseGridVertices[kScalar].setIsAntecessorOfRefinedVertexInCurrentTreeSweep(true);
  enddforx
}


void peano4::grid::Spacetree::split(int newSpacetreeId, int cells) {
  assertion3(
    cells==std::numeric_limits<int>::max() or maySplit(),
	newSpacetreeId, cells, toString()
  );
  assertion3(
    cells!=std::numeric_limits<int>::max() or mayMove(),
	newSpacetreeId, cells, toString()
  );

  _splitTriggered.insert( std::pair<int,int>(newSpacetreeId,cells) );
}


std::string peano4::grid::Spacetree::toString() const {
  std::ostringstream msg;
  msg << "(" << toString(_spacetreeState)
	  << ",statistics=" << _statistics.toString();
  if (_joinTriggered.empty()) {
	msg << ",no-join-triggered-with-any-tree";
  }
  else {
	msg << ",join-triggered={";
	for (const auto& p: _joinTriggered) {
	  msg << "(" << p << ")";
	}
	msg << "}";
  }
  if (_joining.empty()) {
	msg << ",not-joining";
  }
  else {
    msg << ",joining={";
	for (const auto& p: _joining) {
	  msg << "(" << p << ")";
	}
	msg << "}";
  }
  if (_splitTriggered.empty()) {
	msg << ",no-split-triggered";
  }
  else {
	msg << ",split-triggered={";
	for (const auto& p: _splitTriggered) {
	  msg << "(" << p.first << "," << p.second << ")";
	}
	msg << "}";
  }
  if (_splitting.empty()) {
	msg << ",not-splitting";
  }
  else {
	msg << ",splitting={";
	for (const auto& p: _splitting) {
	  msg << "(" << p.first << "," << p.second << ")";
	}
	msg << "}";
  }
  msg << ")";
  return msg.str();
}


bool peano4::grid::Spacetree::mayMove() const {
  return
        _spacetreeState==SpacetreeState::Running
    and _statistics.getStationarySweeps()>NumberOfStationarySweepsToWaitAtLeastTillJoin
    and _joinTriggered.empty()
    and _joining.empty()
    and _splitTriggered.empty()
    and _splitting.empty();
}


bool peano4::grid::Spacetree::maySplit() const {
  return
        _spacetreeState==SpacetreeState::Running
    and _joinTriggered.empty()
    and _joining.empty()
    and _splitting.empty()
	and ( _splitTriggered.empty() or _splitTriggered.begin()->second < std::numeric_limits<int>::max() );
}


bool peano4::grid::Spacetree::mayJoinWithWorker() const {
  return
	    _spacetreeState==SpacetreeState::Running
    and _splitTriggered.empty()
    and _joining.empty()
    and _splitting.empty();
}


bool peano4::grid::Spacetree::mayJoinWithMaster() const {
  bool mandatoryCriteria =
	     _spacetreeState==SpacetreeState::Running
	 and _masterId>=0
     and _statistics.getStationarySweeps()>NumberOfStationarySweepsToWaitAtLeastTillJoin
	 and _splitTriggered.empty()
	 and _splitting.empty()
	 and _joinTriggered.empty()
	 and _joining.empty();

  bool degeneratedTree = _statistics.getNumberOfLocalRefinedCells()==0;

  return mandatoryCriteria and degeneratedTree;
}


void peano4::grid::Spacetree::joinWithMaster() {
  assertion1( mayJoinWithMaster(), _id);
  _spacetreeState = SpacetreeState::JoinTriggered;
  assertion( _splitTriggered.empty() );
  assertion( _splitting.empty() );
}


void peano4::grid::Spacetree::joinWithWorker(int id) {
  logInfo( "joinWithWorker(int)", "add tree " << id << " to tree " << _id );
  assertion2( _joinTriggered.count(id)==0, id, _id );
  _joinTriggered.insert( id );
  assertion2( _splitTriggered.empty(), id, toString() );
  assertion2( _splitting.empty(), id, toString() );
}

