// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_SPACETREE_H_
#define _PEANO4_GRID_SPACETREE_H_



#include "AutomatonState.h"
#include "GridVertex.h"
#include "GridStatistics.h"
#include "GridControlEvent.h"


#include "tarch/logging/Log.h"


#include "peano4/stacks/stacks.h"


#include <vector>
#include <map>
#include <set>


namespace peano4 {
  namespace grid {
    class Spacetree;
    class TraversalObserver;

    namespace tests {
      class SpacetreeTest;
    }
  }

  namespace parallel {
    class SpacetreeSet;
  }
}



/**
 * Represents one tree
 */
class peano4::grid::Spacetree {
  public:
	/**
	 * We need an input and an output stack. Then we need the 2d temporary
	 * stacks. Finally, we need one more stack for joining trees.
	 */
	static constexpr int MaxNumberOfStacksPerSpacetreeInstance = 3 + Dimensions*2;
  private:
	static constexpr int InvalidRank = -1;
	static constexpr int NumberOfStationarySweepsToWaitAtLeastTillJoin = 4;

    static tarch::logging::Log  _log;

    friend class peano4::parallel::SpacetreeSet;
    friend class peano4::grid::tests::SpacetreeTest;

    enum class VertexType {
      New,
	  Hanging,
	  Persistent,
	  Delete
    };

    enum class SpacetreeState {
      /**
       * Set if this tree results from a split and if this is the first
       * grid sweep when the former owner actually is in the mode
       * splitting.
       */
      NewFromSplit,
	  Running,
	  /**
	   * Trigger the join. Nothing is happening yet. It is only the worker
	   * that updates all adjacency lists. These updates however are not
	   * yet given to the master.
	   */
	  JoinTriggered,
	  /**
	   * Merge the actual data into the master.
	   */
	  Joining,
	  /**
	   * Joined means that this tree is basically dead and should be cleaned
	   * up.
	   */
	  Joined
    };

    static std::string toString( SpacetreeState state );
    static std::string toString( VertexType type );

    /**
     * Simple recursive type analysis
     *
     * <h2> Merge semantics </h2>
     *
     * We run over the parent vertices and merge along the faces (per
     * dimension). The priority of flags should be taken from the code
     * directly.
     */
    static VertexType getVertexType(
      GridVertex                         coarseGridVertices[TwoPowerD],
      tarch::la::Vector<Dimensions,int>  position,
	  int                                dimension = Dimensions-1
    );

    int              _id;

    SpacetreeState   _spacetreeState;

    /**
     * The root of a spacetree corresponds to the initial state of the tree
     * traversal automaton. So we reuse the object here. It is basically the
     * bounding box.
     */
    AutomatonState   _root;

    GridStatistics   _statistics;

    /**
     * This is not a static master. There's only a real master-worker relation
     * built into this part of the code when we actually split or join.
     */
    const int        _masterId;

    /**
     * A split is identified by a tuple of id and cell count which tells the
     * code how many cells should go to a particular id. The actual split then
     * is done in a second iteration, i.e. we first bookmark all split requests
     * and then we roll over the requests in the subsequent iteration to be
     * actually performed.
     */
    typedef std::map< int, int >  SplitSpecification;
    SplitSpecification   _splitTriggered;
    SplitSpecification   _splitting;

    constexpr static int NoJoin = -1;
    /**
     * This should be -1 if no join is triggered. Otherwise it holds the id of
     * the joining rank.
     */
    int       _joinTriggered;
    int       _joining;

    std::map< int, peano4::stacks::GridVertexStack >    _vertexStack;

    /**
     * We get these control events when we kick off the traversal and then
     * realise/interpret them.
     */
    std::vector< peano4::grid::GridControlEvent >       _gridControlEvents;

    /**
     * Should only be called for inner cells
     */
    void evaluateGridControlEvents(
      const AutomatonState& fineGridState,
      GridVertex            coarseGridVertices[TwoPowerD],
      GridVertex            fineGridVertices[TwoPowerD]
    );

