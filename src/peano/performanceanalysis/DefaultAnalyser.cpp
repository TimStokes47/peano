#include "peano/performanceanalysis/DefaultAnalyser.h"
#include "tarch/parallel/Node.h"


tarch::logging::Log  peano::performanceanalysis::DefaultAnalyser::_log( "peano::performanceanalysis::DefaultAnalyser" );


peano::performanceanalysis::DefaultAnalyser::DefaultAnalyser():
  _traversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _actualDomainTraversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false) {
/*
  ,
}
  _synchronousHeapWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _asynchronousHeapWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _actualDomainTraversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false) {
*/
}


peano::performanceanalysis::DefaultAnalyser::~DefaultAnalyser() {
}


void peano::performanceanalysis::DefaultAnalyser::beginIteration() {
  logInfo( "beginIteration()", "was here" );
/*
  _watch.stopTimer();

  logInfo(
    "beginIteration()",
    "idle time=" <<
    _watch.getCalendarTime() <<
    ", idle cpu time=" <<
    _watch.getCPUTime()
  );

  _watch.startTimer();
*/
}


void peano::performanceanalysis::DefaultAnalyser::endIteration() {
  logInfo( "beginIteration()", "was here" );
/*
  _watch.stopTimer();

  logInfo(
    "endIteration()",
    "busy time=" <<
    _watch.getCalendarTime(), <<
    ", busy cpu time=" <<
    _watch.getCPUTime()
  );

  _watch.startTimer();
*/
}



void peano::performanceanalysis::DefaultAnalyser::enterCentralElementOfEnclosingSpacetree() {
//  _actualDomainTraversalWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree() {
/*
  _actualDomainTraversalWatch.stopTimer();

  logInfo(
    "leaveCentralElementOfEnclosingSpacetree()",
    "time=" <<
    _actualDomainTraversalWatch.getCalendarTime() <<
    ", cpu time=" <<
    _actualDomainTraversalWatch.getCPUTime()
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<2,double>&  boundingBoxOffset,
  const tarch::la::Vector<2,double>&  boundingBoxSize
) {
/*

  logInfo(
    "addWorker(int,Vector,Vector)",
    tarch::parallel::Node::getInstance().getRank()
    << "->"
    << tarch::parallel::Node::getInstance().getRank()
    << "+"
    << workerRank
    << " [worker's domain:"
    << boundingBoxOffset
    << "x"
    << boundingBoxSize
    << ",level:"
    << level
    << "]"
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
/*
  logInfo(
    "addWorker(int,Vector,Vector)",
    tarch::parallel::Node::getInstance().getRank()
    << "->"
    << tarch::parallel::Node::getInstance().getRank()
    << "+"
    << workerRank
    << " [worker's domain:"
    << boundingBoxOffset
    << "x"
    << boundingBoxSize
    << ",level:"
    << level
    << "]"
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::removeWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
/*
  logInfo(
    "removeWorker()",
    tarch::parallel::Node::getInstance().getRank()
    << "+"
    << workerRank
    << "->"
    << tarch::parallel::Node::getInstance().getRank()
    << " [worker's domain:"
    << boundingBoxOffset
    << "x"
    << boundingBoxSize
    << ",level:"
    << level
    << "]"
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::removeWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<2,double>&  boundingBoxOffset,
  const tarch::la::Vector<2,double>&  boundingBoxSize
) {
/*
  logInfo(
    "removeWorker()",
    tarch::parallel::Node::getInstance().getRank()
    << "+"
    << workerRank
    << "->"
    << tarch::parallel::Node::getInstance().getRank()
    << " [worker's domain:"
    << boundingBoxOffset
    << "x"
    << boundingBoxSize
    << ",level:"
    << level
    << "]"
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::dataWasNotReceivedFromWorker( int fromRank, double calendarTime ) {
/*
  logInfo(
    "dataWasNotReceivedFromWorker()",
    "rank had to wait for worker " << fromRank <<
    " for " << calendarTime <<
    "s"
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize ) {
/*
  logInfo(
    "dataWasNotReceivedInBackground()",
    "rank had to wait for " << cardinality <<
    " record(s) from " << fromRank <<
    " on tag " << tag <<
    " with page size " << pageSize
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::beginToReleaseSynchronousHeapData() {
//  _synchronousHeapWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endToReleaseSynchronousHeapData() {
/*
  _synchronousHeapWatch.stopTimer();

  logInfo(
    "endToReleaseSynchronousHeapData()",
    "time=" <<
    _synchronousHeapWatch.getCalendarTime() <<
    ", cpu time=" <<
    _synchronousHeapWatch.getCPUTime()
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::beginToPrepareAsynchronousHeapDataExchange() {
//  _asynchronousHeapWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange() {
/*
  _asynchronousHeapWatch.stopTimer();

  logInfo(
    "endToPrepareAsynchronousHeapDataExchange()",
    "time=" <<
    _asynchronousHeapWatch.getCalendarTime() <<
    ", cpu time=" <<
    _asynchronousHeapWatch.getCPUTime()
  );
*/
}


void peano::performanceanalysis::DefaultAnalyser::endReleaseOfJoinData() {
/*
  _watch.stopTimer();

  logInfo(
    "endReleaseOfJoinData()",
    "time=" <<
    _watch.getCalendarTime() <<
    ", cpu time=" <<
    _watch.getCPUTime()
  );

  _watch.startTimer();
*/
}


void peano::performanceanalysis::DefaultAnalyser::endReleaseOfBoundaryData() {
/*
  _watch.stopTimer();

  logInfo(
    "endReleaseOfBoundaryData()",
    "time=" <<
    _watch.getCalendarTime() <<
    ", cpu time=" <<
    _watch.getCPUTime()
  );

  _watch.startTimer();
*/
}
