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
  private:
    /**
     * Logging device.
     */
    static tarch::logging::Log _log;

    tarch::multicore::BooleanSemaphore  _semaphore;

    /**
     * Key is from-to.
     */
    std::map< std::pair<int,int>, std::queue<peano4::grid::GridVertex> >  _sendReceiveBuffer;


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
    int getId(int rank, int threadId) const;

    /**
     * The operation is not thread-safe as we call it only internally,
     * i.e. you are not supposed to call this function directly.
     */
    void registerId(int id, int masterId);

  public:
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
     * This operation is not const as it does some internal bookkeeping. It
     * internally invokes  registerId() on the result.
     *
     * @param forTreeId If we run a split, this is the rank that wants to
     *          split. In general, this always is the parent/master of the
     *          new rank. So if you move a rank, you have to ensure that
     *          the master is preserved.
     *
     * @return -1 If there are no ids left anymore
     */
    int reserveId(int rank, int forTreeId);

    int getRank(int id) const;

    /**
     * Only the SpacetreeSet should call this operation.
     */
    void deregisterId(int id);

    /**
     * Hand in a spacetree id and get back the number that we should use to
     * send something to this tree.
     */
    int getOutputStackNumberOfBoundaryExchange(int id) const;

    /**
     * Counterpart of getOutputStackNumberOfBoundaryExchange(int)
     */
    int getInputStackNumberOfBoundaryExchange(int id) const;

    /**
     * See getOutputStackNumberOfBoundaryExchange().
     */
    bool isBoundaryExchangeOutputStackNumber(int number) const;
    bool isBoundaryExchangeInputStackNumber(int number) const;

    /**
     * You may only call this opeartion if isBoundaryExchangeOutputStackNumber()
     * holds. In this case, the routine tells you the id of the spacetree
     * that corresponds to this stack number.
     */
    int getIdOfBoundaryExchangeOutputStackNumber(int number) const;

    void sendVertexSynchronously( const peano4::grid::GridVertex& vertex, int fromId, int toId );
    peano4::grid::GridVertex getVertexSynchronously( int fromId, int toId );

    void treesCantCoarsenBecauseOfVetos( int treeId );

    /**
     * Not const as we need a semaphore to make it thread-safe
     */
    int getParentTree( int treeId );

    /**
     * Not const as we need a semaphore to make it thread-safe
     */
    bool hasChildrenTree( int treeId );

    std::set< int > getChildren( int treeId );
};

#endif
