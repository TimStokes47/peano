#include "EmptyTraversalObserver.h"


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
  bool                                        isRefined
) {
  logInfo( "enterCell(...)", "x=" << x );
  logInfo( "enterCell(...)", "h=" << h );
}
