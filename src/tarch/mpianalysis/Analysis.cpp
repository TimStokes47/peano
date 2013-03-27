#include "tarch/mpianalysis/Analysis.h"
#include "tarch/mpianalysis/DefaultAnalyser.h"
#include "tarch/Assertions.h"


tarch::mpianalysis::Analysis::Analysis():
  _device(0) {
  _device = new DefaultAnalyser();
}

tarch::mpianalysis::Analysis::~Analysis() {
  deleteDevice();
}


tarch::mpianalysis::Analysis& tarch::mpianalysis::Analysis::getInstance() {
  static Analysis singleton;
  return singleton;
}


void tarch::mpianalysis::Analysis::deleteDevice() {
  if (_device!=0) {
    delete _device;
  }
}


void tarch::mpianalysis::Analysis::setDevice( Analyser* device ) {
  assertion( device!=0 );
  deleteDevice();
  _device = device;
}
