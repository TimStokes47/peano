#include "SpacetreeSet.h"
#include "Node.h"
#include "TreeManagementMessage.h"
#include "IntegerMessage.h"


#include "peano4/grid/PeanoCurve.h"


#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/Lock.h"
#include "tarch/mpi/Rank.h"


#include "tarch/services/ServiceRepository.h"


tarch::logging::Log peano4::parallel::SpacetreeSet::_log( "peano4::parallel::SpacetreeSet" );


peano4::parallel::SpacetreeSet::SpacetreeSet() {
  tarch::services::ServiceRepository::getInstance().addService( this, "peano4::parallel::SpacetreeSet" );
}


peano4::parallel::SpacetreeSet&  peano4::parallel::SpacetreeSet::getInstance() {
  static peano4::parallel::SpacetreeSet singleton;
  return singleton;
}


void peano4::parallel::SpacetreeSet::init(
  const tarch::la::Vector<Dimensions,double>&  offset,
  const tarch::la::Vector<Dimensions,double>&  width
) {
  assertion4(
    (peano4::parallel::Node::getInstance().getNumberOfRegisteredTrees()==1 and tarch::mpi::Rank::getInstance().getRank()==0)
	or
    (peano4::parallel::Node::getInstance().getNumberOfRegisteredTrees()==0 and tarch::mpi::Rank::getInstance().getRank()!=0),
	peano4::parallel::Node::getInstance().getNumberOfRegisteredTrees(),
	offset, width, tarch::mpi::Rank::getInstance().getRank()
  );

  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    peano4::grid::Spacetree spacetree( offset, width );
    _spacetrees.push_back( std::move(spacetree) );
  }
}


peano4::parallel::SpacetreeSet::~SpacetreeSet() {
}


