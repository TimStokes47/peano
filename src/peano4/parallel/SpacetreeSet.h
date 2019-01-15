// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_SPACETREE_SET_H_
#define _PEANO4_PARALLEL_SPACETREE_SET_H_


#include "tarch/logging/Log.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/grid/TraversalObserver.h"
#include "tarch/multicore/Tasks.h"
#include "tarch/multicore/BooleanSemaphore.h"
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
	friend class peano4::grid::Spacetree;

	/**
	 * Each task triggers the traversal of one specific spacetree. After
	 * that, we might directly trigger the data exchanges. Yet, this is not a
	 * good idea as other tasks might linger in the background not have sent
	 * the data out yet. So we don't to anything here.
	 */
	class TraverseTask: public tarch::multicore::Task {
	  private:
	    peano4::grid::Spacetree&          _spacetree;
	    SpacetreeSet&                     _spacetreeSet;
	    peano4::grid::TraversalObserver&  _observer;
	  public:
	    TraverseTask( peano4::grid::Spacetree&  tree, SpacetreeSet& set, peano4::grid::TraversalObserver&  observer );

	    /**
	     * I create the copy of the observer, run the traversal on my local
	     * tree _spacetree and finally destroy the local observer copy.
	     */
	    bool run() override;

	    void prefetch() override;
	};

	class DataExchangeTask: public tarch::multicore::Task {
	  private:
	    peano4::grid::Spacetree&          _spacetree;
	    SpacetreeSet&                     _spacetreeSet;
	  public:
	    DataExchangeTask( peano4::grid::Spacetree&  tree, SpacetreeSet& set );

	    /**
	     * I create the copy of the observer, run the traversal on my local
	     * tree _spacetree and finally destroy the local observer copy.
	     */
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

    tarch::multicore::BooleanSemaphore  _semaphore;

    peano4::grid::Spacetree& getSpacetree(int id);

    void traverseTreeSet(peano4::grid::TraversalObserver& observer);
    void exchangeDataBetweenTrees();

    /**
     * Adds the spacetree to the set. The responsibility goes over to the
     * set. Usually, users do invoke this operation only once when they
     * pass the overall spacetree to the set. All the other, split off trees
     * then are added by the Peano4 core internally.
     *
     * As we may not copy spacetrees, the parameter has move semantics. So
     * if you have a spacetree, use std::move around your argument.
     */
    void addSpacetree( peano4::grid::Spacetree&& spacetree );

    bool canJoinWorkerWithMaster( int workerId );

    /**
     * Join the tree into its master. You are not allowed to run this
     * routine unless the tree with treeId holds mayJoinWithMaster().
     */
    void join(int treeId);

    void cleanUpTrees();
  public:
    SpacetreeSet(
      const tarch::la::Vector<Dimensions,double>&  offset,
      const tarch::la::Vector<Dimensions,double>&  width
    );

    /**
     * Invoke traverse on all spacetrees in parallel.
     */
    void traverse(peano4::grid::TraversalObserver& observer);

    /**
     * Return statistics object for primary spacetree.
     */
    peano4::grid::GridStatistics  getGridStatistics() const;

    /**
     * Does exist for debug reasons. Usually not used by codes.
     *
     * If you split a tree, please note that trees go through two states: split
     * triggered and splitting. In the latter case, you may split again once
     * more. Throughout the split-triggered, the tree does not physically exist
     * yet and you are thus not allowed to split it further.
     *
     * If the target tree shall be stored on the local node, then you
     * may wanna pass
     *
     * <pre>
      peano4::parallel::Node::getInstance().getRank(treeId)
       </pre>
     * as last argument. Splits on the local node allow Peano to exploit more
     * cores on the node.
     *
     */
    bool split(int treeId, int cells, int targetRank);

    /**
     *
     * @todo Should there be something alike a target rank?
     */
    bool move(int sourceTreeId, int targetRank);
};

#endif
