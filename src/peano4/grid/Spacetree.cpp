#include "Spacetree.h"
#include "PeanoCurve.h"
#include "TraversalObserver.h"
#include "GridTraversalEvent.h"


#include "peano4/utils/Loop.h"
#include "peano4/parallel/Node.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Lock.h"


tarch::logging::Log  peano4::grid::Spacetree::_log( "peano4::grid::Spacetree" );


const int peano4::grid::Spacetree::InvalidRank(-1);
const int peano4::grid::Spacetree::RankOfCellWitchWillBeJoined(-2);
const int peano4::grid::Spacetree::RankOfPeriodicBoundaryCondition(-3);
const int peano4::grid::Spacetree::NumberOfStationarySweepsToWaitAtLeastTillJoin(2);



peano4::grid::Spacetree::GridVertexStackMap    peano4::grid::Spacetree::_vertexStack;


peano4::grid::Spacetree::Spacetree(
  const tarch::la::Vector<Dimensions,double>&  offset,
  const tarch::la::Vector<Dimensions,double>&  width,
  const std::bitset<Dimensions>&               periodicBC
):
  _id(0),
  _spacetreeState( SpacetreeState::NewRoot ),
  _root(),
  _masterId(-1),
  _periodicBC(periodicBC) {
  _root.setLevel( 0 );
  _root.setX( offset );
  _root.setH( width );
  _root.setInverted( false );
  _root.setEvenFlags( 0 );

  for (int i=0; i<DimensionsTimesTwo; i++) {
    _root.setAccessNumber( i,0 );
  }

  clearStatistics();

  logInfo( "Spacetree(...)", "create spacetree with " << offset << "x" << width );
}


peano4::grid::Spacetree::Spacetree(
  int newId,
  int masterId,
  const tarch::la::Vector<Dimensions,double>&  offset,
  const tarch::la::Vector<Dimensions,double>&  width,
  bool  traversalInverted
):
  _id(newId),
  _spacetreeState( SpacetreeState::NewFromSplit ),
  _root(),
  _statistics(),
  _masterId(masterId),
  _periodicBC(0),
  _splitTriggered(),
  _splitting() {
  _root.setLevel( 0 );
  _root.setX( offset );
  _root.setH( width );
  _root.setInverted( traversalInverted );
  _root.setEvenFlags( 0 );

  clearStatistics();

  logInfo( "Spacetree(...)", "created spacetree " << _id << " with master tree " << masterId );
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
      result |= vertex.getAdjacentRanks(i)==RankOfCellWitchWillBeJoined;

      result |= _splitTriggered.count( vertex.getAdjacentRanks(i) )==1;
      result |= (splittingIsConsideredLocal and _splitting.count( vertex.getAdjacentRanks(i) )==1);

      result |= (joiningIsConsideredLocal and _joining.count( vertex.getAdjacentRanks(i) )==1);
    }
    return result;
  }
}


bool peano4::grid::Spacetree::areAllVerticesNonHanging(
  GridVertex            vertices[TwoPowerD]
) const {
  bool result = true;
  dfor2(k)
    result &= (vertices[kScalar].getState()!=GridVertex::State::HangingVertex);
  enddforx
  return result;
}


bool peano4::grid::Spacetree::areAllVerticesRefined(
  GridVertex            vertices[TwoPowerD]
) const {
  bool result = true;
  dfor2(k)
    result &= (vertices[kScalar].getState()==GridVertex::State::Refined);
  enddforx
  return result;
}


bool peano4::grid::Spacetree::areAllVerticesUnrefined(
  GridVertex            vertices[TwoPowerD]
) const {
  bool result = true;
  dfor2(k)
    result &= (vertices[kScalar].getState()==GridVertex::State::Unrefined);
  enddforx
  return result;
}


bool peano4::grid::Spacetree::isSpacetreeNodeLocal(
  GridVertex    vertices[TwoPowerD],
  bool          splittingIsConsideredLocal,
  bool          joiningIsConsideredLocal
) const {
  bool isLocal = true;
  dfor2(k)
    isLocal &= (
      (vertices[kScalar].getState()==GridVertex::State::HangingVertex)
      or
      (
        vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1)==_id
      )
      or
      ( vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1)==RankOfCellWitchWillBeJoined )
      or
      (
        splittingIsConsideredLocal and _splitting.count(vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1))>0
      )
      or
      (
        joiningIsConsideredLocal and _joining.count(vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1))>0
      )
    );
  enddforx

  return isLocal;
}


/*std::pair<bool,bool> peano4::grid::Spacetree::isSpacetreeFaceLocal(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     relativePositionOfCellToFather,
  int                                          faceNumber
) const {
  std::pair<bool,bool> result(false,false);

// @todo
//  assertionMsg(false, "not implemented yet" );

  return result;
}*/


/*bool peano4::grid::Spacetree::isSpacetreeCoarseVertexLocalInVerticalCommunicationContext(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     relativePositionOfVertexToFather,
  std::bitset<TwoPowerD>                       fatherMask
) const {
  bool result = false;
  if (
    tarch::la::contains(relativePositionOfVertexToFather, 1)
    or
    tarch::la::contains(relativePositionOfVertexToFather, 2)
  ) {
	int recurseAlongDimension = 0;
    for (int d=0; d<Dimensions; d++) {
      if ( relativePositionOfVertexToFather(d)==1 or relativePositionOfVertexToFather(d)==2 ) {
    	recurseAlongDimension = d;
      }
    }
    tarch::la::Vector<Dimensions,int> left  = relativePositionOfVertexToFather;
    tarch::la::Vector<Dimensions,int> right = relativePositionOfVertexToFather;
    left(recurseAlongDimension)  = 0;
    right(recurseAlongDimension) = 3;

    std::bitset<TwoPowerD> leftMask  = fatherMask;
    std::bitset<TwoPowerD> rightMask = fatherMask;

    for(int i=0; i<TwoPowerD; i++) {
      std::bitset<TwoPowerD> currentMaskEntry;
      currentMaskEntry = i;
      currentMaskEntry[recurseAlongDimension] = 0;
      leftMask[ currentMaskEntry.to_ulong() ] = false;
      currentMaskEntry = i;
      currentMaskEntry[recurseAlongDimension] = 1;
      leftMask[ currentMaskEntry.to_ulong() ] = false;
    }

    return isSpacetreeCoarseVertexLocalInVerticalCommunicationContext( coarseGridVertices, left, leftMask )
        or isSpacetreeCoarseVertexLocalInVerticalCommunicationContext( coarseGridVertices, right, rightMask );
  }
  else {
	tarch::la::Vector<Dimensions,int> positionWithinCoarserCell = relativePositionOfVertexToFather / 3;
	int                               coarseVertex = peano4::utils::dLinearised(positionWithinCoarserCell,2);
    for (int i=0; i<TwoPowerD; i++) {
      if (
        fatherMask[i]
        and
		coarseGridVertices[coarseVertex].getState()!=GridVertex::State::HangingVertex
      ) {
        result |= tarch::la::contains( coarseGridVertices[coarseVertex].getAdjacentRanks(), _id );
      }
    }
  }
  return result;
}*/


