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
      class SpreadOut;
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
class toolbox::loadbalancing::strategies::SpreadOut: public toolbox::loadbalancing::AbstractLoadBalancing {
  public:
    SpreadOut(
      Configuration* configuration = new DefaultConfiguration(),
      CostMetrics*   costMetrics   = new toolbox::loadbalancing::metrics::CellCount()
    );
    virtual ~SpreadOut();

    virtual void finishStep() override;

  private:
    static tarch::logging::Log  _log;

    /**
     * Return 0 if the load balancing should not split (yet) and otherwise
     * return number of splits required.
     *
     * There are two reasons to split:
     *
     * - The mesh is stable, i.e. the grid statistics of
     *   peano4::parallel::SpacetreeSet::getGridStatistics() report that no
     *   vertex has been refined and no refinement has been triggered in the
     *   most recent traversal. In this case, we split up as aggressively as
     *   the minimal tree size permits, even if we cannot keep all cores busy.
     *   As the grid statistics are reduced over all ranks, all ranks agree
     *   on this criterion.
     * - The mesh is already big enough to give every core on every rank a
     *   tree of at least the minimal tree size.
     */
    int getNumberOfTreesPerRank() const;

    void updateLoadBalancing();

    void triggerSplit( int numberOfCells, int targetRank );
};

