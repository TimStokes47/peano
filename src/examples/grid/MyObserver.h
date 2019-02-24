// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAMPLES_GRID_MYOBSERVER_H_
#define _EXAMPLES_GRID_MYOBSERVER_H_


#include "peano4/grid/TraversalObserver.h"
#include "peano4/grid/TraversalVTKPlotter.h"


namespace examples {
  namespace grid {
    class MyObserver;
  }
}


class examples::grid::MyObserver: public
  #if PeanoDebug>0
  peano4::grid::TraversalVTKPlotter
  #else
  peano4::grid::TraversalObserver
  #endif
{
  private:
	int _iterationCounter;
  public:
	MyObserver(int spacetreeId = -1, int counter=0);
	virtual ~MyObserver();

	virtual void beginTraversal() override;
	virtual void endTraversal() override;

	virtual void enterCell(
      const peano4::grid::GridTraversalEvent&  event
    ) override;


	virtual void leaveCell(
	  const peano4::grid::GridTraversalEvent&  event
    ) override;

	/**
	 * I use the clone to create one observer object per traversal thread. So
	 * between different spacetrees of one spacetree set, there can be no race
	 * condition. Yet, the clone() itself could be called in parallel.
	 */
	virtual TraversalObserver* clone(int spacetreeId) override;

	virtual std::vector< peano4::grid::GridControlEvent > getGridControlEvents() override;
};

#endif
