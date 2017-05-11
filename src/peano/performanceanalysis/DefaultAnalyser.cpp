#include "peano/performanceanalysis/DefaultAnalyser.h"

#include "tarch/parallel/Node.h"
#include "tarch/logging/CommandLineLogger.h"

#include "tarch/multicore/Lock.h"

#ifdef UseScoreP
#include <scorep/SCOREP_User.h>
#endif


tarch::logging::Log  peano::performanceanalysis::DefaultAnalyser::_log( "peano::performanceanalysis::DefaultAnalyser" );

double peano::performanceanalysis::DefaultAnalyser::MinTimeInBetweenTwoConcurrencyLogs( 1.0e-2 );
double peano::performanceanalysis::DefaultAnalyser::TimeInBetweenTwoConcurrencyDataDumps( 2.0 );


peano::performanceanalysis::DefaultAnalyser::DefaultAnalyser():
  _isSwitchedOn(true),
  _totalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _traversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _actualDomainTraversalWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _waitForWorkerDataWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _synchronousHeapWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _asynchronousHeapWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _concurrencyReportWatch("peano::performanceanalysis::DefaultAnalyser", "-", false),
  _currentConcurrencyLevel(1),
  _currentPotentialConcurrencyLevel(1),
  _maxConcurrencyLevel(1),
  _maxPotentialConcurrencyLevel(1),
  _timeAveragedConcurrencyLevel(1),
  _timeAveragedPotentialConcurrencyLevel(1),
  _numberOfSpawnedBackgroundTask(0),
  _lastConcurrencyDataUpdateRealTimeStamp(0),
  _lastConcurrencyDataWriteRealTimeStamp(0),
  _lastConcurrencyDataWriteCPUTimeStamp(0) {
  if (!tarch::logging::CommandLineLogger::getInstance().getLogMachineName() && tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as logging of machine name is disabled. See command line logger" );
  }
  if (!tarch::logging::CommandLineLogger::getInstance().getLogTrace() && tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as logging of trace is disabled. See command line logger" );
  }
  if (!tarch::logging::CommandLineLogger::getInstance().getLogTimeStamp() && tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as logging of time stamps is disabled. See command line logger" );
  }
  if (tarch::logging::CommandLineLogger::getInstance().filterOut("info","peano::performanceanalysis::DefaultAnalyser") && tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    logWarning( "DefaultAnalyser()", "performance analysis might yield invalid results as log filters for peano::performanceanalysis::DefaultAnalyser are installed" );
  }

  enable(true);
}


peano::performanceanalysis::DefaultAnalyser::~DefaultAnalyser() {
}



void peano::performanceanalysis::DefaultAnalyser::enable(bool value) {
  _isSwitchedOn=value;
  _concurrencyReportWatch.startTimer();

  #ifdef UseScoreP
  if (value) {
    SCOREP_RECORDING_ON()
  }
  else {
    SCOREP_RECORDING_OFF()
  }
  #endif
}


void peano::performanceanalysis::DefaultAnalyser::beginIteration() {
  if (_isSwitchedOn) {
    logInfo( "beginIteration()", "start traversal" );
    _traversalWatch.startTimer();
  }
}


void peano::performanceanalysis::DefaultAnalyser::endIteration(double numberOfInnerLeafCells, double numberOfOuterLeafCells, double numberOfInnerCells, double numberOfOuterCells, double numberOfLocalCells, double numberOfLocalVertices) {
  if (_isSwitchedOn) {
    _traversalWatch.stopTimer();
    _totalWatch.stopTimer();

    logInfo( "endIteration()", "cells=(" << numberOfInnerLeafCells << "," << numberOfOuterLeafCells << "," << numberOfInnerCells << "," << numberOfOuterCells << "," << numberOfLocalCells << ")" );
    logInfo( "endIteration()", "local-vertices=" << numberOfLocalVertices );
    logInfo( "endIteration()", "t_total=(" << _totalWatch.getCalendarTime() << "," << _totalWatch.getCPUTime() << ")" );
    logInfo( "endIteration()", "t_traversal=(" << _traversalWatch.getCalendarTime() << "," << _traversalWatch.getCPUTime() << ")" );

    _totalWatch.startTimer();
    // is required by join time analysis
    _traversalWatch.startTimer();
  }
}


