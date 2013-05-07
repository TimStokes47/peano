// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_ASPECTS_PARALLEL_MERGE_H_
#define _PEANO_GRID_ASPECTS_PARALLEL_MERGE_H_


#include "peano/utils/Globals.h"
#include "tarch/logging/Log.h"
#include "peano/utils/Loop.h"


namespace peano {
  namespace grid {
    namespace aspects {
      class ParallelMerge;
    }
  }
}



class peano::grid::aspects::ParallelMerge {
  private:
    static tarch::logging::Log  _log;
  public:
    /**
     * Merge Two Vertices on Boundary
     *
     * This operation is part of the overall merge process consisting of the
     * three steps
     *
     * # merge the adjacency lists due to peano::kernel::parallel::getMergedAdjacencyLists(),
     * # merge the refinement and vertex states (this operation), and
     * # call the mapping's merge operation, i.e. the one that is PDE-specific.
     *
     * mergeOnDomainBoundary() does some validation checks (in the assert
     * mode). Its more important role is to keep the refinement information
     * consistent.
     *
     * !!! Consistency checks
     *
     * - The spatial positions have to be the same.
     * - The level has to be the same.
     * - The inside/outside flag has to be the same.
     *
     * If can happen that the neighbour holds refining and the local node holds erase-triggered:
     *
     * @image html MergeAtBoundary.png
     */
    template <class Vertex>
    static void mergeOnDomainBoundary(
      Vertex&        localVertex,
      const Vertex&  neighbourVertex,
      int            neighbourRank
    );

    /**
     * Merge vertex on worker with vertex from master
     *
     * This is a standard operation that is called once prior to each iteration
     * by each worker receiving data from its master. When we study the
     * operation, it might seem to be surprising that the refinement state of
     * the worker usually is not modified independently of the master's state.
     * The reason for this is simple: The coarsest @f$ 2^d @f$ vertices on the
     * worker also are adjacent to other domains, i.e. they are both vertices
     * belonging to a master-worker vertical interface and standard boundary
     * vertices. And most of the refinement updates is handled by the standard
     * boundary merges (see mergeOnDomainBoundary()). So, nothing to do here.
     *
     * In most of the cases. However, there's one important special case: If
     * the master does an erase somewhere in the grid hierarchy, this erase
     * propagates down in the spacetree immediately (see
     * LoadVertexLoopBody::updateRefinementFlagsOfVertexAfterLoad()). In such a
     * case, the worker just wouldn't notice that the grid changes dramatically.
     * Therefore, this operation checks the master's vertex. If it holds an
     * erasing state, this erase immediately is anticipated by the worker vertex.
     * Otherwise, we neglect the master's refinement state and rely on
     * mergeOnDomainBoundary() for updates.
     *
     * However, we may follow this scheme if and only if the master's vertex
     * was still adjacent to cells belonging to the master. Otherwise, the
     * master erases due to forks (it erases remote grid regions), and we may
     * not update the local attributes and erase the vertices locally, too:
     * The description before still has one shortcoming. It can happen that the
     * master vertex holds an erasing state but this state may not be updated
     * locally: If the master forks all @f$ 2^d @f$ adjacent subdomains of a
     * vertex to other ranks, it will afterwards set its local vertex to
     * erasing. In this case, this erasing is not to be done on the workers.
     *
     * Vertices of a worker's root cell always are persistent as they are
     * transported via the stacks. Hence, we cannot handle them as hanging
     * nodes if coarser parts of the grid are erased (we never fork subdomains
     * with hanging nodes, so if hanging nodes occur this is always a results of
     * a posteriori grid erasing). Instead, we manually delete all adjacency
     * information. The vertex then is not communicated anymore.
     *
     * A second 'erase' use case pops up if the master has deployed all 2^d
     * adjacent cells of a vertex to different ranks and now erases a coarser
     * vertex at the same (or a related) position. If we have good luck, this
     * leads to a starvation process and everything is handled. If we have bad
     * luck, this erase propagates to the vertex vertex with the adjacent
     * deployed subtrees. This vertex is unrefined on the master, as the
     * master always erases vertices that are remote. Hence, the workers do not
     * get this information 'hey, a subtree has to be removed'. However, they
     * are informed about this one iteration later, when the master's vertex is
     * already a hanging node. In this case, we erase the local vertices, i.e.
     * the erase process here is not propagated in one step (as it is typically
     * done for erase), but spans two grid traversals.
     */
    template <class Vertex>
    static void mergeWithVertexFromMaster(
      Vertex&        localVertex,
      const Vertex&  masterVertex
    );