/*std::pair<bool,bool> peano4::grid::Spacetree::isSpacetreeVertexLocalInVerticalCommunicationContext(
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const tarch::la::Vector<Dimensions,int>&     relativePositionOfCellToFather,
  const std::bitset<Dimensions>&               vertexPosition
) const {
  logTraceInWith2Arguments( "isSpacetreeVertexLocalInVerticalCommunicationContext(...)", relativePositionOfCellToFather, vertexPosition );
  std::pair<bool,bool> result;

  assertion( fineGridVertices[vertexPosition.to_ulong()].getState()!=GridVertex::State::HangingVertex );

  result.first  = tarch::la::contains( fineGridVertices[vertexPosition.to_ulong()].getAdjacentRanks(), _id );

  tarch::la::Vector<Dimensions,int> positionWithinPatch = relativePositionOfCellToFather + tarch::la::Vector<Dimensions,int>(vertexPosition);
  std::bitset<TwoPowerD>            mask;

  mask.set();

  result.second = isSpacetreeCoarseVertexLocalInVerticalCommunicationContext(coarseGridVertices,positionWithinPatch,mask);

  logTraceOutWith2Arguments( "isSpacetreeVertexLocalInVerticalCommunicationContext(...)", result.first, result.second );
  return result;
}*/


int peano4::grid::Spacetree::getTreeOwningSpacetreeNode(
  GridVertex            vertices[TwoPowerD]
) const {
  const int NotSetYet = -1;
  int id     = NotSetYet;

  int weakId = NotSetYet;
  dfor2(k)
    if (
      vertices[kScalar].getState()!=GridVertex::State::HangingVertex
      and
      vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1)!=InvalidRank
    ) {
      weakId = vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1);
    }
    if (
      vertices[kScalar].getState()!=GridVertex::State::HangingVertex
      and
      isVertexAdjacentToLocalSpacetree(vertices[kScalar],true,false)
    ) {
      assertion9(
        id==NotSetYet
        or
        vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1)==id,
        id, kScalar, vertices[kScalar].toString(),
        vertices[0].toString(), vertices[1].toString(), vertices[2].toString(), vertices[3].toString(),
        _id, toString()
      );
      id = vertices[kScalar].getAdjacentRanks(TwoPowerD-kScalar-1);
    }
  enddforx
  assertion1(id!=NotSetYet or not isSpacetreeNodeLocal(vertices,false,false),id);

  if (id==NotSetYet) {
    id = weakId;
  }

  return id;
}


void peano4::grid::Spacetree::traverse(TraversalObserver& observer, bool calledFromSpacetreeSet ) {
  logTraceIn( "traverse(TraversalObserver)" );

  if (calledFromSpacetreeSet) {
    assertion( _joinTriggered.empty() or _splitTriggered.empty() );
    assertion( _joinTriggered.empty() or _splitting.empty() );
    assertion( _joining.empty()       or _splitTriggered.empty() );
    assertion( _joining.empty()       or _splitting.empty() );
    logDebug(
      "traverse(TraversalObserver,SpacetreeSet)",
      _splitTriggered.size() << " tree split triggered and " <<
	  _splitting.size() << " splitting trees on tree " << _id
    );
  }

  clearStatistics();

  if ( _spacetreeState==SpacetreeState::NewFromSplit) {
    _gridControlEvents.clear();
  }
  else {
    _gridControlEvents = observer.getGridControlEvents();
  }

  logDebug( "traverse(TraversalObserver&)", "got " << _gridControlEvents.size() << " grid control event(s)" );

  _splittedCells.clear();

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
    // The global master rank
    vertices[kScalar].setAdjacentRanks(TwoPowerD-1-kScalar,0);
    logDebug( "traverse()", "create " << vertices[kScalar].toString() );
  enddforx

  for (int d=0; d<Dimensions; d++) {
    if (_periodicBC[d]) {
      dfor2(vertex)
        dfor2(entries)
          tarch::la::Vector<Dimensions,int> entry = entries;
          if ( vertex(d)==0 ) {
            entry(d) = 0;
          }
          else {
            entry(d) = 1;
          }
          vertices[vertexScalar].setAdjacentRanks(peano4::utils::dLinearised(entry,2),RankOfPeriodicBoundaryCondition);
        enddforx
        logDebug( "traverse()", "set periodic boundary conditions: " << vertices[vertexScalar].toString() );
      enddforx
    }
  }

  observer.beginTraversal( _root.getX(), _root.getH() );

  descend(_root,vertices,observer);

  observer.endTraversal( _root.getX(), _root.getH() );

  _root.setInverted( not _root.getInverted() );

  if (calledFromSpacetreeSet) {
    for (auto& rank: _splitting) {
      _childrenIds.insert(rank.first);
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
        logDebug( "traverse(...)", "switched tree " << _id << " into running" );
        break;
      case SpacetreeState::JoinTriggered:
        _spacetreeState = SpacetreeState::Joining;
        logDebug( "traverse(...)", "switched tree " << _id << " into joining" );
        assertion( _vertexStack.empty( _id,peano4::parallel::Node::getOutputStackNumberForForkJoinDataExchange( _masterId ) ));
        break;
      case SpacetreeState::Joining:
        _spacetreeState = SpacetreeState::Running;
        logDebug( "traverse(...)", "switched tree " << _id << " back into running" );
        break;
	    case SpacetreeState::Running:
	      break;
	  }
  }

  logTraceOut( "traverse(TraversalObserver)" );
}


bool peano4::grid::Spacetree::isVertexRefined(GridVertex  vertex) {
  return vertex.getState() == GridVertex::State::Refining
      or vertex.getState() == GridVertex::State::Refined
	  or vertex.getState() == GridVertex::State::EraseTriggered
	  or vertex.getState() == GridVertex::State::Erasing;
}


std::bitset<TwoPowerD> peano4::grid::Spacetree::areVerticesRefined(GridVertex  vertices[TwoPowerD]) {
  std::bitset<TwoPowerD> bitset;
  for (int i=0; i<TwoPowerD; i++) {
	bitset.set(i,isVertexRefined(vertices[i]));
  }
  return bitset;
}


