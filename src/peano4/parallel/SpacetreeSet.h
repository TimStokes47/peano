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

  public:
    /**
     * <h2> Rationale </h2>
     *
     * I originally planned to merge this task into the data traversal task. The
     * idea behind such a merger is that a data exchange right after the data
     * traversal increases the concurrency: While one tree already triggers its
     * data exchange, others still might run through the grid.
     *
     * However, this does not work straightforwardly: With my idea of stack-based
     * boundary exchange buffers, two communication partners have to have finished
     * their traversal before they can exchange data. Otherwise, one input buffer
     * of one grid (the one that is still running) might not yet be available when
     * the partner tree tries to send its data over.
     */
    class DataExchangeTask: public tarch::multicore::Task {
      private:
        peano4::grid::Spacetree&          _spacetree;
        SpacetreeSet&  _spacetreeSet;
        peano4::grid::TraversalObserver&  _observer;
      public:
        DataExchangeTask( peano4::grid::Spacetree& spacetree, SpacetreeSet& set, peano4::grid::TraversalObserver&  observer );

        /**
         * I create the copy of the observer, run the traversal on my local
         * tree _spacetree and finally destroy the local observer copy.
         */
        bool run() override;
        void prefetch() override;

        /**
         * This is the kind of routine that does boundary data exchange, i.e.
         * send outs in iteration n and receives (see counterpart routine), but
         * the received data is used only in traversal n+1. The whole exchange
         * thus asynchronous (logically).
         *
         * Typically, this routine is called by user observers to for their
         * user-defined data. We however also use it within the grid to trigger
         * the actual vertex data exchange. As these routines all are invoked
         * via tasks, this routine is typically invoked by multiple threads
         * concurrently.
         *
         * @param symmetricDataCardinality If two ranks A and B are adjacent to
         *   each other, and if A send n entries to B, then B received exactly
         *   n entries in turn.
         *
         * @see finishAllOutstandingSendsAndReceives();
         */
        template <class Container>
        static void exchangeAllHorizontalDataExchangeStacks( Container& stackContainer, int spacetreeId, bool symmetricDataCardinality );

        template <class Container>
        static void exchangeAllPeriodicBoundaryDataStacks( Container& stackContainer, int spacetreeId );

        enum class VerticalDataExchangeMode {
          Running,
          PrepareDryRunForNewSpacetree,
		  SendOutDataForDryRunOfNewSpacetree,
          ReceiveJoinDataForRunOfMaster
        };

  	    /**
         * Counterpart of exchangeStacksAsynchronously() which directly transfers
         * data within a traversal. We use it for synchronous data vertical data
         * exchange and for the transfer of data throughout splits and merges.
         *
         * @param childrenIds Depending on the context, this is either the children or
         *   the new children that are just about to be kicked off
         */
        template <class Container>
        static void exchangeAllVerticalDataExchangeStacks(
          Container& stackContainer,
          int spacetreeId, int parentId, const std::set<int>& childrenIds,
          VerticalDataExchangeMode mode
        );

        template <class Container>
        static void finishAllOutstandingSendsAndReceives( Container& stackContainer, int spacetreeId );
	  };

  private:
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

    /**
     * I create/clone one observer per local tree. This is an
     * on-the-fly process. At the end of the set traversal, we delete all of
     * clones. Originally, I did this delete right after the creation and the
     * subsequent traversal. However, we need the observer clone once more for
     * the data exchange. To avoid reclones, I store all clones in this map and
     * then delete them en bloc.
     *
     * @see deleteClonedObservers
     */
    std::map< int, peano4::grid::TraversalObserver* >    _clonedObserver;

    peano4::grid::Spacetree& getSpacetree(int id);

    /**
     *
     * @see split()
     * @see exchangeDataBetweenExistingAndNewTreesAndRerunClones()
     */
    void traverseNonMergingExistingTrees(peano4::grid::TraversalObserver& observer);

    /**
     * <h2> Forks </h2>
     *
     * If we are forking, we first have to establish the new (forked) trees
     * before we issue any data transer. Otherwise, the xxx
     *
     * <h2> Multithreading </h2>
     *
     * If we run this routine in debug mode, then I do all data exchange
     * sequentially. Otherwise, it is close to impossible to read the
     * assertion messages.
     *
     * @param newTrees This set holds all indices of local ids that are down as
     *          splitting before we issue the traversal preceding this function
     *          call.
     */
    void exchangeDataBetweenTrees(peano4::grid::TraversalObserver&  observer);

    /**
     * @see exchangeDataBetweenTrees() for details.
     * @see split() For a description of the overall split process.
     */
    void createNewTrees();

    /**
     * @see _clonedObserver
     */
    void deleteClonedObservers();

    void exchangeDataBetweenExistingAndNewTreesAndRerunClones(peano4::grid::TraversalObserver& observer);

    /**
     * Counterpart to exchangeDataBetweenExistingAndNewTreesAndRerunClones(), i.e.
     * counterpart to the dry run. We assume the merging worker has already
     * traversed its grid. Therefore, all data which has to be streamed is
     * already in the respective queues. We thus can copy them over and
     * issue the traversal of the master tree.
     */
    void exchangeDataBetweenMergingTreesAndTraverseMaster(const std::set<int>& trees, peano4::grid::TraversalObserver& observer);

    /**
     * Adds a new spacetree to the set. The responsibility goes over to the
     * set. The operation clones the original spacetree handed in into a new
     * spacetree with the id newTreeId.
     *
     * <h2> Local node </h2>
     *
     * If the new tree will be a local tree, we simply add a new tree object.
     * The Spacetree's constructor takes care of all the "cloning". Clone here
     * means a setup. We traverse this tree afterwards separately to stream all
     * data in.
     *
     * <h2> Distributed memory <h2>
     *
     * In a distributed memory environment, we have to break up the creation
     * into a set of sends forth and back through TreeManagementMessages.
     */
    void addSpacetree( int masterId, int newTreeId );

    bool canJoinWorkerWithMaster( int workerId );

    /**
     * Join the tree into its master. You are not allowed to run this
     * routine unless the tree with treeId holds mayJoinWithMaster().
     */
    void join(int treeId);

    /**
     * <h2> Merge process </h2>
     *
     * We make the worker first of all decide whether to merge or not. A
     * worker has the control/knowledge whether to join or not. When a rank
     * wants to join, we first of all run the workers. Then, all data is the
     * join buffers. After that, we sweep through the masters to take up the
     * data. So we may not merge both the worker and the master into their
     * workers at the same time. If we did so, we'd not be able to run all
     * the merging trees in parallel.
     */
    void cleanUpTrees();

    void mergeStatistics();

    /**
     * I need this routine for technical reasons: Prior to the sweep of trees,
     * I have to identify all of those trees which wanna merge with their
     * workers. This is an analysis I have to do before I actually traverse
     * any worker. Because of this traversal, more trees might denote their
     * workers as joining, so if I query the joining field after the traversal,
     * I intermix newly joining and old joining ranks.
     */
    std::set<int>                 getLocalTreesMergingWithWorkers() const;

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
     * @see Spacetree::Spacetree()
     */
    void init(
      const tarch::la::Vector<Dimensions,double>&  offset,
      const tarch::la::Vector<Dimensions,double>&  width,
      const std::bitset<Dimensions>&               periodicBC = 0
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

    /**
     * Codes hold one spacetree set per rank. With this routine, you can find
     * out whether a local set contains a particular id.
     */
    bool isLocalSpacetree(int treeId) const;
};


#include "SpacetreeSet.cpph"


#endif
