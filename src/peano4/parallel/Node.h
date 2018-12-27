// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_NODE_H_
#define _PEANO4_PARALLEL_NODE_H_


#include "tarch/logging/Log.h"
#include "peano4/grid/GridVertex.h"
#include "tarch/multicore/BooleanSemaphore.h"


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

    std::set<int>  _bookedLocalThreads;

    tarch::multicore::BooleanSemaphore  _semaphore;

    /**
     * Key is from-to.
     */
    std::map< std::pair<int,int>, std::queue<peano4::grid::GridVertex> >  _sendReceiveBuffer;

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
     * Is this node the global master process, i.e. does its rank equal get
     * MasterProcessRank()?
     */
    bool isGlobalMaster(int rank, int threadId) const;

    /**
     * Peano maps grid instance threads + mpi ranks onto global IDs through
     * this routine. To identify a global stack number, please use this
     * function and add it to your local stack number.
     */
    int getId(int rank, int threadId) const;

    int getRank(int id) const;

//    int getThreadNumber( int id ) const;

    /**
     * This operation is const, i.e. you have to bookmark it yourself. Is done
     * by Spacetree.
     */
    int getNextFreeLocalId() const;

    void registerId(int id);
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
};

#endif
