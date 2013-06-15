#include "peano/grid/nodes/tasks/LoadVerticesOnRegularRefinedPatch.h"



bool peano::grid::nodes::tasks::isVertexOnLevel1DataDecompositionBoundaryInRegularTree(
  tarch::la::Vector<DIMENSIONS,int>   vertexPosition,
  int                                 numberOfCellsPerDimension,
  const std::bitset<THREE_POWER_D>    forkedSubtree
) {
//  vertexPosition = vertexPosition + cellsVertexEnumerator.getOffset();
//  tarch::la::Vector<DIMENSIONS,int> offset;
//  tarch::la::Vector<DIMENSIONS,int> span;
//  for (int d=0; d<DIMENSIONS; d++) {
//    if (vertexPosition(d) == 0) {
//      span(d)   = 1;
//      offset(d) = 0;
//    }
//    else if (vertexPosition(d)*3/1 == cellsVertexEnumerator.getCellsPerAxis()) {
//      span(d)   = 2;
//      offset(d) = 0;
//    }
//    else if (vertexPosition(d)*3/2 == cellsVertexEnumerator.getCellsPerAxis()) {
//      span(d)   = 2;
//      offset(d) = 1;
//    }
//    else if (vertexPosition(d)*3/3 == cellsVertexEnumerator.getCellsPerAxis()) {
//      span(d)   = 1;
//      offset(d) = 2;
//    }
//    else return false;
//  }
//
//  // at least one entry should be greater than 1
//  if (tarch::la::max(span)==1) {
//    return false;
//  }
//  else {
//    dfor(k,span) {
//      const tarch::la::Vector<DIMENSIONS,int> p = offset + k;
//      if (forkedSubtree[peano::utils::dLinearised(p,3)]) return true;
//    }
//    return false;
//  }
  bool result = tarch::la::oneEquals(vertexPosition,0) || tarch::la::oneEquals(vertexPosition,numberOfCellsPerDimension);

  if (result) {
    tarch::la::Vector<DIMENSIONS,int> a;
    tarch::la::Vector<DIMENSIONS,int> b;
    for (int d=0; d<DIMENSIONS; d++) {
      a(d) = (vertexPosition(d)-1) / (numberOfCellsPerDimension/3);
      b(d) = (vertexPosition(d)  ) / (numberOfCellsPerDimension/3);
      a(d) = a(d)<0 ? 0 : a(d);
      b(d) = b(d)>2 ? 2 : b(d);
    }

    result = false;
    dfor(k,b-a+1) {
      const tarch::la::Vector<DIMENSIONS,int> p = a + k;
      result |= forkedSubtree[peano::utils::dLinearised(p,3)];
    }
  }

  return result;
}
