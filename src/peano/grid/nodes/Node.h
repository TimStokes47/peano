// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_NODE_H_
#define _PEANO_GRID_NODES_NODE_H_


#include "peano/grid/SingleLevelEnumerator.h"
#include "peano/geometry/Geometry.h"


namespace peano {
    namespace grid {
      namespace nodes {
        template <
          class Vertex,
          class Cell,
          class State,
          class VertexStack,
          class CellStack,
          class EventHandle
        > class Node;

        /**
         * Forward declaration
         */
        template <
          class Vertex,
          class Cell,
          class State,
          class VertexStack,
          class CellStack,
          class EventHandle
        > class Leaf;

        /**
         * Forward declaration
         */
        template <
          class Vertex,
          class Cell,
          class State,
          class VertexStack,
          class CellStack,
          class EventHandle
        > class Refined;

        /**
         * Forward declaration
         */
        template <
          class Vertex,
          class Cell,
          class State,
          class VertexStack,
          class CellStack,
          class EventHandle
        > class RegularRefined;

        /**
         * Forward declaration
         */
        template <
          class Vertex,
          class Cell,
          class State,
          class VertexStack,
          class CellStack,
          class EventHandle
        > class Root;
      }
    }
}


/**
 * Node
 *
 * While I do not rely on any inheritance mechanism for the cells, there's a
 * couple of operations and relations that are needed by any node. These
 * attributes and operations are collected in this very class.
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
class peano::grid::nodes::Node {
  private:

    static tarch::logging::Log _log;

  protected:
    /**
     * Holds the vertex stack associated to this event.
     */
    VertexStack& _vertexStack;

    /**
     * Holds the cell stack associated to this event. This is important for
     * all events events - even leaves need it as they might refine further.
     */
    CellStack&   _cellStack;

    /**
     * Holds the event handle associated to this event.
     */
    EventHandle&       _eventHandle;

    /**
     * Geometry handle.
     */
    peano::geometry::Geometry&  _geometry;

    Node(
      VertexStack&                vertexStack,
      CellStack&                  cellStack,
      EventHandle&                eventHandle,
      peano::geometry::Geometry&  geometry
    );

    /**
     * @see Standard constructor. There's nothing to do here.
     */
    ~Node();

    CellStack&    getCellStack() const;
    VertexStack&  getVertexStack() const;


    #ifdef Parallel
    /**
     * Take the cell, assigne it to the remote rank, and also update the
     * adjacency information fo the adjacent vertices.
     */
    void makeCellRemoteCell(
      State&                         state,
      int                            remoteRank,
      Cell&                          fineGridCell,
      Vertex                         fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&   fineGridVerticesEnumerator
    ) const;
    #endif

    /**
     * Validation Routines
     *
     * Validates all the vertex attributes that duplicate the information given
     * by the vertices. That is grid level and vertex's coordinates. If
     * information consistency isn't given, the method finished triggering an
     * assertion. This operation should be used by the load operations to
     * validate the data consistency.
     *
     * The operation becomes the nop operation if either the debug and assert
     * mode is switched off.
     */
    static void validatePositionOfVertices(
      Vertex                                fineGridVertices[FOUR_POWER_D],
      const peano::grid::VertexEnumerator&  fineGridVerticesEnumerator
    );

    /**
     * Update cell's geometric state
     *
     * This little helper is called by the load operation as soon as both the
     * vertices and the cell object are loaded. It analyses the current cell
     * state, the geometry and modifies the cell is necessary.
     *
     * If the cell belongs to a static tree or if the cell is remote, i.e. it
     * is handled by another node, then this operation does not change the
     * cell's state.
     *
     * !!! Hidden Geometries
     *
     * Besides the geometry update, we also have to check whether the
     * computationally is hidden in the cell: This happens if all the vertices
     * adjacent to a cell are outside but the domain is contained within the
     * cell.
     *
     * @image html Node_InvokeEnterCell.png
     *
     * In this special case, we have to refine the vertices even though they
     * are outside.
     */
    void updateCellsGeometryInformationAfterLoad(
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
     * Handle cell's parallel attributes
     *
     * This operation has different jobs:
     *
     * !!! Inherit remote state
     *
     * If a cell is not remote but its parent is remote, then the code has
     * triggered a fork for one of the coarser cells. In this case, we make
     * the children remote as well. There is however an exception to this rule:
     * On parallel workers, the root cell has the rank of the master node which
     * is absolutely correct. In this case, we are not allowed to change the
     * responsible rank of the middle patch cell on level one though.
     *
     * @image html Node_updateCellsParallelStateAfterLoad_inherit_remote_state.png
     *
     * This case basically invokes makeCellRemoteCell(). This operation in turn
     * is invariant on already remote cells, i.e. you may call it several times
     * in a row, it only changes the cell state once at the first call.
     *
     * We now encounter a special case, if a node is in the state
     * fork-triggered and at the same time refines the grid. If that happens,
     * the newly created cells already are assigned to remote nodes. However,
     * the rank lists of the adjacent vertices are not validly set. Hence, we
     * call makeCellRemoteCell() also throughout the create cell process. See
     * Leaf::traverse() for implementation details.
     *
     * !!! Startup remote nodes
     *
     * If a cell is remote but its parent is not, then we have to startup a
     * remote node (if this remote node is not just forked, but that cannot
     * happen, as forks are triggered after this operation has been called).
     *
     * @image html Node_updateCellsParallelStateAfterLoad_startup_remote_node.png
     *
     * The story is a different one for joins: If a remote node is joining, we
     * may not start it up, as it already continues to traverse a second time
     * after the join triggered. The reason for this immediate re-traverse is
     * that the master needs the joined data (cell markers in particular)
     * before the worker is restarted again.
     *
     * !!! Distribute Fork-triggered Information
     *
     * If there is a fork going on (i.e. triggered already has changed to
     * forking), and if the cell is adjacent to one of the new workers, we have
     * to distribute the new parallel data and (if the cell is to be
     * transferred to the remote node) send away data. This is also done in
     * this operation.
     *
     * !!! Receive data from fork
     *
     * The data exchange throughout forks and joins is exclusively based upon
     * cells, i.e. it is always handled exclusively by this operation. So, if
     * we just started up a worker due to a fork, we always check the vertices.
     * If one of them is adjacent to the local node, then we have to receive it
     * from the master.
     *
     * Fork data always comes along in triples: First each cell sends a bitset
     * identifying which vertices are exchanged (see remark on hanging
     * nodes below), second the cell data itself is
     * sent, and, third, all the vertices shared are forwarded. The leading
     * flag is important for the worker to reconstruct the local space-filling
     * curve's properties on level one and to know when to descend from level
     * one to level two. On all other levels, the information is not relevant.
     * This operation nevertheless asks for it to clear the MPI buffers. And we
     * use the data for an assertion.
     *
     * @image html Node_updateCellsParallelStateAfterLoad_forking.png
     *
     * !!! Comments on prepareSendToWorker()
     *
     * For a deployed cell, i.e. a cell that is handled by a remote node, (and
     * all its descendants), the enterCell() event is split up.
     *
     * @image html parallel-root-cell-communication.png
     *
     * First, the grid invokes prepareSendToWorker() on the master. This allows
     * you to modify the vertices adjacent to a remote cell. These vertices and
     * the cell then are sent to the worker. The worker receives them due to
     * mergeWithWorker(). At the end of the traversal, the worker takes its
     * top level cell and the @f$ 2^d @f$ adjacent vertices and sends them back
     * to the master (see prepareSendToMaster()). The master receives this data
     * and merges them into the master data structure due to mergeWithMaster().
     * The second part of the communciation, i.e. all these data transferred
     * from the worker to the master, is omitted if the state reduction is
     * switched off.
     *
     * The master's operations have a huge signature and there is some
     * redundancy hidden in this interface: The @f$ 2^d @f$ adjacent vertices
     * of one remote cell implicitly are parallel boundaries, i.e. they
     * already do communicate with their neighbours. However, if all
     * @f$ 2^d-1 @f$ adjacent cells of a vertex are deployed to other ranks as
     * well, the master doesn't get any notice from this vertex anymore. The
     * latter fact imposes a problem if you are implementing multiscale
     * algorithms. Here, we need a plug-in point to enable programmers to
     * move data between two levels. This is possible only due to the
     * mechanism discussed in this section.
     *
     * !!! Exchanging Hanging Nodes
     *
     * The fork becomes pretty complicated if hanging nodes are involved on
     * rather coarse resolutions.
     *
     * @image html fork-hanging-nodes.png
     *
     * Left illustration above: There is only one vertex refined in the cell
     * that the master wants to deploy to another rank. Now, if the master
     * node runs into the blue cell, this cell is not even visible on the
     * worker node. It also should not be exchanged. This scenario is an
     * example why inheriting adjacency information to hanging nodes is
     * not a clever idea.
     *
     * Right illustartion above: One vertex of the deployed cell is a hanging
     * one. If it does not carry the adjacency flags, we are not able to inform
     * the worker about the order the neighbours are visited. This scenario is
     * an example why inheriting adjacency information is necessary.
     *
     * As a consequence, we exclusively fork cells that are not adjacent
     * to hanging nodes.
     */
    void updateCellsParallelStateAfterLoad(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
    ) const;

     void updateCellsParallelStateAfterLoadForRootOfDeployedSubtree(
       State&                                    state,
       Cell&                                     fineGridCell,
       Vertex                                    fineGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              fineGridVerticesEnumerator,
       Cell&                                     coarseGridCell,
       Vertex                                    coarseGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
       const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
     ) const;

     void updateCellsParallelStateAfterLoadIfNodeIsJoiningWithMaster(
       State&                                    state,
       Cell&                                     fineGridCell,
       Vertex                                    fineGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              fineGridVerticesEnumerator,
       Cell&                                     coarseGridCell,
       Vertex                                    coarseGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
       const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
     ) const;

     void updateCellsParallelStateAfterLoadForNewWorkerDueToForkOfExistingDomain(
       State&                                    state,
       Cell&                                     fineGridCell,
       Vertex                                    fineGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              fineGridVerticesEnumerator,
       Cell&                                     coarseGridCell,
       Vertex                                    coarseGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
       const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
     ) const;

     /**
      *
      * !!! Rationale
      *
      * We switch a cell to remote if we have sent it to a worker and it is not
      * assigned to the local node anymore. However, we do this after the loop
      * running over all forking nodes, not within this loop. If we did it inside
      * the loop, we would run over all forking ranks. If a cell is adjacent to
      * rank k but will belong to rank k+1 from now on, we switch its state already
      * after the send to rank k. We could fix this if we compared to the assigned
      * rank rather than just asking whether it belongs to another rank. This
      * however would not solve the whole issue: If there is also a rank k+1
      * forking, this one again would receive an already destroyed cell.
      */
     void updateCellsParallelStateAfterLoadIfStateIsForking(
       State&                                    state,
       Cell&                                     fineGridCell,
       Vertex                                    fineGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              fineGridVerticesEnumerator,
       Cell&                                     coarseGridCell,
       Vertex                                    coarseGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
       const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
     ) const;

     void updateCellsParallelStateAfterLoadIfNodeIsJoiningWithWorker(
       State&                                    state,
       Cell&                                     fineGridCell,
       Vertex                                    fineGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              fineGridVerticesEnumerator,
       Cell&                                     coarseGridCell,
       Vertex                                    coarseGridVertices[FOUR_POWER_D],
       const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
       const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
     ) const;

    /**
     * Counterpart of updateCellsParallelStateAfterLoad(), i.e. the one
     * operation where we receive data from the workers if the algorithm
     * shall receive data from workers.
     *
     * Indeed, data from the worker is received if and only if
     * - the reduction is switched on,
     * - the node is not joining, and
     * - the node has not just forked.
     *
     * The reason for the latter is a technical one: To setup the worker's
     * grid, Peano needs the whole cell and cell marker information, i.e. it
     * has to know how to run through the @f$ 3^d @f$ cells on the local
     * spacetree level one. This information also requires that the master
     * runs through the whole grid. However, the master in-between requires
     * the worker to send it worker's data. So, we have to switch off this
     * reduction to remove a deadlock.
     *
     * !!! Order of messages
     *
     * For the startup process, we first send the state, then the cells, and,
     * finally, the vertices. This does make sense: The Grid reads the state,
     * as the state is important even before we load the first data from the
     * input stream. The cells and the vertices remain in the MPI queue. Root
     * then finally takes this data from the MPI queue.
     *
     * The reduction process is the other way round: Root sends away the cell
     * and its adjacent vertices. Only afterwards, we send away the state.
     */
    void updateCellsParallelStateBeforeStore(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
    ) const;

    void updateCellsParallelStateBeforeStoreForRootOfDeployedSubtree(
      State&                                    state,
      Cell&                                     fineGridCell,
      Vertex                                    fineGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex                                    coarseGridVertices[FOUR_POWER_D],
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell
    ) const;
  public:
};



#include "peano/grid/nodes/Node.cpph"

#endif
