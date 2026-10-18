// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/logging/Log.h"


#include "State.h"


namespace toolbox {
  namespace loadbalancing {
    class Statistics;
  }
}



/**
 * Statistics helper routine for load balancing
 *
 * Please note that we assume a SPMD paradigm, i.e. if one rank uses
 * statistics, the other ranks all have to use such an object, too. The
 * individual objects communicate with each other.
 *
 * The class mirrors peano4::parallel::SpacetreeSet::getGridStatistics():
 * updateGlobalView() all-reduces the statistics over all ranks, so all ranks
 * hold the same values afterwards. Only the attributes and getters which are
 * explicitly labelled as local are rank-local, even if they have a global
 * counterpart.
 *
 * The class does not hold any cell counts of its own. They are taken from
 * peano4::parallel::SpacetreeSet's grid statistics.
 *
 */
class toolbox::loadbalancing::Statistics {
  public:
    Statistics();

    std::string toString() const;

    /**
     * Update the global statistics view on all ranks
     *
     * Per contract, this function has to be called by any
     * implementation of toolbox::loadbalancing::AbstractLoadBalancing::finishStep().
     *
     * The routine all-reduces the number of trees, the number of ranks with
     * enabled load balancing, the number of splits and the maximum number of
     * trees per rank over all ranks through tarch::mpi::Rank::allReduce().
     * The reduction is blocking, i.e. all ranks hold the same statistics
     * once the routine has terminated. The routine hence has to be called by
     * all ranks at the same time, and only by one thread per rank.
     *
     * The routine also updates getNumberOfStateUpdatesWithoutAnySplit() and
     * resets the local number of splits to zero.
     */
    void updateGlobalView();

    /**
     * Local number of unrefined cells
     *
     * Rank-local: Sum over the statistics of the rank's local trees, i.e.
     * over peano4::parallel::SpacetreeSet::getGridStatistics(int).
     */
    int getLocalNumberOfInnerUnrefinedCells() const;

    /**
     * Global number of unrefined cells
     *
     * Taken from peano4::parallel::SpacetreeSet::getGridStatistics(), which
     * is reduced over all ranks at the end of every traversal.
     */
    int getGlobalNumberOfInnerUnrefinedCells() const;

    int getGlobalNumberOfRanksWithEnabledLoadBalancing() const;
    int getGlobalNumberOfTrees() const;

    /**
     * Number of updateGlobalView() calls without any split on any rank
     *
     * Counts the calls of updateGlobalView() in a row in which the global
     * number of splits has been zero. As the global number of splits is
     * reduced over all ranks, the counter is the same on all ranks.
     */
    int getNumberOfStateUpdatesWithoutAnySplit() const;

    int getMaximumTreesPerRank() const;

    /**
     * Rank-local: Bookmark that this rank has triggered a split
     */
    void incLocalNumberOfSplits( int delta=1 );

    /**
     * Number of splits this rank has triggered since the last
     * updateGlobalView()
     *
     * Rank-local. updateGlobalView() takes these splits into account and
     * then resets the counter to zero.
     */
    int getLocalNumberOfSplits() const;

    /**
     * Rank-local: Memorise whether the load balancing on this rank is
     * switched on
     */
    void notifyOfStateChange( State state );

  private:
    static tarch::logging::Log _log;

    /**
     * Rank-local: Is the load balancing on this rank switched on
     */
    bool _localLoadBalancingEnabled;

    /**
     * Rank-local accumulator where I keep track of how often I did split
     * since the last updateGlobalView().
     */
    int _localNumberOfSplits;

    int _globalNumberOfTrees;

    int _globalNumberOfRanksWithEnabledLoadBalancing;

    /**
     * Sum of all ranks' local number of splits at the last
     * updateGlobalView()
     */
    int _globalNumberOfSplits;

    int _numberOfStateUpdatesWithoutAnySplit;

    int _maximumTreesPerRank;
};