bool peano4::grid::Spacetree::isSpacetreeNodeRefined(GridVertex  vertices[TwoPowerD]) {
  bool result = false;
  dfor2(k)
    result |= isVertexRefined( vertices[kScalar] );
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
    logDebug( "getCellType(...)", "delete cell" );
    return CellType::Delete;
  }
  else if (allVerticesAreNew) {
    logDebug( "getCellType(...)", "create new cell" );
    return CellType::New;
  }
  else {
    logDebug( "getCellType(...)", "keep cell" );
    for (int i=0; i<TwoPowerD; i++) {
      logDebug( "getCellType(...)", "vertex #" << i << ": " << vertices[i].toString() );
	  }
	  return CellType::Persistent;
  }
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
  const tarch::la::Vector<Dimensions,int>&  position,
  TraversalObserver&                        observer
) {
  logTraceInWith1Argument( "updateVertexAfterLoad(GridVertex&)", vertex.toString() );

  assertion( _spacetreeState!=SpacetreeState::NewFromSplit );

  receiveAndMergeVertexIfAdjacentToDomainBoundary( vertex, observer );

  vertex.setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep( vertex.getIsAntecessorOfRefinedVertexInCurrentTreeSweep() );
  vertex.setIsAntecessorOfRefinedVertexInCurrentTreeSweep( false );
  vertex.setNumberOfAdjacentRefinedLocalCells(0);

  if ( vertex.getState()==GridVertex::State::RefinementTriggered ) {
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
	    _statistics.setCoarseningHasBeenVetoed(true);
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


void peano4::grid::Spacetree::sendOutVertexToSplittingTrees(
  GridVertex&                               vertex,
  TraversalObserver&                        observer
) {
  for (auto p: _splitting) {
    logDebug(
      "sendOutVertexToSplittingTrees(...)",
      "stream vertex " << vertex.toString() <<
      " from tree " << _id << " to tree " << p.first << " because of split (stack no " <<
      peano4::parallel::Node::getInstance().getOutputStackNumberForForkJoinDataExchange(p.first) << ")"
    );
    _vertexStack.getForPush(_id, peano4::parallel::Node::getInstance().getOutputStackNumberForForkJoinDataExchange(p.first))->push(vertex);
  }
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
  const tarch::la::Vector<Dimensions,int>&  fineVertexPositionWithinPatch,
  TraversalObserver&                        observer
) {
  logTraceInWith2Arguments( "updateVertexBeforeStore()", vertex.toString(), _id );

  if ( vertex.getState()==GridVertex::State::New ) {
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

  sendOutVertexIfAdjacentToDomainBoundary( vertex, observer );

  if (restrictIsAntecessorOfRefinedVertex) {
	  dfor2(k)
      if (restrictToCoarseGrid(k,fineVertexPositionWithinPatch)) {
        logDebug( "updateVertexBeforeStore(...)", "set antecessor flag (veto coarsenign) on vertex " << coarseGridVertices[kScalar].toString() << " due to vertex " << vertex.toString() );
        coarseGridVertices[kScalar].setIsAntecessorOfRefinedVertexInCurrentTreeSweep(true);
      }
	  enddforx
  }

  if (shouldEraseAdjacencyInformation(vertex, coarseGridVertices, fineVertexPositionWithinPatch)) {
    vertex.setAdjacentRanks(InvalidRank);
  }

  logTraceOutWith2Arguments( "updateVertexBeforeStore()", vertex.toString(), _id );
}



bool peano4::grid::Spacetree::shouldEraseAdjacencyInformation(
  const GridVertex&                  vertex,
  GridVertex                         coarseGridVertices[TwoPowerD],
  tarch::la::Vector<Dimensions,int>  fineVertexPositionWithinPatch
) const {
  if (
    isVertexAdjacentToLocalSpacetree(vertex,true,true)
    or
    vertex.getState() != GridVertex::Unrefined
  ) {
    return false;
  }

  logTraceInWith2Arguments( "shouldEraseAdjacencyInformation(...)", vertex.toString(), fineVertexPositionWithinPatch );
  for (int d=0; d<Dimensions; d++) {
    if (
      fineVertexPositionWithinPatch(d)!=0
      and
      fineVertexPositionWithinPatch(d)!=3
    ) {
      tarch::la::Vector<Dimensions,int>  positionA = fineVertexPositionWithinPatch;
      tarch::la::Vector<Dimensions,int>  positionB = fineVertexPositionWithinPatch;

      positionA(d) = 0;
      positionB(d) = 3;

      bool result =
             shouldEraseAdjacencyInformation(vertex,coarseGridVertices,positionA)
         and shouldEraseAdjacencyInformation(vertex,coarseGridVertices,positionB);

      logTraceOutWith1Argument( "shouldEraseAdjacencyInformation(...)", result );
      return result;
    }
  }

  fineVertexPositionWithinPatch = fineVertexPositionWithinPatch / 3;

  bool result =  not tarch::la::contains( coarseGridVertices[ peano4::utils::dLinearised(fineVertexPositionWithinPatch,2) ].getAdjacentRanks(), _id);
  logTraceOutWith1Argument( "shouldEraseAdjacencyInformation(...)", result );
  return result;
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

    VertexType type  = getVertexType(coarseGridVertices,vertexPositionWithinPatch);
    int  stackNumber = PeanoCurve::getVertexReadStackNumber(fineGridStatesState,vertexIndex);

    // reset to persistent, as new vertex already has been generated
    if ( not PeanoCurve::isInOutStack(stackNumber) and type==VertexType::New ) {
      type = VertexType::Persistent;
      logDebug(
        "loadVertices(...)",
        "reset stack flag for local vertex " << vertexPositionWithinPatch << " from new/hanging to persistent"
      );
    }

    switch (type) {
        case VertexType::New:
          assertion( PeanoCurve::isInOutStack(stackNumber) );
          if ( _spacetreeState==SpacetreeState::NewFromSplit ) {
            const int stackNumber = peano4::parallel::Node::getInputStackNumberForForkJoinDataExchange( _masterId );
            fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();
          }
          else {
            fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = createNewPersistentVertex(coarseGridVertices,x,fineGridStatesState.getLevel(),vertexPositionWithinPatch);
            sendOutVertexToSplittingTrees(fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],observer);
          }
    	  break;
        case VertexType::Hanging:
      	  fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = createHangingVertex(coarseGridVertices,x,fineGridStatesState.getLevel(),vertexPositionWithinPatch);
      	  break;
        case VertexType::Persistent:
          {
            logDebug( "readVertices(...)", "read vertex from stack " << stackNumber );

            if (
              _spacetreeState==SpacetreeState::NewFromSplit
              and
              PeanoCurve::isInOutStack(stackNumber)
            ) {
              const int stackNumber = peano4::parallel::Node::getInputStackNumberForForkJoinDataExchange( _masterId );
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();
            }
            else if (
              _spacetreeState!=SpacetreeState::NewFromSplit
              and
              PeanoCurve::isInOutStack(stackNumber)
            ) {
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();

              updateVertexAfterLoad(
                fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],
                coarseGridVertices,
                vertexPositionWithinPatch,
                observer
              );
              sendOutVertexToSplittingTrees(fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],observer);
            }
            else {
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();
            }
          }
          break;
        case VertexType::Delete:
          {
            logDebug( "readVertices(...)", "read vertex from stack " << stackNumber );

            if (
              _spacetreeState==SpacetreeState::NewFromSplit
              and
              PeanoCurve::isInOutStack(stackNumber)
            ) {
              const int stackNumber = peano4::parallel::Node::getInputStackNumberForForkJoinDataExchange( _masterId );
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();
            }
            else if (
              _spacetreeState!=SpacetreeState::NewFromSplit
              and
              PeanoCurve::isInOutStack(stackNumber)
            ) {
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();

              updateVertexAfterLoad(
                fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],
                coarseGridVertices,
                vertexPositionWithinPatch,
                observer
              );
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].setState(GridVertex::Delete);
              sendOutVertexToSplittingTrees(fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],observer);
            }
            else {
              fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] = _vertexStack.getForPop(_id,stackNumber)->pop();
            }
          }
          break;
    }
    logDebug(
      "loadVertices(...)",
  	  "handled " << toString(type) << " vertex " << vertexIndex << " at " << vertexPositionWithinPatch << ": " <<
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString()
    );

    assertionNumericalEquals5(
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getX(), x,
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString(),
      fineGridStatesState.toString(), vertexIndex, toString(), _id
    );
    assertionEquals5(
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getLevel(), fineGridStatesState.getLevel(),
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].toString(),
      fineGridStatesState.toString(), vertexIndex, toString(), _id
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
			  vertexPositionWithinPatch,
			  observer
            );
          }
          _vertexStack.getForPush(_id,stackNumber)->push( fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ] );
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
      isVertexAdjacentToLocalSpacetree(vertex,false,false);

  std::set<int> neighbourIds;
  for (int i=0; i<TwoPowerD; i++) {
    const bool mandatoryCriteria =
		      vertex.getAdjacentRanks(i)!=_id
		      and
		      vertex.getAdjacentRanks(i)!=InvalidRank
          and
          vertex.getAdjacentRanks(i)!=RankOfPeriodicBoundaryCondition
          and
          vertex.getAdjacentRanks(i)!=RankOfCellWitchWillBeJoined;

    // I should not try to receive anything from a node that we just are
    // splitting into.
/*
    const bool receiverCriteria = (_spacetreeState!=SpacetreeState::NewFromSplit or _masterId!=vertex.getAdjacentRanks(i))
    		                  and (_splitting.count(vertex.getAdjacentRanks(i))==0);
*/
    const bool receiverCriteria = _splitting.count(vertex.getAdjacentRanks(i))==0;
    // I should not send out anything to a node that I will split into in
    // the next sweep
    const bool senderCriteria   = //(_joining.count( vertex.getAdjacentRanks(i)) ==0 )
    		                  //and
    		                  (_splitTriggered.count(vertex.getAdjacentRanks(i))==0);

    const bool insertCriteria = calledByReceivingProcess ?
      (mandatoryCriteria and isLocalVertex and receiverCriteria) :
      (mandatoryCriteria and isLocalVertex and senderCriteria);

    if (insertCriteria) {
      neighbourIds.insert( vertex.getAdjacentRanks(i) );
    }
  }

  return neighbourIds;
}


