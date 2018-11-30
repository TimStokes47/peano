// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_PEANO_CURVE_H_
#define _PEANO4_GRID_PEANO_CURVE_H_


#include "peano4/utils/Loop.h"


namespace peano4 {
  namespace grid {
    class PeanoCurve;
    class AutomatonState;
  }
}


class peano4::grid::PeanoCurve {
  public:
	static bool isTraversePositiveAlongAxis(
	  const AutomatonState&  state,
	  int                    axis
	);

    /**
     * Holds a set bit for each dimension along which the traversal is
     * positive.
     */
	static peano4::utils::LoopDirection getLoopDirection(
	  const AutomatonState&  state
    );

    static void setExitFace(AutomatonState& cell, int axis);
    static void setEntryFace(AutomatonState& cell, int axis);
    static void invertEvenFlag(AutomatonState& cell, int axis);
    static void removeFaceAccessNumber(AutomatonState& cell, int face);
    static void setFaceAccessNumber(AutomatonState& cell, int face, int value);

    /**
     * Looks into a cell of the spacetree and gives the index of the first
     * local vertex. The local enumeration obviously depends on the
     * orientation of the curve. If you iterate over all vertices, an xor
     * with the integer counter and this result makes the iterate a cell-local
     * traversal. So a typical loop over all vertices of a cell along the Peano
     * SFC reads as
     * <pre>
  const std::bitset<Dimensions> coordinates = PeanoCurve::getFirstVertexIndex(state);
  for (int i=0; i<TwoPowerD; i++) {
    std::bitset<Dimensions> currentLocalVertexIndex( coordinates ^ std::bitset<Dimensions>(i) );
    logDebug( "loadVertices(...)", "handle vertex " << currentLocalVertexIndex );
  }
     <\pre>
     *
     * We heavily exploit bitsets here. They allow us, for example, to invert
     * the whole direction by a simple flip.
     */
    static std::bitset<Dimensions> getFirstVertexIndex( const AutomatonState& cell );

    /**
     *
     * <h2> Implementation details </h2>
     *
     * As the access numbers already are inverted in the new code where all
     * access flags are created on the fly, we don't need the inversion flag
     * here anymore. The only situation where we need it is to toggle input
     * and output stacks.
     *
     * @return 0 or 1 if it is about in/out stacks
     */
    static int getReadStackNumber(const AutomatonState& cell, const std::bitset<Dimensions>& vertex );

    /**
     * @return 0 or 1 if it is about in/out stacks
     */
    static int getWriteStackNumber(const AutomatonState& cell, const std::bitset<Dimensions>& vertex );

    static bool isInOutStack( int number );
};

#endif
