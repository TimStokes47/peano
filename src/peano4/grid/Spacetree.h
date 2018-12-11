// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_SPACETREE_H_
#define _PEANO4_GRID_SPACETREE_H_



#include "AutomatonState.h"
#include "GridVertex.h"
#include "GridStatistics.h"


#include "tarch/logging/Log.h"


#include "peano4/stacks/stacks.h"


#include <vector>
#include <map>


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
	static constexpr int MaxNumberOfStacksPerSpacetreeInstance = 2 + Dimensions*2;
  private:
    static tarch::logging::Log  _log;

    friend class peano4::parallel::SpacetreeSet;
    friend class peano4::grid::tests::SpacetreeTest;

    enum class VertexType {
      New,
	  Hanging,
	  Persistent,
	  Delete
    };

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

    /**
     * The root of a spacetree corresponds to the initial state of the tree
     * traversal automaton. So we reuse the object here. It is basically the
     * bounding box.
     */
    AutomatonState   _root;

    GridStatistics   _statistics;

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

    /**
     * Clear the statistics
     */
    void clearStatistics();

    std::map< int, peano4::stacks::GridVertexStack >    _vertexStack;

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
     */
    bool isSpacetreeNodeLocal(
	  GridVertex            vertices[TwoPowerD]
    ) const;

    /**
     * Study the adjacency flags and do ignore hanging nodes.
     */
    bool isVertexAdjacentToLocalSpacetree(GridVertex  vertex) const;

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
     * Implements the standard refinement status transition, i.e. a triggered
     * becomes actually ing. And if a vertex has been refining or erasing in
     * the previous sweep, we finally update it to refined or unrefined.
     *
     * This operation has to be called whenever we load a vertex from the input
     * stream, i.e. we touch it for the very first time.
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
     * @param fineVertexPositionWithinPatch Position of vertex within 3x3 or 3x3x3 patch respectively
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
     * If a cell gets a new id, we have to update its vertices.
     */
    static void updateVertexRanksWithinCell(
      GridVertex  fineGridVertices[TwoPowerD],
	  int         newId
    );

    GridVertex createNewPersistentVertex(
      GridVertex                                   fineGridVertices[TwoPowerD],
	  const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level
    );

    void split(int cells);

    /**
     * This is the parallel version of traverse() as it is used by the
     * spacetree set. The latter passes in a callback. Through this, the
     * tree can actually split and merge.
     */
    void traverse(TraversalObserver& observer, peano4::parallel::SpacetreeSet& spacetreeSet);
  public:
    Spacetree(
	  const tarch::la::Vector<Dimensions,double>&  offset,
	  const tarch::la::Vector<Dimensions,double>&  width
    );

    void traverse(TraversalObserver& observer);

    GridStatistics getGridStatistics() const;
};


#endif
