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
	/**
	 * By setting the value to something bigger than 2, we effectively reserve
	 * NumberOfBaseStacks - 2 as callstack.
	 */
	static constexpr int NumberOfBaseStacks = 3;

	/**
	 * Standard (serial) number of stacks required per spacetree
	 *
	 * We need an input and an output stack. Then we need the 2d temporary
	 * stacks. Finally, we need another stack to represent the call stack.
	 * Inside the spacetree, this one is not required - we use the real
	 * call stack of the recursive formulation - but if we manage the call
	 * stack explicitly, i.e. through grid traversal events, then we have
	 * also to manage the stack explicitly.
	 */
	static constexpr int MaxNumberOfStacksPerSpacetreeInstance = NumberOfBaseStacks + Dimensions*2;

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
    static int getVertexReadStackNumber(const AutomatonState& cell, const std::bitset<Dimensions>& vertex );

    /**
     * @return 0 or 1 if it is about in/out stacks
     */
    static int getVertexWriteStackNumber(const AutomatonState& cell, const std::bitset<Dimensions>& vertex );

    /**
     * Faces are enumerated following the same paradigm as the vertices. Face 0
     * is the face with a x_0 as normal which runs through the origin (left
     * bottom point), face 1 is the face with x_1 as normal, ... Face 0+d is the
     * face parallel to face 0 but on the opposite side of the cell. In the SFC
     * context, we obviously need a different enumeration scheme. With vertices,
     * deriving this scheme is simple: you get the first vertex and then you xor
     * the vertex numbers. Here, this is not possible, i.e. for faces users have
     * to go through this routine.
     */
    static int getFaceNumberAlongCurve(const AutomatonState& cell, int logicalFaceNumber );

    static int getFaceReadStackNumber(const AutomatonState& cell, int face );
    static int getFaceWriteStackNumber(const AutomatonState& cell, int face );

    static int getCellReadStackNumber(const AutomatonState& cell);
    static int getCellWriteStackNumber(const AutomatonState& cell);

    static bool isInOutStack( int number );
};

#endif
