#include "EmptyTraversalObserver.h"
#include "GridControlEvent.h"
#include "GridTraversalEvent.h"


tarch::logging::Log  peano4::grid::EmptyTraversalObserver::_log( "peano4::grid::EmptyTraversalObserver" );


void peano4::grid::EmptyTraversalObserver::createTemporaryCell(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
  logInfo( "createTemporaryCell(...)", "begin traversal" );
}


void peano4::grid::EmptyTraversalObserver::destroyTemporaryCell(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
  logInfo( "destroyTemporaryCell(...)", "end traversal" );
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

