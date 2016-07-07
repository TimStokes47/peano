#include "peano/datatraversal/autotuning/Oracle.h"
#include "peano/datatraversal/autotuning/OracleForOnePhaseDummy.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/Lock.h"


#include <fstream>
#include <sstream>


tarch::logging::Log                 peano::datatraversal::autotuning::Oracle::_log( "peano::datatraversal::autotuning::Oracle" ) ;
tarch::multicore::BooleanSemaphore  peano::datatraversal::autotuning::Oracle::_semaphore;


peano::datatraversal::autotuning::Oracle& peano::datatraversal::autotuning::Oracle::getInstance() {
  static peano::datatraversal::autotuning::Oracle singleton;
  return singleton;
}


peano::datatraversal::autotuning::Oracle::Oracle():
  _oracles(),
  _watchSinceLastSwitchCall("peano::datatraversal::autotuning::Oracle", "Oracle()", false),
  _currentAdapter(0),
  _oraclePrototype(0),
  _numberOfAdapters(0) {
}


void peano::datatraversal::autotuning::Oracle::setOracle( OracleForOnePhase* oraclePrototype ) {
  logTraceIn( "setOracle(Oracle*)");
  assertion( oraclePrototype!=0 );

  if (_oraclePrototype!=0) {
    delete _oraclePrototype;
  }
  _oraclePrototype = oraclePrototype;

  deleteOracles();
  createOracles();
  logTraceOut( "setOracle(Oracle*)");
}


int peano::datatraversal::autotuning::Oracle::getTotalNumberOfOracles() const {
  return (_numberOfAdapters+AdapterStatesReservedForRepositorySteering)*peano::datatraversal::autotuning::NumberOfDifferentMethodsCalling;
}


int peano::datatraversal::autotuning::Oracle::getKey(const MethodTrace& askingMethod ) const {
  assertion( askingMethod>=0 );
  assertion( askingMethod<peano::datatraversal::autotuning::NumberOfDifferentMethodsCalling );
  int result = _currentAdapter*peano::datatraversal::autotuning::NumberOfDifferentMethodsCalling + askingMethod;
  assertion(result>=0);
  assertion(result<getTotalNumberOfOracles());
  return result;
}


void peano::datatraversal::autotuning::Oracle::plotStatistics(const std::string& filename) {
  assertion(_oracles!=0);

  if (!filename.empty()) {
    std::ofstream f(filename.c_str(),std::ios::out );
    if (f.is_open()) {
      f << "total-number-of-oracles=" << getTotalNumberOfOracles() << std::endl;
      f << "number-of-adapters=" << _numberOfAdapters << std::endl;
      f << "adapters-reserved-for-repository-steering=" << AdapterStatesReservedForRepositorySteering << std::endl;
      f << "no-of-methods-calling=" << peano::datatraversal::autotuning::NumberOfDifferentMethodsCalling << std::endl;

      for (int i=0; i<getTotalNumberOfOracles();i++) {
        assertion(_oracles[i]._oracle!=0);
        _oracles[i]._oracle->plotStatistics(f);
      }
      f.flush();
      f.close();
    }
    else {
      logError("plotStatistics", "could not write into " << filename);
    }
  }
  else {
    std::ostringstream f;
    for (int i=0; i<getTotalNumberOfOracles();i++) {
      assertion(_oracles[i]._oracle!=0);
      _oracles[i]._oracle->plotStatistics(f);
    }
    logInfo( "plotStatistics(string)", f.str() );
  }
}


void peano::datatraversal::autotuning::Oracle::loadStatistics(const std::string& filename) {
  assertion(_oracles!=0);
  for (int i=0; i<getTotalNumberOfOracles();i++) {
    assertion(_oracles[i]._oracle!=0);
    _oracles[i]._oracle->loadStatistics(filename);
  }
}


peano::datatraversal::autotuning::Oracle::~Oracle() {
  deleteOracles();
  if (_oraclePrototype!=0) {
    delete _oraclePrototype;
    _oraclePrototype = 0;
  }
}


void peano::datatraversal::autotuning::Oracle::setNumberOfOracles(int value) {
  logTraceInWith1Argument( "setNumberOfOracles(int)", value);
  assertion( value>0 );

  deleteOracles();
  _numberOfAdapters=value;

  logTraceOut( "setNumberOfOracles(int)");
}