    /**
     * Clear the statistics
     */
    void clearStatistics();

    /**
     * If a cell is given away to another rank, we have to mark its vertices
     * manually with our veto flag, such that we do not coarsen above it.
     */
    void updateVerticesAroundForkedCell(
      GridVertex            coarseGridVertices[TwoPowerD]
    ) const;

    void descend(
      const AutomatonState& state,
	  GridVertex            vertices[TwoPowerD],
	  TraversalObserver&    observer
    );

    /**
     * A spacetree node is refined if any of its adjacent vertices holds one of
     * the following flags:
     *
     * - refining If all vertices are refining or hanging or triggered, but
     *     none of them has one of the flags discussed below, then we run into
     *     a brand new cell of the tree.
     * - refinement-triggered
     * - erase-triggered We want to erase this spacetree node, but the erase
     *     process is not triggered yet.
     * - erasing If none of the other vertices holds another flag of this list,
     *     then this cell is to be removed.
     */
    static bool isSpacetreeNodeRefined(GridVertex  vertices[TwoPowerD]);

    /**
     * Every local refined cell should call this routine. We increment the
     * respective counter. Upon storage, we then refine if the counter
     * equals 2^d. This way we avoid hanging vertices within the domain.
     */
    static void incrementNumberOfAdjacentRefinedLocalCells(GridVertex  vertices[TwoPowerD]);

    /**
     * Takes a state (describing a node in the tree) and returns the
     * @f$ 3^d @f$ states on the next finer level along the Peano SFC. This
     * routine is basically the grammar generation of Bader et al. It relies
     * internally on a recursive call of the other refineState() routine
     * along the spatial dimensions.
     *
     * The array fineGrid is filled along the Peano SFC. So you don't have to
     * traverse the array with a zfor - a simple for is sufficient.
     */
    static void refineState(
      const AutomatonState&              coarseGrid,
	  AutomatonState                     fineGrid[ThreePowerD],
	  tarch::la::Vector<Dimensions,int>  fineGridPosition = tarch::la::Vector<Dimensions,int>(0),
	  int                                axis = Dimensions-1
	);

    /**
     * A spacetree node as 2^d adjacent vertices. So there are 2^d integers
     * stored within these vertices that overlap with the current node. They
     * all have to be the same. If they identify the local _id, then the
     * node is local.
     *
     * Throughout the splitting process, an id might be already set to a
     * remote rank, but it still is technically local.
     */
    bool isSpacetreeNodeLocal(
	  GridVertex            vertices[TwoPowerD]
    ) const;

    /**
     * Study the adjacency flags and do ignore hanging nodes.
     *
     * A vertex is remote, if all its adjacent cells are handled by another
     * rank. However, this rank may not have the attribute fork-triggered
     * (because then it does not yet exist) or joining (because then it is
     * already forwarding its work to its master), or forking. The latter
     * case means that the rank is just about to forward all vertices to the
     * new worker, i.e. it does not compute anything anymore on the local
     * vertex, but it still has to do the send/receive stuff, i.e. it still
     * has to handle the vertices.
     */
    bool isVertexAdjacentToLocalSpacetree(
      GridVertex  vertex,
	  bool        splittingIsConsideredLocal,
	  bool        joiningIsConsideredLocal
	) const;

    /**
     * Load the vertices of one cell
     *
     * The load process has to be done along the local order of the Peano
     * SFC. For this, I rely on PeanoCurve::getFirstVertexIndex().
     */
    void loadVertices(
      const AutomatonState&                        fineGridState,
	  GridVertex                                   coarseGridVertices[TwoPowerD],
	  GridVertex                                   fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
	);

    void storeVertices(
      const AutomatonState&                        fineGridState,
	  GridVertex                                   coarseGridVertices[TwoPowerD],
	  GridVertex                                   fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,int>&     cellPositionWithin3x3Patch
    );

    /**
     * Little helper. Should likely go into la or utils.
     */
    static tarch::la::Vector<Dimensions,int> convertToIntegerVector( const std::bitset<Dimensions>& in );

