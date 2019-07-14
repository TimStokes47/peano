#include "SpacetreeSet.h"
#include "Node.h"
#include "TreeManagementMessage.h"


#include "peano4/grid/PeanoCurve.h"


#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/Lock.h"

#include "tarch/mpi/Rank.h"
#include "tarch/mpi/IntegerMessage.h"

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
    _spacetrees.push_back( spacetree );
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

      logInfo( "receiveDanglingMessages()", "reserved tree id " << newSpacetreeId << " for tree " << message.getMasterSpacetreeId() );

      message.setWorkerSpacetreeId( newSpacetreeId );
      message.setAction(TreeManagementMessage::Action::BookedNewRemoteTree);
      message.send(message.getSenderRank(),peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
    }
    else if (message.getAction()==peano4::parallel::TreeManagementMessage::Action::CreateNewRemoteTree) {
      const int tag = peano4::parallel::Node::getInstance().getGridDataExchangeTag(message.getMasterSpacetreeId(),message.getWorkerSpacetreeId(),false);
      peano4::grid::AutomatonState state;
      state.receive(message.getSenderRank(),tag,false,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);
      peano4::grid::Spacetree newTree(
        message.getWorkerSpacetreeId(),
        message.getMasterSpacetreeId(),
        state.getX(),
        state.getH(),
        state.getInverted()
      );
      _spacetrees.push_back( std::move(newTree) );
      logDebug( "receiveDanglingMessages(...)", "created the new tree " << _spacetrees.back().toString() );
    }
  }
  #endif
}


void peano4::parallel::SpacetreeSet::addSpacetree( int masterId, int newTreeId ) {
  tarch::multicore::Lock lock( _semaphore );

  if ( peano4::parallel::Node::getInstance().getRank(masterId)!=peano4::parallel::Node::getInstance().getRank(newTreeId) ) {
    #ifdef Parallel
    const int targetRank = peano4::parallel::Node::getInstance().getRank(newTreeId);

    TreeManagementMessage message(masterId, newTreeId, TreeManagementMessage::CreateNewRemoteTree);
    message.send(targetRank,peano4::parallel::Node::getInstance().getTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    const int tag = peano4::parallel::Node::getInstance().getGridDataExchangeTag(masterId,newTreeId,false);
    peano4::grid::AutomatonState state = _spacetrees.begin()->_root;
    state.send(targetRank,tag,false,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);
    #else
    assertionMsg( false, "should never enter this branch without -DParallel" );
    #endif
  }
  else {
    peano4::grid::Spacetree newTree(
      newTreeId,
      masterId,
      _spacetrees.begin()->_root.getX(),
      _spacetrees.begin()->_root.getH(),
      _spacetrees.begin()->_root.getInverted()
    );
    _spacetrees.push_back( std::move(newTree) );
  }
}


peano4::parallel::SpacetreeSet::TraverseTask::TraverseTask(
  peano4::grid::Spacetree&          tree,
  SpacetreeSet&                     set,
  peano4::grid::TraversalObserver&  observer
):
  _spacetree(tree),
  _spacetreeSet(set),
  _observer( observer ) {
}


bool peano4::parallel::SpacetreeSet::TraverseTask::run() {
  peano4::grid::TraversalObserver* localObserver = _observer.clone( _spacetree._id );
  _spacetree.traverse( *localObserver, true );
  delete localObserver;
  return false;
}


void peano4::parallel::SpacetreeSet::TraverseTask::prefetch() {
}




// @todo Docu: Wir laufen ueber alle Trees drueber, die mindestens mit einem Worker gemerged haben
void peano4::parallel::SpacetreeSet::exchangeDataBetweenMergingTreesAndTraverseMaster(const std::set<int>& trees, peano4::grid::TraversalObserver& observer) {
  for (auto& masterTreeId: trees ) {
    peano4::grid::Spacetree& masterTree = getSpacetree(masterTreeId);

    // ueber alle Kinder drueber gehen
    for (auto workerTreeId: masterTree._joining) {
      logInfo( "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)", "tree " << workerTreeId << " is merging (partially) into tree " << masterTreeId );

      const int masterTreeStack = peano4::parallel::Node::getInputStackNumberForSplitMergeDataExchange(workerTreeId);
      const int workerTreeStack = peano4::parallel::Node::getOutputStackNumberForSplitMergeDataExchange(masterTreeId);

      peano4::grid::Spacetree& workerTree = getSpacetree(workerTreeId);

/*
      logDebug(
        "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)",
        "copy stack " << workerTreeStack << " from tree " << workerTreeId << " into " << masterTreeStack << " on tree " << masterTreeId << ": "
        << workerTree._vertexStack[workerTreeStack].toString()
      );
*/

      assertion( masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].empty() );
      masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].clone( workerTree._vertexStack[ peano4::grid::Spacetree::StackKey(workerTree._id,workerTreeStack) ] );
      masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].reverse();
      workerTree._vertexStack[ peano4::grid::Spacetree::StackKey(workerTree._id,workerTreeStack) ].clear();

      logDebug(
        "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)",
        "target stack now is: " << masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].toString()
      );
      logDebug( "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)", "reversed all " << masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].size() << " entries" );
    }
    logDebug( "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)", "issue task to traverse tree " << masterTree._id << " in state " << peano4::grid::Spacetree::toString(masterTree._spacetreeState) );

    peano4::grid::TraversalObserver* localObserver = observer.clone( masterTree._id );
    masterTree.traverse( *localObserver, true );
    delete localObserver;

    #if PeanoDebug>0
    for (auto workerTreeId: masterTree._hasJoined) {
      const int masterTreeStack = peano4::parallel::Node::getInputStackNumberForSplitMergeDataExchange(workerTreeId);
      assertion3(
        masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].empty(),
        masterTree._vertexStack[ peano4::grid::Spacetree::StackKey(masterTree._id,masterTreeStack) ].toString(),
        masterTree.toString(), workerTreeId
      );
    }
    #endif
  }
