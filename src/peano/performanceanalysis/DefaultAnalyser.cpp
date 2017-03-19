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
  _currentMaxPotentialConcurrencyLevel(1),
  _numberOfSpawnedBackgroundTask(0),
  _concurrentTimeSpent(0),
  _maxPotentialConcurrentTimeSpent(0),
  _maxConcurrencyLevel(1),
  _maxPotentialConcurrencyLevel(1),
  _lastConcurrencyDataUpdateTimeStamp(0),
  _lastConcurrencyDataWriteTimeStamp(0) {
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


#ifdef UseScoreP
SCOREP_USER_GLOBAL_REGION_DEFINE( ScoreP_Default_Analyser )
#endif


void peano::performanceanalysis::DefaultAnalyser::enable(bool value) {
  _isSwitchedOn=value;
  _concurrencyReportWatch.startTimer();

  #ifdef UseScoreP
  if (value) {
    SCOREP_USER_REGION_BEGIN( ScoreP_Default_Analyser, "Default Analyser", SCOREP_USER_REGION_TYPE_PHASE)
  }
  else {
    SCOREP_USER_REGION_END( ScoreP_Default_Analyser )
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
    tarch::multicore::Lock lock(_concurrencyReportSemaphore);

    assertion4(
      (actualChange>=0 && maxPossibleChange>0)
      ||
      (actualChange<=0 && maxPossibleChange<0),
      actualChange, maxPossibleChange, _currentConcurrencyLevel, _currentMaxPotentialConcurrencyLevel
    );

    _currentConcurrencyLevel             += actualChange;
    _currentMaxPotentialConcurrencyLevel += maxPossibleChange;

    assertion4( _currentConcurrencyLevel>=0,             actualChange, maxPossibleChange, _currentConcurrencyLevel, _currentMaxPotentialConcurrencyLevel);
    assertion4( _currentMaxPotentialConcurrencyLevel>=0, actualChange, maxPossibleChange, _currentConcurrencyLevel, _currentMaxPotentialConcurrencyLevel);

    _concurrencyReportWatch.stopTimer();

    const double deltaToLastDataUpdate  = _concurrencyReportWatch.getCalendarTime() - _lastConcurrencyDataUpdateTimeStamp;
    if (
      deltaToLastDataUpdate>MinTimeInBetweenTwoConcurrencyLogs
      &&
      _concurrencyReportWatch.getCPUTime()>0
    ) {
      _maxConcurrencyLevel          = _maxConcurrencyLevel          > _currentConcurrencyLevel             ? _maxConcurrencyLevel : _currentConcurrencyLevel;
      _maxPotentialConcurrencyLevel = _maxPotentialConcurrencyLevel > _currentMaxPotentialConcurrencyLevel ? _maxPotentialConcurrencyLevel : _currentMaxPotentialConcurrencyLevel;

      _concurrentTimeSpent             += _currentConcurrencyLevel             * deltaToLastDataUpdate;
      _maxPotentialConcurrentTimeSpent += _currentMaxPotentialConcurrencyLevel * deltaToLastDataUpdate;

      _lastConcurrencyDataUpdateTimeStamp = _concurrencyReportWatch.getCalendarTime();
    }

    const double deltaToLastDataWrite = _concurrencyReportWatch.getCalendarTime() - _lastConcurrencyDataWriteTimeStamp;
    if ( deltaToLastDataWrite>TimeInBetweenTwoConcurrencyDataDumps ) {
      _lastConcurrencyDataWriteTimeStamp = _concurrencyReportWatch.getCalendarTime();

      logInfo(
        "changeConcurrencyLevel(int,int)",
        "dt=" << _concurrencyReportWatch.getCalendarTime() <<
        ", cpu-time=" << _concurrencyReportWatch.getCPUTime() <<
        ", concurrent-time=" << _concurrentTimeSpent <<
        ", potential-concurrent-time=" << _maxPotentialConcurrentTimeSpent <<
        ", max-concurrency-level=" << _maxConcurrencyLevel <<
        ", max-potential-concurrency-level=" << _maxPotentialConcurrencyLevel <<
        ", background-tasks=" << _numberOfSpawnedBackgroundTask
      );

      _numberOfSpawnedBackgroundTask      = 0;

      _concurrentTimeSpent                = 0.0;
      _maxPotentialConcurrentTimeSpent    = 0.0;

      _maxConcurrencyLevel          = 0;
      _maxPotentialConcurrencyLevel = 0;
    }
  }
}


void peano::performanceanalysis::DefaultAnalyser::fireAndForgetBackgroundTask(int taskCount) {
  if (_isSwitchedOn) {
    tarch::multicore::Lock lock(_concurrencyReportSemaphore);

    _numberOfSpawnedBackgroundTask += taskCount;
  }
}
