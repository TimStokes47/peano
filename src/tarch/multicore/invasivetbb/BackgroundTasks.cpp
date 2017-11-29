#include "tarch/multicore/BackgroundTasks.h"
#include "peano/performanceanalysis/Analysis.h"

#if defined(SharedTBBInvade)

#include "tarch/logging/Log.h"

#include <vector>
#include <tbb/task.h>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_invoke.h>
#include <tbb/tbb_machine.h>
#include <tbb/task.h>
#include <tbb/tbb_thread.h>

#include "SHMInvade.hpp"

namespace {
  /**
   * Use this to launch all background with very low priority
   */
  tbb::task_group_context  _backgroundTaskContext;

  /**
   * Number of actively running background tasks. If a task tries to run, and
   * there are more than a given number of threads already active, it
   * immediately yields again.
   */
  tbb::atomic<int>         _numberOfRunningBackgroundThreads(0);

  /**
   * The active tasks
   */
  tbb::concurrent_queue<tarch::multicore::BackgroundTask*>  _backgroundTasks;

  int                      _maxNumberOfRunningBackgroundThreads(-1);

  tarch::logging::Log _log( "tarch::multicore" );

  class ConsumerTask: public tbb::task {
    public:
      ConsumerTask() {}
      tbb::task* execute() {
        tarch::multicore::processBackgroundTasks();
        _numberOfRunningBackgroundThreads.fetch_and_add(-1);
        return 0;
      }
  };
}


void tarch::multicore::spawnBackgroundTask(BackgroundTask* task) {
  _backgroundTasks.push(task);
  peano::performanceanalysis::Analysis::getInstance().fireAndForgetBackgroundTask(1);

  const int currentlyRunningBackgroundThreads = _numberOfRunningBackgroundThreads;
  if (
    currentlyRunningBackgroundThreads<_maxNumberOfRunningBackgroundThreads
    ||
    (
      task->isLongRunning()
      &&
      _maxNumberOfRunningBackgroundThreads>=0
    )
  ) {
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "no consumer task running yet or long-running task dropped in; kick off" );
    _numberOfRunningBackgroundThreads.fetch_and_add(1);
    // @todo raus
    assertion3(false,currentlyRunningBackgroundThreads,_maxNumberOfRunningBackgroundThreads,task->isLongRunning());
    ConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) ConsumerTask();
    tbb::task::enqueue(*tbbTask);
    _backgroundTaskContext.set_priority(tbb::priority_low);
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "it is out now" );
  }
}


bool tarch::multicore::processBackgroundTasks() {
  SHMInvade invade( 1 );

  BackgroundTask* myTask = nullptr;
  bool gotOne = _backgroundTasks.try_pop(myTask);
  bool result = false;
  while (gotOne) {
    logDebug( "execute()", "consumer task found job to do" );
    myTask->run();
    peano::performanceanalysis::Analysis::getInstance().terminatedBackgroundTask(1);
    const bool hasBeenLongRunning = myTask->isLongRunning();
    delete myTask;
    gotOne = hasBeenLongRunning ? false : _backgroundTasks.try_pop(myTask);
    result = true;
  }

  return result;
}


void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads>=-1);
  _maxNumberOfRunningBackgroundThreads = maxNumberOfRunningBackgroundThreads;
}


#endif
