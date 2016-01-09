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
