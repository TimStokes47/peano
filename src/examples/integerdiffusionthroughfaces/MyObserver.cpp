#include "MyObserver.h"

#include "CellDataPlotter.h"
#include "MyMapping.h"
#include "CompositeMapping.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "peano4/grid/PeanoCurve.h"
#include "peano4/parallel/Node.h"


/**
 * @todo docu
 */
//#pragma GCC diagnostic warning "-Wpragmas"
//#pragma GCC diagnostic ignored "-fpermissive"
//#pragma warning (disable: #num of the warning)

tarch::logging::Log  examples::integerdiffusionthroughfaces::MyObserver::_log( "examples::integerdiffusionthroughfaces::MyObserver" );


std::map< examples::integerdiffusionthroughfaces::MyObserver::DataKey, examples::integerdiffusionthroughfaces::MyObserver::FaceDataContainer >   examples::integerdiffusionthroughfaces::MyObserver::_faceData;
std::map< examples::integerdiffusionthroughfaces::MyObserver::DataKey, examples::integerdiffusionthroughfaces::MyObserver::CellDataContainer >   examples::integerdiffusionthroughfaces::MyObserver::_cellData;


examples::integerdiffusionthroughfaces::MyObserver::MyObserver():
  _spacetreeId(-1),
  _iterationCounter(-1),
  _mapping(nullptr) {
  #if PeanoDebug>0
  CompositeMapping* mapping = new CompositeMapping();
  mapping->append( new MyMapping() );
  mapping->append( new CellDataPlotter() );
  _mapping = mapping;
  #else
  _mapping = new MyMapping();
  #endif
}


examples::integerdiffusionthroughfaces::MyObserver::MyObserver(int spacetreeId, int counter, Mapping* mapping):
  _spacetreeId(spacetreeId),
  _iterationCounter(counter),
  _mapping(mapping) {
}


examples::integerdiffusionthroughfaces::MyObserver::~MyObserver() {
  if (_spacetreeId<0) {
	assertion( _mapping!=nullptr );
    delete _mapping;
    _mapping = nullptr;
  }
}


void examples::integerdiffusionthroughfaces::MyObserver::beginTraversal() {
  logInfo( "beginTraversal()", "here" );
  assertion( _mapping!=nullptr );
  _iterationCounter++;
  _mapping->beginTraversal();
}


//
// @todo
//
// LOP: Wo ist Skeleton/Enclave Marker?
//      Wie identifizeren fuer parallelen Code lokale vs. andere Vertices, Faces und Zellen
//
//

void examples::integerdiffusionthroughfaces::MyObserver::endTraversal() {
  logInfo( "endTraversal()", "here" );
  #if PeanoDebug>4
  for( auto& p: _cellData) {
	logDebug( "endTraversal()", "cell stack (" << p.first.first << "," << p.first.second << "): " << p.second.size() << " entries" );
  }
  #endif
  assertion( _mapping!=nullptr );
  _mapping->endTraversal();
}


