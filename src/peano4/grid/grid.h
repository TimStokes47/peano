// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_H_
#define _PEANO4_GRID_H_


#include <vector>


namespace peano4 {
  /**
   * @namespace peano4::grid
   *
   * The grid namespace is Peano's core.
   *
   */
  namespace grid {
    /**
     * Forward declaration
     */
    class GridStatistics;
    class GridControlEvent;

    /**
     * The term clear() is not 100% correct, as the number of stationary traversals
     * is not reset to a dummy but instead incremented.
     */
    void clear( GridStatistics& statistics, bool isGlobalMasterTree );

    /**
     * Take a set of grid control events and try to consolidate them, i.e.
     * remove overlaps, fuse individual requests, and kick out erase commands
     * where they are overwritten by refinements anyway.
     *
     * The consolidation algorithm is pretty primitive:
     *
     * - We run over all pairs in events. Per pair, we construct a bounding
     *   box. If the bounding box volume is around twice as big as the sum
     *   of the two volumes or smaller, then we merge the two boxes into
     *   one. Otherwise, we add it to the result.
     * - We call the operation recursively if we have fused/eliminated any
     *   element from the input set.
     *
     * I do not fuse/consolidate any erase events. The reason is that
     *
     * @todo See above
     *
     */
    std::vector< GridControlEvent > consolidate( std::vector< GridControlEvent>  events );
  }
}


peano4::grid::GridStatistics operator+( peano4::grid::GridStatistics lhs, peano4::grid::GridStatistics rhs );

#endif
