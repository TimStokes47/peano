#include "MyObserver.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/parallel/Node.h"



int examples::regulargridupscaling::MyObserver::_iterationCounter(0);


examples::regulargridupscaling::MyObserver::MyObserver(int spacetreeId, int counter) {
  if (spacetreeId==0) {
	_iterationCounter++;
  }
}


examples::regulargridupscaling::MyObserver::~MyObserver() {
}


void examples::regulargridupscaling::MyObserver::beginTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
}


void examples::regulargridupscaling::MyObserver::endTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
}


void examples::regulargridupscaling::MyObserver::enterCell(
  const peano4::grid::GridTraversalEvent&  event
) {
}


void examples::regulargridupscaling::MyObserver::leaveCell(
  const peano4::grid::GridTraversalEvent&  event
) {
}


peano4::grid::TraversalObserver* examples::regulargridupscaling::MyObserver::clone(int spacetreeId) {
  return new MyObserver( spacetreeId, _iterationCounter );
}


std::vector< peano4::grid::GridControlEvent > examples::regulargridupscaling::MyObserver::getGridControlEvents() {
  std::vector< peano4::grid::GridControlEvent >  controlEvents;

  if (_iterationCounter<8) {
    peano4::grid::GridControlEvent newEvent;
    newEvent.setRefinementControl( peano4::grid::GridControlEvent::RefinementControl::Refine );
#if Dimensions==2
    newEvent.setOffset( {0.0,0.0} );
    newEvent.setWidth( {0.5,0.5} );
    newEvent.setH( {0.02,0.02} );
#elif Dimensions==3
    newEvent.setOffset( {0.0,0.0,0.0} );
    newEvent.setWidth( {0.5,0.5,0.5} );
    newEvent.setH( {0.02,0.02,0.02} );
#endif
    controlEvents.push_back(newEvent);
  }

  if (_iterationCounter>12) {
    peano4::grid::GridControlEvent newEvent;
	newEvent.setRefinementControl( peano4::grid::GridControlEvent::RefinementControl::Erase );
#if Dimensions==2
	newEvent.setOffset( {0.01,0.01} );
	newEvent.setWidth( {0.99,0.99} );
	newEvent.setH( {0.4,0.4} );
#elif Dimensions==3
	newEvent.setOffset( {0.01,0.01,0.01} );
	newEvent.setWidth( {0.99,0.99,0.99} );
	newEvent.setH( {0.4,0.4,0.4} );
#endif
    controlEvents.push_back(newEvent);
  }

  return controlEvents;
}


void examples::regulargridupscaling::MyObserver::exchangeStacksAfterGridSweep() {
}

