#include "FaceMarker.h"


peano4::datamanagement::FaceMarker::FaceMarker(
  const peano4::grid::GridTraversalEvent&   event
):
  _cellCentre(event.getX()),
  _h(event.getH()),
  _select(0) {


  for (int faceNumber=0; faceNumber<2*Dimensions; faceNumber++) {
    _isRefined[faceNumber] = false;
    _isLocal[faceNumber]   = event.getIsFaceLocal(faceNumber);

    const int normal = faceNumber % Dimensions;
    for (int i=0; i<TwoPowerD; i++) {
      std::bitset<Dimensions> studiedVertex = i;
      studiedVertex.set(normal,faceNumber>=Dimensions);
      _isRefined.set( faceNumber, _isRefined[faceNumber] or event.getIsRefined(studiedVertex.to_ulong()) );
    }
  }
}


int peano4::datamanagement::FaceMarker::normalAxis() const {
  return _select % Dimensions;
}


tarch::la::Vector<Dimensions,double> peano4::datamanagement::FaceMarker::x(int i) const {
  tarch::la::Vector<Dimensions,double> result( _cellCentre );
  int normal = i % Dimensions;
  result(normal) += i >= Dimensions ? _h(normal)/2.0 : -_h(normal)/2.0;
  return result;
}


tarch::la::Vector<Dimensions,double> peano4::datamanagement::FaceMarker::normal(int i) const {
  tarch::la::Vector<Dimensions,double> result( 0.0 );
  int index = i % Dimensions;
  result(index) += i<Dimensions ? -_h(index) : _h(index);
  return result;
}


tarch::la::Vector<Dimensions,double> peano4::datamanagement::FaceMarker::x() const {
  return x(_select);
}


tarch::la::Vector<Dimensions,double> peano4::datamanagement::FaceMarker::normal() const {
  return normal(_select);
}


std::string peano4::datamanagement::FaceMarker::toString() const {
  return "(" + _cellCentre.toString() + "," + _h.toString() + ")";
}


tarch::la::Vector<Dimensions,double>  peano4::datamanagement::FaceMarker::h() const {
  return _h;
}


peano4::datamanagement::FaceMarker& peano4::datamanagement::FaceMarker::select(int face) {
  _select = face;
  return *this;
}


int peano4::datamanagement::FaceMarker::getSelectedFaceNumber() const {
  return _select;
}


bool peano4::datamanagement::FaceMarker::isRefined(int i) const {
  return _isRefined[i];
}


bool peano4::datamanagement::FaceMarker::isRefined() const {
  return isRefined(_select);
}


bool peano4::datamanagement::FaceMarker::isLocal(int i) const {
  return _isLocal[i];
}


bool peano4::datamanagement::FaceMarker::isLocal() const {
  return isLocal(_select);
}


