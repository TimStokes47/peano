// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TOOLBOX_LOAD_BALANCING_RECURSIVE_SUBDIVISION_
#define _TOOLBOX_LOAD_BALANCING_RECURSIVE_SUBDIVISION_



#include "tarch/logging/Log.h"

#include <map>


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
 * <h2> Implementation remarks </h2>
 *
 * The recursive subdivision works with one tree at a time: It always identifies the
 * one tree with the biggest load and subdivides this one further. A tree however
 * requires two traversals to split. If a tree is identified as split candidate, the
 * code hence will tell it to split, but will try to split exactly the same tree again
 * in the subsequent iteration.
 *
 * The tree's stats (how many cells does it own) at this point are skewed, as the
 * tree has not yet successfully given away cells. Indeed, a tree needs at least three
 * iterations to 'recover'. The first one initialises the split. The second traversal
 * conducts the split and eventually moves around data, but the ownership in this
 * traversal still is with the master. Therefore, the master still has all the local
 * cells. After the third split, the master has actually given away its cells.
 *
 * Therefore, we work with a blacklist map: Whenever we have identified a tree that is to
 * be split, we memorise it and give it an internal weight of 3. Memorise means it is
 * stored in a map. As long as a rank is on the blacklist map, it may not be selected
 * as potential split tree. After each iteration, we reduce the weight of the blacklisted
 * trees by one. If a weight becomes zero, we eventually remove it from the blacklist.
 */
class toolbox::loadbalancing::RecursiveSubdivision {
  public:
    RecursiveSubdivision(double percentageOfCoresThatShouldInTheoryGetAtLeastOneCell=0.8);

    void finishStep();
  private:
    /**
     * Is used by tree identification and either indicates that there are no trees
     * at all or means that the heaviest tree is on the blacklist. See implementation
     * remarks in class description.
     */
    static constexpr int NoHeaviestTreeAvailable = -1;

    static tarch::logging::Log  _log;

    const double _PercentageOfCoresThatShouldInTheoryGetAtLeastOneCell;

    std::map< int, int>    _blacklist;

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

    int getMaximumSpacetreeSize() const;

    /**
     * Determines the maximum spacetree size a tree should have in the
     * optimal case.
     *
     * @return NoHeaviestTreeAvailable If there are no local trees or
     *   if the heaviest tree is on the blacklist, i.e. we have to
     *   assume that it still is splitting.
     */
    int getIdOfHeaviestLocalSpacetree() const;

    bool doesBiggestLocalSpactreeViolateOptimalityCondition() const;

    void updateBlacklist();

    /**
     * Wrapper around the spacetree set which also updates the blacklist.
     */
    void triggerSplit( int sourceTree, int numberOfCells, int targetRank );
};


#endif