void peano4::grid::Spacetree::mergeAtDomainBoundary( GridVertex& vertex, const GridVertex& inVertex, TraversalObserver& observer, int neighbour ) {
  assertion4( vertex.getState()!=GridVertex::State::HangingVertex,   inVertex.toString(), vertex.toString(), _id, neighbour );
  assertion4( inVertex.getState()!=GridVertex::State::HangingVertex, inVertex.toString(), vertex.toString(), _id, neighbour );
  assertion6(
    tarch::la::equals(vertex.getX(), inVertex.getX())
    or
    (
      tarch::la::contains(vertex.getAdjacentRanks(),RankOfPeriodicBoundaryCondition)
      and
      (_periodicBC.count()>1 or tarch::la::countEqualEntries(vertex.getX(), inVertex.getX())==Dimensions-_periodicBC.count())
    ),
    inVertex.toString(), vertex.toString(), _id, neighbour,
    tarch::la::countEqualEntries(vertex.getX(), inVertex.getX()), _periodicBC
  );
  assertionEquals4( vertex.getLevel(), inVertex.getLevel(),          inVertex.toString(), vertex.toString(), _id, neighbour );
  assertion4(
    not (vertex.getState()==GridVertex::State::Refined and inVertex.getState()==GridVertex::State::Erasing),
    inVertex.toString(), vertex.toString(), _id, neighbour
  );
  assertion4(
    not (vertex.getState()==GridVertex::State::Refined and inVertex.getState()==GridVertex::State::Unrefined),
    inVertex.toString(), vertex.toString(), _id, neighbour
  );

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


void peano4::grid::Spacetree::receiveAndMergeVertexIfAdjacentToDomainBoundary( GridVertex& vertex, TraversalObserver& observer ) {
  assertion( _spacetreeState!=SpacetreeState::NewFromSplit );

  std::set<int> neighbourIds = getAdjacentDomainIds(vertex,true);
  for (auto neighbour: neighbourIds) {
    assertion1( neighbour>=0, neighbour );
    const int  inStack = peano4::parallel::Node::getInstance().getInputStackNumberOfHorizontalDataExchange(neighbour);

    GridVertex inVertex = _vertexStack.getForPop(_id,inStack)->pop();

    mergeAtDomainBoundary(vertex,inVertex,observer,neighbour);

    //
    // Merge adjacency lists. The neighbour owns some entries
    //
    for (int i=0; i<TwoPowerD; i++) {
      if (vertex.getAdjacentRanks(i)==neighbour and neighbour!=InvalidRank) {
        if (inVertex.getAdjacentRanks(i)!=vertex.getAdjacentRanks(i)) {
        	logDebug(
            "receiveAndMergeVertexIfAdjacentToDomainBoundary(...)",
	          "update " << i << "th entry of adjacency data of vertex " << vertex.toString() <<
		        " with neighbouring vertex " << inVertex.toString() << " on tree " << _id
			    );
        }
        vertex.setAdjacentRanks(i, inVertex.getAdjacentRanks(i) );
      }
    }
  }

  std::set<peano4::parallel::Node::PeriodicBoundaryStackIdentifier> periodicBCStacks =
    _id==0 ? peano4::parallel::Node::getInstance().getInputStacksForPeriodicBoundaryExchange(vertex.getAdjacentRanks())
           : std::set<peano4::parallel::Node::PeriodicBoundaryStackIdentifier>();

  for (auto stackNo: periodicBCStacks) {
    assertion4( _id==0, _id, stackNo.first, stackNo.second, vertex.toString() );
    GridVertex inVertex = _vertexStack.getForPop(_id,stackNo.first)->pop();
    logDebug( "receiveAndMergeVertexIfAdjacentToDomainBoundary(...)", "read periodic BC data from stack " << stackNo.first << ": " << inVertex.toString() );
    logDebug( "receiveAndMergeVertexIfAdjacentToDomainBoundary(...)", "normals of involved symmetry axes: " << stackNo.second );
    mergeAtDomainBoundary(vertex,inVertex,observer,_id);

    dfor2(k)
      assertion3(
        vertex.getAdjacentRanks(kScalar)==RankOfPeriodicBoundaryCondition
        or
        vertex.getAdjacentRanks(kScalar)==_id
        or
        vertex.getAdjacentRanks(kScalar)==InvalidRank
        ,
        vertex.toString(), inVertex.toString(), _id
      );
    enddforx
  }
}


void peano4::grid::Spacetree::sendOutVertexIfAdjacentToDomainBoundary( const GridVertex& vertex, TraversalObserver& observer ) {
  logTraceInWith2Arguments( "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)", vertex.toString(), _id );

  std::set<int> outRanks = getAdjacentDomainIds(vertex,false);
  for (auto p: outRanks) {
    //
    // Boundary exchange
    //
    const int stackNo = peano4::parallel::Node::getInstance().getOutputStackNumberOfHorizontalDataExchange(p);

    GridVertex vertexCopy = vertex;
    for (int i=0; i<TwoPowerD; i++) {
      int targetRank = vertexCopy.getAdjacentRanks(i);
      if (
        vertexCopy.getAdjacentRanks(i)==RankOfCellWitchWillBeJoined
        and
        p != _masterId
      ) {
        targetRank = _masterId;
      }
      else if (
        vertexCopy.getAdjacentRanks(i)==RankOfCellWitchWillBeJoined
      ) {
        targetRank = _id;
      }
      vertexCopy.setAdjacentRanks( i, targetRank );
    }
    _vertexStack.getForPush(_id,stackNo)->push( vertexCopy );

    logDebug(
          "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)",
          "vertex " << vertexCopy.toString() << " on tree " << _id <<
          " goes to tree " << p << " through stack " << stackNo
    );
  }

  std::set<peano4::parallel::Node::PeriodicBoundaryStackIdentifier> periodicBCStacks =
    _id==0 ? peano4::parallel::Node::getInstance().getOutputStacksForPeriodicBoundaryExchange(vertex.getAdjacentRanks())
           : std::set<peano4::parallel::Node::PeriodicBoundaryStackIdentifier>();

  for (auto stackNo: periodicBCStacks) {
    _vertexStack.getForPush(_id,stackNo.first)->push( vertex );
    logDebug(
      "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)",
      "vertex " << vertex.toString() << " on tree " << _id <<
      " goes to stack " << stackNo.first << " to realise periodic BC"
    );
  }

  logTraceOutWith1Argument( "sendOutVertexIfAdjacentToDomainBoundary(GridVertex)", isVertexAdjacentToLocalSpacetree(vertex,true,true) );
}


void peano4::grid::Spacetree::clearStatistics() {
  if (_id==0) {
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

  _statistics.setCoarseningHasBeenVetoed(false);

  if (_spacetreeState==SpacetreeState::Running) {
    _statistics.setStationarySweeps( _statistics.getStationarySweeps()+1 );
    /*if (_statistics.getStationarySweeps()>NumberOfStationarySweepsToWaitAtLeastTillJoin+1) {
      _statistics.setStationarySweeps( NumberOfStationarySweepsToWaitAtLeastTillJoin+1 );
    }*/
  }
  else {
    _statistics.setStationarySweeps( 0 );
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


    mergeCellFromWorkerWithMaster(
      vertices,
      fineGridVertices
    );

    //
    // Mesh refinement
    // Enter cell
    //
    if ( isSpacetreeNodeLocal(fineGridVertices,false,false) ) {
      if ( not isSpacetreeNodeLocal(vertices,false,false) ) {
        markVerticesAroundParentOfForkedCell(vertices);
      }

      evaluateGridControlEvents(fineGridStates[peano4::utils::dLinearised(k,3)], vertices, fineGridVertices);
    }
    else if (
      not isSpacetreeNodeLocal(fineGridVertices,false,false) and isSpacetreeNodeLocal(vertices,false,false)
    ) {
      markVerticesAroundParentOfForkedCell(vertices);
    }

    observer.enterCell(createEnterCellTraversalEvent(
      vertices, fineGridVertices, fineGridStates[peano4::utils::dLinearised(k,3)], k
    ));

    //
    // DFS
    //
    if (isSpacetreeNodeRefined(fineGridVertices)) {
      if ( isSpacetreeNodeLocal(fineGridVertices,false,false) ) {
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
      if ( isSpacetreeNodeLocal(fineGridVertices,true,true) ) {
        _statistics.setNumberOfLocalUnrefinedCells( _statistics.getNumberOfLocalUnrefinedCells()+1 );
      }
      else {
        _statistics.setNumberOfRemoteUnrefinedCells( _statistics.getNumberOfRemoteUnrefinedCells()+1 );
      }
    }

    //
    // Leave cell
    //
    observer.leaveCell(createLeaveCellTraversalEvent(
      vertices, fineGridVertices,fineGridStates[peano4::utils::dLinearised(k,3)],k
    ));

    splitOrJoinCell(
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
  GridVertex                                   coarseGridVertices[TwoPowerD],
  GridVertex                                   fineGridVertices[TwoPowerD],
  const AutomatonState&                        state,
  const tarch::la::Vector<Dimensions,int>&     relativePositionToFather
) const {
  logTraceInWith3Arguments( "createEnterCellTraversalEvent(...)", state.toString(), _id, relativePositionToFather );
  GridTraversalEvent  event;

  event.setX( state.getX() + state.getH()*0.5 );
  event.setH( state.getH() );
  event.setIsRefined( areVerticesRefined(fineGridVertices) );
  event.setRelativePositionToFather( relativePositionToFather );

  if ( _spacetreeState==SpacetreeState::NewFromSplit ) {
    event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::StreamIn);
    logDebug( "createEnterCellTraversalEvent(...)", "if data is moved, it will be streamed in" );
  }
  else if ( _spacetreeState==SpacetreeState::Joining ) {
    event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::StreamOut);
    logDebug( "createEnterCellTraversalEvent(...)", "if data is moved, it will be streamed out" );
  }
  else if ( not _splitting.empty() ) {
    event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::StreamOut);
    logDebug( "createEnterCellTraversalEvent(...)", "if data is moved, it will be streamed out" );
  }
  else if ( not _joining.empty() ) {
    event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::StreamIn);
    logDebug( "createEnterCellTraversalEvent(...)", "if data is moved, it will be streamed in" );
  }
  else {
    event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::NoRebalancing);
  }
  event.setStreamVertexDataRank( TraversalObserver::NoRebalancing );
  event.setStreamFaceDataRank( TraversalObserver::NoRebalancing );
  event.setStreamCellDataRank( TraversalObserver::NoRebalancing );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(state);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>  vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const int  stackNumber    = PeanoCurve::getVertexReadStackNumber(state,vertexIndex);
    const int  vertexPosition = vertexIndex.to_ulong();

    switch ( fineGridVertices[vertexPosition].getState() ) {
      case GridVertex::HangingVertex:
   	    event.setVertexDataFrom(i,TraversalObserver::CreateOrDestroyHangingGridEntity);
    	break;
      case GridVertex::New:
        {
          if ( isVertexAdjacentToLocalSpacetree(fineGridVertices[vertexPosition],true,false)) {
            if ( PeanoCurve::isInOutStack(stackNumber) ) {
              event.setVertexDataFrom(i,TraversalObserver::CreateOrDestroyPersistentGridEntity);
            }
            else {
              event.setVertexDataFrom(i,stackNumber);
            }
          }
          else {
            event.setVertexDataFrom(i,TraversalObserver::NoData);
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
        if ( isVertexAdjacentToLocalSpacetree(fineGridVertices[vertexPosition],true,false)) {
          event.setVertexDataFrom(i,stackNumber);
        }
        else {
          event.setVertexDataFrom(i,TraversalObserver::NoData);
        }
    	break;
    }
    event.setVertexDataTo(i,vertexIndex.to_ulong());

    if (
      _spacetreeState==SpacetreeState::NewFromSplit
      and
      fineGridVertices[vertexPosition].getState()!=GridVertex::HangingVertex
      and
      PeanoCurve::isInOutStack(stackNumber)
    ) {
      for (int j=0; j<TwoPowerD; j++) {
        if (
           tarch::la::contains( fineGridVertices[vertexPosition].getAdjacentRanks(), _id )
        ) {
          logDebug( "createEnterCellTraversalEvent(...)", vertexPosition << "th vertex is to be streamed in from master" );
          event.setStreamVertexDataRank( j + vertexPosition*TwoPowerD, _masterId );
        }
      }
    }
    else if ( _spacetreeState==SpacetreeState::Joining and fineGridVertices[vertexPosition].getState()!=GridVertex::HangingVertex ) {
      //event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::StreamOut);
      assertionMsg(false,"not defined");
    }
    else if (
      not _splitting.empty()
      and
      fineGridVertices[vertexPosition].getState()!=GridVertex::HangingVertex
      and
      PeanoCurve::isInOutStack(stackNumber)
    ) {
      for (int j=0; j<TwoPowerD; j++) {
        int currentRank = fineGridVertices[vertexPosition].getAdjacentRanks(j);
        if ( _splitting.count(currentRank)>0 ) {
          logDebug( "createEnterCellTraversalEvent(...)", vertexPosition << "th vertex is streamed out to tree " << currentRank );
          event.setStreamVertexDataRank( j + vertexPosition*TwoPowerD, currentRank );
        }
      }
    }
    else if ( not _joining.empty() and fineGridVertices[vertexPosition].getState()!=GridVertex::HangingVertex ) {
      //event.setLoadBalancingDataExchange(GridTraversalEvent::LoadBalancingDataExchange::StreamIn);
      assertionMsg(false,"not defined");
    }
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

    event.setStreamFaceDataRank( i, TraversalObserver::NoRebalancing );
    if ( type!=FaceType::Hanging ) {
/*
      std::pair<bool,bool> local = isSpacetreeFaceLocal(coarseGridVertices, fineGridVertices, relativePositionToFather, i);
      bool parentFaceIsLocal   = local.second;
      bool faceIsLocal         = local.first;

      if ( faceIsLocal and _spacetreeState==SpacetreeState::NewFromSplit ) {
        assertionMsg( false, "not yet implemented" );
      }
      else if ( faceIsLocal and _spacetreeState==SpacetreeState::Joining ) {
        assertionMsg( false, "not yet implemented" );
      }
      else if ( not faceIsLocal and _splitting.count( getTreeOwningSpacetreeNode(fineGridVertices) )>0 ) {
        event.setStreamFaceDataRank( i, getTreeOwningSpacetreeNode(fineGridVertices) );
      }
      else if ( not faceIsLocal and _joining.count( getTreeOwningSpacetreeNode(fineGridVertices) )>0 ) {
        assertionMsg( false, "not yet implemented" );
      }
*/
    }
  }

  {
    CellType type = getCellType(fineGridVertices);
    const int  stackNumber = PeanoCurve::getCellReadStackNumber(state);
    switch (type) {
      case CellType::New:
        if (isSpacetreeNodeLocal(fineGridVertices,true,false)) {
          event.setCellData(TraversalObserver::CreateOrDestroyPersistentGridEntity);
        }
        else {
          event.setCellData(TraversalObserver::NoData);
        }
        break;
	  case CellType::Persistent:
        if (isSpacetreeNodeLocal(fineGridVertices,true,false)) {
          event.setCellData(stackNumber);
        }
        else {
          event.setCellData(TraversalObserver::NoData);
        }
        break;
      case CellType::Delete:
        if (isSpacetreeNodeLocal(fineGridVertices,true,false)) {
          event.setCellData(stackNumber);
        }
        else {
          event.setCellData(TraversalObserver::NoData);
        }
        break;
    }

    bool parentCellIsLocal   = isSpacetreeNodeLocal(coarseGridVertices,false,false);
    bool cellIsLocal         = isSpacetreeNodeLocal(fineGridVertices,false,false);

    if ( cellIsLocal and _spacetreeState==SpacetreeState::NewFromSplit ) {
      if ( getTreeOwningSpacetreeNode(fineGridVertices)==_id) {
        logDebug( "createEnterCellTraversalEvent(...)", "cell is to be streamed in from master" );
        event.setStreamCellDataRank( _masterId );
      }
    }
    else if ( cellIsLocal and _spacetreeState==SpacetreeState::Joining ) {
      assertionMsg( false, "not yet implemented" );
    }
    else if ( not cellIsLocal and _splitting.count( getTreeOwningSpacetreeNode(fineGridVertices) )>0 ) {
      logInfo( "createEnterCellTraversalEvent(...)", "cell will become remote: " << event.toString() );
      event.setStreamCellDataRank( getTreeOwningSpacetreeNode(fineGridVertices) );
    }
    else if ( not cellIsLocal and _joining.count( getTreeOwningSpacetreeNode(fineGridVertices) )>0 ) {
      assertionMsg( false, "not yet implemented" );
    }
  }

  for (int i=0;   i<TwoPowerD; i++)
  for (int j=0;   j<TwoPowerD; j++)
  for (int k=j+1; k<TwoPowerD; k++) {
    if (event.getStreamVertexDataRank( j + i*TwoPowerD ) == event.getStreamVertexDataRank( k + i*TwoPowerD ) ) {
      event.setStreamVertexDataRank( k + i*TwoPowerD, peano4::grid::TraversalObserver::NoRebalancing );
    }
  }

  logTraceOutWith3Arguments( "createEnterCellTraversalEvent(...)", state.toString(), event.toString(), _id );
  return event;
}


peano4::grid::GridTraversalEvent peano4::grid::Spacetree::createLeaveCellTraversalEvent(
  GridVertex              coarseGridVertices[TwoPowerD],
  GridVertex              fineGridVertices[TwoPowerD],
  const AutomatonState&   state,
  const tarch::la::Vector<Dimensions,int>&  relativePositionToFather
) const {
  logTraceInWith3Arguments( "createLeaveCellTraversalEvent(...)", state.toString(), _id, relativePositionToFather );
  GridTraversalEvent  event;

  event.setX( state.getX() + state.getH()*0.5 );
  event.setH( state.getH() );
  event.setIsRefined( areVerticesRefined(fineGridVertices) );
  event.setRelativePositionToFather( relativePositionToFather );

  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(state);
  for (int i=0; i<TwoPowerD; i++) {
    const std::bitset<Dimensions>           vertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    const int   stackNumber = PeanoCurve::getVertexWriteStackNumber(state,vertexIndex);

    event.setVertexDataFrom(i,vertexIndex.to_ulong());
    switch ( fineGridVertices[vertexIndex.to_ulong()].getState() ) {
      case GridVertex::HangingVertex:
        event.setVertexDataTo(i,TraversalObserver::CreateOrDestroyHangingGridEntity);
        break;
      case GridVertex::New:
      case GridVertex::Unrefined:
      case GridVertex::Refined:
      case GridVertex::RefinementTriggered:
      case GridVertex::Refining:
      case GridVertex::EraseTriggered:
      case GridVertex::Erasing:
        event.setVertexDataTo(i,stackNumber);
        break;
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
    CellType type = getCellType(fineGridVertices);
    const int  stackNumber = PeanoCurve::getCellWriteStackNumber(state);

    switch (type) {
      case CellType::New:
        event.setCellData(stackNumber);
        break;
	  case CellType::Persistent:
        event.setCellData(stackNumber);
        break;
      case CellType::Delete:
        event.setCellData(TraversalObserver::CreateOrDestroyPersistentGridEntity);
        break;
    }
  }

  logTraceOutWith3Arguments( "createLeaveCellTraversalEvent(...)", state.toString(), event.toString(), _id );
  return event;
}


bool peano4::grid::Spacetree::cellIsMergeCandidate(
  GridVertex  coarseGridVertices[TwoPowerD],
  GridVertex  fineGridVertices[TwoPowerD]
) const {
  return isSpacetreeNodeLocal(fineGridVertices,false,false)
    and
    not isSpacetreeNodeLocal(coarseGridVertices,false,false)
    and
    areAllVerticesUnrefined(fineGridVertices);
}


bool peano4::grid::Spacetree::isCellSplitCandidate(
  GridVertex                         coarseGridVertices[TwoPowerD],
  GridVertex                         fineGridVertices[TwoPowerD]
) const {
  bool isOneVertexAdjacentToPeriodicBC = false;
  dfor2(k)
    isOneVertexAdjacentToPeriodicBC |= tarch::la::contains(fineGridVertices[kScalar].getAdjacentRanks(),RankOfPeriodicBoundaryCondition);
  enddforx

  return isSpacetreeNodeLocal(fineGridVertices,false,false) and
    isSpacetreeNodeLocal(coarseGridVertices,false,false) and
    areAllVerticesNonHanging(fineGridVertices) and
    not isOneVertexAdjacentToPeriodicBC;
}


void peano4::grid::Spacetree::splitOrJoinCell(
  GridVertex  coarseGridVertices[TwoPowerD],
  GridVertex  fineGridVertices[TwoPowerD]
) {
  static int newlyCreatedCells = 0;

  if (not _splitTriggered.empty()) {
    const bool isSplitCandidate = isCellSplitCandidate(coarseGridVertices,fineGridVertices);

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
        reducedMarker     = -1;
        newlyCreatedCells =  0;
      }

      _splittedCells.push_back(reducedMarker);
    }
    else { // not refined
      int targetSpacetreeId = getSplittingTree();

      if (isSplitCandidate and targetSpacetreeId>=0) {
        updateVertexRanksWithinCell( fineGridVertices, targetSpacetreeId );

        newlyCreatedCells++;

        if ( getCellType( fineGridVertices )!=CellType::New or newlyCreatedCells%ThreePowerD==ThreePowerD-1) {
          updateSplittingCounter( targetSpacetreeId );
          newlyCreatedCells = 0;
        }

        _splittedCells.push_back(targetSpacetreeId);
      }
	    else {
	      _splittedCells.push_back(-1);
      }
    }
  }

  if (
    _spacetreeState==SpacetreeState::JoinTriggered
    and
    cellIsMergeCandidate( coarseGridVertices, fineGridVertices )
    and
    _maxJoiningCells>0
  ) {
    logDebug( "splitOrJoinCell(...)", "decided to merge cell from tree " << _id << " into master " << _masterId );

    for (int i=0; i<TwoPowerD; i++) {
      assertion2(fineGridVertices[i].getState()!=GridVertex::State::HangingVertex,fineGridVertices[i].toString(),_id);
      logDebug( "splitOrJoinCell(...)", "- " << fineGridVertices[i].toString() );
    }

    updateVertexRanksWithinCell( fineGridVertices, RankOfCellWitchWillBeJoined );

  	_maxJoiningCells--;
  }

  if (
    _spacetreeState==SpacetreeState::Joining
    and
    isSpacetreeNodeLocal(fineGridVertices,false,false)
    and
    not isSpacetreeNodeLocal(coarseGridVertices,false,false)
  ) {
    logDebug( "splitOrJoinCell(...)", "merge cell at " << fineGridVertices[0].toString() << " from tree " << _id << " into master " << _masterId );
    for (int i=0; i<TwoPowerD; i++) {
      const int stack = peano4::parallel::Node::getOutputStackNumberForForkJoinDataExchange( _masterId );
      logDebug( "splitOrJoinCell(...)", "stream vertex " << fineGridVertices[i].toString() << " on tree " << _id << " to master " << _masterId << " through stack " << stack << " as " << i << "th vertex of cell");
      _vertexStack.getForPush(_id,stack)->push( fineGridVertices[i] );

      // reset the 'local' adjacency entries
      fineGridVertices[i].setAdjacentRanks(TwoPowerD-1-i,
        fineGridVertices[i].getAdjacentRanks(TwoPowerD-1-i)==RankOfCellWitchWillBeJoined ?
        _masterId : fineGridVertices[i].getAdjacentRanks(TwoPowerD-1-i)
      );
      logDebug( "splitOrJoinCell(...)", "updated into " << fineGridVertices[i].toString() );
    }
  }
}