    /**
     * This operation has multiple purposes
     *
     * - Merge with neighbour vertices. See
     *   receiveAndMergeVertexIfAdjacentToDomainBoundary().
     * - Roll over the flags. These guys now are merged already. See
     *   receiveAndMergeVertexIfAdjacentToDomainBoundary().
     * - Do the refinement flag (state) update.
     * - Erase non-local vertices if they do not carry a veto flag.
     *
     *
     * <h2> Flag update </h2>
     *
     * Implements the standard refinement status transition, i.e. a triggered
     * becomes actually ing. And if a vertex has been refining or erasing in
     * the previous sweep, we finally update it to refined or unrefined.
     *
     * This operation has to be called whenever we load a vertex from the input
     * stream, i.e. we touch it for the very first time. We are not allowed to
     * invoke it after we've created a new vertex.
     *
     * We don't do any statistics here. I've moved all the statistics into
     * updateVertexBeforeStore().
     */
    void updateVertexAfterLoad(
      GridVertex&                               vertex,
      GridVertex                                fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,int>&  fineVertexPositionWithinPatch
    );

    /**
     * @see splitOrMoveNode()
     */
    std::vector<int>  _splittedCells;

    /**
     * We do realise a @f$LR(3^d)@f$ grammar to identify admissible splits.
     * First, this routine checks whether splits are active. If so, we do
     * study the split condition or we run into the move branch (which is
     * essentially a split of the whole tree to a different tree).
     *
     * <h2> Split </h2>
     *
     * A node may be split if the parent is still no the same node (so we
     * always have an intermediate mesh layer and preserve the tree topology)
     * and if the node does not act as parent to another rank. If we did
     * omit the latter check, a split might alter the tree topology. Which
     * we don't like.
     *
     * The first check is trivial. The second check is realised through a
     * pushback container and an LR(k) lookup.
     *
     * - Every leaf pushes a marker into this container: _id if it is and
     *   remains local, the new id if the node has been split, and a -1
     *   otherwise.
     * - A refined cell induces a reduce on the container. @f$ 3^d@f$ markers
     *   are removed and analysed: If they all have the same ip as the next
     *   split marker to be used, we add it.
     *
     * - A refined cell finally adds its marker again.
     */
    void splitOrMoveNode(
      GridVertex                                vertex[TwoPowerD],
      GridVertex                                fineGridVertices[TwoPowerD]
    );

    /**
     *
     *
     * <h2> Computational domain </h2>
     *
     * Some of the steps of this routine have to be done totally agnostic of
     * whether the vertex is inside the domain or not. The coarsening for
     * example has to be done outside of the domain as well.
     *
     * For the post-refinement, we also have to take into account outer
     * vertices. A vertex is to be refined, if it is unrefined and surrounded
     * by $2^d$ refined cells. In this case, it still technically is a hanging
     * vertex which should be avoided. This notably can happen outside.
     *
     *
     * <h2> Restriction of veto flags </h2>
     *
     * We make each vertex hold a flag isAntecessorOfRefinedVertex. If it is
     * set, we veto any erasing here. This way, we can ensure that the trees
     * remove at most one level at a time. We illustrate all further
     * explanations with a simple example:
     *
     * @image html Spacetree_updateVertexBeforeStore_restrictIsAntecessorOfRefinedVertex.png
     *
     * Let the red vertices be refined. In a serial code, the tree is not
     * split. You have to glue together the left and right tree. As the middle
     * level's vertex holds the refinement flag, the top level vertex carries
     * the flag isAntecessorOfRefinedVertex. It consequently never is erased.
     * Users first have to remove the finer level.
     *
     * If we split up tree into two threads, this approach fails if the flag
     * is restricted per core in the bottom-up steps. By the end of the sweep,
     * the flag is set on the right tree, but it is not set on the left tree.
     * We consequently might decide to erase on the left tree but veto this on
     * the right tree.
     *
     * To eliminate this behaviour, we split up the flag into a current flag
     * and a flag from the previous solution. This flag is rolled over. If the
     * flag is set, it undoes any erase trigger.
     *
     * @param fineVertexPositionWithinPatch Position of vertex within 3x3 or 3x3x3 patch respectively
     *
     * @see updateVertexAfterLoad()
     */
    void updateVertexBeforeStore(
      GridVertex&                               vertex,
      GridVertex                                fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,int>&  fineVertexPositionWithinPatch
    );

