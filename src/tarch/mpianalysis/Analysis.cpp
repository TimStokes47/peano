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


void tarch::mpianalysis::Analysis::beginIteration() {
  assertion( _device!=0 );
  _device->beginIteration();
}


void tarch::mpianalysis::Analysis::endIteration() {
  assertion( _device!=0 );
  _device->endIteration();
}


void tarch::mpianalysis::Analysis::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
  assertion( _device!=0 );
  _device->addWorker(workerRank,level,boundingBoxOffset,boundingBoxSize);
}


void tarch::mpianalysis::Analysis::removeWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
  assertion( _device!=0 );
  _device->removeWorker(workerRank,level,boundingBoxOffset,boundingBoxSize);
}


void tarch::mpianalysis::Analysis::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<2,double>&  boundingBoxOffset,
  const tarch::la::Vector<2,double>&  boundingBoxSize
) {
  assertion( _device!=0 );
  _device->addWorker(workerRank,level,boundingBoxOffset,boundingBoxSize);
}


void tarch::mpianalysis::Analysis::removeWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<2,double>&  boundingBoxOffset,
  const tarch::la::Vector<2,double>&  boundingBoxSize
) {
  assertion( _device!=0 );
  _device->removeWorker(workerRank,level,boundingBoxOffset,boundingBoxSize);
}


void tarch::mpianalysis::Analysis::tagIsUsedFor( int tag, const std::string& communicationTypeIdentifier ) {
  assertion( _device!=0 );
  _device->tagIsUsedFor(tag,communicationTypeIdentifier);
}


void tarch::mpianalysis::Analysis::dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize ) {
  assertion( _device!=0 );
  _device->dataWasNotReceivedInBackground(fromRank,tag,cardinality,pageSize);
}


void tarch::mpianalysis::Analysis::dataWasNotReceivedFromWorker( int fromRank, double calendarTime ) {
  assertion( _device!=0 );
  _device->dataWasNotReceivedFromWorker(fromRank,calendarTime);
}


void tarch::mpianalysis::Analysis::logNodePoolStatistics(int registeredWorkers, int idleWorkers) {
  assertion( _device!=0 );
  _device->logNodePoolStatistics(registeredWorkers, idleWorkers);
}


void tarch::mpianalysis::Analysis::beginToReleaseSynchronousHeapData() {
  assertion( _device!=0 );
  _device->beginToReleaseSynchronousHeapData();
}


void tarch::mpianalysis::Analysis::endToReleaseSynchronousHeapData() {
  assertion( _device!=0 );
  _device->endToReleaseSynchronousHeapData();
}


void tarch::mpianalysis::Analysis::beginToPrepareAsynchronousHeapDataExchange() {
  assertion( _device!=0 );
  _device->beginToPrepareAsynchronousHeapDataExchange();
}


void tarch::mpianalysis::Analysis::endToPrepareAsynchronousHeapDataExchange() {
  assertion( _device!=0 );
  _device->endToPrepareAsynchronousHeapDataExchange();
}


void tarch::mpianalysis::Analysis::endReleaseOfJoinData() {
  assertion( _device!=0 );
  _device->endReleaseOfJoinData();
}


void tarch::mpianalysis::Analysis::endReleaseOfBoundaryData() {
  assertion( _device!=0 );
  _device->endReleaseOfBoundaryData();
}