void peano4::grid::Spacetree::mergeCellFromWorkerWithMaster(
  GridVertex  coarseGridVertices[TwoPowerD],
  GridVertex  fineGridVertices[TwoPowerD]
) {
  for (auto worker: _joining) {
    if (
      isSpacetreeNodeLocal(coarseGridVertices,false,false)
      and
      getTreeOwningSpacetreeNode(fineGridVertices)==worker
    ) {
      logDebug( "mergeCellFromWorkerWithMaster(...)", "cell from worker " << worker << " is to be merged into master " << _id );
      bool transferredCell = false;
      for (int i=0; i<TwoPowerD; i++) {
        const int  stack = peano4::parallel::Node::getInputStackNumberForForkJoinDataExchange( worker );

        GridVertex receivedVertex = _vertexStack.getForPop(_id,stack)->pop();

        logDebug(
          "mergeCellFromWorkerWithMaster(...)",
          "received vertex " << receivedVertex.toString() << " for local vertex " <<
          fineGridVertices[i].toString() << " from worker " << worker << " on tree " << _id << " through stack " << stack
        );

        assertionVectorNumericalEquals6(fineGridVertices[i].getX(),receivedVertex.getX(),fineGridVertices[i].toString(),receivedVertex.toString(),_id,_vertexStack.toString(),worker,i );
        assertion7( coarseGridVertices[i].getState()!=GridVertex::State::Erasing, worker, _id, i, fineGridVertices[i].toString(), coarseGridVertices[i].toString(),worker,i );
        assertion7( coarseGridVertices[i].getState()!=GridVertex::State::HangingVertex, worker, _id, i, fineGridVertices[i].toString(), coarseGridVertices[i].toString(),worker,i );

        if (receivedVertex.getAdjacentRanks(TwoPowerD-1-i)==RankOfCellWitchWillBeJoined) {
          assertionEquals3(fineGridVertices[i].getAdjacentRanks(TwoPowerD-1-i),worker,fineGridVertices[i].toString(),receivedVertex.toString(),_id);
          transferredCell = true;
        }

        logDebug(
          "mergeCellFromWorkerWithMaster(...)",
          "updated local vertex: " << fineGridVertices[i].toString() << " on tree " << _id
        );

        if ( not isVertexAdjacentToLocalSpacetree(fineGridVertices[i],true,false) ) {
          logDebug(
            "mergeCellFromWorkerWithMaster(...)",
            "have to overwrite adjacency information of local vertex " << fineGridVertices[i].toString() << " on tree " << _id
          );
          fineGridVertices[i].setAdjacentRanks( receivedVertex.getAdjacentRanks() );
          for (int j=0; j<TwoPowerD; j++) {
            fineGridVertices[i].setAdjacentRanks( j,
              fineGridVertices[i].getAdjacentRanks(j)==RankOfCellWitchWillBeJoined ? worker : fineGridVertices[i].getAdjacentRanks(j)
            );
          }
          logDebug(
            "mergeCellFromWorkerWithMaster(...)",
            "overwrote local adjacency information as vertex used to be a remote one: " << fineGridVertices[i].toString() << " on tree " << _id
          );
        }
      }

      if (transferredCell) {
        updateVertexRanksWithinCell(fineGridVertices,_id);
        logDebug(
          "mergeCellFromWorkerWithMaster(...)",
          "updated local vertex as this is a merged cell: " << fineGridVertices[0].toString()
        );
      }
    }
    else {
      for (int i=0; i<TwoPowerD; i++) {
        logDebug( "mergeCellFromWorkerWithMaster(...)", "- " << coarseGridVertices[i].toString() );
      }
      for (int i=0; i<TwoPowerD; i++) {
        logDebug( "mergeCellFromWorkerWithMaster(...)", "- " << fineGridVertices[i].toString() );
      }
    }
  }
}


