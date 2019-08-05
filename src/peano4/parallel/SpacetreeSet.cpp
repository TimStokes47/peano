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
  const tarch::la::Vector<Dimensions,double>&  width,
  const std::bitset<Dimensions>&               periodicBC
) {
  assertion4(
    (peano4::parallel::Node::getInstance().getNumberOfRegisteredTrees()==1 and tarch::mpi::Rank::getInstance().getRank()==0)
    or
    (peano4::parallel::Node::getInstance().getNumberOfRegisteredTrees()==0 and tarch::mpi::Rank::getInstance().getRank()!=0),
    peano4::parallel::Node::getInstance().getNumberOfRegisteredTrees(),
    offset, width, tarch::mpi::Rank::getInstance().getRank()
  );

  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    peano4::grid::Spacetree spacetree( offset, width, periodicBC );
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
  assertion1( _spacetreeSet._clonedObserver.count(_spacetree._id)==0 or _spacetreeSet._clonedObserver[_spacetree._id]==nullptr, _spacetree._id );
  _spacetreeSet._clonedObserver[_spacetree._id] = _observer.clone( _spacetree._id );
  _spacetree.traverse( *_spacetreeSet._clonedObserver[_spacetree._id], true );
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

      DataExchangeTask::exchangeStacksSynchronously( peano4::grid::Spacetree::_vertexStack, workerTreeId, masterTreeId );

      assertion1( _clonedObserver.count(workerTreeId)==1 and _clonedObserver[workerTreeId]!=nullptr, workerTreeId );
      _clonedObserver[workerTreeId]->exchangeStacksSynchronously(masterTreeId);
    }
    logDebug( "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)", "issue task to traverse tree " << masterTree._id << " in state " << peano4::grid::Spacetree::toString(masterTree._spacetreeState) );

    if (
      _clonedObserver.count(masterTree._id )==0 or _clonedObserver[masterTree._id]==nullptr
    ) {
      _clonedObserver[masterTree._id] = observer.clone( masterTree._id );
    }
    assertion1( _clonedObserver.count(masterTree._id )==1 and _clonedObserver[masterTree._id]!=nullptr, masterTree._id );
    masterTree.traverse( *_clonedObserver[masterTree._id ], true );

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


peano4::parallel::SpacetreeSet::DataExchangeTask::DataExchangeTask( int spacetreeId, SpacetreeSet& set, peano4::grid::TraversalObserver&  observer  ):
  _spacetreeId( spacetreeId ),
  _spacetreeSet( set ),
  _observer( observer) {
}


bool peano4::parallel::SpacetreeSet::DataExchangeTask::run() {
  exchangeStacksAsynchronously( peano4::grid::Spacetree::_vertexStack, _spacetreeId );

  if (
    _spacetreeSet._clonedObserver.count(_spacetreeId )==0
    or
    _spacetreeSet._clonedObserver[ _spacetreeId ] == nullptr
  ) {
    _spacetreeSet._clonedObserver[_spacetreeId] = _observer.clone(_spacetreeId);
  }
  assertion1( _spacetreeSet._clonedObserver.count(_spacetreeId )==1, _spacetreeId );
  _spacetreeSet._clonedObserver[ _spacetreeId ]->exchangeStacksAsynchronously();

  return false;
}


void peano4::parallel::SpacetreeSet::DataExchangeTask::prefetch() {
}


