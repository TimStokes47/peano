#include "MyObserver.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "peano4/grid/PeanoCurve.h"
#include "peano4/parallel/Node.h"


tarch::logging::Log  examples::integerdiffusionthroughfaces::MyObserver::_log( "examples::integerdiffusionthroughfaces::MyObserver" );


std::map< examples::integerdiffusionthroughfaces::MyObserver::DataKey, examples::integerdiffusionthroughfaces::MyObserver::FaceDataContainer >   examples::integerdiffusionthroughfaces::MyObserver::_faceData;
std::map< examples::integerdiffusionthroughfaces::MyObserver::DataKey, examples::integerdiffusionthroughfaces::MyObserver::CellDataContainer >   examples::integerdiffusionthroughfaces::MyObserver::_cellData;


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
  // @todo Level 8
  #if PeanoDebug>0
  for( auto& p: _cellData) {
	logDebug( "endTraversal()", "cell stack (" << p.first.first << "," << p.first.second << "): " << p.second.size() << " entries" );
  }
  #endif
}


void examples::integerdiffusionthroughfaces::MyObserver::enterCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  logTraceInWith1Argument("enterCell(...)",event.toString());

  int inCellStack  = event.getCellData();
  int outCellStack = peano4::grid::PeanoCurve::CallStack;
  logDebug("enterCell(...)", "cell " << inCellStack << "->" << outCellStack );
  CellData data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
  assertionVectorNumericalEquals4(data.x,event.getX(),data.value,event.toString(),data.x,data.h);
  assertionVectorNumericalEquals4(data.h,event.getH(),data.value,event.toString(),data.x,data.h);
  _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);

  FaceDataContainer::PushBlockVertexStackView faceView = _faceData[ DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pushBlock(Dimensions*2);
  for (int i=0; i<Dimensions*2; i++) {
    int inFaceStack  = event.getFaceDataFrom(i);
	int outFaceStack = event.getFaceDataTo(i);
    FaceData data = _faceData[ DataKey(_spacetreeId,inFaceStack) ].pop();
    assertion4( tarch::la::allGreaterEquals(data.x,event.getX()-event.getH()/2.0),data.x,data.h,event.toString(),event.getX()-event.getH()/2.0 );
    assertion4( tarch::la::allSmallerEquals(data.x,event.getX()+event.getH()/2.0),data.x,data.h,event.toString(),event.getX()+event.getH()/2.0 );
	logDebug("enterCell(...)", "face " << inFaceStack << "->pos-" << outFaceStack << ": " << data.x << "x" << data.h );
    faceView.set(outFaceStack,data);
  }

  logTraceOutWith1Argument("enterCell(...)",event.toString());
}


void examples::integerdiffusionthroughfaces::MyObserver::leaveCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  logTraceInWith1Argument("leaveCell(...)",event.toString());

  int inCellStack   = peano4::grid::PeanoCurve::CallStack;
  int outCellStack  = event.getCellData();
  logDebug("leaveCell(...)", "cell " << inCellStack << "->" << outCellStack );
  CellData data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
  assertionVectorNumericalEquals4(data.x,event.getX(),data.value,data.x,data.h,event.toString());
  assertionVectorNumericalEquals4(data.h,event.getH(),data.value,data.x,data.h,event.toString());
  _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);

  FaceDataContainer::PopBlockVertexStackView faceView = _faceData[ DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].popBlock(Dimensions*2);
  for (int i=0; i<Dimensions*2; i++) {
    int inFaceStack  = event.getFaceDataFrom(i);
	int outFaceStack = event.getFaceDataTo(i);
    FaceData data = faceView.get(inFaceStack);
    assertion3( tarch::la::allGreaterEquals(data.x,event.getX()-event.getH()/2.0),data.x,data.h,event.toString() );
    assertion3( tarch::la::allSmallerEquals(data.x,event.getX()+event.getH()/2.0),data.x,data.h,event.toString() );
	logDebug("leaveCell(...)", "face pos-" << inFaceStack << "->" << outFaceStack  << ": " << data.x << "x" << data.h);
    _faceData[ DataKey(_spacetreeId,outFaceStack) ].push(data);
  }

  logTraceOutWith1Argument("leaveCell(...)",event.toString());
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
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::createHangingVertexAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::destroyPersistentVertexAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::destroyHangingVertexAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          stackNumber
) {

}