void examples::integerdiffusionthroughfaces::MyObserver::enterCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  logTraceInWith1Argument("enterCell(...)",event.toString());

  // @todo
  // Vertex processing -> nop here
  // =============================


  MyMapping::Faces faces(event.getX(),event.getH());

  // Face processing
  // ===============
  // @todo If there were vertices, then we would pass in vertices via an enumerator
  FaceDataContainer::PushBlockVertexStackView faceView = _faceData[ DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pushBlock(Dimensions*2);
  for (int i=0; i<Dimensions*2; i++) {
    int inFaceStack  = event.getFaceDataFrom(i);
	int outFaceStack = event.getFaceDataTo(i);
    FaceData data;

	logDebug("enterCell(...)", "face " << inFaceStack << "->pos-" << outFaceStack );
    if (inFaceStack==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {
      _mapping->createPersistentFace(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      _mapping->touchFaceFirstTime(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      #if PeanoDebug>=1
      data.x = faces.x(outFaceStack);
      data.h = event.getH();
      #endif
    }
    else if (inFaceStack==peano4::grid::TraversalObserver::CreateOrDestroyHangingGridEntity) {
      _mapping->createHangingFace(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      _mapping->touchFaceFirstTime(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      #if PeanoDebug>=1
      data.x = faces.x(outFaceStack);
      data.h = event.getH();
      #endif
    }
    else {
      data = _faceData[ DataKey(_spacetreeId,inFaceStack) ].pop();
      assertionVectorNumericalEquals3(data.x,faces.x(outFaceStack),event.toString(),faces.toString(),data.x );
      assertionVectorNumericalEquals3(data.h,event.getH(),event.toString(),faces.toString(),data.x );
      if (peano4::grid::PeanoCurve::isInOutStack(inFaceStack)) {
        _mapping->touchFaceFirstTime(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      }
    }
    FaceData* pointer = faceView.set(outFaceStack,data);
    faces.setPointer(outFaceStack, pointer );
  }

  // Cell processing
  // ===============
  // @todo faces + enumerator
  // @todo coarser level + enumerator
  int inCellStack  = event.getCellData();
  int outCellStack = peano4::grid::PeanoCurve::CallStack;
  CellData data;
  logDebug("enterCell(...)", "cell " << inCellStack << "->pos-" << outCellStack << ": " << data.x << "x" << data.h );
  if (inCellStack==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {
    _mapping->createCell(event.getX(),event.getH(),data,faces);
    #if PeanoDebug>=1
    data.x = event.getX();
    data.h = event.getH();
    #endif
  }
  else {
    data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
  }
  assertionVectorNumericalEquals4(data.x,event.getX(),data.value,event.toString(),data.x,data.h);
  assertionVectorNumericalEquals4(data.h,event.getH(),data.value,event.toString(),data.x,data.h);

  // @todo Es gibt noch kein inside/outside hier, oder?
  // @todo Enclaves fehlen halt auch noch

  // @todo Ich will pointer durchreichen, aber nix auf dem Call-Stack
  _mapping->touchCellFirstTime( event.getX(), event.getH(), data, faces );
  _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);

  logTraceOutWith1Argument("enterCell(...)",event.toString());
}


void examples::integerdiffusionthroughfaces::MyObserver::leaveCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  logTraceInWith1Argument("leaveCell(...)",event.toString());

  // @todo Es gibt noch kein inside/outside hier, oder?
  // @todo Enclaves fehlen halt auch noch

  int inCellStack   = peano4::grid::PeanoCurve::CallStack;
  int outCellStack  = event.getCellData();
  logDebug("leaveCell(...)", "cell " << inCellStack << "->" << outCellStack );
  CellData data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
  assertionVectorNumericalEquals4(data.x,event.getX(),data.value,data.x,data.h,event.toString());
  assertionVectorNumericalEquals4(data.h,event.getH(),data.value,data.x,data.h,event.toString());

  // @todo Muss ich mir erst ueberlegen
  MyMapping::Faces faces(event.getX(),event.getH());

  // faces ist halt ueberhaupt noch net initialisiert hier!

  // @todo Ich will pointer durchreichen, aber nix auf dem Call-Stack -> moves directly
  _mapping->touchCellLastTime( event.getX(), event.getH(), data, faces );

  if (outCellStack==TraversalObserver::CreateOrDestroyPersistentGridEntity) {
    _mapping->destroyCell(event.getX(),event.getH(),data,faces);
  }
  else {
    _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);
  }

  FaceDataContainer::PopBlockVertexStackView faceView = _faceData[ DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].popBlock(Dimensions*2);
  for (int i=0; i<Dimensions*2; i++) {
    int inFaceStack  = event.getFaceDataFrom(i);
	int outFaceStack = event.getFaceDataTo(i);

	FaceData data = faceView.get(inFaceStack);
	logDebug("leaveCell(...)", "face pos-" << inFaceStack << "->" << outFaceStack << ": " << data.x << "x" << data.h << "x" << data.value );

    tarch::la::Vector<Dimensions,double> x = event.getX();
    int normal = inFaceStack % Dimensions;
    x(normal) += inFaceStack >= Dimensions ? event.getH(normal)/2.0 : -event.getH(normal)/2.0;

    assertionVectorNumericalEquals4(data.x,x,event.toString(),normal,data.x,x );
    assertionVectorNumericalEquals4(data.h,event.getH(),event.toString(),normal,data.x,x );

    if (outFaceStack==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {
      _mapping->touchFaceLastTime(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      _mapping->destroyPersistentFace(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
    }
    else if (outFaceStack==peano4::grid::TraversalObserver::CreateOrDestroyHangingGridEntity) {
      _mapping->touchFaceLastTime(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      _mapping->destroyHangingFace(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
    }
    else {
      if (peano4::grid::PeanoCurve::isInOutStack(outFaceStack)) {
        _mapping->touchFaceLastTime(faces.x(outFaceStack),event.getH(),faces.normal(outFaceStack),data);
      }
      _faceData[ DataKey(_spacetreeId,outFaceStack) ].push(data);
    }
  }

  logTraceOutWith1Argument("leaveCell(...)",event.toString());
}


peano4::grid::TraversalObserver* examples::integerdiffusionthroughfaces::MyObserver::clone(int spacetreeId) {
  return new MyObserver( spacetreeId, _iterationCounter, _mapping );
}


std::vector< peano4::grid::GridControlEvent > examples::integerdiffusionthroughfaces::MyObserver::getGridControlEvents() {
	// @todoo Das sollte natuerlich auch in mapping raus und muss dann dort u.U. gemerged werden
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

  // @todo rein
/*
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
*/

  return controlEvents;
}

