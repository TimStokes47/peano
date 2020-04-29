#include "CellMarker.h"


peano4::datamanagement::CellMarker::CellMarker(
  const peano4::grid::GridTraversalEvent& event
):
  _centre(event.getX()),
  _h(event.getH()),
  _isRefined(event.getIsRefined()!=0),
  _isLocal(event.getIsCellLocal()) {
}


bool peano4::datamanagement::CellMarker::isRefined() const {
  return _isRefined;
}


bool peano4::datamanagement::CellMarker::isLocal() const {
  return _isLocal;
}


tarch::la::Vector<Dimensions,double>  peano4::datamanagement::CellMarker::x() const {
  return _centre;
}


tarch::la::Vector<Dimensions,double>  peano4::datamanagement::CellMarker::h() const {
  return _h;
}


std::string peano4::datamanagement::CellMarker::toString() const {
  std::ostringstream msg;
  msg << "(x=" << _centre << ",h=" << _h << ",refined=" << _isRefined << ")";
  return msg.str();
}
