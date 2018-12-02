#include "SpacetreeSet.h"


tarch::logging::Log peano4::parallel::SpacetreeSet::_log( "peano4::parallel::SpacetreeSet" );


void peano4::parallel::SpacetreeSet::addSpacetree( const peano4::grid::Spacetree&  spacetree ) {
  _spacetrees.push_back( spacetree );
}


peano4::parallel::SpacetreeSet::TraverseTask::TraverseTask(
  peano4::grid::Spacetree&          tree,
  peano4::grid::TraversalObserver&  observer
):
  _spacetree(tree),
  _observer( observer ) {
}


bool peano4::parallel::SpacetreeSet::TraverseTask::run() {
  _spacetree.traverse( _observer );
  return false;
}


void peano4::parallel::SpacetreeSet::TraverseTask::prefetch() {
}


void peano4::parallel::SpacetreeSet::traverse(peano4::grid::TraversalObserver& observer) {
  std::vector< tarch::multicore::Task* > traverseTasks;
  traverseTasks.reserve( _spacetrees.size() );

  for (auto& p: _spacetrees) {
	traverseTasks.push_back( new TraverseTask(
      p, observer
    ));
  }

  static int multitaskingRegion = peano4::parallel::Tasks::getLocationIdentifier( "peano4::parallel::SpacetreeSet::traverse" );
  peano4::parallel::Tasks runTraversals(traverseTasks,peano4::parallel::Tasks::TaskType::Task,multitaskingRegion);
}
