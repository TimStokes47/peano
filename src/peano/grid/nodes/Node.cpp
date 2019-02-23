#include "Node.h"


/**
 * @todo Muss in den Header hoch
 *
 * We do refine along the domain boundaries if this flag holds. This is
 * important as it allows the code to partition along the boundary, too.
 * Peano forks away cells if and only if all of a cell's vertices are
 * refined. So we do refine cells next to the boundary artificially.
 * This is realised in Node::updateCellsGeometryInformationAfterLoad().
 *
 * If we naively refined in updateCellsGeometryInformationAfterLoad(),
 * we would introduce grid oscillations. We refine and then the
 * LoadVertexLoopBody::updateGeometry() would immediately erase again. So
 * this routine checks against refineArtificiallyOutsideDomain(), too, and
 * stops any refinement outside of the domain as long as this routine holds.
 *
 *
 */
bool peano::grid::nodes::refineArtificiallyOutsideDomain() {
  #ifdef Parallel
  return true;
  #else
  return false;
  #endif
}