void examples::integerdiffusionthroughfaces::MyObserver::createPersistentFaceAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int                                          stackNumber
) {
  logTraceInWith4Arguments( "createPersistentFaceAndPushOnStack(...)", x, h, normal, stackNumber );

  FaceData data;
  #if PeanoDebug>0
  data.x = x;
  data.h = h;
  #endif
  _mapping.createPersistentFace( x, h, normal, data.value );
  DataKey key(_spacetreeId,stackNumber);
  if ( _faceData.count(key)==0 ) {
    _faceData.insert( std::pair< DataKey, FaceDataContainer >(key, FaceDataContainer()) );
  }
  _faceData[key].push(data);

  logTraceOutWith5Arguments( "createPersistentFaceAndPushOnStack(...)", x, h, normal, stackNumber, data.value );
}


void examples::integerdiffusionthroughfaces::MyObserver::createHangingFaceAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int                                          stackNumber
) {
  logTraceInWith4Arguments( "createHangingFaceAndPushOnStack(...)", x, h, normal, stackNumber );

  FaceData data;
  #if PeanoDebug>0
  data.x = x;
  data.h = h;
  #endif
  _mapping.createHangingFace( x, h, normal, data.value );
  DataKey key(_spacetreeId,stackNumber);
  if ( _faceData.count(key)==0 ) {
    _faceData.insert( std::pair< DataKey, FaceDataContainer >(key, FaceDataContainer()) );
  }
  _faceData[key].push(data);

  logTraceOutWith5Arguments( "createHangingFaceAndPushOnStack(...)", x, h, normal, stackNumber, data.value );
}


void examples::integerdiffusionthroughfaces::MyObserver::destroyPersistentFaceAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int                                          stackNumber
) {
  logTraceInWith4Arguments( "destroyPersistentFaceAndPopFromStack(...)", x, h, normal, stackNumber );
  DataKey key(_spacetreeId,stackNumber);
  FaceData data = _faceData[key].pop();
  #if PeanoDebug>0
  data.x = x;
  data.h = h;
  #endif
  _mapping.destroyPersistentFace( x, h, normal, data.value );
  logTraceOutWith4Arguments( "destroyPersistentFaceAndPopFromStack(...)", x, h, normal, stackNumber );
}


void examples::integerdiffusionthroughfaces::MyObserver::destroyHangingFaceAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          normal,
  int                                          stackNumber
) {
  logTraceInWith4Arguments( "destroyHangingFaceAndPopFromStack(...)", x, h, normal, stackNumber );
  DataKey key(_spacetreeId,stackNumber);
  FaceData data = _faceData[key].pop();
  #if PeanoDebug>0
  data.x = x;
  data.h = h;
  #endif
  _mapping.destroyHangingFace( x, h, normal, data.value );
  logTraceOutWith4Arguments( "destroyHangingFaceAndPopFromStack(...)", x, h, normal, stackNumber );
}


void examples::integerdiffusionthroughfaces::MyObserver::createCellAndPushOnStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          stackNumber
) {
  logTraceInWith3Arguments( "createCellAndPushOnStack(...)", x, h, stackNumber );

  CellData data;
  #if PeanoDebug>0
  data.x = x;
  data.h = h;
  #endif
  _mapping.createCell( x, h, data.value );
  DataKey key(_spacetreeId,stackNumber);
  if ( _cellData.count(key)==0 ) {
    _cellData.insert( std::pair< DataKey, FaceDataContainer >(key, FaceDataContainer()) );
  }
  _cellData[key].push(data);

  logTraceOutWith4Arguments( "createCellAndPushOnStack(...)", x, h, stackNumber, data.value );
}


void examples::integerdiffusionthroughfaces::MyObserver::destroyCellAndPopFromStack(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h,
  int                                          stackNumber
) {
  logTraceInWith3Arguments( "destroyCellAndPopFromStack(...)", x, h, stackNumber );
  DataKey key(_spacetreeId,stackNumber);
  CellData data = _cellData[key].pop();
  #if PeanoDebug>0
  data.x = x;
  data.h = h;
  #endif
  _mapping.destroyCell( x, h, data.value );
  logTraceOutWith3Arguments( "destroyCellAndPopFromStack(...)", x, h, stackNumber );
}
