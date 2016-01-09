#include "multigrid/Vertex.h"
#include "peano/utils/Loop.h"
#include "peano/grid/Checkpoint.h"


multigrid::Vertex::Vertex():
  Base() { 
  // @todo Insert your code here
}


multigrid::Vertex::Vertex(const Base::DoNotCallStandardConstructor& value):
  Base(value) { 
  // Please do not insert anything here
}


multigrid::Vertex::Vertex(const Base::PersistentVertex& argument):
  Base(argument) {
  // @todo Insert your code here
}