/*
      if (Node::getInstance().getRank(p._masterId)==tarch::mpi::Rank::getInstance().getRank()) {
        logDebug( "exchangeDataBetweenNewTreesAndRerunClones()", "parent tree " << p._masterId << " is local" );
        const int outStack = Node::getOutputStackNumberForSplitMergeDataExchange(p._id);
        const int inStack  = Node::getInputStackNumberForSplitMergeDataExchange(p._masterId);
        logDebug( "exchangeDataBetweenNewTreesAndRerunClones()", "clone parent's stack " << outStack << " into " << inStack << " and reverse order (make it a stream): " << getSpacetree(p._masterId)._vertexStack[outStack].size() << " entries");
        p._vertexStack[inStack].clone( getSpacetree(p._masterId)._vertexStack[outStack] );
        p._vertexStack[inStack].reverse();
      }
*/
}


void peano4::parallel::SpacetreeSet::traverseNonMergingExistingTrees(peano4::grid::TraversalObserver& observer) {
  std::vector< tarch::multicore::Task* > traverseTasksForRunningTrees;

  for (auto& p: _spacetrees) {
    bool traverse = false;
    switch (p._spacetreeState) {
      case peano4::grid::Spacetree::SpacetreeState::NewRoot:
      case peano4::grid::Spacetree::SpacetreeState::Running:
      case peano4::grid::Spacetree::SpacetreeState::JoinTriggered:
      case peano4::grid::Spacetree::SpacetreeState::Joining:
        traverse = p._joining.empty();
        break;
      case peano4::grid::Spacetree::SpacetreeState::NewFromSplit:
        traverse = false;
        break;
    }
    if (traverse) {
      traverseTasksForRunningTrees.push_back( new TraverseTask(
        p, *this, observer
      ));
      logDebug( "traverseNonMergingExistingTrees(TraversalObserver&)", "issue task to traverse tree " << p._id << ": " << p.toString() );
    }
  }

  if ( not traverseTasksForRunningTrees.empty() ) {
    logInfo( "traverseNonMergingExistingTrees(TraversalObserver&)", "spawn " << traverseTasksForRunningTrees.size() << " concurrent traversal tasks" );
    static int multitaskingRegionForAllOtherTrees = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::traverseNonMergingExistingTrees" );
    peano4::parallel::Tasks run(traverseTasksForRunningTrees,peano4::parallel::Tasks::TaskType::Task,multitaskingRegionForAllOtherTrees);
  }
}


