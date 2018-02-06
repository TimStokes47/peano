#if defined(SharedTBB)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/tbb/Core.h"
#include "tarch/Assertions.h"
#include <thread>


tarch::logging::Log  tarch::multicore::Core::_log( "tarch::multicore::Core" );


tarch::multicore::Core::Core():
  _numberOfThreads(std::thread::hardware_concurrency()),
  _globalControl(nullptr),
  _pinningObserver(1) {
}


tarch::multicore::Core::~Core() {
  if (_globalControl!=nullptr) {
    delete _globalControl;
    _globalControl = nullptr;
  }
}


void tarch::multicore::Core::pinThreads(bool value) {
  _pinningObserver.observe(value);
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static tarch::multicore::Core singleton;
  return singleton;
}


void tarch::multicore::Core::shutDown() {
  if (_globalControl!=nullptr) {
    delete _globalControl;
    _globalControl = nullptr;
  }
  _numberOfThreads = -1;
}


void tarch::multicore::Core::configure( int numberOfThreads ) {
  logInfo( "configure(int)", "manually set number of threads to " << numberOfThreads );

  if (_globalControl!=nullptr) {
    delete _globalControl;
  }

  if (numberOfThreads==UseDefaultNumberOfThreads) {
    _numberOfThreads = std::thread::hardware_concurrency();
  }
  else {
    _numberOfThreads = numberOfThreads;
  }

  _globalControl = new tbb::global_control(tbb::global_control::max_allowed_parallelism,_numberOfThreads);
}


int tarch::multicore::Core::getNumberOfThreads() const {
  assertion( isInitialised() );
  return _numberOfThreads;
}


bool tarch::multicore::Core::isInitialised() const {
  //return _globalControl!=nullptr;
  return true;
}

#endif
