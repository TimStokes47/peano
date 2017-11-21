#include "tarch/multicore/BackgroundTasks.h"
#include "peano/performanceanalysis/Analysis.h"

#if defined(SharedTBB)

#include <vector>
#include <tbb/task.h>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_invoke.h>
#include <tbb/tbb_machine.h>
#include <tbb/task.h>
#include <tbb/tbb_thread.h>

namespace {
  class ConsumerTask: public tbb::task {
    public:
      ConsumerTask() {}
      tbb::task* execute() {
        tarch::multicore::processBackgroundTasks();
        return 0;
      }
  };

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

  int                      _maxNumberOfRunningBackgroundThreads(1);

  tarch::logging::Log _log( "tarch::multicore" );
}


void tarch::multicore::spawnBackgroundTask(BackgroundTask* task) {
  _backgroundTasks.push(task);
  peano::performanceanalysis::Analysis::getInstance().fireAndForgetBackgroundTask(1);

  const int currentlyRunningBackgroundThreads = _numberOfRunningBackgroundThreads;
  if (
    currentlyRunningBackgroundThreads<_maxNumberOfRunningBackgroundThreads
    ||
    task->isLongRunning()
  ) {
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "no consumer task running yet; kick off" );
    if ( !task->isLongRunning() ) {
      _numberOfRunningBackgroundThreads.fetch_and_add(1);
    }
    ConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) ConsumerTask();
    tbb::task::enqueue(*tbbTask);
    _backgroundTaskContext.set_priority(tbb::priority_low);
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "it is out now" );
  }
}


void tarch::multicore::processBackgroundTasks() {
  logDebug( "execute()", "background consumer task becomes awake" );

  BackgroundTask* myTask = nullptr;
  bool gotOne = _backgroundTasks.try_pop(myTask);
  bool taskHasBeenLongRunning = false;
  while (gotOne) {
    logDebug( "execute()", "consumer task found job to do" );
    peano::performanceanalysis::Analysis::getInstance().terminatedBackgroundTask(1);
    myTask->run();
    taskHasBeenLongRunning = myTask->isLongRunning();
    delete myTask;
    gotOne = taskHasBeenLongRunning ? false : _backgroundTasks.try_pop(myTask);
  }

  if (!taskHasBeenLongRunning) {
    _numberOfRunningBackgroundThreads.fetch_and_add(-1);
  }

  logDebug( "execute()", "background task consumer is done and kills itself" );
  tbb::this_tbb_thread::yield();
}


void tarch::multicore::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads>=1);
  _maxNumberOfRunningBackgroundThreads = maxNumberOfRunningBackgroundThreads;
}


#endif
