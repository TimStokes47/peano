#include "tarch/multicore/BackgroundTasks.h"
#include "tarch/Assertions.h"


void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(const MaxNumberOfRunningBackgroundThreads& maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads != MaxNumberOfRunningBackgroundThreads::SmallestValue);
  setMaxNumberOfRunningBackgroundThreads( static_cast<int>(maxNumberOfRunningBackgroundThreads) );
}


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


int tarch::multicore::getNumberOfWaitingBackgroundTasks() {
  return 0;
}

#endif
