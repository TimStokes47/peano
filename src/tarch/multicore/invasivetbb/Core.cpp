#if defined(SharedTBBInvade)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/invasivetbb/Core.h"
#include "tarch/Assertions.h"


tarch::logging::Log  tarch::multicore::Core::_log( "tarch::multicore::Core" );


tarch::multicore::Core::Core():
  _invadeRoot() {
  SHMController::cleanup();
  // @todo Waere schoen, wenn ich das rausnehmen koennte
  static      SHMInvade s(1);
}


tarch::multicore::Core::~Core() {
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static tarch::multicore::Core singleton;
  return singleton;
}


void tarch::multicore::Core::shutDown() {
/*
  @todo

  _task_scheduler_init.terminate();
  _numberOfThreads = -1;
*/
}


void tarch::multicore::Core::configure( int numberOfThreads ) {
/*
  if (_task_scheduler_init.is_active()) {
    _task_scheduler_init.terminate();
  }

  if (numberOfThreads==UseDefaultNumberOfThreads) {
    _numberOfThreads = ::tbb::task_scheduler_init::default_num_threads();
  }
  else {
    _numberOfThreads = numberOfThreads;
  }

  _task_scheduler_init.initialize( _numberOfThreads );
*/
}


int tarch::multicore::Core::getNumberOfThreads() const {
  return _invadeRoot.get_num_active_threads();
}


bool tarch::multicore::Core::isInitialised() const {
  return true;
}

#endif
