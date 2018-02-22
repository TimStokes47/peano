#include "tarch/multicore/Core.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/compiler/CompilerSpecificSettings.h"


#ifdef CompilerHasSysinfo
#include <sched.h>
#endif

#ifdef SharedCPP


tarch::multicore::Core::Core():
  _numberOfThreads(-1) {
  configure( std::thread::hardware_concurrency() );
}


tarch::multicore::Core::~Core() {
  JobConsumer::shutdownAllJobConsumers();
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static Core instance;
  return instance;
}


void tarch::multicore::Core::configure( int numberOfThreads ) {
  _numberOfThreads = numberOfThreads;
  @todo Jetzt starten
  dann detach
}


void tarch::multicore::Core::shutDown() {
  JobConsumer::shutdownAllJobConsumers();
}


bool tarch::multicore::Core::isInitialised() const {
  return true;
}


int tarch::multicore::Core::getNumberOfThreads() const {
  return _numberOfThreads;
}


void tarch::multicore::Core::pinThreads(bool value) {
  JobConsumer::shutdownAllJobConsumers();
  @todo
  dann detach
}

#endif
