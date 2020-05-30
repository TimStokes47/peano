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
     * Merge set such that many events next to each other are fused into one
     *
     * Take a set of grid control events and try to consolidate them, i.e.
     * remove overlaps and fuse individual requests. The consolidation
     * algorithm is pretty primitive:
     *
     * - We remove the first event from the input sequence and hold it.
     * - We check it against all remaining elements from the input set. If
     *   they fit together, merge into the one currently held and remove the
     *   other one from the input.
     * - Once we've run through the input set, dump the one orginally taken
     *   from the input set (and maybe merged with others) and place it in
     *   the output set.
     * - If we have been able to merge any two events in the input set, we
     *   call the routine again (tail recursion). This is important as the
     *   merger of two regions might yield a new region which is adjacent to
     *   another big region which can be merged.
     *
     * The "next to each other" implementation is again simplistic: We
     * construct the bounding box around two events and compare its volume
     * to the sum of the volumes of the two elements. If the bounding box
     * volume is only around a few percent bigger than the sum, then we use
     * the boundary box to replace the original two elements.
     *
     * What I don't do in this algorithm is to take overlaps into account.
     * Indeed, the slack (percentage) accommodates slight overlaps. But the
     * code still might not recognise two elements if they massively overlap.
     */
    std::vector< GridControlEvent > merge( std::vector< GridControlEvent>  events, const double Tolerance = 0.1 );
  }
}


peano4::grid::GridStatistics operator+( peano4::grid::GridStatistics lhs, peano4::grid::GridStatistics rhs );

#endif
