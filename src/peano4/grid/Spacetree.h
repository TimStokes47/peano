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
  }
}



class peano4::grid::Spacetree {
  private:
    static tarch::logging::Log  _log;

    /**
     * The root of a spacetree corresponds to the initial state of the tree
     * traversal automaton. So we reuse the object here. It is basically the
     * bounding box.
     */
    AutomatonState _root;

    std::vector< GridVertex >  _vertexStack[TwoPowerDPlusTwo];

    Spacetree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width);

    void descend(
      const AutomatonState& state,
	  GridVertex            vertices[TwoPowerD]
    );

    static bool isSpacetreeNodeRefined(GridVertex  vertices[TwoPowerD]);

    static void refineState(const AutomatonState& coarseGrid, AutomatonState fineGrid[ThreePowerD] );
    static void refineState(const AutomatonState& coarseGrid, AutomatonState fineGrid[ThreePowerD], tarch::la::Vector<Dimensions,int>  fineGridPosition, int  axis );

    static void loadVertices(  const AutomatonState& state, GridVertex  vertices[TwoPowerD]);
    static void storeVertices( const AutomatonState& state, GridVertex  vertices[TwoPowerD]);
  public:
    static Spacetree createTrivialTree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width);

    void traverse();
};


#endif
