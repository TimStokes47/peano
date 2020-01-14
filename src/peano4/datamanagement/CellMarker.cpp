#include "CellMarker.h"
#include "peano4/grid/GridTraversalEvent.h"


peano4::datamanagement::CellMarker::CellMarker(const peano4::grid::GridTraversalEvent&  event):
  isRefined( event.getIsRefined() ),
  isAdjacentToTreeBoundary(false) {
}