void peano::performanceanalysis::DefaultAnalyser::enterCentralElementOfEnclosingSpacetree() {
  if (_isSwitchedOn) {
    logInfo( "enterCentralElementOfEnclosingSpacetree()", "enter central element" );
    _actualDomainTraversalWatch.startTimer();
  }
}


void peano::performanceanalysis::DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree() {
  if (_isSwitchedOn) {
    _actualDomainTraversalWatch.stopTimer();
    logInfo( "leaveCentralElementOfEnclosingSpacetree()", "t_central-tree-traversal=(" << _actualDomainTraversalWatch.getCalendarTime() << "," << _actualDomainTraversalWatch.getCPUTime() << ")" );
  }
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
  if (_isSwitchedOn) {
    _waitForWorkerDataWatch.startTimer();
  }
}


void peano::performanceanalysis::DefaultAnalyser::endToReceiveDataFromWorker( int fromRank ) {
  if (_isSwitchedOn) {
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
}


void peano::performanceanalysis::DefaultAnalyser::dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize ) {
  if (_isSwitchedOn) {
    logInfo(
      "dataWasNotReceivedInBackground()",
      "rank had to wait for " << cardinality <<
      " record(s) from " << fromRank <<
      " on tag " << tag <<
      " with page size " << pageSize
    );
  }
}


void peano::performanceanalysis::DefaultAnalyser::beginToReleaseSynchronousHeapData() {
  if (_isSwitchedOn) {
    _synchronousHeapWatch.startTimer();
  }
}


void peano::performanceanalysis::DefaultAnalyser::endToReleaseSynchronousHeapData() {
  if (_isSwitchedOn) {
    _synchronousHeapWatch.stopTimer();

    logInfo(
      "endToReleaseSynchronousHeapData()",
      "time=" <<
      _synchronousHeapWatch.getCalendarTime() <<
      ", cpu time=" <<
      _synchronousHeapWatch.getCPUTime()
    );
  }
}


void peano::performanceanalysis::DefaultAnalyser::beginToPrepareAsynchronousHeapDataExchange() {
  if (_isSwitchedOn) {
    _asynchronousHeapWatch.startTimer();
  }
}


void peano::performanceanalysis::DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange() {
  if (_isSwitchedOn) {
    _asynchronousHeapWatch.stopTimer();

    logInfo(
      "endToPrepareAsynchronousHeapDataExchange()",
      "time=" <<
      _asynchronousHeapWatch.getCalendarTime() <<
      ", cpu time=" <<
      _asynchronousHeapWatch.getCPUTime()
    );
  }
}


