// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_VERTEX_H_
#define _PEANO_GRID_VERTEX_H_


#include "tarch/la/Vector.h"
#include "peano/utils/PeanoOptimisations.h"
#include "peano/grid/VertexEnumerator.h"
#include "peano/grid/State.h"


#include <set>


namespace peano {
  namespace grid {
      template <class VertexData>
      class Vertex;

      namespace tests {
        class SetCounterTest;
      }

      namespace aspects {
        class ParallelMerge;
      }
    }

    namespace parallel {
      template <class T>
      class AdjacencyListAspect;
  }
}



/**
 * This class is the base class of all spacetree grid vertices, i.e. the
 * application's vertex class extends this class.
 *
 * @author Tobias Weinzierl
 */
template <class VertexData>
class peano::grid::Vertex {
  public:
    /**
     * This class exists to allow Peano to overload the standard constructor.
     */
    class DoNotCallStandardConstructor {};

    #if defined(StoreOnlyPersistentAttributes) && defined(PackedRecords)
    typedef typename VertexData::Packed::PersistentRecords  PersistentVertex;
    #endif

    #if !defined(StoreOnlyPersistentAttributes) && defined(PackedRecords)
    typedef typename VertexData::Packed                     PersistentVertex;
    #endif

    #if defined(StoreOnlyPersistentAttributes) && !defined(PackedRecords)
    typedef typename VertexData::PersistentRecords          PersistentVertex;
    #endif

    #if !defined(StoreOnlyPersistentAttributes) && !defined(PackedRecords)
    typedef VertexData                                      PersistentVertex;
    #endif

    #ifdef Parallel
    /**
     * This operation is robust, i.e. you can call it several times.
     */
    static void initDatatype();
    static void shutdownDatatype();

    friend class peano::parallel::AdjacencyListAspect<Vertex<VertexData> >;
    #endif

    #if defined(ParallelExchangePackedRecords)
    typedef typename VertexData::Packed       MPIDatatypeContainer;
    #else
    typedef VertexData                        MPIDatatypeContainer;
    #endif

    typedef VertexData                                      Records;

    friend class peano::grid::tests::SetCounterTest;
    friend class peano::grid::aspects::ParallelMerge;

  protected:
    VertexData _vertexData;

    /**
     * Standard constructor.
     *
     * The created vertex by default is an outer vertex, and it is not
     * persistent, i.e. a hanging node.
     */
    Vertex();

    Vertex(const PersistentVertex& argument);

    Vertex(const DoNotCallStandardConstructor&);

  public:
    ~Vertex();

    /**
     * Proxy.
     */
    std::string toString() const;

    /**
     * Proxy.
     */
    void toString(std::ostream& out) const;

    /**
     * A freshly generated vertex is a hanging node. You can however switch its
     * state to non-hanging with this operation.
     */
    void switchToNonhangingNode();

    /**
     * Is vertex outside, i.e. can we perhaps skip to invoke any events?
     */
    bool isOutside() const;
    bool isBoundary() const;
    bool isInside() const;

    /**
     * A node is considered to be hanging if not all @f$ 2^d @f$ adjacent
     * cells exist.
     */
    bool isHangingNode() const;

    /**
     * Make vertex be inside or on the boundary of the domain.
     *
     * Means that the vertex either is inside or at the boundary of the domain.
     * This implies that the events for this vertex have to be called and that
     * there is at least one adjacent cell inside the domain, too.
     *
     * @image html ../gridinterface/geometry-vertex-inside-outside.png
     *
     * !!! Assertions
     *
     * Before we switch, we validate the following:
     *
     * - The vertex should be boundary or outside. There's no reason to switch
     *   an inner vertex to inside.
     * - If the vertex is a hanging vertex, we may switch to inside always.
     *   However, the underlying tree height attributes (adjacent cell heights)
     *   then have to be invalid.
     * - Due to the switch, the adjacent cell heights should be invalidated.
     *
     * The last issue is interesting: If we switch a vertex, the geometry
     * before has told the algorithm that this part of the domain will
     * change. At this time, the algorithm already has set the whole data
     * in the tracked region to instationary. Hence, when this operation is
     * called, everything already has been set to instationary due to
     * invalidateAdjacentCellInformation().
     */
    void switchToInside();

    /**
     * Make vertex be outside the domain.
     *
     * Means that the vertex is outside and this implies that the events for
     * this vertex are not called anymore. Furthermore, none of the adjacent
     * cells is inside the domain, too.
     *
     * @image html ../gridinterface/geometry-vertex-inside-outside.png
     *
     */
    void switchToOutside();

    /**
     * Well, that works always.
     */
    void switchToBoundary();

    typename Records::RefinementControl getRefinementControl() const;


    /**
     * @return The persistent subattributes of the vertex.
     */
    PersistentVertex getRecords() const;


