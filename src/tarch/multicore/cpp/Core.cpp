#include "tarch/multicore/Core.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/compiler/CompilerSpecificSettings.h"


#ifdef CompilerHasSysinfo
#include <sched.h>
#endif



/*
int tarch::multicore::getCPUNumber() {
  #ifdef CompilerHasSysinfo
  return sched_getcpu();
  #else
  //  https://stackoverflow.com/questions/33745364/sched-getcpu-equivalent-for-os-x
  return 1;
  #endif
}
*/


#if defined(SharedCPP)
tarch::multicore::Core::Core() {
}


tarch::multicore::Core::~Core() {
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static Core instance;
  return instance;
}

void tarch::multicore::Core::configure( int numberOfThreads, int maxNumberOfConcurrentBackgroundTasks, int maxNumberOfConcurrentBandwidthBoundTasks ) {
}


void tarch::multicore::Core::shutDown() {
}


bool tarch::multicore::Core::isInitialised() const {
  return true;
}


int tarch::multicore::Core::getNumberOfThreads() const {
	// @todo
  return std::thread::hardware_concurrency();
}


/*
int tarch::multicore::Core::getThreadNumber() const {
  return std::this_thread::get_id();
}
*/

#endif