    /**
     * Merge with data from master
     *
     * This operation is basically a copy from the master's data to the
     * local data. Besides the copy, we do the following things:
     *
     * - We validate some vertex properties such as the spatial position
     *   or the level (only in debug mode).
     * - We set the inside/outside flag according to the geometry, i.e. to
     *   the value we locally had before the merge, if the master is not
     *   responsible for this vertex anymore. In this case, the master's
     *   information might be wrong, as the master might not have updated
     *   the geometry anymore as he felt not responsible.
     *
     * If the master node is hanging, the local node should be hanging as
     * well. If the nodes are hanging, their adjacent cells height flag has
     * no semantics.
     *
     * If the master currently is erasing but the remote vertex is refined, we
     * overwrite the erase process. According to Vertex::erase(), the
     * refinement process has higher priority.
     */
    template <class Vertex>
    static void mergeWithForkedVertexFromMaster(
      Vertex&        localVertex,
      const Vertex&  masterVertex
    );

    enum MergeVertexDueToJoinEffect {
      MasterVertexStateRemainsUnaltered,
      CreateBoundaryVertexOnMaster,
      CreateInnerVertexOnMaster
    };

    /**
     * Merge with worker data due to join
     *
     * This operation is called for the phase Joining. It updates the grid
     * attributes and gives the calling routine a hint what to call next for
     * this vertex (see the results): If the vertex was outside before, the
     * calling routine has to invoke a create operation for the vertex. This
     * is done only once, when the vertex is merged for the very first time.
     * Afterwards, it is set to inside even though it seems that the master
     * is not responsible for this node right now, i.e. the following assertion
     * would fail though its the assertion that is wrong:
     *
     * \code
         if (LocalNodeHasNotBeenResponsibleBefore) {
         assertion2(localVertex.isOutside(),localVertex.toString(),workerVertex.toString());
       \endcode
     *
     * !!! Refinement update
     *
     * Besides in/out checks, the operation also validates weather the worker's
     * grid has been refined and the local grid is not refined. In this case, it
     * triggers a refinement on the local grid, i.e. it manually sets the
     * vertex state to refining. Please not that this relation is asymmetric. A
     * join always extends the local grid (see remarks on erasing rollbacks
     * below).
     *
     * If the local vertex was refined but the worker's vertex was not,
     * this means usually that the worker vertex was outside the worker's domain,
     * i.e. in this case we have nothing to change locally. Such vertices by the
     * way are not exchanged throughout the joins anyway, i.e. we don't have to
     * check for this case.
     *
     * Please note that joined vertices never can hold refinement-triggered or
     * erase-triggered. The triggered can be set by the mappings only. In the
     * join traversal, no mappings are invoked on the worker. On the master (and
     * new responsible node for the grid entities), the mappings are invoked
     * after the join, i.e. after this operation has been called. So the only
     * way to have a triggered on a vertex is that a mapping has triggered a
     * grid modification in the traversal before. This 'triggered' however then
     * has been set to refining or erasing right after the pop from the input
     * container and before the vertex has been sent to the worker or merged
     * with the worker data, respectively.
     *
     * !!! Erasing rollback?
     *
     * Merging subdomains becomes tricky if the worker tries to erase right in the
     * iteration when it joins its data into its master:
     *
     * @image html EraseRollback.png
     *
     * In the example abover, we study two nodes. Node 1 holds the grey cells. Node
     * 2 holds the green cells and the blue one. The later is subject of our study.
     *
     * Let node 1 merge the green partition (holding the blue one as well) into its
     * grid. In the blue cell, node 2 has marked one vertex refining, the other one
     * is erasing. They are not at the domain boundary, i.e. the corresponding
     * vertices on node 1 all are unrefined (or don't even exist before the join).
     *
     * The refining vertex on node 2 triggers node 1 to make the vertex at this
     * location refining as well. This vertex does not cause any problems or demands
     * for special attention.
     *
     * The erasing vertex on level 2 however has to trigger a refinement on node 1.
     * Otherwise, the two grids are inconsistent. If we set the vertex to refining
     * on node 1, we however loose that information that this vertex should be
     * coarsened. One solution to this issue is to introduce a new state
     * 'vertex now shall be refined and should be erased immediately in the next
     * traversal'.
     *
     * !!! Update of adjacency list entries
     *
     * If a vertex is not adjacent to the local partition anymore, the node
     * is not informed about changes of the vertex anymore. Load rebalancing
     * info is not communicated globally. If we join with worker data, the
     * worker typically has information that is up-to-date, while the master
     * might not be up-to-date anymore.
     *
     * As a consequence, we typically copy the worker's adjacency information
     * into the local node if two entries in the adjacency list are not equal.
     * The only exception is if the worker claims that the master is reponsible
     * for an entry while the master says that it is another node. In this case,
     * the worker anticipates the join. We set the responsibility manually to
     * teh worker, well-aware that this is switched back to the master at the
     * end of the iteration anyway.
     *
     * Some illustration that led to the identification of this bug:
     *
     * @image html ParallelMergeJoin00.jpg
     * @image html ParallelMergeJoin01.jpg
     */
    template <class Vertex>
    static MergeVertexDueToJoinEffect mergeWithJoinedVertexFromWorker(
      Vertex&        localVertex,
      const Vertex&  workerVertex,
      int            workerRank
    );

