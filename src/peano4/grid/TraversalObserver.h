// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_TRAVERSAL_OBSERVER_H_
#define _PEANO4_GRID_TRAVERSAL_OBSERVER_H_


#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"


#include <vector>


namespace peano4 {
  namespace grid {
    class TraversalObserver;
    class GridControlEvent;
  }
}


class peano4::grid::TraversalObserver {
  public:
	virtual void beginTraversal() = 0;
	virtual void endTraversal() = 0;

	virtual void enterCell(
      const tarch::la::Vector<Dimensions,double>& x,
	  const tarch::la::Vector<Dimensions,double>& h,
	  bool                                        isRefined,
	  int                                         spacetreeId
    ) = 0;


	virtual void leaveCell(
      const tarch::la::Vector<Dimensions,double>& x,
	  const tarch::la::Vector<Dimensions,double>& h,
	  bool                                        isRefined,
	  int                                         spacetreeId
    ) = 0;

	/**
	 * I use the clone to create one observer object per traversal thread. So
	 * between different spacetrees of one spacetree set, there can be no race
	 * condition. Yet, the clone() itself could be called in parallel.
	 */
	virtual TraversalObserver* clone(int spacetreeId) = 0;

	/**
	 * The tree traversal invokes this operation before beginIteration.
	 *
	 * <h2> Content </h2>
	 *
	 * Dynamic AMR is controlled via a sequence of grid control events. Each
	 * event spans a certain region and prescribes an h resolution over this
	 * region. Depending on the type of the event (erase or refine), the grid
	 * adopts. A simple snippet just creating a refined area in a square is
	 *
	 * <pre>
std::vector< peano4::grid::GridControlEvent > applications4::grid::MyObserver::getGridControlEvents() {
  std::vector< peano4::grid::GridControlEvent >  controlEvents;
  peano4::grid::GridControlEvent newEvent;
  newEvent.setRefinementControl( peano4::grid::GridControlEvent::RefinementControl::Refine );
  newEvent.setOffset( {0.0,0.0} );
  newEvent.setWidth( {0.5,0.5} );
  newEvent.setH( {0.05,0.05} );
  controlEvents.push_back(newEvent);
  return controlEvents;
}
	 </pre>
	 *
	 * The entries are logically ordered. The later the entry, the more
	 * important it is. So entry 2 overrules entry 1.
	 */
	virtual std::vector< GridControlEvent > getGridControlEvents() = 0;
};

#endif