void peano::datatraversal::autotuning::Oracle::createOracles() {
  #if defined(SharedMemoryParallelisation)
  logTraceIn( "createOracles()");
  assertionMsg( _numberOfAdapters>0, "total number of oracles not set. Invoke setNumberOfOracles() first. This happens if shared memory oracle is initialised before repository is created. Create repository first" );

  if (_oraclePrototype==0) {
	  logWarning( "createOracles(int)", "no oracle type configured. Perhaps forgot to call peano::datatraversal::autotuning::Oracle::setOracle(). Peano uses default oracle" );
	  _oraclePrototype = new OracleForOnePhaseDummy(true);
  }
  else {
    assertion( _oracles==0 );
    assertion( _oraclePrototype!=0 );

    _oracles = new ValuesPerOracleKey[getTotalNumberOfOracles()];


    // @todo _numberOfAdapters -> numberOfAdapters
    for (int currentAdapter=0; currentAdapter<_numberOfAdapters+AdapterStatesReservedForRepositorySteering; currentAdapter++) {
        // @todo currentAdapter
      _currentAdapter = currentAdapter;
      for (int methodTraceNumber=0; methodTraceNumber<peano::datatraversal::autotuning::NumberOfDifferentMethodsCalling; methodTraceNumber++) {
        int key = getKey( toMethodTrace(methodTraceNumber) );
        _oracles[key]._measureTime = false;
        _oracles[key]._watch       = new tarch::timing::Watch("peano::datatraversal::autotuning::Oracle", "createOracles(int)", false);
        _oracles[key]._oracle      = _oraclePrototype->createNewOracle(currentAdapter,toMethodTrace(methodTraceNumber));
        _oracles[key]._recursiveCallsForThisOracle  = 0;
      }
    }
  }

  logTraceOut( "createOracles()");
  #endif
}


void peano::datatraversal::autotuning::Oracle::deleteOracles() {
  logTraceIn( "deleteOracles()");
  if (_oracles != 0) {
    for (int i=0; i<getTotalNumberOfOracles(); i++) {
      delete _oracles[i]._oracle;
      delete _oracles[i]._watch;
    }

    delete[] _oracles;
    _oracles = 0;
  }
  logTraceOut( "deleteOracles()");
}


void peano::datatraversal::autotuning::Oracle::switchToOracle(int id) {
  #if defined(SharedMemoryParallelisation)
  assertion1( id>=0, id );

  _watchSinceLastSwitchCall.stopTimer();

  assertion(_oraclePrototype!=0);

  const double erasedTime = _watchSinceLastSwitchCall.getCalendarTime();

  for (int methodTraceNumber=0; methodTraceNumber<peano::datatraversal::autotuning::NumberOfDifferentMethodsCalling; methodTraceNumber++) {
    int key = getKey( toMethodTrace(methodTraceNumber) );
    _oracles[key]._oracle->informAboutElapsedTimeOfLastTraversal(erasedTime);
  }

  _currentAdapter=id;

  _watchSinceLastSwitchCall.startTimer();
  #endif
}


int peano::datatraversal::autotuning::Oracle::parallelise(int problemSize, MethodTrace askingMethod ) {
  logTraceInWith3Arguments( "parallelise(int,int)", problemSize, askingMethod, toString(askingMethod) );

  assertion2( problemSize>=0, problemSize, toString(askingMethod) );

  int result;

  #if defined(SharedMemoryParallelisation)
  const int key = getKey(askingMethod);
  assertionMsg(_oracles!=0, "forgot to initialise shared memory environment due to setOracle()?");
  assertion(key>=0);
  assertion(key<getTotalNumberOfOracles());

  tarch::multicore::Lock lock(_semaphore);

  assertion2( !_oracles[key]._recursiveCallsForThisOracle>=0, toString( askingMethod), _currentAdapter );
  _oracles[key]._recursiveCallsForThisOracle++;

  if (problemSize>0 && _oracles[key]._recursiveCallsForThisOracle==1) {
    const std::pair<int,bool> oracleDecision =  _oracles[key]._oracle->parallelise(problemSize);
    result                               = oracleDecision.first;
    _oracles[key]._lastGrainSizeReturned = oracleDecision.first;
    _oracles[key]._measureTime           = oracleDecision.second;
    if ( oracleDecision.second ) {
      _oracles[key]._watch->startTimer();
    }
  }
  else if (problemSize>0) {
    result = _oracles[key]._lastGrainSizeReturned;
  }
  else {
    result = 0;
   _oracles[key]._measureTime = false;
  }
  #else
  result = 0;
  #endif

  assertion3( result >=0, result, problemSize, toString(askingMethod) );
  assertion3( result <= problemSize, result, problemSize, toString(askingMethod) );
  assertion2( problemSize>0 || result==0, problemSize, toString(askingMethod) );

  logTraceOutWith1Argument( "parallelise(int,int)", result );
  return result;
}


void peano::datatraversal::autotuning::Oracle::parallelSectionHasTerminated(MethodTrace askingMethod) {
  logTraceInWith1Argument( "parallelSectionHasTerminated(...)", toString(askingMethod) );
  #if defined(SharedMemoryParallelisation)
  const int key = getKey(askingMethod);
  assertion(_oraclePrototype!=0);
  assertion(_oracles!=0);
  assertion(key>=0);
  assertion(key<getTotalNumberOfOracles());

  tarch::multicore::Lock lock(_semaphore);

  _oracles[key]._recursiveCallsForThisOracle--;
  assertion2( _oracles[key]._recursiveCallsForThisOracle>=0, toString( askingMethod), _currentAdapter );

  if (_oracles[key]._measureTime && _oracles[key]._recursiveCallsForThisOracle==0 ) {
    _oracles[key]._watch->stopTimer();
    _oracles[key]._oracle->parallelSectionHasTerminated(_oracles[key]._watch->getCalendarTime());
  }
  #endif
  logTraceOut( "parallelSectionHasTerminated(...)" );
}