void peano4::parallel::SpacetreeSet::receiveDanglingMessages() {
  #ifdef Parallel
  if (peano4::parallel::TreeManagementMessage::isMessageInQueue(peano4::parallel::Node::getInstance().getTreeManagementTag(),true)) {
    peano4::parallel::TreeManagementMessage message;
    message.receive(MPI_ANY_SOURCE,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    if (message.getAction()==peano4::parallel::TreeManagementMessage::Action::RequestNewRemoteTree) {
      int newSpacetreeId = peano4::parallel::Node::getInstance().reserveId(
        tarch::mpi::Rank::getInstance().getRank(),  // on current node
        message.getMasterSpacetreeId()
      );

      logDebug( "receiveDanglingMessages()", "reserved tree id " << newSpacetreeId << " for tree " << message.getMasterSpacetreeId() );

      message.setWorkerSpacetreeId( newSpacetreeId );
      message.setAction(TreeManagementMessage::Action::BookedNewRemoteTree);
      message.send(message.getSenderRank(),peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
    }
    else if (message.getAction()==peano4::parallel::TreeManagementMessage::Action::CreateNewRemoteTree) {
      peano4::grid::AutomatonState state;
      const int tag = peano4::parallel::Node::getInstance().getGridDataExchangeTag(message.getMasterSpacetreeId(),false);
      state.receive(message.getSenderRank(),tag,false,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);

      peano4::grid::Spacetree newTree(
        state.getX(),
        state.getH()
      );

      newTree._id        = message.getWorkerSpacetreeId();
      newTree._masterId  = message.getMasterSpacetreeId();
      newTree._root      = state;
      newTree._spacetreeState = peano4::grid::Spacetree::SpacetreeState::NewFromSplit;

      IntegerMessage messageStackKey(0);
      while (messageStackKey.getValue()>=0) {
    	messageStackKey.receive(message.getSenderRank(), tag, true, IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
        if (messageStackKey.getValue()>=0) {
          assertion(peano4::grid::PeanoCurve::isInOutStack(messageStackKey._value));

          IntegerMessage numberOfElements;
          numberOfElements.receive( message.getSenderRank(), tag, true, IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests );
          logDebug( "receiveDanglingMessages()", "received notification to receive " << numberOfElements._value << " entries for stack " << messageStackKey.getValue() );
          assertion(numberOfElements._value>0);

          std::pair< int, peano4::stacks::GridVertexStack > newEntry( messageStackKey.getValue(), peano4::stacks::GridVertexStack() );
          newTree._vertexStack.insert( newEntry );
          newTree._vertexStack[messageStackKey.getValue()].startReceive(message.getSenderRank(), tag, numberOfElements._value);
        }
      }

      logDebug( "receiveDanglingMessages()", "finalise all receives" );
      for (auto& p: newTree._vertexStack ) {
  		p.second.finishSendOrReceive();
      }

      logDebug( "receiveDanglingMessages()", "tree fraction is completely copied to new node" );

      for (auto& p: newTree._vertexStack) {
    	logDebug( "run()", "stack " << p.first << ": " << p.second.size() << " element(s)" );
      }

      _spacetrees.push_back( std::move(newTree) );
    }
    else {
      // unknown message type
      assertion1(false,message.toString());
    }
  }
  #endif
}


void peano4::parallel::SpacetreeSet::addSpacetree( peano4::grid::Spacetree& originalSpacetree, int id ) {
  tarch::multicore::Lock lock( _semaphore );

  if ( peano4::parallel::Node::getInstance().getRank(id)!=tarch::mpi::Rank::getInstance().getRank() ) {
    #ifdef Parallel
    const int targetRank = peano4::parallel::Node::getInstance().getRank(id);

    TreeManagementMessage message(originalSpacetree._id, id, TreeManagementMessage::CreateNewRemoteTree);
    message.send(targetRank,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    peano4::grid::AutomatonState state = originalSpacetree._root;
    const int tag = peano4::parallel::Node::getInstance().getGridDataExchangeTag(originalSpacetree._id,false);
    state.send(targetRank,tag,false,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);

    for (auto& p: originalSpacetree._vertexStack ) {
      if ( peano4::grid::PeanoCurve::isInOutStack(p.first) and p.second.size()>0) {
        IntegerMessage messageStackKey(p.first);
        messageStackKey.send(targetRank,tag,true,IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

        IntegerMessage numberOfElements(p.second.size());
        numberOfElements.send(targetRank,tag,true,IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

        p.second.startSend(targetRank, tag);
      }
    }
    for (auto& p: originalSpacetree._vertexStack ) {
      if ( p.second.isSendingOrReceiving() ) {
        p.second.finishSendOrReceive();
      }
    }
    int terminateSymbol = -1;
    MPI_Send( &terminateSymbol, 1, MPI_INT, targetRank, tag, tarch::mpi::Rank::getInstance().getCommunicator() );
    #else
    assertionMsg( false, "should never enter this branch without -DParallel" );
    #endif
  }
  else {
    peano4::grid::Spacetree newTree(originalSpacetree, id);
    _spacetrees.push_back( std::move(newTree) );
  }
}


peano4::parallel::SpacetreeSet::TraverseTask::TraverseTask(
  peano4::grid::Spacetree&          tree,
  SpacetreeSet& set,
  peano4::grid::TraversalObserver&  observer
):
  _spacetree(tree),
  _spacetreeSet(set),
  _observer( observer ) {
}


bool peano4::parallel::SpacetreeSet::TraverseTask::run() {
  peano4::grid::TraversalObserver* localObserver = _observer.clone( _spacetree._id );
  _spacetree.traverse( *localObserver, _spacetreeSet );
  delete localObserver;
  return false;
}


void peano4::parallel::SpacetreeSet::TraverseTask::prefetch() {
}


void peano4::parallel::SpacetreeSet::traverseTrees(peano4::grid::TraversalObserver& observer) {
  std::vector< tarch::multicore::Task* > traverseTasksForNewSplitTrees;
  std::vector< tarch::multicore::Task* > traverseTasksForAllOtherTrees;

  for (auto& p: _spacetrees) {
    if ( p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::NewFromSplit ) {
      traverseTasksForNewSplitTrees.push_back( new TraverseTask(
        p, *this, observer
      ));
      logDebug( "traverseTrees(TraversalObserver&)", "issue task to traverse tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
    }
    else {
      traverseTasksForAllOtherTrees.push_back( new TraverseTask(
        p, *this, observer
      ));
      logDebug( "traverseTrees(TraversalObserver&)", "issue task to traverse tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
    }
  }

  if ( not traverseTasksForAllOtherTrees.empty() ) {
    logInfo( "traverseTrees(TraversalObserver&)", "spawn " << traverseTasksForAllOtherTrees.size() << " concurrent traversal tasks for all trees without new-from-split marker" );
    static int multitaskingRegionForAllOtherTrees = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::traverseTreeSet" );
    peano4::parallel::Tasks runTraversalsForAllOtherTrees(traverseTasksForAllOtherTrees,peano4::parallel::Tasks::TaskType::Task,multitaskingRegionForAllOtherTrees);
  }

  exchangeDataBetweenNewOrMergingTrees();

  if (not traverseTasksForNewSplitTrees.empty() ) {
    logInfo( "traverseTrees(TraversalObserver&)", "spawn " << traverseTasksForNewSplitTrees.size() << " concurrent traversal tasks for all new trees" );
    static int multitaskingRegionForNewSplitTrees = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::traverseTreeSet" );
    peano4::parallel::Tasks runTraversalsForNewSplitTrees(traverseTasksForNewSplitTrees,peano4::parallel::Tasks::TaskType::Task,multitaskingRegionForNewSplitTrees);
  }
}


peano4::parallel::SpacetreeSet::DataExchangeTask::DataExchangeTask( peano4::grid::Spacetree&  tree, SpacetreeSet& set ):
  _spacetree( tree ),
  _spacetreeSet( set ) {
}


bool peano4::parallel::SpacetreeSet::DataExchangeTask::run() {
  // Trigger all send and receives required
  // --------------------------------------
  // We exploit all the symmetries
  for (auto& sourceStack: _spacetree._vertexStack) {
    if (
      Node::getInstance().isBoundaryExchangeOutputStackNumber(sourceStack.first)
      and
      not sourceStack.second.empty()
      and
      Node::getInstance().getRank( Node::getInstance().getIdOfExchangeStackNumber(sourceStack.first) )!=tarch::mpi::Rank::getInstance().getRank()
    ) {
      int targetId  = Node::getInstance().getIdOfExchangeStackNumber(sourceStack.first);
      int rank      = Node::getInstance().getRank( targetId );
      int count     = sourceStack.second.size();
      int inStack   = Node::getInstance().getInputStackNumberOfBoundaryExchange(_spacetree._id);
      // @todo Get the tags right
      int tag       = 32;

      // @todo debug
      logInfo( "run()", "send stack " << sourceStack.first << " to rank " << rank << " with tag " << tag );
      logInfo( "run()", "in return, receive " << count << " element(s) from rank " << rank << " with tag " << tag << " into stack " << inStack );
      sourceStack.second.startSend(rank,tag);
      assertion(not Node::getInstance().isBoundaryExchangeOutputStackNumber(inStack));
      _spacetree._vertexStack[inStack].startReceive(rank,tag,count);
    }
  }

  // All local stacks
  // ----------------
  for (auto sourceStack = _spacetree._vertexStack.begin(); sourceStack != _spacetree._vertexStack.end(); ) {
    if (
      Node::getInstance().isBoundaryExchangeOutputStackNumber(sourceStack->first)
      and
      not sourceStack->second.empty()
      and
      Node::getInstance().getRank( Node::getInstance().getIdOfExchangeStackNumber(sourceStack->first) )==tarch::mpi::Rank::getInstance().getRank()
    ) {
      const int targetId = Node::getInstance().getIdOfExchangeStackNumber(sourceStack->first);

      // @todo Id might not be local
      peano4::grid::Spacetree& targetTree = _spacetreeSet.getSpacetree( targetId );
      const int targetStack = Node::getInstance().getInputStackNumberOfBoundaryExchange(_spacetree._id);
      logDebug(
         "traverse(Observer)",
         "map output stream " << sourceStack->first << " of tree " <<
		 _spacetree._id << " onto input stream " << targetStack <<
         " of tree " << targetId <<
         ". Copy/clone " << _spacetree._vertexStack[sourceStack->first].size() << " entries"
      );

      assertion3( _spacetree._id != targetId,                   _spacetree._id, targetId, targetStack);
      if( not targetTree._vertexStack[targetStack].empty() ) {
        assertion6( targetTree._vertexStack[targetStack].empty(), _spacetree._id, targetId, targetStack,  targetTree._vertexStack[targetStack].size(), targetTree._vertexStack[targetStack].pop().toString(), sourceStack->second.size() );
      }
      assertion5( targetTree._vertexStack[targetStack].empty(), _spacetree._id, targetId, targetStack,  targetTree._vertexStack[targetStack].size(), sourceStack->second.size() );

      targetTree._vertexStack[ targetStack ].clone( sourceStack->second );

      #if PeanoDebug>0
      const int comparisonStackForTarget = Node::getInstance().getOutputStackNumberOfBoundaryExchange(_spacetree._id);
      assertion6(
        targetTree._vertexStack[ targetStack ].size() == targetTree._vertexStack[ comparisonStackForTarget ].size()
        or
        targetTree._vertexStack[ comparisonStackForTarget ].empty(),
        targetTree._vertexStack[ targetStack ].size(),
        targetTree._vertexStack[ comparisonStackForTarget ].size(),
        targetStack, comparisonStackForTarget, targetTree._id, _spacetree._id
      );
      #endif

      _spacetree._vertexStack[sourceStack->first].clear();

      sourceStack++;
    }
    else if (
      Node::getInstance().isBoundaryExchangeOutputStackNumber(sourceStack->first)
	  and
	  sourceStack->second.empty()
    ) {
      sourceStack = _spacetree._vertexStack.erase(sourceStack);
    }
    else {
      sourceStack++;
    }
  }

  // Finalise data exchange
  // ----------------------
  // @todo Das ist jetzt halt wieder net so klasse, weil das logisch ordnet.
  for (auto& sourceStack: _spacetree._vertexStack) {
    if ( sourceStack.second.isSendingOrReceiving() ) {
      sourceStack.second.finishSendOrReceive();
    }
  }

  return false;
}


void peano4::parallel::SpacetreeSet::DataExchangeTask::prefetch() {
}


void peano4::parallel::SpacetreeSet::exchangeDataBetweenTrees() {
  std::vector< tarch::multicore::Task* > dataExchangeTasks;
  dataExchangeTasks.reserve( _spacetrees.size() );

  for (auto& p: _spacetrees) {
    if (
      p._spacetreeState!=peano4::grid::Spacetree::SpacetreeState::Joined
    ) {
        dataExchangeTasks.push_back( new DataExchangeTask(
        p, *this
      ));
      logInfo( "exchangeDataBetweenTrees(TraversalObserver&)", "issue task to manage data transfer of tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
    }
  }
  logInfo( "exchangeDataBetweenTrees(TraversalObserver&)", "trigger " << dataExchangeTasks.size() << " concurrent data exchange tasks" );

  static int multitaskingRegion = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::exchangeDataBetweenTrees" );
  peano4::parallel::Tasks runTraversals(dataExchangeTasks,peano4::parallel::Tasks::TaskType::Task,multitaskingRegion);
}


void peano4::parallel::SpacetreeSet::exchangeDataBetweenNewOrMergingTrees() {
  for (auto& tree: _spacetrees) {
    for (auto& stacks: tree._vertexStack) {
      if (Node::isSplitMergeOutputStackNumber(stacks.first)) {
        const int targetTreeId = Node::getIdOfExchangeStackNumber(stacks.first);
        const int inStack      = Node::getInputStackNumberForSplitMergeDataExchange(tree._id);
        if ( Node::getInstance().getRank(targetTreeId)==tarch::mpi::Rank::getInstance().getRank()) {
          logInfo( "exchangeDataBetweenNewOrMergingTrees()",
            "clone " << stacks.second.size() << " entries from stack " << stacks.first << " of tree " << tree._id <<
            " into stack " << inStack << " of tree " << targetTreeId
          );
          getSpacetree(targetTreeId)._vertexStack[inStack].clone( stacks.second );
          stacks.second.clear();
        }
        else {
          logInfo( "exchangeDataBetweenNewOrMergingTrees()",
            "send " << stacks.second.size() << " entries from stack " << stacks.first << " of tree " << tree._id <<
            " to rank " << Node::getInstance().getRank(targetTreeId) << " holding tree " << targetTreeId
          );
          assertion(false);
          stacks.second.clear();
        }
      }
    }
  }

  @todo Hier sollte man jetzt warten, bis die Daten da sind. Dazu muss aber in der Assertion oben auch mal erst was weggeschickt werden.
}


void peano4::parallel::SpacetreeSet::traverse(peano4::grid::TraversalObserver& observer) {
  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    peano4::parallel::Node::getInstance().continueToRun();
  }

  traverseTrees(observer);

  //
  // Merge local copies of the statistics
  //
  for (auto&  from: _spacetrees) {
    if ( from._id != _spacetrees.begin()->_id ) {
      merge(from._statistics, _spacetrees.begin()->_statistics );
    }
  }

  exchangeDataBetweenTrees();

  cleanUpTrees();
}


void peano4::parallel::SpacetreeSet::cleanUpTrees() {
  std::set< int > childrenThatShouldMerge;

  for (auto p = _spacetrees.begin(); p!=_spacetrees.end(); ) {
  	if (
	  p->_coarseningHasBeenVetoed
	  and
	  not p->mayJoinWithMaster()
	  and
	  p->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running
    ) {
      logInfo( "traverse(Observer)", "can't join of tree " << p->_id << " with its master tree " << p->_masterId );
      logDebug( "traverse(Observer)", "tree: " << p->toString() );

      std::set< int > children = peano4::parallel::Node::getInstance().getChildren( p->_id );
      childrenThatShouldMerge.insert( children.begin(), children.end() );
    }
    else if (
	  p->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running
	  and
	  p->getGridStatistics().getNumberOfLocalRefinedCells() + p->getGridStatistics().getNumberOfLocalUnrefinedCells() == 0
	)  {
      logInfo( "traverse(Observer)", "tree " << p->_id << " is degenerated as it does not hold any local cells. Remove" );
      Node::getInstance().deregisterId(p->_id);
      p = _spacetrees.erase(p);
      p--;
    }
	else if (
	  p->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Joined
	)  {
      logInfo( "traverse(Observer)", "tree " << p->_id << " has joined, so remove" );

      // Has to be a reference. Otherwise, access to the iterator copies the
      // stack which is not allowed
      for (auto& s: p->_vertexStack) {
        assertion3(
          not Node::getInstance().isBoundaryExchangeOutputStackNumber(s.first)
		  or
          s.second.empty(),
		  p->_id, s.first, s.second.size()
        );
    	if (not (
          not Node::getInstance().isBoundaryExchangeInputStackNumber(s.first)
		  or
          s.second.empty()
    	)) {
    	  logWarning( "traverse(Observer)", s.second.pop().toString() );
    	}
    	assertion4(
          not Node::getInstance().isBoundaryExchangeInputStackNumber(s.first)
		  or
          s.second.empty(),
		  p->_id, s.first, s.second.size(),
		  p->toString()
        );
      }

      Node::getInstance().deregisterId(p->_id);
      p = _spacetrees.erase(p);
      p--;
	}

	p++;
  }


  for (auto& p: _spacetrees ) {
  	if (
      childrenThatShouldMerge.count(p._id)==1
	  and
	  p.mayJoinWithMaster()
  	  and
	  getSpacetree(p._masterId).mayJoinWithWorker()
    ) {
  	  logInfo( "traverse(Observer)", "trigger join of tree " << p._id << " with its master tree " << p._masterId << " to enable further grid erases");
  	  join(p._id);
  	}
  }
}


void peano4::parallel::SpacetreeSet::merge(
  const peano4::grid::GridStatistics&   from,
  peano4::grid::GridStatistics&         to
) {
  to.setNumberOfErasingVertices( to.getNumberOfErasingVertices() + from.getNumberOfErasingVertices() );
  to.setNumberOfRefiningVertices( to.getNumberOfRefiningVertices() + from.getNumberOfRefiningVertices() );
  to.setNumberOfUnrefinedVertices( to.getNumberOfUnrefinedVertices() + from.getNumberOfUnrefinedVertices() );
  to.setNumberOfRefinedVertices( to.getNumberOfRefinedVertices() + from.getNumberOfRefinedVertices() );
}


peano4::grid::GridStatistics peano4::parallel::SpacetreeSet::getGridStatistics() const {
  return _spacetrees.begin()->getGridStatistics();
}


void peano4::parallel::SpacetreeSet::join(int treeId) {
  peano4::grid::Spacetree* tree = nullptr;
  for (auto& p: _spacetrees) {
	if (p._id==treeId) tree = &p;
  }
  if (tree==nullptr) {
	assertionMsg(false, "unknown tree Id");
  }
  assertion1( tree->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running, treeId);
  assertion1( tree->mayJoinWithMaster(), treeId );

  tree->joinWithMaster();
  const int fatherId = tree->_masterId;

  tree = nullptr;
  for (auto& p: _spacetrees) {
	if (p._id==fatherId) tree = &p;
  }
  if (tree==nullptr) {
	assertionMsg(false, "unknown father tree Id");
  }
  assertion1( tree->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running, treeId);

  tree->joinWithWorker(treeId);
}


bool peano4::parallel::SpacetreeSet::split(int treeId, int cells, int targetRank) {
  peano4::grid::Spacetree&  tree = getSpacetree( treeId );

  if ( tree.maySplit() ) {
    int newSpacetreeId = -1;

    if (tarch::mpi::Rank::getInstance().getRank()!=targetRank) {
      #ifdef Parallel
      logDebug( "split(int,int,int)", "request new tree on rank " << targetRank );
      peano4::parallel::TreeManagementMessage message(treeId,-1,peano4::parallel::TreeManagementMessage::Action::RequestNewRemoteTree);
      message.send(targetRank,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

      message.receive(targetRank,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
      assertion(message.getAction()==TreeManagementMessage::Action::BookedNewRemoteTree);
      newSpacetreeId = message.getWorkerSpacetreeId();
      #else
      newSpacetreeId = -1;
      assertionMsg( false, "can't split into tree on a different rank if not compiled with mpi");
      #endif
    }
    else {
      #if !defined(SharedMemoryParallelisation)
      if ( peano4::parallel::Node::getInstance().getRank(treeId) == targetRank ) {
        logWarning( "split(int,int)", "tree " << treeId << " tries to split up into another on rank " << targetRank << " even though no multithreading enabled. This might lead to deadlocks" );
      }
      #endif

      newSpacetreeId = peano4::parallel::Node::getInstance().reserveId(
        peano4::parallel::Node::getInstance().getRank(treeId),
        treeId
      );
    }

    if (newSpacetreeId>=0) {
      tree.split(newSpacetreeId, cells);
      logInfo( "split(int,int)", "trigger split of tree " << treeId << " into tree " << newSpacetreeId << " with " << cells << " fine grid cells" );
      return true;
    }
  }

  return false;
}


peano4::grid::Spacetree&  peano4::parallel::SpacetreeSet::getSpacetree(int id) {
  for (auto& p: _spacetrees) {
	if (p._id==id) return p;
  }
  assertion3( false, "no spacetree found", id, tarch::mpi::Rank::getInstance().getRank() );
  return *_spacetrees.begin(); // just here to avoid warning
}


bool peano4::parallel::SpacetreeSet::move(int sourceTreeId, int targetRank) {
  peano4::grid::Spacetree&  sourceTree = getSpacetree( sourceTreeId );

  int result = -1;
  if (
    not peano4::parallel::Node::getInstance().hasChildrenTree(sourceTreeId)
    and
	sourceTree.mayMove()
  ) {
	result = peano4::parallel::Node::getInstance().reserveId(targetRank,sourceTree._masterId);
	if ( result>=0 ) {
      sourceTree.split(result, std::numeric_limits<int>::max());
      logInfo( "move(int,int)", "move tree " << sourceTreeId << " to tree " << result );
	}
  }
  return result>=0;
}