    /**
     * Refine Vertex
     *
     * Refine all adjacent elements along every axis. If this operation is
     * called on a vertex that does not correspond to fine grid vertex, i.e. a
     * vertex not having the state unrefined, the operation degenerates to nop.
     *
     * The refinement will not happen immediately but in the next iteration.
     */
    void refine();

    /**
     * Counterpart of refine().
     *
     * The illustartion below is out-of-date as coarse now is called erase.
     * However, it still illustrates the right process.
     *
     * @image html erase-process.png
     *
     * An erase becomes nop if it is done for a vertex belonging to an
     * instationary or forked subpartition, i.e. parallelisation and refinement
     * have a higher priority than making the grid coarser.
     */
    void erase();

    /**
     * Rollback any triggered operation.
     *
     * This operation is provided for the load vertex process. We recommend not
     * to use it for any PDE solver.
     */
    void rollbackRefinementOrEraseTriggered();

    /**
     * Switch State's Refinement Triggered into Refining
     *
     * This operation reads the refinement flag. If it is set to
     * refinement triggered, the operation changes refinement triggered into
     * refining. This operation does not analyse the parallel state of the
     * vertex.
     *
     * If the vertex's refinement state is not equal to refinement triggered,
     * the operation becomes nop.
     *
     * @return Has done a transition.
     */
    bool switchRefinementTriggeredToRefining();

    /**
     * Switch State's Erase Triggered into Erasing
     *
     * This operation reads the refinement flag. If it is set to
     * erase triggered, the operation changes erase triggered into
     * erasing. This operation does not analyse the parallel state of the
     * vertex.
     *
     * If the vertex's refinement state is not equal to erase triggered,
     * the operation becomes nop.
     *
     * @return Has done a transition.
     */
    bool switchEraseTriggeredToErasing();

    /**
     * Switch refinement flags
     *
     * There are two operations that indicate a transient state: refining and
     * erasing. These states are set whenever the code reads data with a
     * triggered state from the input stack to indicate that now the vertex
     * really is refined. Before somebody writes back such vertices to the
     * output stacks, it has to switch refining to refined or erasing to
     * unrefined, respectively. This operation does this.
     */
    void updateTransientRefinementFlagsBeforeVertexIsStoredToOutputStack();

    /**
     * If we just use Peano's refinement facilities, the following scenario
     * often occurs: Two patches are refined and spread in space. At some
     * point, they becomes adjacent to each other. However, the coarse
     * vertices along the boundary are not refined. And they won't be refined
     * as the solution is rather smooth along the patch interface. However,
     * these hanging node pollute the solution. Consequently, I introdcued a
     * counter for each node that counts how many of the adjacent cells are
     * refined. Before I store the vertex, it is checked whether this counter
     * equals @f$ 2^d @f$ and whether the vertex is unrefined. In this case, I
     * trigger a refinement.
     *
     * @image html hanging-nodes-in-patches.png
     */
    void clearCounterOfAdjacentRefinedCells();

    /**
     * Save and clear adjacency information
     *
     * - Store the current adjacency information into the field holding the last iteration's record.
     * - If the vertex is unrefined, set current height to zero.
     * - Else iIf the vertex is not stationary, set the current height to instationary.
     * - If the vertex is adjacent to a parallel domain boundary (only in parallel mode), invalidate vertex flag.
     */
    void saveAndClearAdjacentCellsInformation();

    /**
     * @see
     */
    peano::grid::CellFlags getCurrentAdjacentCellsHeight() const;
    peano::grid::CellFlags getAdjacentCellsHeightOfPreviousIteration() const;
    void setCurrentAdjacentCellsHeight(peano::grid::CellFlags value);

    /**
     * Invalidate all the adjacency information
     *
     * The operation erases all the information about the heights and
     * invariancies of adjacent trees. If you invalidate this information,
     * you switch off the static tree optimisations and the recursion
     * unrolling manually.
     *
     * This is for example done due to a merge on the worker side. Implicitely,
     * any erase() or refine() call invalidates as well, so usually it is not a
     * PDE task to invalidate any vertex anytime.
     */
    void invalidateAdjacentCellInformation();

    /**
     * @see clearCounterOfAdjacentRefinedCells()
     */
    void incCounterOfAdjacentRefinedCells();

    /**
     * @see clearCounterOfAdjacentRefinedCells()
     * @see StoreVertexLoopBody::updateRefinementFlagsAndStoreVertexToOutputStream()
     */
    void refineVertexIfItHasOnlyRefinedAdjacentCells();

    #ifdef Asserts
    /**
     * Only available if compiled with -DAsserts
     *
     * Returns the position of the vertex in assertion mode, i.e. the operation is
     * not available in the release mode. If you need the vertex's position
     * outside an assertion guarded by a ifdef Asserts, you have to recalculate
     * the position manually.
     */
    tarch::la::Vector<DIMENSIONS,double> getX() const;

