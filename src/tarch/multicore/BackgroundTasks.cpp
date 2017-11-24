#include "tarch/multicore/BackgroundTasks.h"

#if !defined(SharedOMP) && !defined(SharedTBB) && !defined(SharedTBBInvade)

#include <vector>


void tarch::multicore::spawnBackgroundTask(BackgroundTask* task) {
  task->run();
  delete task;
}


bool tarch::multicore::processBackgroundTasks() {
  return false;
}


void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
}

#endif
