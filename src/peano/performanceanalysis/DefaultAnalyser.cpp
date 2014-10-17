#include "peano/performanceanalysis/DefaultAnalyser.h"
#include "tarch/parallel/Node.h"
#include "tarch/logging/CommandLineLogger.h"


tarch::logging::Log  peano::performanceanalysis::DefaultAnalyser::_log( "peano::performanceanalysis::DefaultAnalyser" );


peano::performanceanalysis::DefaultAnalyser::DefaultAnalyser():
  _totalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _traversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _actualDomainTraversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _waitForWorkerDataWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _synchronousHeapWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _asynchronousHeapWatch("peano::performanceanalysis::DefaultAnalyser", "-", false) {
  if (!tarch::logging::CommandLineLogger::getInstance().getLogMachineName()) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as logging of machine name is disabled. See command line logger" );
  }
  if (!tarch::logging::CommandLineLogger::getInstance().getLogTrace()) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as logging of trace is disabled. See command line logger" );
  }
  if (tarch::logging::CommandLineLogger::getInstance().filterOut("info","peano::performanceanalysis::DefaultAnalyser")) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as log filters for peano::performanceanalysis::DefaultAnalyser are installed" );
  }
}


peano::performanceanalysis::DefaultAnalyser::~DefaultAnalyser() {
}


void peano::performanceanalysis::DefaultAnalyser::beginIteration() {
  _traversalWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endIteration(double numberOfInnerLeafCells, double numberOfOuterLeafCells, double numberOfInnerCells, double numberOfOuterCells, double numberOfLocalCells, double numberOfLocalVertices) {
  _traversalWatch.stopTimer();
  _totalWatch.stopTimer();

  logInfo( "endIteration()", "cells=(" << numberOfInnerLeafCells << "," << numberOfOuterLeafCells << "," << numberOfInnerCells << "," << numberOfOuterCells << "," << numberOfLocalCells << ")" );
  logInfo( "endIteration()", "local-vertices=" << numberOfLocalVertices );
  logInfo( "endIteration()", "t_total=(" << _totalWatch.getCalendarTime() << "," << _totalWatch.getCPUTime() << ")" );
  logInfo( "endIteration()", "t_traversal=(" << _traversalWatch.getCalendarTime() << "," << _traversalWatch.getCPUTime() << ")" );

  _totalWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::enterCentralElementOfEnclosingSpacetree() {
  _actualDomainTraversalWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree() {
  _actualDomainTraversalWatch.stopTimer();
  logInfo( "leaveCentralElementOfEnclosingSpacetree()", "t_central-tree-traversal=(" << _actualDomainTraversalWatch.getCalendarTime() << "," << _actualDomainTraversalWatch.getCPUTime() << ")" );
}


void peano::performanceanalysis::DefaultAnalyser::addWorker(
  int                                 workerRank,
  int                                 level
) {
  logInfo(
    "addWorker(int,Vector,Vector)",
    tarch::parallel::Node::getInstance().getRank()
    << "->"
    << tarch::parallel::Node::getInstance().getRank()
    << "+"
    << workerRank
    << " [level:"
    << level
    << "]"
  );
}


void peano::performanceanalysis::DefaultAnalyser::removeWorker(
  int                                 workerRank,
  int                                 level
) {
  logInfo(
    "removeWorker()",
    tarch::parallel::Node::getInstance().getRank()
    << "+"
    << workerRank
    << "->"
    << tarch::parallel::Node::getInstance().getRank()
    << " [level:"
    << level
    << "]"
  );
}


void peano::performanceanalysis::DefaultAnalyser::beginToReceiveDataFromWorker() {
  _waitForWorkerDataWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endToReceiveDataFromWorker( int fromRank ) {
  _waitForWorkerDataWatch.stopTimer();
  const double elapsedTime = _waitForWorkerDataWatch.getCalendarTime();

  if (tarch::la::greater(elapsedTime,0.0)) {
    logInfo(
      "endToReceiveDataFromWorker()",
      "rank had to wait for worker " << fromRank <<
      " for " << elapsedTime <<
      "s"
    );
  }
  _waitForWorkerDataWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize ) {
  logInfo(
    "dataWasNotReceivedInBackground()",
    "rank had to wait for " << cardinality <<
    " record(s) from " << fromRank <<
    " on tag " << tag <<
    " with page size " << pageSize
  );
}


void peano::performanceanalysis::DefaultAnalyser::beginToReleaseSynchronousHeapData() {
  _synchronousHeapWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endToReleaseSynchronousHeapData() {
  _synchronousHeapWatch.stopTimer();

  logInfo(
    "endToReleaseSynchronousHeapData()",
    "time=" <<
    _synchronousHeapWatch.getCalendarTime() <<
    ", cpu time=" <<
    _synchronousHeapWatch.getCPUTime()
  );
}


void peano::performanceanalysis::DefaultAnalyser::beginToPrepareAsynchronousHeapDataExchange() {
  _asynchronousHeapWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange() {
  _asynchronousHeapWatch.stopTimer();

  logInfo(
    "endToPrepareAsynchronousHeapDataExchange()",
    "time=" <<
    _asynchronousHeapWatch.getCalendarTime() <<
    ", cpu time=" <<
    _asynchronousHeapWatch.getCPUTime()
  );
}


void peano::performanceanalysis::DefaultAnalyser::endReleaseOfJoinData() {
  _traversalWatch.stopTimer();

  logInfo(
    "endReleaseOfJoinData()",
    "time=" <<
    _traversalWatch.getCalendarTime() <<
    ", cpu time=" <<
    _traversalWatch.getCPUTime()
  );

  _traversalWatch.startTimer();
}


void peano::performanceanalysis::DefaultAnalyser::endReleaseOfBoundaryData() {
  _traversalWatch.stopTimer();

  logInfo(
    "endReleaseOfBoundaryData()",
    "time=" <<
    _traversalWatch.getCalendarTime() <<
    ", cpu time=" <<
    _traversalWatch.getCPUTime()
  );

  _traversalWatch.startTimer();
}
