// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_SPACETREE_SET_H_
#define _PEANO4_PARALLEL_SPACETREE_SET_H_


#include "tarch/logging/Log.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/grid/TraversalObserver.h"
#include "tarch/multicore/Tasks.h"
#include "Tasks.h"


#include <list>
#include <map>


namespace peano4 {
  namespace parallel {
    class SpacetreeSet;
  }
}


/**
 * @author Tobias Weinzierl
 */
class peano4::parallel::SpacetreeSet {
  private:
	class TraverseTask: public tarch::multicore::Task {
	  private:
	    peano4::grid::Spacetree&          _spacetree;
	    peano4::grid::TraversalObserver&  _observer;
	  public:
	    TraverseTask( peano4::grid::Spacetree&  tree, peano4::grid::TraversalObserver&  observer );
	    bool run() override;
	    void prefetch() override;
	};

    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    void merge(
      const peano4::grid::GridStatistics&   from,
	  peano4::grid::GridStatistics&         to
    );

    std::list< peano4::grid::Spacetree >  _spacetrees;

    /**
     * Links from worker to masters. We use the ids from the
     * Node class, so we have unique numbers for rank+thread
     * combinations.
     */
    std::map<int,int>  _master;
  public:
    void addSpacetree( const peano4::grid::Spacetree& spacetree );

    /**
     * Invoke traverse on all spacetrees in parallel.
     */
    void traverse(peano4::grid::TraversalObserver& observer);

    /**
     * Return statistics object for primary spacetree.
     */
    peano4::grid::GridStatistics  getGridStatistics() const;
};

#endif