    int getLevel() const;

    /**
     * Set the vertex's position
     *
     * This position is stored persistently only if we are in debug mode. Here,
     * it acts as validation field.
     */
    void setPosition( const tarch::la::Vector<DIMENSIONS,double>&  x, int level );
    #endif

    #ifdef Parallel
    /**
     * Is vertex remote?
     *
     * A vertex is remote, if all its adjacent cells are handled by another
     * rank. However, this rank may not have the attribute fork-triggered
     * (because then it does not yet exist) or joining (because then it is
     * already forwarding its work to its master), or forking. The latter
     * case means that the rank is just about to forward all vertices to the
     * new worker, i.e. it does not compute anything anymore on the local
     * vertex, but it still has to do the send/receive stuff, i.e. it still
     * has to handle the vertices.
     *
     * !!! Optimisation
     *
     * Instead of checking always first all the ranks, and then check once
     * again the state and its joining and fork-triggered ranks, I would like
     * to analyse the state once when I load the vertex and set the remote
     * flag. Unfortunately this does not work out, as the forks might occur
     * later, i.e. the remote() state might change in time.
     */
    template <class State>
    bool isRemote(
      const State&  state,
      bool          considerForkingNodesToBeRemote,
      bool          considerJoiningNodesToBeRemote
    ) const;

    /**
     * This operation is used by the send and receive buffers and should not
     * be used by other components of Peano.
     */
    void setVertexData(const VertexData& vertexData);

    /**
     * This operation is used by the send and receive buffers and should not
     * be used by other components of Peano.
     */
    VertexData getVertexData() const;

    /**
     * Changes the adjacency lists
     *
     */
    void setAdjacentRanks( const tarch::la::Vector<TWO_POWER_D,int>& ranks );

    /**
     * Modify one adjacency entry
     *
     * Should probably be visible only to the kernel's parallel namespace. This
     * operation changes one entry of the adjacency lists. This change will not
     * be visible immediately. Instead, it will take effect only in the next
     * traversal.
     */
    void setAdjacentRank( int position, int rank );

    /**
     * Return set of adjacent ranks
     *
     * Returns the set of adjacent remote ranks (the own rank is not included).
     * Please do not call this operation for vertices that do not belong to the
     * parallel boundary, i.e. call it only for vertices for which
     * isAdjacentToRemoteRank() does hold. If you take the size of the result
     * and add one, you know how often this vertex globally does exist.
     *
     * @return Ranks of nodes that do hold a copy of the vertex besides the
     *         local rank.
     */
    std::set<int> getAdjacentRemoteRanks() const;

    /**
     * List of adjacent ranks
     *
     * This operation returns the ranks that are responsible for the
     * @f$ 2^d @f$ adjacent cells. It is not defined on vertices that are not
     * part of a parallel domain boundary (see isAdjacentToRemoteRank()). The
     * result is ordered along the z-curve, i.e. the very first entry is the
     * rank of the node that is responsible for the left bottom adjacent cell
     * (for d=2). The next entry corresponds to the right bottom cell, and so
     * forth.
     *
     * This operation was written for the grid management. For PDE-solvers,
     * please use getAdjacentRemoteRanks() instead.
     *
     * @return Ranks that are responsible for adjacent cells.
     */
    tarch::la::Vector<TWO_POWER_D,int> getAdjacentRanks() const;

    /**
      * Blocking send. initDatatype() has to be called before.
      */
    void send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallel);

    /**
      * Blocking receive. initDatatype() has to be called before.
     */
    void receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallel);

    /**
     * Does Vertex Belong to Parallel Boundary
     *
     * Analyses the adjacency list. If there is one entry that is not equal to
     * the local node's rank, then this attribute holds.
     *
     * This definition is important throughout forks for existing domains.
     * Here, adjacency lists are updated and it might happen that a vertex
     * belongs not remote, but this flag nevertheless still holds.
     */
    bool isAdjacentToRemoteRank() const;

    bool isAdjacentToDomainOf( const std::set<int>& ranks ) const;
    bool isAdjacentToDomainOf( int rank ) const;

    void replaceAdjancyEntriesOfLocalRankWithMaster();
    void replaceAdjancyEntriesOfRemoteRankWithLocalRank(int rank);

    static tarch::la::Vector<TWO_POWER_D_TIMES_TWO_POWER_D,int> readAdjacencyLists(
      const peano::grid::VertexEnumerator&  enumerator,
      const Vertex* const                   vertices
    );

    void eliminateAllAdjacencyInformationThatIdentifiesNeitherWorkerNorMaster();
    #endif
};


template <class VertexData>
std::ostream& operator<<(std::ostream& out, const peano::grid::Vertex<VertexData>& vertex);


#include "peano/grid/Vertex.cpph"

#endif
