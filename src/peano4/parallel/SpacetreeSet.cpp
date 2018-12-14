#include "SpacetreeSet.h"
#include "Node.h"


#include "tarch/multicore/Lock.h"


tarch::logging::Log peano4::parallel::SpacetreeSet::_log( "peano4::parallel::SpacetreeSet" );


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


void peano4::parallel::SpacetreeSet::traverse(peano4::grid::TraversalObserver& observer) {
  //
  // Set up the traversal tasks and kick them off
  //
  std::vector< tarch::multicore::Task* > traverseTasks;
  traverseTasks.reserve( _spacetrees.size() );

  for (auto& p: _spacetrees) {
	traverseTasks.push_back( new TraverseTask(
      p, *this, observer
    ));
	logInfo( "traverse(TraversalObserver&)", "issue task to traverse tree " << p._id );
  }
  logInfo( "traverse(TraversalObserver&)", "spawn " << traverseTasks.size() << " concurrent traversal tasks" );

  static int multitaskingRegion = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::traverse" );
  peano4::parallel::Tasks runTraversals(traverseTasks,peano4::parallel::Tasks::TaskType::Task,multitaskingRegion);

  //
  // Merge local copies of the statistics
  //
  for (const auto& from: _spacetrees)
  for (auto& to: _spacetrees) {
    if (_master.count(from._id)>0 and _master[from._id]==to._id) {
      merge(from._statistics, to._statistics);
    }
  }


  // @todo Das sollte auch alles parallel gehen. In eigene Jobs auslagern
  //       Eigentlich koennte man hier mit Conflicts arbeiten

  //
  // Do the local boundary data exchange.
  //
  for (auto& sourceTree: _spacetrees) {
    for (auto& sourceStack: sourceTree._vertexStack) {
      if ( Node::getInstance().isBoundaryExchangeOutputStackNumber(sourceStack.first) ) {
        const int targetId = Node::getInstance().getIdOfBoundaryExchangeOutputStackNumber(sourceStack.first);

        // @todo Id might not be local
        peano4::grid::Spacetree& targetTree = getSpacetree( targetId );
        const int targetStack = Node::getInstance().getInputStackNumberOfBoundaryExchange(sourceTree._id);
        logInfo(
          "traverse(Observer)",
		  "map output stream " << sourceStack.first << " of tree " <<
		  sourceTree._id << " onto input stream " << targetStack <<
		  " of tree " << targetId
		);

        assertion4( sourceTree._id != targetId, sourceTree._id, targetId, sourceStack.first, targetStack);

        assertion( targetTree._vertexStack[targetStack].empty() );
        logInfo(
          "traverse(Observer)",
		  "copy/clone " << sourceTree._vertexStack[sourceStack.first].size() << " entries"
		);
        targetTree._vertexStack[ targetStack ].clone( sourceStack.second );

        #if PeanoDebug>0
        const int comparisonStackForTarget = Node::getInstance().getOutputStackNumberOfBoundaryExchange(sourceTree._id);
        assertion5(
          targetTree._vertexStack[ targetStack ].size() == targetTree._vertexStack[ comparisonStackForTarget ].size()
		  or
		  targetTree._vertexStack[ comparisonStackForTarget ].empty(),
          targetTree._vertexStack[ targetStack ].size(),
          targetTree._vertexStack[ comparisonStackForTarget ].size(),
		  targetStack, comparisonStackForTarget, targetTree._id
        );
        #endif

        sourceTree._vertexStack[sourceStack.first].clear();
      }
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


void peano4::parallel::SpacetreeSet::split(int treeId, int cells) {
  peano4::grid::Spacetree* tree = nullptr;
  for (auto& p: _spacetrees) {
	if (p._id==treeId) tree = &p;
  }
  if (tree==nullptr) {
	assertionMsg(false, "unknown tree Id");
  }
  assertion3(
    cells < tree->getGridStatistics().getNumberOfLocalUnrefinedCells(),
	cells, tree->getGridStatistics().getNumberOfLocalUnrefinedCells(), treeId
  );
  // @todo
  tree->split(cells);
}


peano4::grid::Spacetree&  peano4::parallel::SpacetreeSet::getSpacetree(int id) {
  for (auto& p: _spacetrees) {
	if (p._id==id) return p;
  }
  assertion2( false, "no spacetree found", id );
  _spacetrees.begin(); // just here to avoid warning
}
