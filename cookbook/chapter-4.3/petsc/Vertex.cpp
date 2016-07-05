#include "petsc/Vertex.h"
#include "peano/utils/Loop.h"
#include "peano/grid/Checkpoint.h"


/*
namespace petsc {
  class Vertex;

  *
   * Forward declaration

  class VertexOperations;

  *
   * These are the global vectors that we use to make the adapter communicate
   * with PETSc:

  extern Vec  x;
  extern Vec  rhs;
  extern Mat  A;
}
*/

Vec  petsc::x;
Vec  petsc::rhs;
Mat  petsc::A;



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