peano4::parallel::SpacetreeSet::DataExchangeTask::DataExchangeTask( int spacetreeId, SpacetreeSet& set ):
  _spacetreeId( spacetreeId ),
  _spacetreeSet( set ) {
}


bool peano4::parallel::SpacetreeSet::DataExchangeTask::run() {
  peano4::grid::Spacetree& _spacetree = _spacetreeSet.getInstance().getSpacetree( _spacetreeId );

  triggerExchange( _spacetree._vertexStack, _spacetreeId );

  return false;
}



void peano4::parallel::SpacetreeSet::DataExchangeTask::prefetch() {
}



// @todo Could be tasks for its own. Two different task types though?
void peano4::parallel::SpacetreeSet::exchangeDataBetweenNewTreesAndRerunClones(
  peano4::grid::TraversalObserver&  observer
) {
  for (auto& p: _spacetrees) {
    if (p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::NewFromSplit) {
      logInfo( "exchangeDataBetweenNewTreesAndRerunClones()", "tree " << p._id << " is new. Initialise it through clone" );
      const int inStack  = Node::getInputStackNumberForSplitMergeDataExchange(p._masterId);

      if (Node::getInstance().getRank(p._masterId)==tarch::mpi::Rank::getInstance().getRank()) {
        // @todo Debug
        logInfo( "exchangeDataBetweenNewTreesAndRerunClones()", "parent tree " << p._masterId << " is local" );
        const int outStack = Node::getOutputStackNumberForSplitMergeDataExchange(p._id);
        // @todo Debug
//        logInfo( "exchangeDataBetweenNewTreesAndRerunClones()", "clone parent's stack " << outStack << " into " << inStack << " and reverse order (make it a stream): " << getSpacetree(p._masterId)._vertexStack[outStack].size() << " entries");
        peano4::grid::Spacetree::_vertexStack[ peano4::grid::Spacetree::StackKey(p._id,inStack) ].clone( peano4::grid::Spacetree::_vertexStack[ peano4::grid::Spacetree::StackKey(p._masterId,outStack) ] );
        peano4::grid::Spacetree::_vertexStack[ peano4::grid::Spacetree::StackKey(p._id,inStack) ].reverse();
      }
      else {
        #if defined(Parallel)
        const int source = Node::getInstance().getRank(p._masterId);
        const int tag    = Node::getInstance().getGridDataExchangeTag( p._masterId, p._id, false );
        // @todo Debug
        logInfo(
          "exchangeDataBetweenNewTreesAndRerunClones()",
          "receive into stack " << inStack << " for tree " << p._id << " from rank " << source <<
          " (tree " << p._masterId << ") on tag " << tag
        );
        tarch::mpi::IntegerMessage message;
        message.receive( source, tag, false, tarch::mpi::IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests );
        // @todo Debug
        logInfo(
          "exchangeDataBetweenNewTreesAndRerunClones()",
          "will receive " << message.getValue() << " entries"
        );
        assertion1(message.getValue()>0,message.toString());
        p._vertexStack[inStack].startReceive(source,tag,message.getValue());
        p._vertexStack[inStack].finishSendOrReceive();
        p._vertexStack[inStack].reverse();
        #else
        assertionMsg(false, "should not be called");
        #endif
      }

      // @todo Debug
      logInfo( "exchangeDataBetweenNewTreesAndRerunClones()", "run tree " << p._id << " in dry mode (data exchange only): " << p.toString() );

      peano4::grid::TraversalObserver* localObserver = observer.clone( p._id );
      p.traverse(*localObserver,true);
      delete localObserver;

      // @todo Debug
      logInfo( "exchangeDataBetweenNewTreesAndRerunClones()", "tree after dry run: " << p.toString() );
    }
    else if (p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running) {
      // @todo raus mit dem if samt Info
      if (not p._hasSplit.empty()) {
        logInfo( "exchangeDataBetweenNewTreesAndRerunClones()", "tree " << p._id << " did split: " << p.toString() );
      }
      for (auto& newWorker: p._hasSplit) {
        if (Node::getInstance().getRank(newWorker.first)!=tarch::mpi::Rank::getInstance().getRank()) {
          #if defined(Parallel)
          const int target    = Node::getInstance().getRank(newWorker.first);
          const int tag       = Node::getInstance().getGridDataExchangeTag( p._id, newWorker.first, false );
          const int outStack  = Node::getOutputStackNumberForSplitMergeDataExchange(newWorker.first);
          // @todo Debug
          logInfo(
            "exchangeDataBetweenNewTreesAndRerunClones()",
            "send stack " << outStack << " for tree " << newWorker.first << " from tree " << p._id << " on tag " << tag <<
            ". size=" << p._vertexStack[outStack].size()
          );
          assertion2(not p._vertexStack[outStack].empty(),p._vertexStack[outStack].toString(),p.toString());
          tarch::mpi::IntegerMessage message(p._vertexStack[outStack].size());
          message.send( target, tag, false, tarch::mpi::IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests );
          p._vertexStack[outStack].startSend(target,tag);
          p._vertexStack[outStack].finishSendOrReceive();
          #else
          assertionMsg(false, "should not be called");
          #endif
        }
      }
    }
  }

  for (auto& p: _spacetrees) {
	  if (p._spacetreeState!=peano4::grid::Spacetree::SpacetreeState::NewFromSplit) {
      for (auto& stack: p._vertexStack) {
        if (Node::isSplitMergeOutputStackNumber(stack.first.second)) {
    	    stack.second.clear();
    	  }
      }
	  }
  }
}


