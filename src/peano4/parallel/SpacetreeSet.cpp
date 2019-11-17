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


peano4::parallel::SpacetreeSet::SpacetreeSet():
  _state( SpacetreeSetState::Waiting) {
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
  if (
    peano4::parallel::TreeManagementMessage::isMessageInQueue(peano4::parallel::Node::getInstance().getAsynchronousTreeManagementTagWhichDoesNotChangeASpacetreeState(),true)
  ) {
    peano4::parallel::TreeManagementMessage message;
    message.receive(MPI_ANY_SOURCE,peano4::parallel::Node::getInstance().getAsynchronousTreeManagementTagWhichDoesNotChangeASpacetreeState(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    switch ( message.getAction() ) {
      case peano4::parallel::TreeManagementMessage::Action::RequestNewRemoteTree:
        {
          int newSpacetreeId = peano4::parallel::Node::getInstance().reserveId(
            tarch::mpi::Rank::getInstance().getRank(),  // on current node
            message.getMasterSpacetreeId()
          );

          logInfo( "receiveDanglingMessages()", "reserved tree id " << newSpacetreeId << " for tree " << message.getMasterSpacetreeId() );

          message.setWorkerSpacetreeId( newSpacetreeId );
          message.setAction(TreeManagementMessage::Action::Acknowledgement);
          message.send(message.getSenderRank(),peano4::parallel::Node::getInstance().getBlockingTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
        }
        break;
      default:
    	assertionMsg( false, "should not be called" );
    }
  }

  if (
    _state==SpacetreeSetState::Waiting
    and
    peano4::parallel::TreeManagementMessage::isMessageInQueue(peano4::parallel::Node::getInstance().getAsynchronousTreeManagementTagWhichChangesASpacetreeState(),true)
  ) {
    peano4::parallel::TreeManagementMessage message;
    message.receive(MPI_ANY_SOURCE,peano4::parallel::Node::getInstance().getAsynchronousTreeManagementTagWhichChangesASpacetreeState(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    switch ( message.getAction() ) {
      case peano4::parallel::TreeManagementMessage::Action::CreateNewRemoteTree:
        {
          const int tag = peano4::parallel::Node::getInstance().getGridDataExchangeTag(message.getMasterSpacetreeId(),message.getWorkerSpacetreeId(),peano4::parallel::Node::ExchangeMode::ForkJoinData);
          // @todo Debug
          logInfo( "receiveDanglingMessages(...)", "wait for new state on tag " << tag );
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
          message.setAction(TreeManagementMessage::Action::Acknowledgement);
          message.send(message.getSenderRank(),peano4::parallel::Node::getInstance().getBlockingTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
        }
        break;
      case peano4::parallel::TreeManagementMessage::Action::Acknowledgement:
        assertionMsg( false, "should only be passed synchronously" );
        break;
      case peano4::parallel::TreeManagementMessage::Action::RemoveChildTreeFromBooksAsChildBecameEmpty:
        logInfo( "receiveDanglingMessages(...)", "learned that remote child tree " << message.getWorkerSpacetreeId() << " of local tree " << message.getMasterSpacetreeId() << " is degenerated thus had been removed" );
        getSpacetree( message.getMasterSpacetreeId() )._childrenIds.erase(message.getWorkerSpacetreeId());
        break;
      default:
    	assertionMsg( false, "should not be called" );
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
    message.send(targetRank,peano4::parallel::Node::getInstance().getAsynchronousTreeManagementTagWhichChangesASpacetreeState(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

    const int tag = peano4::parallel::Node::getInstance().getGridDataExchangeTag(masterId,newTreeId,peano4::parallel::Node::ExchangeMode::ForkJoinData);
    peano4::grid::AutomatonState state = _spacetrees.begin()->_root;

    logDebug( "addSpacetree(int,int)", "send state " << state.toString() << " to rank " << targetRank << " via tag " << tag );
    state.send(targetRank,tag,false,peano4::grid::AutomatonState::ExchangeMode::NonblockingWithPollingLoopOverTests);

    message.receive(targetRank,peano4::parallel::Node::getInstance().getBlockingTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
    assertion(message.getAction()==TreeManagementMessage::Action::Acknowledgement);
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


void peano4::parallel::SpacetreeSet::createObserverCloneIfRequired(peano4::grid::TraversalObserver& observer, int treeId) {
  if (
    _clonedObserver.count(treeId)==0 or _clonedObserver[treeId]==nullptr
  ) {
    _clonedObserver[treeId] = observer.clone(treeId);
  }
  assertion1( _clonedObserver.count(treeId)==1 and _clonedObserver[treeId]!=nullptr, treeId );
}


void peano4::parallel::SpacetreeSet::exchangeDataBetweenMergingTreesAndTraverseMaster(const std::set<int>& trees, peano4::grid::TraversalObserver& observer) {
  for (auto& masterTreeId: trees ) {
    peano4::grid::Spacetree& masterTree = getSpacetree(masterTreeId);

    for (auto workerTreeId: masterTree._joining) {
      logInfo( "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)", "tree " << workerTreeId << " is merging (partially) into tree " << masterTreeId );

      DataExchangeTask::exchangeAllVerticalDataExchangeStacks( peano4::grid::Spacetree::_vertexStack, masterTree._id, masterTree._masterId, masterTree._childrenIds, DataExchangeTask::VerticalDataExchangeMode::ReceiveJoinDataForRunOfMaster );
      DataExchangeTask::finishAllOutstandingSendsAndReceives( peano4::grid::Spacetree::_vertexStack, masterTree._id );

      assertion1( _clonedObserver.count(workerTreeId)==1 and _clonedObserver[workerTreeId]!=nullptr, workerTreeId );
    }
    logDebug( "exchangeDataBetweenMergingTreesAndTraverseMaster(TraversalObserver&)", "issue task to traverse tree " << masterTree._id << " in state " << peano4::grid::Spacetree::toString(masterTree._spacetreeState) );

    createObserverCloneIfRequired(observer, masterTree._id );
    masterTree.traverse( *_clonedObserver[masterTree._id ], true );
    // @todo Eigentlich Bloedsinn. Also wenn eh alles Stacks sind, dann kann ich es gleich selber machen
    _clonedObserver[masterTree._id]->exchangeStacksAfterGridSweep();

    #if PeanoDebug>0
    for (auto workerTreeId: masterTree._hasJoined) {
      const int masterTreeStack = peano4::parallel::Node::getInputStackNumberForVerticalDataExchange(workerTreeId);
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


peano4::parallel::SpacetreeSet::DataExchangeTask::DataExchangeTask( peano4::grid::Spacetree& spacetree, SpacetreeSet& set, peano4::grid::TraversalObserver&  observer  ):
  _spacetree( spacetree ),
  _spacetreeSet( set ),
  _observer( observer) {
}


bool peano4::parallel::SpacetreeSet::DataExchangeTask::run() {
  exchangeAllVerticalDataExchangeStacks( peano4::grid::Spacetree::_vertexStack, _spacetree._id, _spacetree._masterId, _spacetree._childrenIds, VerticalDataExchangeMode::Running );
  exchangeAllHorizontalDataExchangeStacks( peano4::grid::Spacetree::_vertexStack, _spacetree._id, true );
  exchangeAllPeriodicBoundaryDataStacks( peano4::grid::Spacetree::_vertexStack, _spacetree._id );
  finishAllOutstandingSendsAndReceives( peano4::grid::Spacetree::_vertexStack, _spacetree._id );

  // @todo Der Clone hier muss raus!
  _spacetreeSet.createObserverCloneIfRequired(_observer, _spacetree._id );
  _spacetreeSet._clonedObserver[ _spacetree._id ]->exchangeStacksAfterGridSweep();

  return false;
}


void peano4::parallel::SpacetreeSet::DataExchangeTask::prefetch() {
}


/*
Das Problem ist, dass die geforkten Trees one-by-one abgearbeitet werden. Wenn diese nun auch noch miteinander kommunizieren sollen,
dann geht das nicht weil der erste traversiert wird, dann austauschen will, aber evtl bereits Daten vom zweiten erwartet.
Man muss also Traversierung von Datenaustausch trennen oder, nohc besser, alles parallel machen a priori. Bin mir sicher dass
der zweite Weg der gute ist. Aber der funktioniert net seriell. Also muss man zwei Loops haben, so wie in den normalen Trees auch.
Man darf Traversierung nie mit Datenaustausch koppelnr
*/


void peano4::parallel::SpacetreeSet::exchangeDataBetweenExistingAndNewTreesAndRerunClones(
  peano4::grid::TraversalObserver&  observer
) {
  for (auto& p: _spacetrees) {
    if (p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::NewFromSplit) {
      logInfo( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "tree " << p._id << " is new. Initialise it through clone" );

      createObserverCloneIfRequired(observer, p._masterId );
      createObserverCloneIfRequired(observer, p._id );

      assertion1( _clonedObserver.count(p._id)==1 and _clonedObserver[p._id]!=nullptr, p._id );
      DataExchangeTask::exchangeAllVerticalDataExchangeStacks( peano4::grid::Spacetree::_vertexStack, p._id, p._masterId, p._childrenIds, DataExchangeTask::VerticalDataExchangeMode::PrepareDryRunForNewSpacetree);
      DataExchangeTask::finishAllOutstandingSendsAndReceives( peano4::grid::Spacetree::_vertexStack, p._id );
      logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "tree " << p._id << " and its master " << p._masterId << " exchanged all vertical data");
      p.traverse(*_clonedObserver[p._id],true);
      logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "tree " << p._id << " finished its dry run's traversal. Tree state=" << p.toString() );
      _clonedObserver[p._masterId]->exchangeStacksAfterGridSweep();
      logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "tree after dry run: " << p.toString() );
    }
    else if (p._spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running) {
      if (not p._hasSplit.empty()) {
        std::set<int> newChildren;
        for (auto k: p._hasSplit) {
          newChildren.insert(k.first);
        }
        logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "send out data for try runs to all new subtrees of " << p._id );
        DataExchangeTask::exchangeAllVerticalDataExchangeStacks( peano4::grid::Spacetree::_vertexStack, p._id, p._masterId, newChildren, DataExchangeTask::VerticalDataExchangeMode::SendOutDataForDryRunOfNewSpacetree);
        DataExchangeTask::finishAllOutstandingSendsAndReceives( peano4::grid::Spacetree::_vertexStack, p._id );
        logDebug( "exchangeDataBetweenExistingAndNewTreesAndRerunClones()", "sent out data for try runs to all new subtrees of " << p._id );
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
        p, *this, observer
      ));
      logDebug( "exchangeDataBetweenTrees(TraversalObserver&)", "issue task to manage data transfer of tree " << p._id << " in state " << peano4::grid::Spacetree::toString(p._spacetreeState) );
    }
  }
  logDebug( "exchangeDataBetweenTrees(TraversalObserver&)", "trigger " << dataExchangeTasks.size() << " concurrent data exchange tasks" );

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
  logDebug( "traverse(TraversalObserver)", "start new grid sweep" );

  _state = SpacetreeSetState::TraverseTreesAndExchangeData;

  // Have to extract these first, as there might be more with this flag once
  // we've done our local traversals
  std::set<int>  mergingTrees     = getLocalTreesMergingWithWorkers();

  traverseNonMergingExistingTrees(observer);

  exchangeDataBetweenMergingTreesAndTraverseMaster(mergingTrees,observer);

  // @todo
  // Leert offensichtlich alles fusion Stacks. Sollte es aber net. Das ist nicht sauber!
  // Sobald das sauber ist, kann man die beiden exchange Dinger wieder vertauschen
  exchangeDataBetweenExistingAndNewTreesAndRerunClones(observer);
  _state = SpacetreeSetState::Waiting;

  mergeStatistics();

  _state = SpacetreeSetState::TraverseTreesAndExchangeData;
  exchangeDataBetweenTrees(observer);
  _state = SpacetreeSetState::Waiting;

  cleanUpTrees();
  createNewTrees();

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
  	// @todo Geht halt net
/*
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
*/
  	// @todo Join
/*
    else if (
        // @todo muss ein "has spacetree"
      p->_masterId>=0
      and

	  der Master koennte auch net lokal sein -> Problem

      getSpacetree(p->_masterId).getGridStatistics().getCoarseningHasBeenVetoed()
      and
      p->mayJoinWithMaster()
      and
      getSpacetree(p->_masterId).mayJoinWithWorker()
    ) {
      logInfo( "traverse(Observer)", "trigger join of tree " << p->_id << " with its master tree " << p->_masterId << " to as master struggles to coarsen");
      join(p->_id);
    }
*/
  	// @todo Funktioniert auch net
/*
    else if (
      p->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running
      and
      p->getGridStatistics().getNumberOfLocalRefinedCells() + p->getGridStatistics().getNumberOfLocalUnrefinedCells() == 0
    )  {
      logInfo( "traverse(Observer)", "tree " << p->_id << " is degenerated as it does not hold any local cells. Remove" );
      Node::getInstance().deregisterId(p->_id);

      if ( Node::getInstance().getRank(p->_masterId)!=tarch::mpi::Rank::getInstance().getRank() ) {
        logInfo( "traverse(Observer)", "parent tree " << p->_masterId << " is local on this rank. Remove child reference" );
        #ifdef Parallel
        TreeManagementMessage message( p->_masterId, p->_id, TreeManagementMessage::Action::RemoveChildTreeFromBooksAsChildBecameEmpty );
        message.send( Node::getInstance().getRank(p->_masterId), Node::getInstance().getAsynchronousTreeManagementTag(), false, TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests );
        #else
        assertionMsg( false, "branch may not be entered" );
        #endif
      }
      else {
        // @todo Debug
        logInfo( "traverse(Observer)", "parent tree " << p->_masterId << " is local on this rank. Remove child reference" );
        getSpacetree( p->_masterId )._childrenIds.erase(p->_id);
      }

      p = _spacetrees.erase(p);
      p--;
    }
*/
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


bool peano4::parallel::SpacetreeSet::isLocalSpacetree(int treeId) const {
  for (auto& p: _spacetrees) {
    if (p._id==treeId) return true;
  }
  return false;
}


std::set<int> peano4::parallel::SpacetreeSet::getLocalSpacetrees() const {
  std::set<int> result;

  for (auto& p: _spacetrees) {
    result.insert( p._id );
  }

  return result;
}


bool peano4::parallel::SpacetreeSet::split(int treeId, int cells, int targetRank) {
  peano4::grid::Spacetree&  tree = getSpacetree( treeId );

  if ( tree.maySplit() ) {
    int newSpacetreeId = -1;

    if (tarch::mpi::Rank::getInstance().getRank()!=targetRank) {
      #ifdef Parallel
      logDebug( "split(int,int,int)", "request new tree on rank " << targetRank );
      peano4::parallel::TreeManagementMessage message(treeId,-1,peano4::parallel::TreeManagementMessage::Action::RequestNewRemoteTree);
      message.send(targetRank,peano4::parallel::Node::getInstance().getAsynchronousTreeManagementTagWhichDoesNotChangeASpacetreeState(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);

      message.receive(targetRank,peano4::parallel::Node::getInstance().getBlockingTreeManagementTag(),true,TreeManagementMessage::ExchangeMode::NonblockingWithPollingLoopOverTests);
      assertion(message.getAction()==TreeManagementMessage::Action::Acknowledgement);
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
