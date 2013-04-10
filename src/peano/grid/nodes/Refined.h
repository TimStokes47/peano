// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_REFINED_H_
#define _PEANO_GRID_NODES_REFINED_H_


#include "peano/utils/Globals.h"
#include "peano/grid/SingleLevelEnumerator.h"
#include "peano/grid/nodes/Node.h"

#include <bitset>


/**
 * Refined Node
 *
 * t.b.d. This is the former RefinedEvent
 *
 * @author Tobias Weinzierl
 */
template <
  class Vertex,
  class Cell,
  class State,
  class VertexStack,
  class CellStack,
  class EventHandle
>
class peano::grid::nodes::Refined: public peano::grid::nodes::Node<Vertex,Cell,State,VertexStack,CellStack,EventHandle> {
  private:
    typedef peano::grid::nodes::Node<Vertex,Cell,State,VertexStack,CellStack,EventHandle>               Base;
    typedef peano::grid::nodes::Leaf<Vertex,Cell,State,VertexStack,CellStack,EventHandle>               LeafNode;
    typedef peano::grid::nodes::RegularRefined<Vertex,Cell,State,VertexStack,CellStack,EventHandle>     RegularRefinedNode;

    static tarch::logging::Log _log;

    LeafNode&            _leafNode;
    RegularRefinedNode&  _regularRefinedNode;

    /**
     * Set Counter, create hanging nodes and new nodes
     *
     * @param coarseGridVertices That is the coarse grid vertices throughout
     *                           the descend process, i.e. it is the fine grid
     *                           vertices of the refined event.
     */
    void setCounter(
      Vertex                        coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&  coarseGridVerticesEnumerator,
      int                           fineGridCounter[FOUR_POWER_D]
    );

    /**
     * @see Node::updateCellAfterLoad()
     */
    void updateRefinedCellBeforeStore(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
    );

    void validateRefinementStateOfVertices(
      Vertex const * const          fineGridVertices,
      const SingleLevelEnumerator&  fineGridVerticesEnumerator
   );

   /**
    * Get flag for a cell identified by verticesEnumerator.
    *
    * This operation runs throught the adjacent vertices and sets the right
    * height. See SingleLevelEnumerator::updateAdjacentCellsFlag().
    *
    * This operation has to be called before traverse() is invoked on a child
    * cell. The load operation might trigger some immediate refinements in a
    * neighbouring cell (see documentation of
    * peano::grid::nodes::loops::StoreVertexLoopBody::invalidateCoarseGridTreeHeightAttributesIfRefined())
    * and this refinement might change the tree information of this cell. So,
    * updating the flags after the load is not an option - it has to happen
    * before the descend.
    *
    * !!! Parallelisation
    *
    * If the oracle of the local node is told to fork, we always invalidate
    * the enumerator's optimisation flags, as this command means that we
    * should try to fork and not do any static optimisation. So, when a fork
    * has failed on one node, this node stops to fork on this node.
    *
    * @param vertices in   Vertices adjacent to current cell identified by
    *                      enumerator
    * @param verticesEnumerator inout The enumerator's spatial data is not
    *                      modified, but the static tree properties are
    *                      overwritten
    */
   void updateRefinedEnumeratorsCellFlag(
     const State&            state,
     const Vertex            vertices[FOUR_POWER_D],
     SingleLevelEnumerator&  verticesEnumerator
   ) const;

     /**
      * Split up the grid further
      *
      * This operation is active if and only
      * - if the code is compiled with MPI, and
      * - if the current node is the master node or its oracle is set to fork at least once, and
      * - if the current node is not involved in a fork.
      */
     void splitUpGrid(
       State&                                       state,
       Cell&                                        fineGridCell,
       Vertex                                       fineGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&                 fineGridVerticesEnumerator,
       Cell                                         newFineGridCells[THREE_POWER_D],
       const std::vector< SingleLevelEnumerator >&  newFineGridVerticesEnumerators,
       Vertex*                                      newFineGridVertices
    );

