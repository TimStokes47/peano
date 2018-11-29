#include "PeanoCurve.h"
#include "AutomatonState.h"


int peano4::grid::PeanoCurve::lineariseCellIndex( const tarch::la::Vector<Dimensions,int>& cellPosition ) {
  int base   = 1;
  int result = 0;
  for (int d=0; d<Dimensions; d++) {
    assertion1(cellPosition(d)>=0,cellPosition);
    assertion1(cellPosition(d)<=2,cellPosition);
    result += cellPosition(d)*base;
    base   *= 3;
  }
  assertion( result>= 0 );
  assertion( result< ThreePowerD );
  return result;
}


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

