#include "../../examples/grid/MyObserver.h"

#include "peano4/grid/GridControlEvent.h"


//    TraversalVTKPlotter( const std::string& filename, int treeId=-1, int counter=0 );

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
/*
    if (
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getX(0)<0.4 and
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getX(1)<0.4 and
      #if PeanoDebug>0
	  fineGridStatesState.getLevel()<4 and
      #else
	  fineGridStatesState.getLevel()<6 and
      #endif
	  fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getState()==GridVertex::State::Unrefined
	  and
	  isVertexAdjacentToLocalSpacetree(fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],false,false)
    // und das killt ihn jetzt
      and
         _id ==1
	  and not hasStartedToCoarsen
	) {
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].setState( GridVertex::State::RefinementTriggered );
    }

    if (
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getX(0)<0.6 and
      fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getX(1)<0.6 and
	  fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getState()==GridVertex::State::Refined and
//				  not fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() and
	  isVertexAdjacentToLocalSpacetree(fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ],false,false)
    // und das killt ihn jetzt
    and
     _id ==1
  and fineGridStatesState.getLevel()==1
	) {
      static int iterationCounter = 0;
      iterationCounter++;
      if (iterationCounter>500) {
        fineGridVertices[ peano4::utils::dLinearised(vertexIndex) ].setState( GridVertex::State::EraseTriggered );
      }
      hasStartedToCoarsen = true;
    }


            	static bool hasStartedToCoarsen = false;
            // @todo Raus



*/
}


peano4::grid::TraversalObserver* applications4::grid::MyObserver::clone(int spacetreeId) {
  #if PeanoDebug>0
  if (_spacetreeId==-1) {
	updateMetaFile( spacetreeId );
  }

  return new MyObserver( spacetreeId, TraversalVTKPlotter::_counter );
  #else
  return new MyObserver( spacetreeId, 0 );
  #endif
}


std::vector< peano4::grid::GridControlEvent > applications4::grid::MyObserver::getGridControlEvents() {
  std::vector< peano4::grid::GridControlEvent >  controlEvents;
  if (_iterationCounter<8) {
    peano4::grid::GridControlEvent newEvent;
    newEvent.setRefinementControl( peano4::grid::GridControlEvent::RefinementControl::Refine );
    newEvent.setOffset( {0.0,0.0} );
    newEvent.setWidth( {0.5,0.5} );
    newEvent.setH( {0.02,0.02} );
    controlEvents.push_back(newEvent);
  }

  if (_iterationCounter>12) {
    peano4::grid::GridControlEvent newEvent;
	newEvent.setRefinementControl( peano4::grid::GridControlEvent::RefinementControl::Erase );
	newEvent.setOffset( {0.01,0.01} );
	newEvent.setWidth( {0.99,0.99} );
	newEvent.setH( {0.4,0.4} );
    controlEvents.push_back(newEvent);
  }

  return controlEvents;
}
