// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once



#include "tarch/logging/Log.h"

#include "peano4/parallel/Node.h"

#include <map>

#include "toolbox/loadbalancing/AbstractLoadBalancing.h"

#include "toolbox/loadbalancing/metrics/CellCount.h"


namespace toolbox {
  namespace loadbalancing {
    namespace strategies {
      class SpreadOutOnceGridStagnates;
    }
  }
}





/**
 * Spread out
 *
 * Try to give each thread on each rank one tree in one big decomposition rush.
 * After that, switch into stagnation.
 *
 * The routine tries to get as many cores/ranks in as possible. Therefore, we
 * employ a bottom-up splitting strategy which prioritises fair fine grid
 * splits over an "efficient" tree decomposition. Consult a discussion of
 * splitting variants and constraints in peano4::grid::Spacetree::isCellTopDownSplitCandidate()
 * and  peano4::grid::Spacetree::isCellBottomUpSplitCandidate(),
 * peano4::grid::Spacetree::splitOrJoinCell() and notably the general
 * @ref page_peano_domain_decomposition "domain decomposition overview" for the
 * rationale.
 */
class toolbox::loadbalancing::strategies::SpreadOutOnceGridStagnates: public toolbox::loadbalancing::AbstractLoadBalancing {
  public:
    SpreadOutOnceGridStagnates(Configuration* configuration = new DefaultConfiguration(), CostMetrics*   costMetrics = new toolbox::loadbalancing::metrics::CellCount() );
    virtual ~SpreadOutOnceGridStagnates() override = default;

    virtual void finishStep() override;
  private:
    static tarch::logging::Log  _log;

    /**
     * Return 0 if the load balancing should not split (yet) and otherwise
     * return number of splits required.
     *
     * The strategy only splits once the grid has stagnated, i.e. once the
     * grid statistics of peano4::parallel::SpacetreeSet::getGridStatistics()
     * report that no vertex has been refined and no refinement has been
     * triggered in the most recent traversal. As these statistics are
     * reduced over all ranks, all ranks agree on this criterion.
     */
    int getNumberOfTreesPerRank() const;

    void updateLoadBalancing();

    void triggerSplit( int numberOfCells, int targetRank );
};

