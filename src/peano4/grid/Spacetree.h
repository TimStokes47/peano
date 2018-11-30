// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_SPACETREE_H_
#define _PEANO4_GRID_SPACETREE_H_



#include "AutomatonState.h"
#include "GridVertex.h"


#include "tarch/logging/Log.h"


#include <vector>


namespace peano4 {
  namespace grid {
    class Spacetree;
    class TraversalObserver;
  }
}



class peano4::grid::Spacetree {
  private:
    static tarch::logging::Log  _log;

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

    /**
     * The root of a spacetree corresponds to the initial state of the tree
     * traversal automaton. So we reuse the object here. It is basically the
     * bounding box.
     */
    AutomatonState _root;

    std::vector< GridVertex >  _vertexStack[Dimensions*2+2];

    Spacetree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width);

    void descend(
      const AutomatonState& state,
	  GridVertex            vertices[TwoPowerD],
	  TraversalObserver&    observer
    );

    /**
     * A spacetree node is refined if any of its adjacent vertices holds a
     * refining or refinement-triggered flag.
     */
    static bool isSpacetreeNodeRefined(GridVertex  vertices[TwoPowerD]);

    /**
     * Takes a state (describing a node in the tree) and returns the
     * @f$ 3^d @f$ states on the next finer level along the Peano SFC. This
     * routine is basically the grammar generation of Bader et al. It relies
     * internally on a recursive call of the other refineState() routine
     * along the spatial dimensions.
     */
    static void refineState(
      const AutomatonState&              coarseGrid,
	  AutomatonState                     fineGrid[ThreePowerD],
	  tarch::la::Vector<Dimensions,int>  fineGridPosition = tarch::la::Vector<Dimensions,int>(0),
	  int                                axis = Dimensions-1
	);

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
     */
    void updateVertexAfterLoad( GridVertex& vertex );
  public:
    static Spacetree createTrivialTree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width);

    void traverse(TraversalObserver& observer);
};


#endif