    static bool restrictToCoarseGrid(
      const tarch::la::Vector<Dimensions,int>&  coarseVertexPosition,
      const tarch::la::Vector<Dimensions,int>&  fineVertexPosition
    );

    /**
     * If a cell gets a new id, we have to update its vertices. This routine is
     * only used for splits. I originally thought I might use it for joins as
     * well. Indeed I can. But I can only do this on the master. The worker may
     * not update any cell immediately. If I do this, then the adjacency
     * information of the adjacent vertices is overwritten and I loose this
     * clue that these vertices are adjacent to the local, joining rank.
     */
    static void updateVertexRanksWithinCell(
      GridVertex  fineGridVertices[TwoPowerD],
	  int         newId
    );

    tarch::la::Vector<TwoPowerD,int> getAdjacentRanksForNewVertex(
      GridVertex                                   coarseGridVertices[TwoPowerD],
      const tarch::la::Vector<Dimensions,int>&     vertexPositionWithin3x3Patch
    ) const;

    /**
     * @image html Spacetree_createNewPersistentVertex.png
     */
    GridVertex createNewPersistentVertex(
      GridVertex                                   fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
	  const tarch::la::Vector<Dimensions,int>&     vertexPositionWithin3x3Patch
    ) const;

    /**
     * @image html Spacetree_createNewPersistentVertex.png
     */
    GridVertex createHangingVertex(
      GridVertex                                   fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
	  const tarch::la::Vector<Dimensions,int>&     vertexPositionWithin3x3Patch
    ) const;

    /**
     *
     *
     * <h2> Admissible splits </h2>
     *
     * Splits must preserve a master-worker, i.e. a tree topology. In the
     * example below, a grey rank is the worker of a blue rank (left). If
     * the grey rank splits up, there always has to be a layer in-between
     * the blue and green rank to preserve a unique master-worker topology.
     * Therefore, the middle layout is not admissible whereas the right one
     * is.
     *
     * @image html Spacetree_split.png
     *
     * There are many reasons to enforce this constraints. The two most
     * important ones are:
     *
     * - I want to have a clear tree topology on the MPI ranks.
     * - These splits make joins easier. Joins are allowed between
     *   masters and workers (in the very tree topology) and only for
     *   ranks which hold no refined local cells. However, to be able to
     *   realise the join, the master has to cover the whole area (on a
     *   coarser level) joining into its area. If we did not enforce the
     *   tree topology, i.e. the level in-between two ranks, then the
     *   greyish rank in the example above would fork away a subdomain and
     *   consequently coarsen its grid where the green rank now steps in.
     *   So this grid is "lost" and it can't join these data in anymore.
     */
    void split(int treeId, int fineGridCells);

    /**
     * Get the ids of the surrounding cells.
     *
     * This operation studies the vertex only, i.e. it is your responsibility
     * to take the tree state (new, running, joining, ...) into account: The
     * operation is called on the receiver side if and only if the spacetree is
     * not new. On the sender side, the routine is used to analyse outflowing data if and
     * only if the state is not joining.
     */
    std::set<int>  getAdjacentDomainIds( const GridVertex& vertex, bool calledByReceivingProcess ) const;

    /**
     * This is the parallel version of traverse() as it is used by the
     * spacetree set. The latter passes in a callback. Through this, the
     * tree can actually split and merge.
     */
    void traverse(TraversalObserver& observer, peano4::parallel::SpacetreeSet& spacetreeSet);