void peano4::grid::Spacetree::updateSplittingCounter( int treeId ) {
  for (auto& p: _splitTriggered) {
    if (p.first==treeId) {
      p.second--;
      break;
    }
  }
}


int peano4::grid::Spacetree::getSplittingTree() const {
  for (auto& p: _splitTriggered) {
    if (p.second>0) {
      return p.first;
    }
  }
  return -1;
}


peano4::grid::GridStatistics peano4::grid::Spacetree::getGridStatistics() const {
  return _statistics;
}


void peano4::grid::Spacetree::markVerticesAroundParentOfForkedCell(
  GridVertex            coarseGridVertices[TwoPowerD]
) const {
  dfor2(k)
    assertion1( coarseGridVertices[kScalar].getState()!=GridVertex::State::HangingVertex, coarseGridVertices[kScalar].toString() );
    coarseGridVertices[kScalar].setIsAntecessorOfRefinedVertexInCurrentTreeSweep(true);
  enddforx
}


void peano4::grid::Spacetree::split(int newSpacetreeId, int cells) {
  assertion3(
    cells==std::numeric_limits<int>::max() or maySplit(),
    newSpacetreeId, cells, toString()
  );
  assertion3(
    cells!=std::numeric_limits<int>::max(),
    newSpacetreeId, cells, toString()
  );
  assertion3(
    cells>=1,
    newSpacetreeId, cells, toString()
  );

  _splitTriggered.insert( std::pair<int,int>(newSpacetreeId,cells) );
}


