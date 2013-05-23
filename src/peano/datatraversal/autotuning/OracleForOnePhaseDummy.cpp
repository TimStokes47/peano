#include "peano/datatraversal/autotuning/OracleForOnePhaseDummy.h"
#include "tarch/Assertions.h"

#include <cstdlib>
#include <limits>


tarch::logging::Log  peano::datatraversal::autotuning::OracleForOnePhaseDummy::_log( "peano::datatraversal::autotuning::OracleForOnePhaseDummy" );


/**

                      grainsize     concurrency   local speedup   global speedup
 enterCell/leaveCell: (3*3)^d/2^d   400%          3.5-4           <1
                      12/2^d        220%          
                      16/2^d        230%          3.7             1.03 bzw 1.06
                      (3*3*3)^d/2^d <200%         3.5-4           <1 jedoch besser




 */
peano::datatraversal::autotuning::OracleForOnePhaseDummy::OracleForOnePhaseDummy(
  bool                useMultithreading,
  bool                measureAlsoSerialProgramParts,
  int                 splitTheTree,
  bool                pipelineDescendProcessing,
  bool                pipelineAscendProcessing,
  int                 smallestGrainSize1DForCellEvents,
  int                 smallestGrainSize1DForVertexEvents,
  int                 smallestGrainSizeForLoadStoreSplits,
  const MethodTrace&  methodTrace
):
  _useMulticore(useMultithreading),
  _measureAlsoSerialProgramParts(measureAlsoSerialProgramParts),
  _executionTime(),
  _methodTrace(methodTrace),
  _splitTheTree(splitTheTree),
  _pipelineDescendProcessing(pipelineDescendProcessing),
  _pipelineAscendProcessing(pipelineAscendProcessing),
  _smallestGrainSize(std::numeric_limits<int>::max()),
  _lastProblemSize(-1),
  _smallestGrainSize1DForCellEvents(smallestGrainSize1DForCellEvents),
  _smallestGrainSize1DForVertexEvents(smallestGrainSize1DForVertexEvents),
  _smallestGrainSizeForLoadStoreSplits(smallestGrainSizeForLoadStoreSplits) {
  if (
    ( _methodTrace==CallEnterCellOnRegularStationaryGrid ||
      _methodTrace==CallLeaveCellOnRegularStationaryGrid ||
      _methodTrace==AscendOnRegularStationaryGrid        ||
      _methodTrace==DescendOnRegularStationaryGrid
    ) &&
    _splitTheTree !=2
  ) {
    _smallestGrainSize            = 1;
    const int smallestGrainSize1D = _smallestGrainSize1DForCellEvents;
    for (int d=0; d<DIMENSIONS; d++) {
      _smallestGrainSize *= smallestGrainSize1D;
    }
    _smallestGrainSize /= TWO_POWER_D;
  }
  else
  if (
    (
      _methodTrace==CallTouchFirstTimeOnRegularStationaryGrid ||
      _methodTrace==CallTouchLastTimeOnRegularStationaryGrid
    ) &&
    _splitTheTree !=2
  ) {
    _smallestGrainSize            = 1;
    const int smallestGrainSize1D = _smallestGrainSize1DForVertexEvents;
    for (int d=0; d<DIMENSIONS; d++) {
      _smallestGrainSize *= smallestGrainSize1D;
    }
    _smallestGrainSize /= 2;
  }
  else if (
    _splitTheTree > 0  &&
    (
      _methodTrace == SplitLoadVerticesTaskOnRegularStationaryGrid  ||
      _methodTrace == SplitStoreVerticesTaskOnRegularStationaryGrid
    )
  ) {
    _smallestGrainSize  = _smallestGrainSizeForLoadStoreSplits;
  }
  else if ( _pipelineAscendProcessing && _methodTrace == PipelineAscendTask ) {
    _smallestGrainSize = 1;
  }
  else if ( _pipelineDescendProcessing && _methodTrace == PipelineDescendTask ) {
    _smallestGrainSize = 1;
  }
}


std::pair<int,bool> peano::datatraversal::autotuning::OracleForOnePhaseDummy::parallelise(int problemSize) {
  assertionEquals1( _lastProblemSize, -1, toString(_methodTrace) );
  if (_useMulticore) {
    if (problemSize < _smallestGrainSize) {
      if (_measureAlsoSerialProgramParts) _lastProblemSize = problemSize;
      return std::pair<int,bool>(0,_measureAlsoSerialProgramParts);
    }
    else {
      _lastProblemSize = problemSize;
      return std::pair<int,bool>(_smallestGrainSize,true);
    }
  }
  else {
    if (_measureAlsoSerialProgramParts) _lastProblemSize = problemSize;
    return std::pair<int,bool>(0,_measureAlsoSerialProgramParts);
  }
}


void peano::datatraversal::autotuning::OracleForOnePhaseDummy::parallelSectionHasTerminated(double elapsedCalendarTime) {
  assertion( _lastProblemSize>0 );
  _executionTime[_lastProblemSize].setValue( elapsedCalendarTime );
  _executionTime[_lastProblemSize].setAccuracy(1.0e-5);
  _lastProblemSize = -1;
}


void peano::datatraversal::autotuning::OracleForOnePhaseDummy::plotStatistics() const {
  for (std::map<int, tarch::timing::Measurement>::const_iterator p=_executionTime.begin(); p!=_executionTime.end(); p++) {
    if (p->second.getNumberOfMeasurements()>0) {
      logInfo(
        "plotRuntimes()",
        "averaged runtime for " << toString(_methodTrace)
        << " for problem size " << p->first << ": " <<
        p->second.toString()
      );
    }
  }
}


peano::datatraversal::autotuning::OracleForOnePhaseDummy::~OracleForOnePhaseDummy() {
}


peano::datatraversal::autotuning::OracleForOnePhase* peano::datatraversal::autotuning::OracleForOnePhaseDummy::createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const {
  return new OracleForOnePhaseDummy(
    _useMulticore,
    _measureAlsoSerialProgramParts,
    _splitTheTree,
    _pipelineDescendProcessing,
    _pipelineAscendProcessing,
    _smallestGrainSize1DForCellEvents,
    _smallestGrainSize1DForVertexEvents,
    _smallestGrainSizeForLoadStoreSplits,
    methodTrace
  );
}


void peano::datatraversal::autotuning::OracleForOnePhaseDummy::informAboutElapsedTimeOfLastTraversal(double elapsedTime) {
}
