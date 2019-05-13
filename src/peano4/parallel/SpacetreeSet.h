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
     *
     * @see split()
     */
    void traverseTrees(peano4::grid::TraversalObserver& observer);

    /**
     * exchangeDataBetweenTrees() only handles those data transfers which are logically
     * asynchronous..
     */
    void exchangeDataBetweenTrees();

    /**
     * We iterate over all local existing trees and trigger the swap of
     * all data.
     *
     * @see split() For a description of the overall split process.
     */
    void exchangeDataBetweenNewOrMergingTrees(const std::set<int>& newTrees);

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

    /**
     * Runs through all local trees and gets the ids of all the splitting
     * ranks from the local trees. We have to call this routine in-between
     * the iteration that adds this rank as split-triggered and the routine
     * that realises this split.
     */
    std::set<int> getLocalSplittingRanks() const;

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
     * If the target tree shall be stored on the local node, then you pass
     *
     * <pre>
      peano4::parallel::Node::getInstance().getRank(treeId)
       </pre>
     * as last argument. Splits on the local node allow Peano to exploit more
     * cores on the node.
     *
     * <h2> Split process </h2>
     *
     * If you split a tree, the original tree, i.e. the source of the data,
     * makes the new tree run through two states: split triggered and splitting.
     *
     * While an id is associated by one tree with split triggered, the tree
     * does not yet physically exist. The original (source) tree has the
     * complete ownership of all data, i.e. it does all refines and coarses and
     * also creates all events. However, it already enters the new, yet to be
     * created, tree's indices into all adjacency lists with are consequently
     * sent out to the other ranks. After the grid sweep, we mark the new
     * tree id as splitting.
     *
     * While an id is associated with splitting, we know that all the other
     * trees around start to send data to the new tree that is yet to be
     * created. The data we've received from our partners however has not been
     * aware yet of the new rank. So the source tree once again traverses its
     * tree and does all the admin, i.e. takes care of all refining an
     * coarsening. This time, it already starts to send out boundary data to
     * the newish tree. Furthermore, it pipes all read data into a splitting
     * stream.
     *
     * After the splitting tree traversal has terminated, we (i) establish
     * the new tree and mark it as new. See exchangeDataBetweenNewOrMergingTrees()
     * and createNewTrees()
     * for some details on this creation plus the data transfer as
     * sketched below. (ii) We immediately trigger a grid
     * sweep over the new tree. This grid sweep however does not receive any
     * standard data but reads out the splitting stream from above. It also
     * does not do any further grid modifications (refines or coarsens). But
     * the new tree does send out data along MPI boundaries. (iii) We mark
     * the new tree as a standard one.
     *
     *
     * Does not run through new trees for which no data might be available yet.
     * The reason is simple: When we split a tree, this tree pipes its data
     * (synchronously) into the new worker. In a shared memory environmment,
     * we thus have to run all 'running' (read non-splitting) trees first.
     */
    bool split(int treeId, int cells, int targetRank);
};

#endif