    /**
     * Identify Inner Cells of Patch That Could be Forked
     *
     * Runs through the cells of a @f$ 3^d @f$ patch. The bitset at the
     * beginning is unset. The operation then flags each bit for each
     * cell that is local.
     *
     * Furthermore, it ensures that only cells are forked that are not
     * adjacent to hanging nodes. If we would allow hanging nodes to be
     * part of a forked cell, things become really messy. Some illustrations
     * on this issue can be found in Node::updateCellsParallelStateAfterLoad().
     *
     * If all adjacent vertices of a forked cell are refined, we ensure that
     * the boundary topology of the domains will not change significantly, and
     * all the algorithmics become a little bit simpler.
     */
    std::bitset<THREE_POWER_D> getForkCandidates(
      State&                 state,
      const std::vector< SingleLevelEnumerator >&  newFineGridVerticesEnumerators,
      Cell                   newFineGridCells[THREE_POWER_D],
      Vertex                 fineGridVertices[FOUR_POWER_D]
    ) const;
  public:
    Refined(
      VertexStack&                vertexStack,
      CellStack&                  cellStack,
      EventHandle&                eventHandle,
      peano::geometry::Geometry&  geometry,
      LeafNode&                   leafNode,
      RegularRefinedNode&         regularRefinedNode
    );

    /**
     * @see Standard constructor
     */
    virtual ~Refined();

    void load(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell,
      int                                       counter[FOUR_POWER_D]
    );

    /**
     * Traverse the subcells
     *
     * This operation executes the following task sequence:
     *
     * - Load the subcells,
     * - call enterCell() for the refined cell,
     * - invoke splitUpGrid() which is only relevant in the parallel mode,
     * - invoke descend(),
     * - call leaveCell() for the refined cell, and
     * - store the refined cells away.
     *
     * As load and stores are inherently sequential due to the stack data
     * structure, we can only call step 1 and 2 and the last two stepts in
     * parallel, i.e. the maximum concurrency level is two.
     *
     * The fine grid cell is the one identified by the fine grid enumerator.
     */
    void traverse(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
    );

    /**
     * Descend in spacetree
     *
     * This operation is called for the subcells of a refined cell, i.e. state,
     * cell, and fine grid vertices all correspond to one cell within a
     * @f$ 3^d @f$ patch.
     *
     * We already have loaded the @f$ 3^d @f$ subsequent grid cells on the next
     * finer level, i.e. one level smaller than fineGridVerticesEnumerator. So,
     * the algorithm performs the following steps:
     *
     * - Create array of enumerators. These enumerators are analysed from the
     *   refined cell, i.e. it can contain solely static information. In the
     *   first loop, we set their offsets correctly. In the second loop, we can
     *   then try to find out whether they represent regular subtrees, e.g.
     *   This would be impossible a priori, as we need the subgrid's vertices
     *   to do this analysis.
     * - Load all the vertices of the @f$ 3^d @f$ subcells and see whether we
     *   can modify (restrict) the vertex enumerators' flags (for example find
     *   out whether a subcell is a regular refined grid).
     *
     * !!! Regular parallel subtrees
     *
     * On regular subtrees, the descend() tries to use recursion unrolling
     * instead of the depth-frist traversal. This however is possible if and
     * only if
     *
     * - recursion unrolling is switched on as part of Peano's optimisations,
     * - the main memory can hold the whole unrolled tree, and
     * - the subtree is not marked as remote.
     *
     * The last issue might be surprising, as a remote subtree induces parallel
     * boundary vertices, and, hence, the upper tree part is not regular but
     * only static. This assumption however does not hold throughout the fork,
     * when the remote flag propagates top-down. In this case, I switch off the
     * unrolling.
     */
    void descend(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell                                      newFineGridCells[THREE_POWER_D],
      bool                                      destroySubLevelsFinally
    );

    void store(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell,
      int                                       counter[FOUR_POWER_D]
    );
};


#include "peano/grid/nodes/Refined.cpph"

#endif