    /**
     * This one is to be invoked if and only if a vertex goes to the in/out
     * stacks. The routine should be called just before the vertex goes to
     * the output stack. We call it in updateVertexBeforeStore() here, so
     * this constraints automatically is followed.
     *
     * The routine builds up a set over integers into which it adds all ranks
     * that have to receive a copy. Once this is done, it iterates over the
     * set and sends out data. The two-step scheme becomes necessary, as we
     * have to avoid that vertices are sent around multiple times.
     */
    void sendOutVertexIfAdjacentToDomainBoundary( const GridVertex& vertex );

    /**
     * Manage the data exchange after a vertex is loaded for the first time
     *
     * The operation has three jobs to do:
     * - If this is the first sweep on a new tree, i.e. if we are in the
     *   splitting phase, we have to recive data from the tree that is
     *   splitting and merge them into the local data as it is forwarded.
     * - Exchange vertices along domain boundary.
     * - Send/stream data to another rank which is just splitting (see item
     *   one).
     *
     * The order of these three steps is important.
     *
     *
     * <h2> Boundary data exchange </h2>
     *
     * For a local vertex, i.e. one adjacent to the current active tree, which
     * is neighbouring another tree, we do receive this tree's data copy and
     * then merge it into the local tree.
     *
     * This approach doesn't work straightforwardly when we split: Lets assume
     * tree 0 is triggered to split into 0 and 1. In a first step, tree 0 enters
     * the state split triggered. It now updates all adjacency lists, so
     * boundary data is already sent out to 1 - even though 1 is not instantiated
     * yet. Once we are done, we copy over (parts of) tree 0 into tree 1, so
     * tree 1 now does exist and is well-prepared to receive the data already
     * dropped by 0. This second step is the actual splitting step. Now, other
     * ranks have still sent in data to 0 which actually should go to 1. We
     * receive those guys, but we have to eliminate explicitly any incoming data
     * we'd expect from 1 as 1 didn't have the chance yet to send it out.
     *
     * The function is called directly
     * after a vertex has been read from the input stream. Please note that
     * an update of the refinement states (e.g. switch from
     * refinement-triggered to refining) happens after the merge. Any update of the
     * refinement state in this operation hence immediately affects the
     * vertex updates in this very iteration.
     *
     * The update of the adjacency information is simple: If a neighbour tells us
     * that it has changed an adjacency entry in one of its own fields, i.e. if
     * it has changed its own entry, we copy this information over. Otherwise, we
     * ignore any updates (there should not be any).
     *
     */
    void receiveAndMergeVertexIfAdjacentToDomainBoundary( GridVertex& vertex );

    /**
     * The original tree registers itself. But we can't register the copied
     * tree that late. We have to register its new id as soon as we trigger the
     * split.
     */
    Spacetree( const Spacetree& otherTree, int newId);

    /**
     * Don't copy a tree as it is tied to some stacks.
     *
     * Unfortunately, this does not work, as we need it for the vector.
     */
    // Spacetree( const Spacetree& ) = delete;

    /**
     * Don't copy a tree as it is tied to some stacks.
     *
     * Unfortunately, this does not work, as we need it for the vector.
     */
    // Spacetree& operator=( const Spacetree& ) = delete;

    /**
     * Join with master. Call this routine only for degenerated trees,
     * i.e. for trees without any refined local cells.
     *
     * We have to be careful with the merges. Often when we fork a tree,
     * this tree first is not refined further. We need a couple of sweeps
     * to see whether a tree is refined further or not.
     */
    void joinWithMaster();
    void joinWithWorker(int id);

    /**
     *
     * No kids policy
     *
     * Only ranks that have no kids are allowed to join. This is
     * implicitly ensured as we have only degenerated trees here.
     *
     * @see join()
     */
    bool mayJoinWithMaster() const;

    /**
     * A spacetree may be moved if this attribute holds and there are no
     * kids.
     */
    bool mayMove() const;
  public:
    Spacetree(
	  const tarch::la::Vector<Dimensions,double>&  offset,
	  const tarch::la::Vector<Dimensions,double>&  width
    );

    void traverse(TraversalObserver& observer);

    GridStatistics getGridStatistics() const;

    std::string toString() const;
};


#endif
