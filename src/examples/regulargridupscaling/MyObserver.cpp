#include "MyObserver.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/parallel/Node.h"
#include "peano4/grid/GridTraversalEvent.h"


tarch::logging::Log examples::regulargridupscaling::MyObserver::_log( "examples::regulargridupscaling::MyObserver" );


examples::regulargridupscaling::MyObserver::MyObserver(int spacetreeId, double h, int flopsPerCell):
  _h(h),
  _flopsPerCell(flopsPerCell),
  _accumulator(0.0) {
}


examples::regulargridupscaling::MyObserver::~MyObserver() {
}


void examples::regulargridupscaling::MyObserver::beginTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
  _accumulator = 0.0;
}


void examples::regulargridupscaling::MyObserver::endTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
  logDebug( "endTraversal", _accumulator );
}


void examples::regulargridupscaling::MyObserver::enterCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  if (
    event.getIsRefined()==false
    and
    event.getCellData() != TraversalObserver::NoData
  ) {
    for (int i=0; i<_flopsPerCell; i++) {
      _accumulator += 1.0;
    }
  }
}


void examples::regulargridupscaling::MyObserver::leaveCell(
  const peano4::grid::GridTraversalEvent&  event
) {
}


peano4::grid::TraversalObserver* examples::regulargridupscaling::MyObserver::clone(int spacetreeId) {
  return new MyObserver( spacetreeId, _h, _flopsPerCell );
}


std::vector< peano4::grid::GridControlEvent > examples::regulargridupscaling::MyObserver::getGridControlEvents() {
  std::vector< peano4::grid::GridControlEvent >  controlEvents;

  peano4::grid::GridControlEvent newEvent;
  newEvent.setRefinementControl( peano4::grid::GridControlEvent::RefinementControl::Refine );
  #if Dimensions==2
    newEvent.setOffset( {0.0,0.0} );
    newEvent.setWidth( {1.0,1.0} );
    newEvent.setH( {_h,_h} );
  #elif Dimensions==3
    newEvent.setOffset( {0.0,0.0,0.0} );
    newEvent.setWidth( {1.0,1.0,1.0} );
    newEvent.setH( {_h,_h,_h} );
  #endif
  controlEvents.push_back(newEvent);

  return controlEvents;
}
