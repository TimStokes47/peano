#include "multigrid/Cell.h"


multigrid::Cell::Cell():
  Base() { 
  // @todo Insert your code here
}


multigrid::Cell::Cell(const Base::DoNotCallStandardConstructor& value):
  Base(value) { 
  // Please do not insert anything here
}

multigrid::Cell::Cell(const Base::PersistentCell& argument):
  Base(argument) {
  // @todo Insert your code here
}


double multigrid::Cell::getEpsilon() const {
    return _cellData.getEpsilon();
}


double multigrid::Cell::getF() const {
  return _cellData.getF();
}


tarch::la::Vector<DIMENSIONS,double> multigrid::Cell::getV() const {
  return _cellData.getV();
}


void multigrid::Cell::init(double epsilon, double f, const tarch::la::Vector<DIMENSIONS,double>& v) {
  _cellData.setEpsilon(epsilon);
  _cellData.setF(f);
  _cellData.setV(v);
}

