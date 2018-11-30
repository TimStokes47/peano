#include "PeanoCurve.h"
#include "AutomatonState.h"


bool peano4::grid::PeanoCurve::isTraversePositiveAlongAxis(
  const AutomatonState&  state,
  int                    axis
) {
  assertion( axis >= 0 );
  assertion( axis < Dimensions );

  std::bitset<Dimensions> influencingEvenFlags = state.getEvenFlags();
  influencingEvenFlags.set(axis,false);

  bool result = ( influencingEvenFlags.count() % 2) == 0;
  return result ^ state.getInverted();
}


peano4::utils::LoopDirection peano4::grid::PeanoCurve::getLoopDirection(
  const AutomatonState&  state
) {
  peano4::utils::LoopDirection result;
  for (int i=0; i<Dimensions; i++) {
    result[i] = isTraversePositiveAlongAxis(state,i);
  }
  return result;
}


void peano4::grid::PeanoCurve::setExitFace(AutomatonState& cell, int axis) {
  if ( isTraversePositiveAlongAxis(cell, axis) ) {
    removeFaceAccessNumber( cell, axis+Dimensions );
    setFaceAccessNumber( cell, axis+Dimensions, 1 );
  }
  else {
    removeFaceAccessNumber( cell, axis );
    setFaceAccessNumber( cell, axis, 1 );
  }
}


void peano4::grid::PeanoCurve::setEntryFace(AutomatonState& cell, int axis) {
  if ( isTraversePositiveAlongAxis(cell, axis) ) {
    removeFaceAccessNumber( cell, axis );
    setFaceAccessNumber( cell, axis, -1 );
  }
  else {
    removeFaceAccessNumber( cell, axis+Dimensions );
    setFaceAccessNumber( cell, axis+Dimensions, -1 );
  }
}


void peano4::grid::PeanoCurve::invertEvenFlag(AutomatonState& cell, int axis) {
  assertion( axis >= 0 );
  assertion( axis < Dimensions );
  cell.flipEvenFlags( axis );
}


void peano4::grid::PeanoCurve::removeFaceAccessNumber(AutomatonState& cell, int face) {
  assertion( face >= 0 );
  assertion( face < 2*Dimensions );

  tarch::la::Vector<DimensionsTimesTwo,short int> accessNumber = cell.getAccessNumber();
  int oldFaceNumber = accessNumber(face);

  if (accessNumber(face)>0) {
    for (int i=0; i<2*Dimensions; i++)
      if ( accessNumber(i) >= oldFaceNumber ) accessNumber(i)--;
  }
  if (accessNumber(face)<0) {
    for (int i=0; i<2*Dimensions; i++)
      if ( accessNumber(i) <= oldFaceNumber ) accessNumber(i)++;
  }

  accessNumber(face) = 0;
  cell.setAccessNumber(accessNumber);
}


std::bitset<Dimensions> peano4::grid::PeanoCurve::getFirstVertexIndex( const AutomatonState& cell ) {
  std::bitset<Dimensions> result;
  std::bitset<Dimensions> currentMask;

  for (int i=0; i<Dimensions; i++) {
    if ( cell.getEvenFlags(i) ) {
      currentMask[i] = true;
      currentMask.flip();

      result ^= currentMask;

      currentMask.flip();
      currentMask[i] = false;
    }
  }

  if (cell.getInverted()) {
	result.flip();
  }

  return result;
}


void peano4::grid::PeanoCurve::setFaceAccessNumber(AutomatonState& cell, int face, int value) {
  assertion( face >= 0 );
  assertion( face < 2*Dimensions );
  assertion( value != 0 );
  assertion( cell.getAccessNumber()(face) == 0 );

  tarch::la::Vector<DimensionsTimesTwo,short int> accessNumber = cell.getAccessNumber();

  if (value > 0 ) {
    for (int i=0; i<2*Dimensions; i++)
      if ( accessNumber(i) >= value ) accessNumber(i)++;
  }
  else {
    for (int i=0; i<2*Dimensions; i++)
      if ( accessNumber(i) <= value ) accessNumber(i)--;
  }

  accessNumber(face) = static_cast<short int>(value);
  cell.setAccessNumber(accessNumber);
}


int peano4::grid::PeanoCurve::getReadStackNumber(const AutomatonState& cell, const std::bitset<Dimensions>& vertex ) {
  int smallestValue = -2*Dimensions-1;
  int result       = cell.getInverted() ? -1 : -2;
  int direction     = -1;

  for (int d=0; d<Dimensions; d++) {
    const int face = vertex[d]==0 ? d : d+Dimensions;
    const int faceAccessNumber = cell.getAccessNumber(face);
    if (faceAccessNumber<0 && faceAccessNumber>smallestValue) {
      result        = face;
      smallestValue = faceAccessNumber;
      direction     = d;
    }
  }

  if ( direction>=0 and cell.getEvenFlags(direction) ) {
    result = result<Dimensions ? result+Dimensions : result-Dimensions;
  }

  return result+2;
}


bool peano4::grid::PeanoCurve::isInOutStack( int number ) {
  assertion1(number>=0,number);
  return number==0 or number==1;
}

int peano4::grid::PeanoCurve::getWriteStackNumber(const AutomatonState& cell, const std::bitset<Dimensions>& vertex ) {
  int biggestValue = 2*Dimensions+1;
  int result       = cell.getInverted() ? -2 : -1;
  int direction    = -1;

  for (int d=0; d<Dimensions; d++) {
    const int face = vertex[d]==0 ? d : d+Dimensions;
    const int faceAccessNumber = cell.getAccessNumber(face);
    if (faceAccessNumber>0 && faceAccessNumber<biggestValue) {
      result       = face;
      biggestValue = faceAccessNumber;
      direction    = d;
    }
  }

  if ( direction>=0 and cell.getEvenFlags(direction) ) {
    result = result<Dimensions ? result+Dimensions : result-Dimensions;
  }

  return result+2;
}
