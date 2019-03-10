// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_CELL_MARKER_H_
#define _PEANO4_DATAMANAGEMENT_CELL_MARKER_H_


namespace peano4 {
  namespace datamanagement {
    struct CellMarker;
  }
}



struct peano4::datamanagement::CellMarker {
  CellMarker( bool isRefined_, bool isAdjacentToTreeBoundary_ ):
    isRefined(isRefined_),
	isAdjacentToTreeBoundary(isAdjacentToTreeBoundary_) {
  }

  bool isRefined;
  bool isAdjacentToTreeBoundary;
};

#endif
