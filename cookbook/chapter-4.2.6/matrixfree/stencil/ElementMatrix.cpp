#include "matrixfree/stencil/ElementMatrix.h"

#include "peano/utils/Loop.h"



matrixfree::stencil::ElementWiseAssemblyMatrix
matrixfree::stencil::getElementWiseAssemblyMatrix( const matrixfree::stencil::Stencil& stencil ) {
  /**
   * @todo Die Abbildung sollte man im Konstruktor einmal bauen und dann hier
   * nur noch anwenden. Deshalb ist das Ding ja eine Methode und kein
   * statisches Ding.
   */
  ElementWiseAssemblyMatrix result;

  dfor2(j)
  dfor2(i)
    tarch::la::Vector<DIMENSIONS,int> stencilEntry;
    double    commonFacesPowerTwo = 1.0;
    for (int d=0; d<DIMENSIONS; d++) {
      stencilEntry(d) = i(d)-j(d)+1;
      if (i(d)==j(d)) commonFacesPowerTwo *= 2.0;
    }
    result(jScalar,iScalar) = stencil(peano::utils::dLinearised(stencilEntry,3)) / commonFacesPowerTwo;
  enddforx
  enddforx

  return result;
}

matrixfree::stencil::ComplexElementWiseAssemblyMatrix
matrixfree::stencil::getElementWiseAssemblyMatrix( const matrixfree::stencil::ComplexStencil& complexStencil ) {
  /**
   * @todo Die Abbildung sollte man im Konstruktor einmal bauen und dann hier
   * nur noch anwenden. Deshalb ist das Ding ja eine Methode und kein
   * statisches Ding.
   */
  ComplexElementWiseAssemblyMatrix result;

  dfor2(j)
  dfor2(i)
    tarch::la::Vector<DIMENSIONS,int> stencilEntry;
    double    commonFacesPowerTwo = 1.0;
    for (int d=0; d<DIMENSIONS; d++) {
      stencilEntry(d) = i(d)-j(d)+1;
      if (i(d)==j(d)) commonFacesPowerTwo *= 2.0;
    }
    result(jScalar,iScalar) = complexStencil(peano::utils::dLinearised(stencilEntry,3)) / commonFacesPowerTwo;
  enddforx
  enddforx

  return result;
}


double matrixfree::stencil::getDiagonalElement( const ElementWiseAssemblyMatrix& matrix ) {
  return matrix(0,0) * TWO_POWER_D;
}


double matrixfree::stencil::getDiagonalElement( const Stencil& stencil ) {
  return stencil(THREE_POWER_D / 2);
}



matrixfree::stencil::Stencil
matrixfree::stencil::reconstructStencil(const ElementWiseAssemblyMatrix& matrix ) {
  matrixfree::stencil::Stencil result(0.0);

  dfor2(j)
    dfor2(k)
      tarch::la::Vector<DIMENSIONS,int> stencilEntry;
      for (int d=0; d<DIMENSIONS; d++) {
        if (j(d)>k(d)) {
          stencilEntry(d)=2;
        }
        else if (j(d)<k(d)) {
          stencilEntry(d)=0;
        }
        else {
          stencilEntry(d)=1;
        }
      }
      result(
        getStencilEntryLinearisedIndex(stencilEntry)
      ) += matrix(jScalar,kScalar);
    enddforx
  enddforx

  return result;
}


matrixfree::stencil::ElementWiseAssemblyMatrix
matrixfree::stencil::getElementWiseAssemblyMatrix( const VectorOfStencils& vectorOfStencils ) {
  ElementWiseAssemblyMatrix result;

  dfor2(j)
  dfor2(i)
    const int                         stencilOffset = jScalar * THREE_POWER_D;
    tarch::la::Vector<DIMENSIONS,int> stencilEntry;
    double                            commonFacesPowerTwo = 1.0;
    for (int d=0; d<DIMENSIONS; d++) {
      if (i(d)==j(d)) {
        stencilEntry(d)      = 1;
        commonFacesPowerTwo *= 2.0;
      }
      else if (i(d)<j(d)) {
        stencilEntry(d)      = 0;
      }
      else {
        stencilEntry(d)      = 2;
      }
    }
    const int vectorOfStencilIndex = stencilOffset+peano::utils::dLinearised(stencilEntry,3);
    result(jScalar,iScalar) = vectorOfStencils( vectorOfStencilIndex ) / commonFacesPowerTwo;
  enddforx
  enddforx

  return result;
}

