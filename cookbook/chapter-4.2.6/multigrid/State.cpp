#include "multigrid/State.h"
#include "multigrid/Cell.h"
#include "multigrid/Vertex.h"

#include "peano/grid/Checkpoint.h"



multigrid::State::State():
  Base() { 
  // @todo Insert your code here
}


multigrid::State::State(const Base::PersistentState& argument):
  Base(argument) {
  // @todo Insert your code here
}


void multigrid::State::writeToCheckpoint( peano::grid::Checkpoint<multigrid::Vertex,multigrid::Cell>& checkpoint ) const {
  // @todo Insert your code here
}

    
void multigrid::State::readFromCheckpoint( const peano::grid::Checkpoint<multigrid::Vertex,multigrid::Cell>& checkpoint ) {
  // @todo Insert your code here
}
