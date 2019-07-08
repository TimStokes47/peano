// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_NODE_H_
#define _PEANO4_PARALLEL_NODE_H_


#include "tarch/logging/Log.h"
#include "peano4/grid/GridVertex.h"
#include "tarch/multicore/BooleanSemaphore.h"


#include "TreeEntry.h"


#include <set>
#include <queue>
#include <map>


namespace peano4 {
  namespace parallel {
    class Node;
  }
}


/**
 * Node is Peano's abstraction for the hybrid of MPI/threads. It represents
 * one spacetree instance basically.
 *
 * @author Tobias Weinzierl
 */
class peano4::parallel::Node {
  public:
    static constexpr int        Terminate = -2;
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    tarch::multicore::BooleanSemaphore  _semaphore;

    /**
     * Value for _currentProgramStep.
     */
    static constexpr int        UndefProgramStep = -1;
    /**
     * We need one stack for outgoing data, one for incoming
     * (boundary) data. And then we need one in/out stack for
     * splits and joins.
     */
    static constexpr int        StacksPerCommunicationPartner = 4;

    int                         _currentProgramStep;

    int                         _rankOrchestrationTag;
    int                         _treeManagementTag;

    std::map< int, TreeEntry >  _treeEntries;

    int                         _maxTreesPerRank;

    /**
     * The base tag (smallest one) used for vertex data exchange. I use
     * some lazy initialisation, i.e. it is set to -1 a priori and then
     * set properly upon first usage. The reason is that I otherwise
     * don't give the user the opportunity to set the maximum number of
     * threads per rank.
     */
    int                         _gridVertexDataExchangeBaseTag;

    /**
     * The standard constructor assigns the attributes default values and
     * checks whether the program is compiled using the -DParallel option.
     * If this is not the case, a warning is logged.
     */
    Node();

    /**
     * The copy constructor is private.
     */
    Node( const Node& node ) = delete;

    /**
     * Peano maps grid instance threads + mpi ranks onto global IDs through
     * this routine. To identify a global stack number, please use this
     * function and add it to your local stack number.
     */
    int getId(int rank, int localTreeId) const;

    /**
     * @see getId(int,int)
     */
    int getLocalTreeId(int treeId) const;

    /**
     * The operation is not thread-safe as we call it only internally,
     * i.e. you are not supposed to call this function directly.
     *
     * @see getId(int,int)
     */
    void registerId(int id, int masterId);

  public:
    /**
     * I originally wanted to embed these guys into the singleton's
     * constructor. However, the singleton might not yet be up when
     * I run the (mpi) tests.
     */
    static void initMPIDatatypes();
    static void shutdownMPIDatatypes();

    /**
     * This operation returns the singleton instance. Before using this
     * instance, one has to call the init() operation on the instance returned.
     *
     * @return The singleton instance
     */
    static Node& getInstance();

    /**
     * The standard destructor calls MPI_Finalize().
     */
    virtual ~Node();

    /**
     * Is this the global master?
     */
    static bool isGlobalMaster(int treeId);

    /**
     * @return Number of registered trees on this rank
     */
    int getNumberOfRegisteredTrees() const;

    /**
     * This operation is not const as it does some internal bookkeeping. It
     * internally invokes  registerId() on the result.
     *
     * @param forTreeId If we run a split, this is the rank that wants to
     *          split. In general, this always is the parent/master of the
     *          new rank. So if you move a rank, you have to ensure that
     *          the master is preserved.
     *
     * @return -1 If there are no ids left anymore
     * @see getId(int,int)
     */
    int reserveId(int rank, int forTreeId);

    /**
     * You hand in a tree number and the node tells you on which rank such a
     * tree is hosted.
     * @see getId(int,int)
     */
    int getRank(int treeId) const;

    /**
     * Only the SpacetreeSet should call this operation.
     * @see getId(int,int)
     */
    void deregisterId(int id);

    /**
     * Hand in a spacetree id and get back the number that we should use to
     * send something to this tree.
     */
    static int getOutputStackNumberOfBoundaryExchange(int id);

    static int getInputStackNumberForSplitMergeDataExchange(int id);
    static int getOutputStackNumberForSplitMergeDataExchange(int id);

    /**
     * Counterpart of getOutputStackNumberOfBoundaryExchange(int)
     */
    static int getInputStackNumberOfBoundaryExchange(int id);

    /**
     * See getOutputStackNumberOfBoundaryExchange().
     */
    static bool isBoundaryExchangeOutputStackNumber(int number);
    static bool isBoundaryExchangeInputStackNumber(int number);

    /**
     * There are communication stacks and storage stacks. This is
     * a storage stack.
     */
    static bool isStorageStackNumber(int number);

    // @todo Die gibt es eigentlich nimmer
    static bool isSplitMergeOutputStackNumber(int number);
    static bool isSplitMergeInputStackNumber(int number);

    /**
     */
    static int getIdOfExchangeStackNumber(int number);

    /**
     * Has a tree forked once before? If so, we may not move it between ranks.
     * Otherwise, we destroy our master-worker topology.
     */
    bool hasTreeForkedBefore( int treeId );

    /**
     * We know for each tree hosted on one particular rank who its children
     * are. But we only have data on nodes that are local.
     */
    std::set< int > getChildren( int treeId );

    /**
     * You should call this operation only on the ranks >0 to find out whether
     * you should do more iteration/sweeps. The spacetree set internally
     * hijacks this operation also on rank 0 to trigger the send out of startup
     * messages. Therefore, the operation is not const.
     */
    bool continueToRun();

    /**
     * The user tells the set which program step to use, i.e. which adapter
     * to use for the next grid run or which step to run next. Should only
     * be called on global master.
     */
    void setNextProgramStep( int number );

    int getCurrentProgramStep() const;

    int getTreeManagementTag() const;

    /**
     * Set in initialisation (or otherwise left to concurrency).
     */
    int getMaximumNumberOfTreesPerRank() const;

    /**
     * The shutdown is invoked by peano4::shutdownParallelEnvironment().
     */
    void shutdown();

    /**
     * I use two tags per spacetree per rank: one for boundary data
     * (horizontal) and one for up-down and synchronous (forks) data
     * exchanges (vertical and fork/join). The operation allocates
     * tags lazily and thus is not const.
     *
     * The nice thing about Peano's new data management is that it is
     * stack-only. Furthermore, all stack data exchange is triggered via the
     * spacetree set, i.e. the order of the stacks is known externally, too.
     * So we effectively do not need that many tags even though we need
     * different tags per tree pair.
     */
    int getGridDataExchangeTag( int sendingTreeId, int receivingTreeId, bool boundaryDataExchange );
};

#endif
