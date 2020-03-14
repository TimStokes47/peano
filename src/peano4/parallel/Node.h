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

    namespace tests {
      class NodeTest;
    }
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
    static constexpr int        MaxSpacetreesPerRank = 64;
  private:
    friend class peano4::parallel::tests::NodeTest;

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
     * splits and joins. But this nomenclature is slightly wrong.
     * We should rather speak of horizontal and vertical data.
     */
    static constexpr int        StacksPerCommunicationPartner = 6;

    /**
     * The MPI standard specifies that the tag upper bound must be at least 32767.
     */
    static constexpr int ReservedMPITagsForDataExchange = MaxSpacetreesPerRank * MaxSpacetreesPerRank * StacksPerCommunicationPartner;

    int                         _currentProgramStep;

    int                         _rankOrchestrationTag;
    int                         _blockingTreeManagementTag;
    int                         _asynchronousTreeManagementTagWhichChangesSpacetreeState;
    int                         _asynchronousTreeManagementTagWhichDoesNotChangeASpacetreeState;
    /**
     * We do actually reserve ReservedMPITagsForDataExchange tags in one rush,
     * but this one is the smallest one
     */
    int                         _dataExchangeBaseTag;

    std::map< int, TreeEntry >  _treeEntries;

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
     * The operation is not thread-safe as we call it only internally,
     * i.e. you are not supposed to call this function directly.
     *
     * @see getId(int,int)
     */
    void registerId(int id, int masterId);

    /**
     * We enumerate again in the way we always enumerate, i.e. first the face
     * with the x axis as normal, then the one with the y axis, ... Always
     * those guys running through the left bottom vertex. And after these d
     * faces, we enumerate all the opposite faces on the cube.
     */
    static std::bitset<2*Dimensions> getPeriodicBoundaryNumber(const tarch::la::Vector<TwoPowerD,int>& flags);
  public:
    /**
     * @see getId(int,int)
     */
    int getLocalTreeId(int treeId) const;
    int getGlobalTreeId(int treeId) const;

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
     * @return -1 If there are no ids left anymore. In principle, we do not
     *            limit the number of threads at all. However, we have to
     *            work with a limited number of tags in MPI, so we put a
     *            cap on the number of trees per node.
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
    static int getOutputStackNumberOfHorizontalDataExchange(int id);

    /**
     * Get the input stack where a tree writes all of its vertical
     * data to/from when it exchanges information with id. Such vertical
     * data is multiscale information or split/join data.
     */
    static int getInputStackNumberForVerticalDataExchange(int id);
    static int getOutputStackNumberForVerticalDataExchange(int id);

    static int getInputStackNumberForForkJoinDataExchange(int id);
    static int getOutputStackNumberForForkJoinDataExchange(int id);

    /**
     * Counterpart of getOutputStackNumberOfBoundaryExchange(int)
     */
    static int getInputStackNumberOfHorizontalDataExchange(int id);

    /**
     * A periodic boundary stack is basically a stack (an integer), but I do
     * augment it by a bitset which identifies which symmetry axis belong to
     * this stack number. So if we return (23,010), then data goes to stack 23
     * and it is a stack that does realise periodic boundary conditions along
     * the y axis.
     *
     * The second argument (as sketched above) is of type std::bitset<Dimensions>.
     * Unfortunately, bitsets do not work properly within the pair type as they
     * seem not to define the comparison operator properly. So I use an int
     * here and wrap it into a bitset whenever necessary.
     */
    typedef std::pair<int,int> PeriodicBoundaryStackIdentifier;

    /**
     * Analyses whether/which periodic boundary data exchange is involved. The
     * routine however does not know/take into account that indeed only tree 0
     * should do periodic boundary data exchange. It is up to the calling
     * routine to ensure that stuff is only done on tree 0.
     */
    static std::set< PeriodicBoundaryStackIdentifier >  getInputStacksForPeriodicBoundaryExchange(const tarch::la::Vector<TwoPowerD,int>& flags);
    static std::set< PeriodicBoundaryStackIdentifier >  getOutputStacksForPeriodicBoundaryExchange(const tarch::la::Vector<TwoPowerD,int>& flags);

    /**
     * See getOutputStackNumberOfBoundaryExchange().
     */
    static bool isHorizontalDataExchangeOutputStackNumber(int number);
    static bool isHorizontalDataExchangeInputStackNumber(int number);

    static bool isVerticalDataExchangeOutputStackNumber(int number);
    static bool isVerticalDataExchangeInputStackNumber(int number);

    static bool isForkJoinDataExchangeOutputStackNumber(int number);
    static bool isForkJoinDataExchangeInputStackNumber(int number);

    static bool isPeriodicBoundaryExchangeOutputStackNumber(int number);
    static int  getPeriodicBoundaryExchangeInputStackNumberForOutputStack(int outputStackNumber);


    /**
     * There are communication stacks and storage stacks. This is
     * a storage stack.
     */
    static bool isStorageStackNumber(int number);

    /**
     * Gives you back the id of a communication partner, i.e. a spacetree
     * number, which corresponds to stack number. This works for all kinds
     * of stacks.
     */
    static int getTreeNumberTiedToExchangeStackNumber(int number);

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

    /**
     * This one has to be used if you wanna send out a message to another rank
     * which may not be grabbed by receiveDanglingMessages() in-between. This
     * is typical an answer to a request.
     */
    int getBlockingTreeManagementTag() const;

    /**
     * Used to invoke an asynchronous service on another rank.
     *
     * @see getAsynchronousTreeManagementTagWhichDoesNotChangeASpacetreeState()
     */
    int getAsynchronousTreeManagementTagWhichChangesASpacetreeState() const;

    /**
     * An example for such a message is TreeManagementMessage::RequestNewRemoteTree.
     * This routine only reserves an id but does not alter the actual spacetrees.
     * Consequently, we can always serve it even if we are right in the processing/
     * traversal of trees and consequently may not alter tree and stack entries.
     * Other routines such as those inserting new trees can only be handled at certain
     * points of the algorithm.
     */
    int getAsynchronousTreeManagementTagWhichDoesNotChangeASpacetreeState() const;

    /**
     * The shutdown is invoked by peano4::shutdownParallelEnvironment().
     *
     * If we run it on the global master, we set the program state to terminate
     * and distribute this message. For this, we hijack the routine continueToRun().
     * This is really a hack. We in particular have to be careful and may not
     * call the continue operation on the other ranks. They typically are stuck
     * in a continue while loop in their main routine and just wait for the
     * global master to send out this terminate command.
     */
    void shutdown();

    enum class ExchangeMode {
      HorizontalData,
      ForkJoinData,
	  VerticalData
    };


    static std::string toString( ExchangeMode mode );


    /**
     * I use two tags per spacetree per rank: one for boundary data
     * (horizontal) and one for up-down and synchronous (forks) data
     * exchanges (vertical and fork/join).
     *
     * The nice thing about Peano's new data management is that it is
     * stack-only. Furthermore, all stack data exchange is triggered via the
     * spacetree set, i.e. the order of the stacks is known externally, too.
     * So we effectively do not need that many tags even though we need
     * different tags per tree pair.
     */
    int getGridDataExchangeTag( int sendingTreeId, int receivingTreeId, ExchangeMode exchange ) const;

    /**
     * I use this for debugging. When I have a tag, I can ask the node whether
     * the node is aware what this tag actually means.
     */
    static std::string getSemanticsForTag( int tag );
};

#endif
