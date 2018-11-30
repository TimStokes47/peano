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
	  bool                                        isRefined
    ) = 0;
};

#endif
