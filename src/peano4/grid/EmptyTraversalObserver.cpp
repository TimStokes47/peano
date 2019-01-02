#include "EmptyTraversalObserver.h"
#include "GridControlEvent.h"


tarch::logging::Log  peano4::grid::EmptyTraversalObserver::_log( "peano4::grid::EmptyTraversalObserver" );


void peano4::grid::EmptyTraversalObserver::beginTraversal() {
  logInfo( "beginTraversal(...)", "begin traversal" );
}


void peano4::grid::EmptyTraversalObserver::endTraversal() {
  logInfo( "beginTraversal(...)", "end traversal" );
}


void peano4::grid::EmptyTraversalObserver::enterCell(
  const tarch::la::Vector<Dimensions,double>& x,
  const tarch::la::Vector<Dimensions,double>& h,
  bool                                        isRefined,
  int                                         spacetreeId
) {
  logTraceInWith2Arguments( "enterCell(...)", x, h );
  logTraceOut( "enterCell(...)" );
}



void peano4::grid::EmptyTraversalObserver::leaveCell(
  const tarch::la::Vector<Dimensions,double>& x,
  const tarch::la::Vector<Dimensions,double>& h,
  bool                                        isRefined,
  int                                         spacetreeId
) {
  logTraceInWith2Arguments( "leaveCell(...)", x, h );
  logTraceOut( "leaveCell(...)" );
}


peano4::grid::TraversalObserver*  peano4::grid::EmptyTraversalObserver::clone(int spacetreeId) {
  return new EmptyTraversalObserver();
}


std::vector< peano4::grid::GridControlEvent > peano4::grid::EmptyTraversalObserver::getGridControlEvents() {
  return std::vector< peano4::grid::GridControlEvent >();
}

