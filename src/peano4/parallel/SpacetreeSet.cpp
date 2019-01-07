#include "SpacetreeSet.h"
#include "Node.h"


#include "tarch/multicore/Lock.h"


tarch::logging::Log peano4::parallel::SpacetreeSet::_log( "peano4::parallel::SpacetreeSet" );


peano4::parallel::SpacetreeSet::SpacetreeSet(
  const tarch::la::Vector<Dimensions,double>&  offset,
  const tarch::la::Vector<Dimensions,double>&  width
) {
  peano4::grid::Spacetree spacetree( offset, width );

  addSpacetree( std::move(spacetree) );
}


void peano4::parallel::SpacetreeSet::addSpacetree( peano4::grid::Spacetree&&  spacetree ) {
  tarch::multicore::Lock lock( _semaphore );

  _spacetrees.push_back( std::move(spacetree) );
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
      // @todo Der ging schief
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

  //
  // Cleanup spacetrees
  //
  for (auto p = _spacetrees.begin(); p!=_spacetrees.end(); ) {
	if (
	  p->_splitTriggered.empty()
	  and
	  p->_splitting.empty()
	  and
	  p->_joinTriggered==peano4::grid::Spacetree::NoJoin
	  and
	  p->_joining==peano4::grid::Spacetree::NoJoin
	  and
	  p->_spacetreeState==peano4::grid::Spacetree::SpacetreeState::Running
	)  {
      logDebug( "traverse(Observer)", "tree " << p->_id << "'s statistics: " << p->toString() );
      if ( p->getGridStatistics().getNumberOfLocalRefinedCells() + p->getGridStatistics().getNumberOfLocalUnrefinedCells() == 0 ) {
        logInfo( "traverse(Observer)", "tree " << p->_id << " is degenerated as it does not hold any local cells. Remove" );
        Node::getInstance().deregisterId(p->_id);
        p = _spacetrees.erase(p);
        p--;
	  }
	  else if (
        p->mayJoinWithMaster()
      ) {
        logInfo( "traverse(Observer)", "trigger join of tree " << p->_id << " with its master tree " << p->_masterId );
        join(p->_id);
	  }
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

  @todo Das koennte aber schief gehen

  tree->joinWithWorker(treeId);
}


bool peano4::parallel::SpacetreeSet::split(int treeId, int cells) {
  peano4::grid::Spacetree&  tree = getSpacetree( treeId );

  // @todo Updpate

  logWarning( "move(int,int)", "have to check whether there are new ids available; introduce upper limit on trees per rank; parallel Node knows" );

  // @todo Das koennte auch -1 sein und dann waere Split nicht erfolgreich
  const int newSpacetreeId = peano4::parallel::Node::getInstance().getNextFreeLocalId();

  tree.split(newSpacetreeId, cells);
  peano4::parallel::Node::getInstance().registerId( newSpacetreeId );

  logInfo( "split(int,int)", "trigger split of tree " << treeId << " into tree " << newSpacetreeId << " with " << cells << " fine grid cells" );
  return true;
}


peano4::grid::Spacetree&  peano4::parallel::SpacetreeSet::getSpacetree(int id) {
  for (auto& p: _spacetrees) {
	if (p._id==id) return p;
  }
  assertion2( false, "no spacetree found", id );
  _spacetrees.begin(); // just here to avoid warning
}


bool peano4::parallel::SpacetreeSet::move(int sourceTreeId, int targetTreeId) {
  for (auto& p: _spacetrees) {
	assertion2(p._id!=targetTreeId,sourceTreeId,targetTreeId);
  }

  logWarning( "move(int,int)", "have to check whether there are kids; parallel Node knows" );

  // @todo Muss jetzt noch unterstuetzt werden

  peano4::grid::Spacetree&  sourceTree = getSpacetree( sourceTreeId );

  if (sourceTree.mayMove()) {
    sourceTree.split(targetTreeId, std::numeric_limits<int>::max());
    // @todo Das muessen wir auch noch anpassen, denn wir wollen ja nur den Rank angeben
    peano4::parallel::Node::getInstance().registerId( targetTreeId );
    return true;
  }
  else return false;
}
