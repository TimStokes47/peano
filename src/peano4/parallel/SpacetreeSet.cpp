#include "SpacetreeSet.h"
#include "Node.h"


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


  // Merge local copies
  for (const auto& from: _spacetrees)
  for (auto& to: _spacetrees) {
    if (_master.count(from._id)>0 and _master[from._id]==to._id) {
      merge(from._statistics, to._statistics);
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