void peano4::parallel::SpacetreeSet::exchangeDataBetweenTrees() {
  std::vector< tarch::multicore::Task* > dataExchangeTasks;
  dataExchangeTasks.reserve( _spacetrees.size() );

  for (auto& p: _spacetrees) {
    if (p._spacetreeState!=peano4::grid::Spacetree::SpacetreeState::NewFromSplit) {
      dataExchangeTasks.push_back( new DataExchangeTask(
        p._id, *this
      ));
      logDebug( "exchangeDataBetweenTrees(TraversalObserver&)", "issue task to manage data transfer of tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
    }
  }
  logInfo( "exchangeDataBetweenTrees(TraversalObserver&)", "trigger " << dataExchangeTasks.size() << " concurrent data exchange tasks" );

  #if PeanoDebug>0
  for (auto& p: dataExchangeTasks) {
    p->run();
    delete p;
  }
  #else
  static int multitaskingRegion = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::exchangeDataBetweenTrees" );
  peano4::parallel::Tasks runTraversals(dataExchangeTasks,peano4::parallel::Tasks::TaskType::Task,multitaskingRegion);
  #endif
}


std::set<int> peano4::parallel::SpacetreeSet::getLocalTreesMergingWithWorkers() const {
  std::set<int> result;
  for (const auto& tree: _spacetrees) {
    if (not tree._joining.empty()) {
      result.insert( tree._id );
    }
  }
  return result;
}


void peano4::parallel::SpacetreeSet::createNewTrees() {
  for (const auto& tree: _spacetrees) {
    for (auto& p: tree._splitting) {
	    addSpacetree( tree._id, p.first );
    }
  }
}


void peano4::parallel::SpacetreeSet::traverse(peano4::grid::TraversalObserver& observer) {
  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    peano4::parallel::Node::getInstance().continueToRun();
  }

  logInfo( "traverse(TraversalObserver)", "start new grid sweep" );

  // Have to extract these first, as there might be more with this flag once
  // we've done our local traversals
  std::set<int>  mergingTrees     = getLocalTreesMergingWithWorkers();

  traverseNonMergingExistingTrees(observer);

  exchangeDataBetweenMergingTreesAndTraverseMaster(mergingTrees,observer);

  // @todo
  // Leert offensichtlich alles fusion Stacks. Sollte es aber net. Das ist nicht sauber!
  // Sobald das sauber ist, kann man die beiden exchange Dinger wieder vertauschen
  exchangeDataBetweenNewTreesAndRerunClones(observer);

//  evlt falsch

  mergeStatistics();

  createNewTrees();

  exchangeDataBetweenTrees();

  cleanUpTrees();
}