    /**
     * Merge With Cell Received From Master Throughout Fork
     *
     * Basically, this operation takes the master's data and copies it into the
     * local data structure. The only difference occurs if the master's data is
     * a refined cell but the local cell is a leaf. In this case, the new
     * worker has to reconstruct the master's tree, i.e. all the vertices that
     * are already refined on the master are locally set to refining, and all
     * the cells that are refined on the master remain leaves locally, as they
     * are refined to the local refinement procedure.
     *
     * !!! Validations
     *
     * I added a couple of assertions:
     * - The even flags have to be the same on the master and the worker.
     * - The inside/outside information has to be the same as we assume that
     *   there is one global geometry.
     * - The access numbers are the same, i.e. the space-filling curve is
     *   (locally) the same on the master and the worker.
     * - The level is the same on master and worker for a cell.
     *
     * The inside/outside information has to be equal. For this, the worker has
     * called the geometry before the merge happens.. However, it does not do this
     * for the @f$ 3^d-1 @f$ cells of level one besides the middle one. Those
     * 'belong' to the master, those are not updated locally, and thus their
     * inside/outside information might be wrong locally. Summed up, this means
     * that the inside/outside flag is equal if and only if the master forks
     * this element currently to the local rank.
     *
     * It can happen that the master has refined a cell that is not yet refined
     * on the worker. Indeed, that's a standard case. Here, we set the cell's
     * local state to leaf and rely on mergeWithForkedVertexFromMaster() to set
     * the vertex's state to refining().
     *
     * !!! Reassignment of remote cells
     *
     * If a cell is remote, i.e. not handled by the local node anymore, its
     * responsible information is taken from the master and it is consistent.
     * However, if the master decides to rebalance, we will not get this information
     * and the information will become invalid. Thus, I erase the responsibility
     * information and indicate that this remote cell belongs somehow to the master
     * and that the master knows more about the real resopnsibility. If we remove
     * this reset mechanism, inconsistencies can occur. See
     * StoreVertexLoopBody::exchangeVerticesDueToParallelisation() for a scenario
     * where also this code part fails.
     *
     * @see mergeWithJoinedCellFromWorker()
     * @see mergeWithForkedVertexFromMaster()
     */
    template <class Cell>
    static void mergeWithForkedCellFromMaster(
      Cell&        localCell,
      const Cell&  masterCell
    );


    /**
     * Merge cell from worker into master throughout a joint
     *
     * This operation is basically the counterpart of mergeWithForkedCellFromMaster().
     * There are some slight differences however:
     *
     * -
     *
     * @return This cells hasn't existed before, so immediately after the
     *         merge the grid management should call a create for this
     *         cell.
     */
    template <class Cell>
    static bool mergeWithJoinedCellFromWorker(
      Cell&        localCell,
      const Cell&  workerCell,
      int          workerRank
    );
};


#include "peano/grid/aspects/ParallelMerge.cpph"

#endif

