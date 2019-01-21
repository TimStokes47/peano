#include "EmptyTraversalObserver.h"
#include "GridControlEvent.h"
#include "GridTraversalEvent.h"


tarch::logging::Log  peano4::grid::EmptyTraversalObserver::_log( "peano4::grid::EmptyTraversalObserver" );


void peano4::grid::EmptyTraversalObserver::beginTraversal() {
  logInfo( "beginTraversal(...)", "begin traversal" );
}


void peano4::grid::EmptyTraversalObserver::endTraversal() {
  logInfo( "beginTraversal(...)", "end traversal" );
}


void peano4::grid::EmptyTraversalObserver::enterCell(
  const GridTraversalEvent&  event
) {
  logTraceInWith1Argument( "enterCell(...)", event.toString() );
  logTraceOut( "enterCell(...)" );
}



void peano4::grid::EmptyTraversalObserver::leaveCell(
  const GridTraversalEvent&  event
) {
  logTraceInWith1Argument( "leaveCell(...)", event.toString() );
  logTraceOut( "leaveCell(...)" );
}


peano4::grid::TraversalObserver*  peano4::grid::EmptyTraversalObserver::clone(int spacetreeId) {
  return new EmptyTraversalObserver();
}


std::vector< peano4::grid::GridControlEvent > peano4::grid::EmptyTraversalObserver::getGridControlEvents() {
  return std::vector< peano4::grid::GridControlEvent >();
}

