#include "tarch/multicore/BackgroundTasks.h"
#include "tarch/Assertions.h"


#if defined(SharedOMP)

#include <vector>

namespace {
  std::vector<tarch::multicore::BackgroundTask*>  _backgroundTasks;
  int                                             _maxNumberOfBackgroundTasks(1);
}


void tarch::multicore::spawnBackgroundTask(BackgroundTask* task) {
  #pragma omp critical(BackgroundCriticalSection)
  _backgroundTasks.push_back(task);
}


bool tarch::multicore::processBackgroundTasks() {
  int tasksToTrigger;
  std::vector<tarch::multicore::BackgroundTask*>  triggeredTasks;

  #pragma omp critical(BackgroundCriticalSection)
  {
    tasksToTrigger = std::min( _maxNumberOfBackgroundTasks, static_cast<int>(_backgroundTasks.size()) );
    triggeredTasks = std::vector<tarch::multicore::BackgroundTask*>(_backgroundTasks.begin(),_backgroundTasks.begin()+tasksToTrigger);
    _backgroundTasks.erase(_backgroundTasks.begin(),_backgroundTasks.begin()+tasksToTrigger);
  }

  #pragma omp parallel for
  for (int i=0; i<tasksToTrigger; i++) {
    triggeredTasks[i]->run();
    delete triggeredTasks[i];
  }

  return tasksToTrigger;
}


void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads>0);
  _maxNumberOfBackgroundTasks = maxNumberOfRunningBackgroundThreads;
}



int tarch::multicore::getNumberOfWaitingBackgroundTasks() {
  int result = 0;

  #pragma omp critical(BackgroundCriticalSection)
  {
    result = _backgroundTasks.size();
  }

  return result;
}


#endif
