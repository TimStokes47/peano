#include "MyObserver.h"

#include "InitData.h"
#include "CompositeMapping.h"
#include "PeanoFormatCellDataPlotter.h"

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

tarch::logging::Log  examples::delta::MyObserver::_log( "examples::delta::MyObserver" );


std::map< examples::delta::MyObserver::DataKey, examples::delta::MyObserver::CellDataContainer >   examples::delta::MyObserver::_cellData;


examples::delta::MyObserver::MyObserver():
  _spacetreeId(-1),
  _iterationCounter(-1),
  _mapping(nullptr) {
//  #if PeanoDebug>0
  CompositeMapping* mapping = new CompositeMapping();
  mapping->append( new InitData() );
  mapping->append( new PeanoFormatCellDataPlotter("marker",false));
  _mapping = mapping;
/*
  #else
  _mapping = new InitData();
  #endif
*/
}


examples::delta::MyObserver::MyObserver(int spacetreeId, int counter, Mapping* mapping):
  _spacetreeId(spacetreeId),
  _iterationCounter(counter),
  _mapping(mapping) {
}


examples::delta::MyObserver::~MyObserver() {
  if (_spacetreeId<0) {
	assertion( _mapping!=nullptr );
    delete _mapping;
    _mapping = nullptr;
  }
}


void examples::delta::MyObserver::beginTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
  assertion( _mapping!=nullptr );

  _iterationCounter++;
  _mapping->beginTraversal();

  int outCellStack = peano4::grid::PeanoCurve::CallStack;
  CellData data;
  #if PeanoDebug>=1
  data.x = x;
  data.h = h;
  #endif
  _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);
}


//
// @todo
//
// LOP: Wo ist Skeleton/Enclave Marker?
//      Wie identifizeren fuer parallelen Code lokale vs. andere Vertices, Faces und Zellen
//
//

void examples::delta::MyObserver::endTraversal(
  const tarch::la::Vector<Dimensions,double>&  x,
  const tarch::la::Vector<Dimensions,double>&  h
) {
  int cellStack = peano4::grid::PeanoCurve::CallStack;
  _cellData[ DataKey(_spacetreeId,cellStack) ].pop();

  #if PeanoDebug>4
  for( auto& p: _cellData) {
	logDebug( "endTraversal()", "cell stack (" << p.first.first << "," << p.first.second << "): " << p.second.size() << " entries" );
  }
  #endif
  assertion( _mapping!=nullptr );

  _mapping->endTraversal();
}


void examples::delta::MyObserver::enterCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  logTraceInWith1Argument("enterCell(...)",event.toString());

  // @todo
  // Vertex processing -> nop here
  // =============================
  // @todo
  // Faces processing -> nop here
  // =============================


  // Cell processing
  // ===============
  int inCellStack  = event.getCellData();
  int outCellStack = peano4::grid::PeanoCurve::CallStack;
  logDebug("enterCell(...)", "cell " << inCellStack << "->pos-" << outCellStack );
  if (inCellStack==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {
    CellData data;
	  // @todo coarse info
    _mapping->createCell(
      event.getX(),event.getH(),data,
      _cellData[ DataKey(_spacetreeId,outCellStack) ].top(1)
    );
    #if PeanoDebug>=1
    data.x = event.getX();
    data.h = event.getH();
    #endif
    assertionVectorNumericalEquals3(data.x,event.getX(),event.toString(),data.x,data.h);
    assertionVectorNumericalEquals3(data.h,event.getH(),event.toString(),data.x,data.h);
    _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);
  }
  else {
    CellData data;
    data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
    assertionVectorNumericalEquals3(data.x,event.getX(),event.toString(),data.x,data.h);
    assertionVectorNumericalEquals3(data.h,event.getH(),event.toString(),data.x,data.h);
    _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);
  }

  // @todo Es gibt noch kein inside/outside hier, oder? Was ist remote?
  peano4::datamanagement::CellMarker marker(event.getIsRefined(),false);
  assertionNumericalEquals2(
    _cellData[ DataKey(_spacetreeId,outCellStack) ].top(0).h(0)*3.0,
    _cellData[ DataKey(_spacetreeId,outCellStack) ].top(1).h(0),
    _cellData[ DataKey(_spacetreeId,outCellStack) ].top(0).h,
    _cellData[ DataKey(_spacetreeId,outCellStack) ].top(1).h
  );
  _mapping->touchCellFirstTime(
    event.getX(), event.getH(),
    _cellData[ DataKey(_spacetreeId,outCellStack) ].top(0),
    _cellData[ DataKey(_spacetreeId,outCellStack) ].top(1),
    marker
  );

  logTraceOutWith1Argument("enterCell(...)",event.toString());
}


void examples::delta::MyObserver::leaveCell(
  const peano4::grid::GridTraversalEvent&  event
) {
  logTraceInWith1Argument("leaveCell(...)",event.toString());

  // @todo Es gibt noch kein inside/outside hier, oder?
  // @todo Enclaves fehlen halt auch noch
  int inCellStack   = peano4::grid::PeanoCurve::CallStack;
  int outCellStack  = event.getCellData();

  peano4::datamanagement::CellMarker marker(event.getIsRefined(),false);
  _mapping->touchCellLastTime(
    event.getX(), event.getH(),
    _cellData[ DataKey(_spacetreeId,inCellStack) ].top(0),
    _cellData[ DataKey(_spacetreeId,inCellStack) ].top(1),
    marker
  );

  logDebug("leaveCell(...)", "cell " << inCellStack << "->" << outCellStack );
  CellData data = _cellData[ DataKey(_spacetreeId,inCellStack) ].pop();
  assertionVectorNumericalEquals3(data.x,event.getX(),data.x,data.h,event.toString());
  assertionVectorNumericalEquals3(data.h,event.getH(),data.x,data.h,event.toString());

  if (outCellStack==TraversalObserver::CreateOrDestroyPersistentGridEntity) {
	  // @todo update
    _mapping->destroyCell(
      event.getX(),event.getH(),data,
      _cellData[ DataKey(_spacetreeId,outCellStack) ].top(1)
    );
  }
  else {
    _cellData[ DataKey(_spacetreeId,outCellStack) ].push(data);
  }

  logTraceOutWith1Argument("leaveCell(...)",event.toString());
}


peano4::grid::TraversalObserver* examples::delta::MyObserver::clone(int spacetreeId) {
  return new MyObserver( spacetreeId, _iterationCounter, _mapping );
}


std::vector< peano4::grid::GridControlEvent > examples::delta::MyObserver::getGridControlEvents() {
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