matrixfree::stencil::ComplexElementWiseAssemblyMatrix
matrixfree::stencil::getElementWiseAssemblyMatrix( const VectorOfComplexStencils& vectorOfComplexStencils ) {
  ComplexElementWiseAssemblyMatrix result;

  dfor2(j)
  dfor2(i)
    const int                         stencilOffset = jScalar * THREE_POWER_D;
    tarch::la::Vector<DIMENSIONS,int> stencilEntry;
    double                            commonFacesPowerTwo = 1.0;
    for (int d=0; d<DIMENSIONS; d++) {
      if (i(d)==j(d)) {
        stencilEntry(d)      = 1;
        commonFacesPowerTwo *= 2.0;
      }
      else if (i(d)<j(d)) {
        stencilEntry(d)      = 0;
      }
      else {
        stencilEntry(d)      = 2;
      }
    }
    const int vectorOfComplexStencilIndex = stencilOffset+peano::utils::dLinearised(stencilEntry,3);
    result(jScalar,iScalar) = vectorOfComplexStencils( vectorOfComplexStencilIndex ) / commonFacesPowerTwo;
  enddforx
  enddforx

  return result;
}


void matrixfree::stencil::createUpwindBoundaryMatrix( ElementWiseAssemblyMatrix& matrix, int dimension ) {
  if (dimension==0) {
    assertionMsg(false, "not implemented yet");
  }
  #ifdef Dim3
  if (dimension==2) {
    matrix(4,4) += matrix(0,0);
    matrix(4,5) += matrix(0,1);
    matrix(4,6) += matrix(0,2);
    matrix(4,7) += matrix(0,3);

    matrix(5,4) += matrix(1,4);
    matrix(5,5) += matrix(1,1);
    matrix(5,6) += matrix(1,2);
    matrix(5,7) += matrix(1,3);

    matrix(6,4) += matrix(2,4);
    matrix(6,5) += matrix(2,1);
    matrix(6,6) += matrix(2,2);
    matrix(6,7) += matrix(2,3);

    matrix(7,4) += matrix(3,4);
    matrix(7,5) += matrix(3,1);
    matrix(7,6) += matrix(3,2);
    matrix(7,7) += matrix(3,3);
  }
  #endif
  else {
    assertionMsg(false, "not implemented yet");
  }
}


void matrixfree::stencil::createDownwindBoundaryMatrix( ElementWiseAssemblyMatrix& matrix, int dimension ) {
  if (dimension==0) {
    assertionMsg(false, "not implemented yet");
  }
  #ifdef Dim3
  if (dimension==2) {
/*
    matrix(0,0) += matrix(4,4);
    matrix(0,1) += matrix(4,5);
    matrix(0,2) += matrix(4,6);
    matrix(0,3) += matrix(4,7);

    matrix(1,0) += matrix(5,4);
    matrix(1,1) += matrix(5,5);
    matrix(1,2) += matrix(5,6);
    matrix(1,3) += matrix(5,7);

    matrix(2,0) += matrix(6,4);
    matrix(2,1) += matrix(6,5);
    matrix(2,2) += matrix(6,6);
    matrix(2,3) += matrix(6,7);

    matrix(3,0) += matrix(7,4);
    matrix(3,1) += matrix(7,5);
    matrix(3,2) += matrix(7,6);
    matrix(3,3) += matrix(7,7);
*/
    matrix(0,0) *= 4.0;
    matrix(0,1) *= 4.0;
    matrix(0,2) *= 4.0;
    matrix(0,3) *= 4.0;

    matrix(1,0) *= 4.0;
    matrix(1,1) *= 4.0;
    matrix(1,2) *= 4.0;
    matrix(1,3) *= 4.0;

    matrix(2,0) *= 4.0;
    matrix(2,1) *= 4.0;
    matrix(2,2) *= 4.0;
    matrix(2,3) *= 4.0;

    matrix(3,0) *= 4.0;
    matrix(3,1) *= 4.0;
    matrix(3,2) *= 4.0;
    matrix(3,3) *= 4.0;

  }
  #endif
  else {
    assertionMsg(false, "not implemented yet");
  }
}
