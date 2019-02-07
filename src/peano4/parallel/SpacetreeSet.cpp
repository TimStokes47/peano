#include "SpacetreeSet.h"
#include "Node.h"


#include "peano4/parallel/TreeManagementMessage.h"
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
  if (peano4::parallel::TreeManagementMessage::isMessageInQueue(peano4::parallel::Node::getInstance().getTreeManagementTag(),true)) {
    peano4::parallel::TreeManagementMessage message;
    message.receive(MPI_ANY_SOURCE,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::Blocking);

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
      state.receive(message.getSenderRank(),peano4::parallel::Node::getInstance().getTreeManagementTag(),true,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);

      peano4::grid::Spacetree newTree(
        state.getX(),
		state.getH()
	  );

      int key = 0;
      while (key>=0) {
   		MPI_Recv( &key, 1, MPI_INT, message.getSenderRank(), peano4::parallel::Node::getInstance().getTreeManagementTag(), tarch::mpi::Rank::getInstance().getCommunicator(), MPI_STATUS_IGNORE );
        if (key>=0) {
          assertion(peano4::grid::PeanoCurve::isInOutStack(key));
          std::pair< int, peano4::stacks::GridVertexStack > newEntry( key, peano4::stacks::GridVertexStack() );
          newTree._vertexStack.insert( newEntry );
          // @todo Hier was mit Priorities?
          newTree._vertexStack.startToReceive(message.getSenderRank(), peano4::parallel::Node::getInstance().getTreeManagementTag());
        }
      }

      for (auto& p: newTree._vertexStack ) {
  		p.second.finishToReceive();
      }

      _spacetrees.push_back( std::move(newTree) );
    }
    else {
      // unknown message type
      assertion1(false,message.toString());
    }
  }
}


void peano4::parallel::SpacetreeSet::addSpacetree( const peano4::grid::Spacetree& originalSpacetree, int id ) {
  tarch::multicore::Lock lock( _semaphore );

  if ( peano4::parallel::Node::getInstance().getRank(id)!=tarch::mpi::Rank::getInstance().getRank() ) {
	const int targetRank = peano4::parallel::Node::getInstance().getRank(id);

    TreeManagementMessage message(originalSpacetree._id, id, TreeManagementMessage::CreateNewRemoteTree);
    message.send(targetRank,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    peano4::grid::AutomatonState state = originalSpacetree._root;
    state.send(targetRank,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);

    for (auto& p: originalSpacetree._vertexStack ) {
      if ( peano4::grid::PeanoCurve::isInOutStack(p.first) ) {
		MPI_Send( &p.first, 1, MPI_INT, targetRank, peano4::parallel::Node::getInstance().getTreeManagementTag(), tarch::mpi::Rank::getInstance().getCommunicator() );

		p.second.startToSend(targetRank, peano4::parallel::Node::getInstance().getTreeManagementTag());
      }
    }
    for (auto& p: originalSpacetree._vertexStack ) {
      if ( peano4::grid::PeanoCurve::isInOutStack(p.first) ) {
		p.second.finishToSend();
      }
    }
    int terminateSymbol = -1;
	MPI_Send( &terminateSymbol, 1, MPI_INT, targetRank, peano4::parallel::Node::getInstance().getTreeManagementTag(), tarch::mpi::Rank::getInstance().getCommunicator() );
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

  // @todo Here are the MPI sends the very latest; but perhaps we do it completely different

  return false;
}


void peano4::parallel::SpacetreeSet::TraverseTask::prefetch() {
}


void peano4::parallel::SpacetreeSet::traverseTreeSet(peano4::grid::TraversalObserver& observer) {
  std::vector< tarch::multicore::Task* > traverseTasks;
  traverseTasks.reserve( _spacetrees.size() );

  for (auto& p: _spacetrees) {
	traverseTasks.push_back( new TraverseTask(
      p, *this, observer
    ));
	logInfo( "traverse(TraversalObserver&)", "issue task to traverse tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
  }
  logInfo( "traverse(TraversalObserver&)", "spawn " << traverseTasks.size() << " concurrent traversal tasks" );

  static int multitaskingRegion = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::traverseTreeSet" );
  peano4::parallel::Tasks runTraversals(traverseTasks,peano4::parallel::Tasks::TaskType::Task,multitaskingRegion);
}


peano4::parallel::SpacetreeSet::DataExchangeTask::DataExchangeTask( peano4::grid::Spacetree&  tree, SpacetreeSet& set ):
  _spacetree( tree ),
  _spacetreeSet( set ) {

}


bool peano4::parallel::SpacetreeSet::DataExchangeTask::run() {
  for (auto sourceStack = _spacetree._vertexStack.begin(); sourceStack != _spacetree._vertexStack.end(); ) {
    if (
      Node::getInstance().isBoundaryExchangeOutputStackNumber(sourceStack->first)
      and
      not sourceStack->second.empty()
    ) {
	  const int targetId = Node::getInstance().getIdOfBoundaryExchangeOutputStackNumber(sourceStack->first);

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
      logInfo( "traverse(TraversalObserver&)", "issue task to manage data transfer of tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
	}
  }
  logInfo( "traverse(TraversalObserver&)", "spawn " << dataExchangeTasks.size() << " concurrent data exchange tasks" );

  static int multitaskingRegion = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::exchangeDataBetweenTrees" );
  peano4::parallel::Tasks runTraversals(dataExchangeTasks,peano4::parallel::Tasks::TaskType::Task,multitaskingRegion);
}


void peano4::parallel::SpacetreeSet::traverse(peano4::grid::TraversalObserver& observer) {
  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    peano4::parallel::Node::getInstance().continueToRun();
  }

  traverseTreeSet(observer);

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
