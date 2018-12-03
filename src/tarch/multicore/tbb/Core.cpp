#include "tarch/multicore/Core.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/compiler/CompilerSpecificSettings.h"


#ifdef CompilerHasSysinfo
#include <sched.h>
#endif


#if defined(SharedTBB)
#include <sstream>
#include "tarch/Assertions.h"


// This seems to be an Intel requirement as this feature isnt' released yet officially.
#define TBB_PREVIEW_GLOBAL_CONTROL 1
#include <tbb/global_control.h>
#include <tbb/task_scheduler_init.h>


#include "Tasks.h"

namespace {
  ::tbb::global_control*      __globalThreadCountControl;
  int                         __numberOfThreads;
}


tarch::multicore::Core::Core() {
}


tarch::multicore::Core::~Core() {
  shutDown();
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static Core instance;
  return instance;
}

void tarch::multicore::Core::configure( int numberOfThreads, int maxNumberOfConcurrentBackgroundTasks, int maxNumberOfConcurrentBandwidthBoundTasks ) {
  shutDown();

  if (numberOfThreads==UseDefaultNumberOfThreads) {
    __numberOfThreads = tbb::task_scheduler_init::default_num_threads();
  }
  else if (numberOfThreads==UseMaximumNumberOfAvailableThreads) {
    __numberOfThreads = std::thread::hardware_concurrency();
  }
  else {
    __numberOfThreads = numberOfThreads;
  }

  __globalThreadCountControl = new tbb::global_control(tbb::global_control::max_allowed_parallelism,__numberOfThreads);

  internal::setMaxNumberOfConcurrentBackgroundTasks(maxNumberOfConcurrentBackgroundTasks);
  internal::setMaxNumberOfConcurrentHighBandwidthTasks(maxNumberOfConcurrentBandwidthBoundTasks);
}


void tarch::multicore::Core::shutDown() {
  if (__globalThreadCountControl!=nullptr) {
    delete __globalThreadCountControl;
    __globalThreadCountControl = nullptr;
  }

  __numberOfThreads = -1;
}


bool tarch::multicore::Core::isInitialised() const {
  return true;
}


int tarch::multicore::Core::getNumberOfThreads() const {
  assertion( isInitialised() );
  return __numberOfThreads;
}


std::string tarch::multicore::Core::getThreadId() const {
  std::ostringstream msg;
  msg << std::this_thread::get_id();
  return msg.str();
}


int tarch::multicore::Core::getCoreNumber() const {
  #ifdef CompilerHasSysinfo
  return sched_getcpu();
  #else
  //  https://stackoverflow.com/questions/33745364/sched-getcpu-equivalent-for-os-x
  return 1;
  #endif
}

#endif
