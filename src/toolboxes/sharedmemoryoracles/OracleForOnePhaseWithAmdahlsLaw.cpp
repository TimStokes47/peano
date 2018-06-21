#include "sharedmemoryoracles/OracleForOnePhaseWithAmdahlsLaw.h"
#include "tarch/Assertions.h"
#include "peano/utils/Globals.h"

#include <cstdlib>
#include <math.h>
#include <fstream>


tarch::logging::Log  sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::_log( "sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw" );


sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::OracleForOnePhaseWithAmdahlsLaw():
  _executionTimeDatabase(),
  _sampleEveryXQueries(4),
  _sampleCounter(0) {
}


sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::~OracleForOnePhaseWithAmdahlsLaw() {
}


peano::datatraversal::autotuning::GrainSize sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::parallelise(int problemSize, peano::datatraversal::autotuning::MethodTrace askingMethod) {
  logTraceInWith1Argument( "parallelise(...)", problemSize );

  _sampleCounter++;
  _sampleCounter = _sampleCounter%_sampleEveryXQueries;

  const int baseSize = _executionTimeDatabase[askingMethod]._optimalGrainSize > problemSize ? 0 : _executionTimeDatabase[askingMethod]._optimalGrainSize;

  logDebug( "parallelise(...)", "optimal base size is " << baseSize );

  _executionTimeDatabase[askingMethod]._maxProblemSize = std::max( _executionTimeDatabase[askingMethod]._maxProblemSize, problemSize );

  if (_sampleCounter==0) {
    const int delta       = baseSize / 2;
    const int startSearch = std::max(1,baseSize-delta);
    return peano::datatraversal::autotuning::GrainSize(
      startSearch + (rand() % 3) * delta,
	  true,
      problemSize,
      askingMethod,
      this
    );
  }
  else {
    return peano::datatraversal::autotuning::GrainSize(
      baseSize, false,
      problemSize,
      askingMethod,
      this
    );
  }
}


void sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::parallelSectionHasTerminated(int problemSize, int grainSize, peano::datatraversal::autotuning::MethodTrace askingMethod, double costPerProblemElement) {//  assertion(_executionTimes.count(askingMethod)==1);
  _executionTimeDatabase[askingMethod]._statistics.addMeasurement(grainSize,costPerProblemElement);
}



void sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::loadStatistics(const std::string& filename, int oracleNumber) {
  logWarning( "loadStatistics(std::string)", "reload of statistics is not supported. Statistics file will be overwritten" );
}


void sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::plotStatistics(std::ostream& out, int oracleNumber) const {
  out << "begin OracleForOnePhaseWithAmdahlsLaw" << std::endl;
  out << "adapter-number=" << oracleNumber << std::endl
	  << "sample-delta=" << _sampleEveryXQueries << std::endl;

  for (auto measurement: _executionTimeDatabase) {
    out << peano::datatraversal::autotuning::toString(measurement.first)
        << "=" << measurement.second._optimalGrainSize
		<< "," << measurement.second._maxProblemSize
        << " " << measurement.second._statistics.toShortString() << std::endl;
  }

  out << "end OracleForOnePhaseWithAmdahlsLaw" << std::endl;
}


peano::datatraversal::autotuning::OracleForOnePhase* sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::createNewOracle() const {
  return new OracleForOnePhaseWithAmdahlsLaw();
}



void sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::deactivateOracle() {
  bool hasChangedAnEntry = false;
  for (auto& measurement: _executionTimeDatabase) {
	int oldValue = measurement.second._optimalGrainSize;
	measurement.second._statistics.relaxAmdahlsLawWithThreadStartupCost();
	int newValue = std::min( measurement.second._statistics.getOptimalNumberOfThreads(), measurement.second._maxProblemSize );
	if (oldValue!=newValue) {
      logInfo(
        "deactivateOracle()",
		"change optimal grain size for " << toString(measurement.first)
		<< " from " << oldValue << " to " << newValue
	  );
      measurement.second._optimalGrainSize = newValue;
      hasChangedAnEntry = true;
	}
  }

  if (!hasChangedAnEntry) {
    _sampleEveryXQueries++;
    _sampleEveryXQueries = std::min(_sampleEveryXQueries,65536);
    logInfo(
      "deactivateOracle()",
       "new sample interval: sample every " << _sampleEveryXQueries << "th request "
    );
  }
  else {
    _sampleEveryXQueries /= 2;
    _sampleEveryXQueries = std::max(_sampleEveryXQueries,2);
  }
}


void sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::activateOracle() {
}


sharedmemoryoracles::OracleForOnePhaseWithAmdahlsLaw::DataBaseEntry::DataBaseEntry():
  _statistics(),
  _optimalGrainSize(1),
  _maxProblemSize(1) {
}

