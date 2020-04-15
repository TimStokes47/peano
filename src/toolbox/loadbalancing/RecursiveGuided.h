// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TOOLBOX_LOAD_BALANCING_RECURSIVE_GUIDED_
#define _TOOLBOX_LOAD_BALANCING_RECURSIVE_GUIDED_

namespace toolbox {
  namespace loadbalancing {
    class RecursiveGuided;
  }
}


/**
 * Recursive load balancing micking the behaviour of the guided partitioning in OpenMP
 *
 * This is one of the simplest load balancing strategies one can think about. We
 * try to mimick the behaviour of OpenMP's guided scheduling, i.e. the first set
 * of local trees is twice as big as the second set, is twice as big as the third
 * one, and so forth.
 *
 * <h2> Algorithm </h2>
 *
 * The algorithm runs through different phases:
 *
 * - It waits until the total number of cells exceeds the total number of ranks. Once
 *   we are there, the strategy tells the spacetree set to populate all ranks with
 *   roughly the same number of cells. This step is obviously skipped if we don't use
 *   MPI. It also affects only rank 0 and only one thread there as it describes the
 *   serial setup of the overall algorithm.
 *
 *
 */
class toolbox::loadbalancing::RecursiveGuided {
  public:
    RecursiveGuided();

    void finishTimeStep( int totalNumberOfInnerUnrefinedCellsOnThisRank );
  private:
    bool _hasSpreadOutOverAllRanks;

    /**
     * Status variable required to compute good load balancing
     */
    int _localNumberOfInnerUnrefinedCell;

    /**
     * Status variable required to compute good load balancing
     */
    int _globalNumberOfInnerUnrefinedCell;
};


#endif