std::string peano4::grid::Spacetree::toString() const {
  std::ostringstream msg;
  msg << "(id=" << _id
      << ",master=" << _masterId
      << ",state=" << toString(_spacetreeState)
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
/*
  if (_hasJoined.empty()) {
	msg << ",noone-has-joined-recently";
  }
  else {
    msg << ",has-joined-recently={";
    for (const auto& p: _hasJoined) {
      msg << "(" << p << ")";
    }
    msg << "}";
  }
*/
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
  msg << ",stacks:" << _vertexStack.toString() << ")";
  return msg.str();
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
    and _splitting.empty()
    and _joinTriggered.empty();
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
  return mandatoryCriteria;
}


void peano4::grid::Spacetree::joinWithMaster(int maxCellsToJoin) {
  assertion1( mayJoinWithMaster(), _id);
  _spacetreeState = SpacetreeState::JoinTriggered;
  assertion( _splitTriggered.empty() );
  assertion( _splitting.empty() );
  assertion( _splittedCells.empty() );
  assertion( _joining.empty() );
  assertion( maxCellsToJoin>0 );

  _maxJoiningCells = maxCellsToJoin;
}


void peano4::grid::Spacetree::joinWithWorker(int id) {
  logInfo( "joinWithWorker(int)", "add (parts of) tree " << id << " to tree " << _id );
  assertion2( _joinTriggered.count(id)==0, id, _id );
  _joinTriggered.insert( id );
  assertion2( _splitTriggered.empty(), id, toString() );
  assertion2( _splitting.empty(), id, toString() );
}


