#include "tarch/multicore/BackgroundTasks.h"
#include "tarch/Assertions.h"


void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(const MaxNumberOfRunningBackgroundThreads& maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads != MaxNumberOfRunningBackgroundThreads::SmallestValue);
  setMaxNumberOfRunningBackgroundThreads( static_cast<int>(maxNumberOfRunningBackgroundThreads) );
}


int  tarch::multicore::BackgroundTask::_maxNumberOfRunningBackgroundThreads(1);



void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
  assertion1(maxNumberOfRunningBackgroundThreads > static_cast<int>(MaxNumberOfRunningBackgroundThreads::SmallestValue), maxNumberOfRunningBackgroundThreads );


#ifdef SharedOMP
#error Fix; should only accept 0 and -1
#endif

  BackgroundTask::_maxNumberOfRunningBackgroundThreads = maxNumberOfRunningBackgroundThreads;
}


//#if !defined(SharedOMP) && !defined(SharedTBB) && !defined(SharedTBBInvade)
#if !defined(SharedTBB) && !defined(SharedTBBInvade)

#include <vector>


void tarch::multicore::spawnBackgroundTask(BackgroundTask* task) {
  task->run();
  delete task;
}


bool tarch::multicore::processBackgroundTasks() {
  return false;
}


int tarch::multicore::getNumberOfWaitingBackgroundTasks() {
  return 0;
}

#endif
