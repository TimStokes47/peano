#include "tarch/multicore/Core.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"

#ifdef CompilerHasSysinfo
#include <sched.h>
#endif

#include <sys/sysinfo.h>
#include <map>
#include <thread>

int tarch::multicore::getCPUNumber() {
  #ifdef CompilerHasSysinfo
  return sched_getcpu();
  #else
  //  https://stackoverflow.com/questions/33745364/sched-getcpu-equivalent-for-os-x
  return 1;
  #endif
}

int tarch::multicore::getNumberOfHWThreads() {
    static int numHWThreads=-1;

    if(numHWThreads==-1)
    	numHWThreads = get_nprocs();

	return numHWThreads;
}

std::thread::id tarch::multicore::getThreadNumber() {
  return std::this_thread::get_id();
}


#ifndef SharedMemoryParallelisation

#include "tarch/multicore/Core.h"


tarch::multicore::Core::Core() {
}


tarch::multicore::Core::~Core() {
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static Core instance;
  return instance;
}

void tarch::multicore::Core::configure( int numberOfThreads ) {
}


void tarch::multicore::Core::shutDown() {
}


bool tarch::multicore::Core::isInitialised() const {
  return true;
}


int tarch::multicore::Core::getNumberOfThreads() const {
  return 1;
}

#endif
