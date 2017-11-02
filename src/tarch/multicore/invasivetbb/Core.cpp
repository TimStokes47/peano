#if defined(SharedTBBInvade)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/invasivetbb/Core.h"
#include "tarch/Assertions.h"


tarch::logging::Log  tarch::multicore::Core::_log( "tarch::multicore::Core" );


tarch::multicore::Core::Core():
  _invadeRoot(),
  _basicInvasion( new SHMInvade(1) ) {
  SHMController::cleanup();
}


tarch::multicore::Core::~Core() {
  assertion(_basicInvasion != nullptr);
  delete _basicInvasion;
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static tarch::multicore::Core singleton;
  return singleton;
}


void tarch::multicore::Core::shutDown() {
}


void tarch::multicore::Core::configure( int numberOfThreads ) {
  assertion(_basicInvasion != nullptr);
  assertion(numberOfThreads>=0 || numberOfThreads==UseDefaultNumberOfThreads);

  delete _basicInvasion;
  _basicInvasion = new SHMInvade(numberOfThreads);
}


int tarch::multicore::Core::getNumberOfThreads() const {
  return _invadeRoot.get_num_active_threads();
}


bool tarch::multicore::Core::isInitialised() const {
  return true;
}

#endif
