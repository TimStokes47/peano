#include "../../examples/grid/MyObserver.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/parallel/Node.h"


applications4::grid::MyObserver::MyObserver(int spacetreeId, int counter):
  #if PeanoDebug>0
  TraversalVTKPlotter( "grid-construction", spacetreeId, counter ),
  #endif
  _iterationCounter(counter)
  {
}


void applications4::grid::MyObserver::beginTraversal() {
  #if PeanoDebug>0
  TraversalVTKPlotter::beginTraversal();
  #endif
  _iterationCounter++;
}


void applications4::grid::MyObserver::endTraversal() {
  #if PeanoDebug>0
  TraversalVTKPlotter::endTraversal();
  #endif
}


void applications4::grid::MyObserver::enterCell(
  const tarch::la::Vector<Dimensions,double>& x,
  const tarch::la::Vector<Dimensions,double>& h,
  bool                                        isRefined,
  int                                         spacetreeId
) {
  #if PeanoDebug>0
  TraversalVTKPlotter::enterCell(x,h,isRefined,spacetreeId);
  #endif
}


void applications4::grid::MyObserver::leaveCell(
  const tarch::la::Vector<Dimensions,double>& x,
  const tarch::la::Vector<Dimensions,double>& h,
  bool                                        isRefined,
  int                                         spacetreeId
) {
}


peano4::grid::TraversalObserver* applications4::grid::MyObserver::clone(int spacetreeId) {
  #if PeanoDebug>0
  MyObserver* result = new MyObserver( spacetreeId, TraversalVTKPlotter::_counter );

  if (_spacetreeId!=-1) {
	assertionMsg( false, "clone() should not be called for particular spacetree plotter" );
  }
  else {
    updateMetaFile(spacetreeId);
  }

  if (peano4::parallel::Node::isGlobalMaster(spacetreeId)) {
    startNewSnapshot(true);
  }

  return result;
  #else
  return new MyObserver( spacetreeId, 0 );
  #endif
}


std::vector< peano4::grid::GridControlEvent > applications4::grid::MyObserver::getGridControlEvents() {
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
