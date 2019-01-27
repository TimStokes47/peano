#include "MyObserver.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "peano4/grid/PeanoCurve.h"
#include "peano4/parallel/Node.h"


tarch::logging::Log  examples::integerdiffusionthroughfaces::MyObserver::_log( "examples::integerdiffusionthroughfaces::MyObserver" );


examples::integerdiffusionthroughfaces::MyObserver::MyObserver(int spacetreeId, int counter):
  _spacetreeId(spacetreeId),
  _iterationCounter(counter)
  {
}


void examples::integerdiffusionthroughfaces::MyObserver::beginTraversal() {
  _iterationCounter++;
}


//
// @todo
//
// LOP: Wo ist Skeleton/Enclave Marker?
//      Wie identifizeren fuer parallelen Code lokale vs. andere Vertices, Faces und Zellen
//
//

void examples::integerdiffusionthroughfaces::MyObserver::endTraversal() {
}


void examples::integerdiffusionthroughfaces::MyObserver::enterCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  int inCellStack  = event.getCellData();
  int outCellStack = peano4::grid::PeanoCurve::CallStack;

  logTraceInWith3Arguments("centerCell(...)",inCellStack,outCellStack,event.toString());
  Data data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
  logTraceOutWith3Arguments("centerCell(...)",inCellStack,outCellStack,event.toString());
}


void examples::integerdiffusionthroughfaces::MyObserver::leaveCell(
  const peano4::grid::GridTraversalEvent&  event
) {
}


peano4::grid::TraversalObserver* examples::integerdiffusionthroughfaces::MyObserver::clone(int spacetreeId) {
  return new MyObserver( spacetreeId, _iterationCounter );
}


std::vector< peano4::grid::GridControlEvent > examples::integerdiffusionthroughfaces::MyObserver::getGridControlEvents() {
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

void examples::integerdiffusionthroughfaces::MyObserver::createPersistentVertexAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::createHangingVertexAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::destroyPersistentVertexAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::destroyHangingVertexAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::createPersistentFaceAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          normal,
  int                                          stackNumber
) {
  logTraceInWith4Arguments( "createPersistentFaceAndPushOnStack(...)", x, level, normal, stackNumber );
  logTraceOutWith4Arguments( "createPersistentFaceAndPushOnStack(...)", x, level, normal, stackNumber );
}


void examples::integerdiffusionthroughfaces::MyObserver::createHangingFaceAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          normal,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::destroyPersistentFaceAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          normal,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::destroyHangingFaceAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          normal,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::createCellAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          stackNumber
) {
  logTraceInWith3Arguments( "createCellAndPushOnStack(...)", x, level, stackNumber );
  logTraceOutWith3Arguments( "createCellAndPushOnStack(...)", x, level, stackNumber );
}


void examples::integerdiffusionthroughfaces::MyObserver::destroyCellAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  int                                          level,
  int                                          stackNumber
) {
  logTraceInWith3Arguments( "destroyCellAndPopFromStack(...)", x, level, stackNumber );
  logTraceOutWith3Arguments( "destroyCellAndPopFromStack(...)", x, level, stackNumber );
}
