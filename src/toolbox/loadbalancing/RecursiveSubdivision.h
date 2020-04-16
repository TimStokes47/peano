// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TOOLBOX_LOAD_BALANCING_RECURSIVE_SUBDIVISION_
#define _TOOLBOX_LOAD_BALANCING_RECURSIVE_SUBDIVISION_



#include "tarch/logging/Log.h"



namespace toolbox {
  namespace loadbalancing {
    class RecursiveSubdivision;
  }
}


/**
 * Recursive load balancing micking the behaviour of the guided partitioning in OpenMP
 *
 * This is one of the simplest load balancing strategies one can think about. We
 * try to mimick the behaviour of OpenMP's guided scheduling: We determine the
 * maximum number of unrefined inner cells around and compute an optimal load per
 * core. If we used this tree size, we'd not have any flexibility to move stuff around.
 * So we say that no tree anywhere should be bigger than half of this size. So, in
 * theory, we overbook all of the cores by a factor of two. Whenever a rank decides
 * that it has to split up its tree once more, it deploys the new tree to the rank
 * with the smallest total load (with a slight bias to keep stuff local). The new
 * tree results from classic bipartitioning.
 *
 * If we have N cells in total and run with R ranks, then the algorithm first spreads
 * these cells over the ranks with N/R cells per rank. After that, each rank hosts
 * one spacetree.
 *
 * In an idea world, each core would in this scenario host N/R/C cells. We however set
 * the maximum size of an admissible tree to N/R/C/2.
 *
 * Whenever we run into a situation where not all cores of the local rank are occupied,
 * we cut the local tree into two equally sized pieces and deploy the new tree locally.
 * This way, we flood all local cores.
 *
 * Whenever any local tree violates the magic threshold of N/R/C/2, we split it up into
 * two parts. This time, we deploy the new subtree to the MPI rank with the smallest
 * total load. This way, we balance across MPI ranks.
 *
 */
class toolbox::loadbalancing::RecursiveSubdivision {
  public:
    RecursiveSubdivision();

    void finishTimeStep();
  private:
    static tarch::logging::Log  _log;

    bool _hasSpreadOutOverAllRanks;

    /**
     * Status variable required to compute good load balancing
     */
    int _localNumberOfInnerUnrefinedCell;

    /**
     * Status variable required to compute good load balancing
     */
    int _globalNumberOfInnerUnrefinedCell;

    void updateGlobalView();

    /**
     * Determines the maximum spacetree size a tree should have in the
     * optimal case.
     */
    int getMaximumSpacetreeSize() const;

    int getIdOfHeaviestLocalSpacetree() const;

    bool doesBiggestLocalSpactreeViolateOptimalityCondition() const;
};


#endif

