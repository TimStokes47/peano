// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_PARALLEL_NODE_H_
#define _PEANO4_PARALLEL_NODE_H_


#include "tarch/logging/Log.h"


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
    int getGlobalId(int rank, int threadId) const;

    int getRank(int id) const;

    int getThreadId( int id ) const;

    int getTag( int id ) const;
};

#endif
