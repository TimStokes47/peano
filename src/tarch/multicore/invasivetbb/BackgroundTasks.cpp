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

#include "shminvade/SHMInvade.h"

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
  if (
    _maxNumberOfRunningBackgroundThreads==static_cast<int>(MaxNumberOfRunningBackgroundThreads::ProcessBackgroundTasksImmediately)
  ) {
    task->run();
    delete task;
    return;
  }

  Redo

  _backgroundTasks.push(task);
  peano::performanceanalysis::Analysis::getInstance().fireAndForgetBackgroundTask(1);

  const int currentlyRunningBackgroundThreads = _numberOfRunningBackgroundThreads;
  if (
    currentlyRunningBackgroundThreads<_maxNumberOfRunningBackgroundThreads
    ||
    (
      task->isLongRunning()
      &&
      _maxNumberOfRunningBackgroundThreads>=static_cast<int>(MaxNumberOfRunningBackgroundThreads::DontUseBackgroundTasksForNormalTasks)
    )
  ) {
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "no consumer task running yet or long-running task dropped in; kick off" );
    _numberOfRunningBackgroundThreads.fetch_and_add(1);
    ConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) ConsumerTask();
    tbb::task::enqueue(*tbbTask);
    _backgroundTaskContext.set_priority(tbb::priority_low);
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "it is out now" );
  }
}


bool tarch::multicore::processBackgroundTasks() {
  logDebug( "execute()", "background consumer task becomes awake" );

  shminvade::SHMInvade invade( 1 );

  BackgroundTask* myTask = nullptr;
  bool gotOne = _backgroundTasks.try_pop(myTask);
  bool result = false;
  while (gotOne) {
    logDebug( "execute()", "consumer task found job to do" );
    peano::performanceanalysis::Analysis::getInstance().terminatedBackgroundTask(1);
    myTask->run();
    const bool taskHasBeenLongRunning = myTask->isLongRunning();
    delete myTask;
    gotOne = taskHasBeenLongRunning ? false : _backgroundTasks.try_pop(myTask);
    result = true;
  }

  invade.retreat();
  logDebug( "execute()", "background task consumer is done and kills itself" );

  return result;
}


int tarch::multicore::getNumberOfWaitingBackgroundTasks() {
  return _numberOfRunningBackgroundThreads + _backgroundTasks.unsafe_size();
}

#endif
