// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_SPACETREE_SET_H_
#define _PEANO4_PARALLEL_SPACETREE_SET_H_


#include "tarch/logging/Log.h"
#include "tarch/services/Service.h"
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
 * The spacetree set has to be a singleton, as it is reponsible to accept
 * requests for new trees from remote ranks. So there is only one set.
 *
 * @author Tobias Weinzierl
 */
class peano4::parallel::SpacetreeSet: public tarch::services::Service {
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

    tarch::multicore::BooleanSemaphore    _semaphore;

    peano4::grid::Spacetree& getSpacetree(int id);

    /**
     * Does not run through new trees for which no data might be available yet.
     * The reason is simple: When we split a tree, this tree pipes its data
     * (synchronously) into the new worker. In a shared memory environmment,
     * we thus have to run all 'running' (read non-splitting) trees first.
     */
    void traverseTrees(peano4::grid::TraversalObserver& observer);

    /**
     * Operation is idempotent, i.e. you can call it multiple times in a row.
     */
    void exchangeDataBetweenTrees();

    /**
     * exchangeDataBetweenTrees() only handles those data transfers which are logically
     * asynchronous..
     */
    void exchangeDataBetweenNewOrMergingTrees();

    /**
     * Adds a new spacetree to the set. The responsibility goes over to the
     * set. The operation clones the original spacetree handed in into a new
     * spacetree with the id id.
     *
     * <h2> Implementation </h2>
     *
     * Logically, the passed spacetree is const as we do not change it.
     * Technically, we may not make it const. We invoke MPI sends on its
     * stacks. Therefore, it is just a reference.
     */
    void addSpacetree( peano4::grid::Spacetree& originalSpacetree, int id );

    bool canJoinWorkerWithMaster( int workerId );

    /**
     * Join the tree into its master. You are not allowed to run this
     * routine unless the tree with treeId holds mayJoinWithMaster().
     */
    void join(int treeId);

    void cleanUpTrees();

    void mergeStatistics();

    SpacetreeSet();
    SpacetreeSet(const SpacetreeSet& ) = delete;
    SpacetreeSet& operator=(const SpacetreeSet& ) = delete;

  public:
    ~SpacetreeSet();

    static SpacetreeSet& getInstance();

    /**
     * We poll the tree management messages.
     *
     * If a new spacetree request drops in, we book the tree and send it back.
     */
    void receiveDanglingMessages() override;

    /**
     *
     */
    void init(
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

    bool move(int sourceTreeId, int targetRank);
};

#endif
