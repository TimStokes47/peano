// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_GRID_SPACETREE_H_
#define _PEANO4_GRID_SPACETREE_H_



#include "AutomatonState.h"
#include "tarch/logging/Log.h"


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
    const AutomatonState _root;

    Spacetree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width);
  public:
    static Spacetree createTrivialTree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width);
};


#endif
