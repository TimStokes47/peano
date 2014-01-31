#include "tarch/mpianalysis/DefaultAnalyser.h"
#include "tarch/parallel/Node.h"


tarch::logging::Log  tarch::mpianalysis::DefaultAnalyser::_log( "tarch::mpianalysis::DefaultAnalyser" );


tarch::mpianalysis::DefaultAnalyser::DefaultAnalyser():
  _watch( "tarch::mpianalysis::DefaultAnalyser", "-", false ) {
}


tarch::mpianalysis::DefaultAnalyser::~DefaultAnalyser() {
}


void tarch::mpianalysis::DefaultAnalyser::beginIteration() {
  _watch.stopTimer();

  logInfo(
    "beginIteration()",
    "idle time=" <<
    _watch.getCalendarTime() <<
    ", idle cpu time=" <<
    _watch.getCPUTime()
  );

  _watch.startTimer();
}


void tarch::mpianalysis::DefaultAnalyser::endIteration() {
  _watch.stopTimer();

  logInfo(
    "endIteration()",
    "busy time=" <<
    _watch.getCalendarTime() <<
    ", busy cpu time=" <<
    _watch.getCPUTime()
  );

  _watch.startTimer();
}



void tarch::mpianalysis::DefaultAnalyser::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<2,double>&  boundingBoxOffset,
  const tarch::la::Vector<2,double>&  boundingBoxSize
) {

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
}


void tarch::mpianalysis::DefaultAnalyser::addWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
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
}


void tarch::mpianalysis::DefaultAnalyser::removeWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<3,double>&  boundingBoxOffset,
  const tarch::la::Vector<3,double>&  boundingBoxSize
) {
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
}


void tarch::mpianalysis::DefaultAnalyser::removeWorker(
  int                                 workerRank,
  int                                 level,
  const tarch::la::Vector<2,double>&  boundingBoxOffset,
  const tarch::la::Vector<2,double>&  boundingBoxSize
) {
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
}


void tarch::mpianalysis::DefaultAnalyser::tagIsUsedFor( int tag, const std::string& communicationTypeIdentifier ) {
  if (
    tarch::parallel::Node::getInstance().isInitialised() && tarch::parallel::Node::getInstance().isGlobalMaster()
  ) {
    logInfo(
      "reserveFreeTag()",
      "assigned message " << communicationTypeIdentifier
       << " the free tag " << tag
    );
  }
}


void tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedFromWorker( int fromRank, double calendarTime ) {
  logInfo(
    "dataWasNotReceivedFromWorker()",
    "rank had to wait for worker " << fromRank <<
    " for " << calendarTime <<
    "s"
  );
}


void tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedInBackground( int fromRank, int tag, int cardinality, int pageSize ) {
  logInfo(
    "dataWasNotReceivedInBackground()",
    "rank had to wait for " << cardinality <<
    " record(s) from " << fromRank <<
    " on tag " << tag <<
    " with page size " << pageSize
  );
}


void tarch::mpianalysis::DefaultAnalyser::logNodePoolStatistics(int registeredWorkers, int idleWorkers) {
}
