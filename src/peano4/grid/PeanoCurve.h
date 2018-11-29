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

	static peano4::utils::LoopDirection getLoopDirection(
	  const AutomatonState&  state
    );

    static int lineariseCellIndex( const tarch::la::Vector<Dimensions,int>& cellPosition );
    static void setExitFace(AutomatonState& cell, int axis);
    static void setEntryFace(AutomatonState& cell, int axis);
    static void invertEvenFlag(AutomatonState& cell, int axis);
    static void removeFaceAccessNumber(AutomatonState& cell, int face);
    static void setFaceAccessNumber(AutomatonState& cell, int face, int value);
};

#endif