void peano4::parallel::SpacetreeSet::exchangeDataBetweenExistingAndNewTreesAndRerunClones(
  peano4::grid::TraversalObserver&  observer
) {
  for (auto& p: _spacetrees) {
    if (p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::NewFromSplit) {
      logInfo( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "tree " << p._id << " is new. Initialise it through clone" );

      DataExchangeTask::exchangeStacksSynchronously( peano4::grid::Spacetree::_vertexStack, p._masterId, p._id );

      if (
        _clonedObserver.count(p._masterId)==0 or _clonedObserver[p._masterId]==nullptr
      ) {
        _clonedObserver[p._masterId] = observer.clone(p._masterId);
      }
      assertion1( _clonedObserver.count(p._masterId)==1 and _clonedObserver[p._masterId]!=nullptr, p._masterId );
      _clonedObserver[p._masterId]->exchangeStacksSynchronously(p._id);

/*
      const int inStack  = Node::getInputStackNumberForSplitMergeDataExchange(p._masterId);

      if (Node::getInstance().getRank(p._masterId)==tarch::mpi::Rank::getInstance().getRank()) {
        logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "parent tree " << p._masterId << " is local" );
        const int outStack = Node::getOutputStackNumberForSplitMergeDataExchange(p._id);

        peano4::grid::Spacetree::_vertexStack[ peano4::grid::Spacetree::StackKey(p._id,inStack) ].clone( peano4::grid::Spacetree::_vertexStack[ peano4::grid::Spacetree::StackKey(p._masterId,outStack) ] );
        peano4::grid::Spacetree::_vertexStack[ peano4::grid::Spacetree::StackKey(p._id,inStack) ].reverse();

      }
      else {
        #if defined(Parallel)
        const int source = Node::getInstance().getRank(p._masterId);
        const int tag    = Node::getInstance().getGridDataExchangeTag( p._masterId, p._id, false );
        // @todo Debug
        logInfo(
          "exchangeDataBetweenExistingAndNewTreesAndRerunClones()",
          "receive into stack " << inStack << " for tree " << p._id << " from rank " << source <<
          " (tree " << p._masterId << ") on tag " << tag
        );
        tarch::mpi::IntegerMessage message;
        message.receive( source, tag, false, tarch::mpi::IntegerMessage::ExchangeMode::NonblockingWithPollingLoopOverTests );
        // @todo Debug
        logInfo(
          "exchangeDataBetweenExistingAndNewTreesAndRerunClones()",
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
*/

      logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "run tree " << p._id << " in dry mode (data exchange only): " << p.toString() );
      if (
        _clonedObserver.count(p._id)==0 or _clonedObserver[p._id]==nullptr
      ) {
        _clonedObserver[p._id] = observer.clone(p._id);
      }

      assertion1( _clonedObserver.count(p._id)==1 and _clonedObserver[p._id]!=nullptr, p._id );
      p.traverse(*_clonedObserver[p._id],true);

      logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "tree after dry run: " << p.toString() );
    }
    else if (p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running) {
      for (auto& newWorker: p._hasSplit) {
        if (Node::getInstance().getRank(newWorker.first)!=tarch::mpi::Rank::getInstance().getRank()) {
          #if defined(Parallel)
          const int target    = Node::getInstance().getRank(newWorker.first);
          const int tag       = Node::getInstance().getGridDataExchangeTag( p._id, newWorker.first, false );
          const int outStack  = Node::getOutputStackNumberForSplitMergeDataExchange(newWorker.first);
          // @todo Debug
          logInfo(
            "exchangeDataBetweenExistingAndNewTreesAndRerunClones()",
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


void peano4::parallel::SpacetreeSet::exchangeDataBetweenTrees(peano4::grid::TraversalObserver&  observer) {
  std::vector< tarch::multicore::Task* > dataExchangeTasks;
  dataExchangeTasks.reserve( _spacetrees.size() );

  for (auto& p: _spacetrees) {
    if (p._spacetreeState!=peano4::grid::Spacetree::SpacetreeState::NewFromSplit) {
      dataExchangeTasks.push_back( new DataExchangeTask(
        p._id, *this, observer
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


void peano4::parallel::SpacetreeSet::deleteClonedObservers() {
  for (auto& p: _clonedObserver) {
    if (p.second != nullptr) {
      delete p.second;
      p.second = nullptr;
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
  exchangeDataBetweenExistingAndNewTreesAndRerunClones(observer);

  mergeStatistics();

  createNewTrees();

  exchangeDataBetweenTrees(observer);

  cleanUpTrees();

  deleteClonedObservers();
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
