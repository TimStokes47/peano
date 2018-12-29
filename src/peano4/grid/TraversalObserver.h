// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_TRAVERSAL_OBSERVER_H_
#define _PEANO4_GRID_TRAVERSAL_OBSERVER_H_


#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"


namespace peano4 {
  namespace grid {
    class TraversalObserver;
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

	/**
	 * I use the clone to create one observer object per traversal thread. So
	 * between different spacetrees of one spacetree set, there can be no race
	 * condition. Yet, the clone() itself could be called in parallel.
	 */
	virtual TraversalObserver* clone(int spacetreeId) = 0;
};

#endif