void peano::performanceanalysis::DefaultAnalyser::endReleaseOfJoinData() {
  if (_isSwitchedOn) {
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
}


void peano::performanceanalysis::DefaultAnalyser::endReleaseOfBoundaryData() {
  if (_isSwitchedOn) {
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
}


void peano::performanceanalysis::DefaultAnalyser::changeConcurrencyLevel(int actualChange, int maxPossibleChange) {
  if (_isSwitchedOn) {
/*
    actualChange      = actualChange>0      ? actualChange-1 : actualChange+1;
    maxPossibleChange = maxPossibleChange>0 ? maxPossibleChange-1 : maxPossibleChange+1;
*/
    tarch::multicore::Lock lock(_concurrencyReportSemaphore);

    assertion4(
      actualChange==0
      ||
      (actualChange<0 && maxPossibleChange<0)
      ||
      (actualChange>0 && maxPossibleChange>0),
      actualChange, maxPossibleChange, _currentConcurrencyLevel, _currentPotentialConcurrencyLevel
    );

    _currentConcurrencyLevel             += actualChange;
    _currentPotentialConcurrencyLevel    += maxPossibleChange;

    assertion4( _currentConcurrencyLevel>=0,          actualChange, maxPossibleChange, _currentConcurrencyLevel, _currentPotentialConcurrencyLevel);
    assertion4( _currentPotentialConcurrencyLevel>=0, actualChange, maxPossibleChange, _currentConcurrencyLevel, _currentPotentialConcurrencyLevel);
    assertion4( _currentConcurrencyLevel<=_currentPotentialConcurrencyLevel, _currentConcurrencyLevel, _currentPotentialConcurrencyLevel, actualChange, maxPossibleChange );

    _concurrencyReportWatch.stopTimer();

    const double deltaToLastDataUpdate  = _concurrencyReportWatch.getCalendarTime() - _lastConcurrencyDataUpdateRealTimeStamp;
    if (
      deltaToLastDataUpdate>MinTimeInBetweenTwoConcurrencyLogs
      &&
      _concurrencyReportWatch.getCPUTime()>0
    ) {
      _maxConcurrencyLevel          = _maxConcurrencyLevel          > _currentConcurrencyLevel          ? _maxConcurrencyLevel          : _currentConcurrencyLevel;
      _maxPotentialConcurrencyLevel = _maxPotentialConcurrencyLevel > _currentPotentialConcurrencyLevel ? _maxPotentialConcurrencyLevel : _currentPotentialConcurrencyLevel;

      _timeAveragedConcurrencyLevel          += _currentConcurrencyLevel          * deltaToLastDataUpdate;
      _timeAveragedPotentialConcurrencyLevel += _currentPotentialConcurrencyLevel * deltaToLastDataUpdate;

      _lastConcurrencyDataUpdateRealTimeStamp = _concurrencyReportWatch.getCalendarTime();
    }

    const double deltaRealTimeToLastDataWrite = _concurrencyReportWatch.getCalendarTime() - _lastConcurrencyDataWriteRealTimeStamp;
    const double deltaCPUTimeToLastDataWrite  = _concurrencyReportWatch.getCPUTime()      - _lastConcurrencyDataWriteCPUTimeStamp;

    if ( deltaRealTimeToLastDataWrite>TimeInBetweenTwoConcurrencyDataDumps ) {
      logInfo(
        "changeConcurrencyLevel(int,int)",
        "dt_real=" << deltaRealTimeToLastDataWrite <<
        ", dt_cpu=" << deltaCPUTimeToLastDataWrite <<
        ", time-averaged-concurrency-level=" << _timeAveragedConcurrencyLevel <<
        ", time-averaged-potential-concurrency-level=" << _timeAveragedPotentialConcurrencyLevel <<
        ", max-concurrency-level=" << _maxConcurrencyLevel <<
        ", max-potential-concurrency-level=" << _maxPotentialConcurrencyLevel <<
        ", background-tasks=" << _numberOfSpawnedBackgroundTask
      );

      _numberOfSpawnedBackgroundTask      = 0;

      _timeAveragedConcurrencyLevel          = 0;
      _timeAveragedPotentialConcurrencyLevel = 0;

      _maxConcurrencyLevel          = 0;
      _maxPotentialConcurrencyLevel = 0;

      _lastConcurrencyDataWriteRealTimeStamp = _concurrencyReportWatch.getCalendarTime();
      _lastConcurrencyDataWriteCPUTimeStamp  = _concurrencyReportWatch.getCPUTime();

      // to ensure that measurement inaccuracies do not immediately afterwards trigger update
      _lastConcurrencyDataUpdateRealTimeStamp = _concurrencyReportWatch.getCalendarTime();
    }
  }
}


void peano::performanceanalysis::DefaultAnalyser::fireAndForgetBackgroundTask(int taskCount) {
  if (_isSwitchedOn) {
    tarch::multicore::Lock lock(_concurrencyReportSemaphore);

    _numberOfSpawnedBackgroundTask += taskCount;
  }
}
