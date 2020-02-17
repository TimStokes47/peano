#include "CellMarker.h"


peano4::datamanagement::CellMarker::CellMarker(
  const peano4::grid::GridTraversalEvent& event
):
  _centre(event.getX()),
  _h(event.getH())
/*
,
  _isRefined(event.getIsRefined())     // @todo Fixen
*/
{
}



bool peano4::datamanagement::CellMarker::isRefined() const {
  return _isRefined;
}


tarch::la::Vector<Dimensions,double>  peano4::datamanagement::CellMarker::x() const {
  return _centre;
}


tarch::la::Vector<Dimensions,double>  peano4::datamanagement::CellMarker::h() const {
  return _h;
}