bool peano4::grid::Spacetree::isInvolvedInJoinOrFork() const {
  return not _joinTriggered.empty()
      or not _joining.empty()
      or not _splitTriggered.empty()
	    or not _splitting.empty();
}

/*

peano4::grid::Spacetree::CellEvent peano4::grid::Spacetree::getCellEvent(
  GridVertex                         coarseGridVertices[TwoPowerD],
  GridVertex                         fineGridVertices[TwoPowerD]
) const {
  CellEvent result;

  // kann ich mir selber herholen
  CellType type = getCellType(fineGridVertices);

  bool parentCellIsLocal   = isSpacetreeNodeLocal(coarseGridVertices);
  bool cellIsLocal         = isSpacetreeNodeLocal(fineGridVertices);

  logTraceInWith6Arguments("getCellEvent(...)",toString(type), parentCellIsLocal, cellIsLocal, getTreeOwningSpacetreeNode(fineGridVertices),_splitting.size(),_id);

  if ( cellIsLocal and _spacetreeState==SpacetreeState::NewFromSplit ) {
    result = CellEvent::NewFromSplit;
  }
  else if ( not cellIsLocal and _splitting.count( getTreeOwningSpacetreeNode(fineGridVertices) )>0 ) {
    switch (type) {
      case CellType::New:
        result = CellEvent::MovingNewCellToWorker;
        break;
      case CellType::Persistent:
        result = CellEvent::MovingPersistentCellToWorker;
        break;
      case CellType::Delete:
        result = CellEvent::MovingDeletingCellToWorker;
        break;
    }
  }
  else if ( not cellIsLocal and _spacetreeState==SpacetreeState::Joining and getTreeOwningSpacetreeNode(fineGridVertices)==_masterId ) {
    result = CellEvent::JoiningWithMaster;
  }
  else if (
    parentCellIsLocal and cellIsLocal
  ) {
    switch (type) {
      case CellType::New:
        result = CellEvent::NewLocal;
        break;
      case CellType::Persistent:
        result = CellEvent::PersistentLocal;
        break;
      case CellType::Delete:
        result = CellEvent::DeleteLocal;
        break;
    }
  }
  else if (
    not parentCellIsLocal and cellIsLocal
  ) {
    result = CellEvent::TopCellOfLocalForest;
  }
  else if (
    parentCellIsLocal and not cellIsLocal
  ) {
    result = CellEvent::TopCellOfRemoteWorker;
  }
//  not parentCellIsLocal and not cellIsLocal
  else {
    result = CellEvent::Remote;
  }

  logTraceOutWith1Argument("getCellEvent(...)",toString(result));
  return result;
}
*/