void peano4::parallel::SpacetreeSet::mergeStatistics() {
  for (auto&  from: _spacetrees) {
    if (
      from._id != _spacetrees.begin()->_id
	  and
	  from._spacetreeState!=peano4::grid::Spacetree::SpacetreeState::NewFromSplit
	) {
      merge(from._statistics, _spacetrees.begin()->_statistics );
    }
  }
}


void peano4::parallel::SpacetreeSet::cleanUpTrees() {
  for (auto p = _spacetrees.begin(); p!=_spacetrees.end(); ) {
  	if (
      p->getGridStatistics().getCoarseningHasBeenVetoed()
      and
      not p->mayJoinWithMaster()
    ) {
  	  if (p->_id!=0 and p->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running) {
        logInfo( "traverse(Observer)", "can't join tree " << p->_id << " with its master tree " << p->_masterId << ". Domain decomposition vetoes coarsening (would destroy MPI topology), but worker tree is not ready to be merged: " << p->toString());
  	  }
    }
  	else if (
      p->getGridStatistics().getCoarseningHasBeenVetoed()
      and
      p->mayJoinWithMaster()
      and
      getSpacetree(p->_masterId).mayJoinWithWorker()
    ) {
      logInfo( "traverse(Observer)", "trigger join of tree " << p->_id << " with its master tree " << p->_masterId << " to enable further grid erases");
      join(p->_id);
    }
    else if (
        // @todo muss ein "has spacetree"
      p->_masterId>=0
      and
      getSpacetree(p->_masterId).getGridStatistics().getCoarseningHasBeenVetoed()
      and
      p->mayJoinWithMaster()
      and
      getSpacetree(p->_masterId).mayJoinWithWorker()
    ) {
      logInfo( "traverse(Observer)", "trigger join of tree " << p->_id << " with its master tree " << p->_masterId << " to as master struggles to coarsen");
      join(p->_id);
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
  	p++;
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
  assertion(tree!=nullptr);
  assertion1( tree->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running, treeId);
  assertion1( tree->mayJoinWithMaster(), treeId );

  const int numberOfMaxJoinedCells = std::max(1,tree->getGridStatistics().getNumberOfLocalUnrefinedCells()/2);
  logDebug( "join(int)", "max=" << numberOfMaxJoinedCells );
  tree->joinWithMaster( numberOfMaxJoinedCells );
  const int fatherId = tree->_masterId;

  tree = nullptr;
  for (auto& p: _spacetrees) {
    if (p._id==fatherId) tree = &p;
  }
  assertion(tree!=nullptr);

  assertion2(
    tree->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running
    or
    tree->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::JoinTriggered,
    fatherId, tree->toString()
  );

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
