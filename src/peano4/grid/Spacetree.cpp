#include "Spacetree.h"
#include "tarch/utils/Loop.h"


tarch::logging::Log  peano4::grid::Spacetree::_log( "peano4::grid::Spacetree" );



peano4::grid::Spacetree::Spacetree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width):
  _root(
    0, // level
    offset,
    width
  ) {
  logInfo( "Spacetree(...)", "create spacetree with " << offset << "x" << width );
}


peano4::grid::Spacetree peano4::grid::Spacetree::createTrivialTree(const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width) {
  Spacetree result(offset,width);

  dfor(k)
  enddfor

  return result;
}
