#include "tarch/mpianalysis/DefaultAnalyser.h"


tarch::mpianalysis::DefaultAnalyser::DefaultAnalyser() {
}


tarch::mpianalysis::DefaultAnalyser::~DefaultAnalyser() {
}


void tarch::mpianalysis::DefaultAnalyser::beginIteration() {
}


void tarch::mpianalysis::DefaultAnalyser::endIteration() {
}


void tarch::mpianalysis::DefaultAnalyser::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
}
