// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_CELL_MARKER_H_
#define _PEANO4_DATAMANAGEMENT_CELL_MARKER_H_


#include "tarch/la/Vector.h"
#include "peano4/utils/Globals.h"
#include "peano4/grid/GridTraversalEvent.h"


namespace peano4 {
  namespace datamanagement {
    struct CellMarker;
  }
}



/**
 *
 */
struct peano4::datamanagement::CellMarker {
  private:
    tarch::la::Vector<Dimensions,double>  _centre;

    tarch::la::Vector<Dimensions,double>  _h;

    bool _isRefined;
  public:
    CellMarker(const peano4::grid::GridTraversalEvent& event);
  //CellMarker(const peano4::grid::GridTraversalEvent& event,const tarch::la::Vector<Dimensions,int>&  relativePositionToFather);

    bool isRefined() const;

    /**
     * The x coordinate of a cell is its centre.
     */
    tarch::la::Vector<Dimensions,double>  x() const;

    tarch::la::Vector<Dimensions,double>  h() const;

    std::string toString() const;
};


#endif
