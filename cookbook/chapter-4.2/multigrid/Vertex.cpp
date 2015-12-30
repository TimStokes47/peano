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


void multigrid::Vertex::initInnerVertex(double f) {
  _vertexData.setVertexType( Records::Unknown );
  _vertexData.setU(0.0);
  _vertexData.setF(f);
}


void multigrid::Vertex::initDirichletVertex(double u) {
  _vertexData.setVertexType( Records::Unknown );
  _vertexData.setU(u);
}


double multigrid::Vertex::getF() const {
  return _vertexData.getF();
}


double multigrid::Vertex::getR() const {
  return _vertexData.getR();
}



double multigrid::Vertex::getU() const {
  return _vertexData.getU();
}


void multigrid::Vertex::clearAccumulatedAttributes() {
  _vertexData.setR(0.0);
  _vertexData.setD(0.0);
}


void multigrid::Vertex::performJacobiSmoothingStep( double omega ) {
  assertion1( _vertexData.getD()>0.0, toString() );
  assertion2( omega>0.0, toString(), omega );

  _vertexData.setU( _vertexData.getU() + omega / _vertexData.getD() * _vertexData.getR() );
}
