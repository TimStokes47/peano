#include "petsc/Vertex.h"
#include "peano/utils/Loop.h"
#include "peano/grid/Checkpoint.h"


petsc::Vertex::Vertex():
  Base() { 
  // @todo Insert your code here
}


petsc::Vertex::Vertex(const Base::DoNotCallStandardConstructor& value):
  Base(value) { 
  // Please do not insert anything here
}


petsc::Vertex::Vertex(const Base::PersistentVertex& argument):
  Base(argument) {
  // @todo Insert your code here
}


double petsc::Vertex::getU() const {
  return 0.0;
}

