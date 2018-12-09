// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined(_TARCH_MULTICORE_TBB_TASKS_H_) && defined(SharedTBB)
#define _TARCH_MULTICORE_TBB_TASKS_H_

#include "../Tasks.h"

#include <tbb/task.h>
#include <tbb/concurrent_queue.h>

namespace tarch {
 namespace multicore {
  namespace internal {
  /**
   * Helper operation. Plots quite some statistics if code is
   * translated with -DTBB_USE_THREADING_TOOLS.
   *
   * <h2> Statistics format </h2>
   *
   * You receive the following information:
   *
   * - Total number of consumer runs and of different task types that have
   *   been spawned.
   * - Histograms for high priority tasks and background tasks.
   *
   * The histograms tell you how many background/priority tasks a consumer
   * has seen upon launch. So if there always have been either none or ten
   * tasks, you'll get two outputs alike [0]=200 and [10]=400.
   *
   * <h2> Statistics visualisation </h2>
   *
   * There's a helper file in the directory (Python script) which helps
   * you to understand what's going on with the tasks.
   */
  void plotStatistics();

  void terminateAllPendingBackgroundConsumerJobs();

  enum class HighPriorityTaskProcessing {
		ProcessAllHighPriorityTasksInARush,
		ProcessAllHighPriorityTasksInARushAndRunBackgroundTasksOnlyIfNoHighPriorityTasksAreLeft,
		ProcessOneHighPriorityTasksAtATime,
		ProcessOneHighPriorityTasksAtATimeAndRunBackgroundTasksOnlyIfNoHighPriorityTasksAreLeft,
		MapHighPriorityTasksToRealTBBTasks,
		MapHighPriorityAndBackgroundTasksToRealTBBTasks
  };

  /**
   * Configure TBB runtime.
   */
  void setMinMaxNumberOfJobsToConsumeInOneRush(int min=1, int max=std::numeric_limits<int>::max());
  void setHighPriorityJobBehaviour(HighPriorityTaskProcessing behaviour);
  void setMaxNumberOfConcurrentHighBandwidthTasks(int max=1);
  void setMaxNumberOfConcurrentBackgroundTasks(int max=1);

    /**
     * Number of actively running background consumer tasks.
     *
     * @see BackgroundJobConsumerTask
     */
    extern tbb::atomic<int>            _numberOfRunningJobConsumerTasks;
    extern tbb::atomic<int>            _numberOfRunningHighBandwidthTasks;

    extern int                         _minimalNumberOfJobsPerConsumerRun;
    extern int                         _maximumNumberOfJobsPerConsumerRun;
    extern HighPriorityTaskProcessing  _processHighPriorityJobsAlwaysFirst;

    constexpr int NumberOfTaskQueues = 32;
    struct TaskQueue {
      tbb::concurrent_queue<tarch::multicore::Task*>   tasks;

      /**
       * This is not the real value but an estimate. Whenever a new
       * background job is enqueued, we check that this field is
       * as least as big as the current queue size. If the queue is
       * smaller than MaxSizeOfBackgroundQueue, we do decrease
       * MaxSizeOfBackgroundQueue by one. Therefore, MaxSizeOfBackgroundQueue
       * is monotonically bigger than the maximum queue size, but it is
       * decreases steadily to the real size if this size is significantly
       * smaller than MaxSizeOfBackgroundQueue.
       *
       * @see spawnBackgroundJob()
       */
      tbb::atomic<double>         maxSize;
    };

    extern TaskQueue _pendingJobs[NumberOfTaskQueues];

    constexpr int BackgroundTasksJobClassNumber    = NumberOfTaskQueues-1;
    constexpr int HighPriorityTasksJobClassNumber  = NumberOfTaskQueues-2;
    constexpr int HighBandwidthTasksJobClassNumber = NumberOfTaskQueues-3;

    /**
     * Each consumer should roughly process MaxSizeOfBackgroundQueue
     * jobs divided by the number of threads. A consumer should at least
     * process one job.
     */
    int getNumberOfJobsPerConsumerRun( int jobClass );

    /**
     * Return job queue for one type of job. Does not hold for background jobs.
     * They are a completely different beast. If a job queue for one class does
     * not exist yet, it is created, i.e. there's a lazy creation mechanism
     * implemented here.
     */
    TaskQueue& getTaskQueue( int jobClass );

    void insertTask( int taskClass, Task* task );
    int  getTaskQueueSize( int taskClass );

    /**
     * This is a task which consumes background jobs, as it invokes
     * processBackgroundJobs(). Typically, I make such a job consume up to
     * half of the available background jobs, before it then stops the
     * processing. When it stops and finds out that there would still
     * have been more jobs to process, then it enqueues another consumer task
     * to continue to work on the jobs at a later point.
     */
    class JobConsumerTask: public tbb::task {
      private:
        const int _maxJobs;
        JobConsumerTask(int maxJobs);

      public:
        #if TBB_USE_THREADING_TOOLS>=1
        static tbb::atomic<int>                    _numberOfConsumerRuns;
        static tbb::concurrent_hash_map<int,int>   _histogramOfHighPriorityTasks;
        static tbb::concurrent_hash_map<int,int>   _histogramOfBackgroundTasks;
        static tbb::concurrent_hash_map<int,int>   _histogramOfRunningConsumers;
        static tbb::atomic<int>                    _numberOfHighBandwidthTasks;
        static tbb::atomic<int>                    _numberOfHighPriorityTasks;
        static tbb::atomic<int>                    _numberOfBackgroundTasks;
        static tbb::concurrent_hash_map<int,int>   _histogramOfBackgroundTasksProcessed;
        #endif

        static tbb::task_group_context  backgroundTaskContext;

        /**
         * Schedule a new background job consumer task. We have to tell
         * each consumer how many jobs it may process at most. By default,
         * I have a look into the background queue and divide this number
         * by the number of existing threads. If it is smaller than the
         * magic constant TBBMinimalNumberOfJobsPerBackgroundConsumerRun,
         * then I use this one instead. So this approach balanced between
         * a reasonable distribution of jobs among all available threads
         * and a reasonable overhead (materialising in queue locking, e.g.).
         *
         * @see TBBMinimalNumberOfJobsPerBackgroundConsumerRun
         */
        static void enqueue();

        JobConsumerTask(const JobConsumerTask& copy);

        /**
         * Process _maxJobs from the background job queue and then
         * terminate. This doesn't mean that the background consumer
         * task really dies, as processJobs() might reschedule a new
         * one.
         *
         * @see enqueue()
         */
        tbb::task* execute();
    };

    class TBBWrapperAroundTask: public tbb::task {
      private:
    	Task*  _task;
      public:
    	TBBWrapperAroundTask( Task* task ):
    	  _task(task) {
    	}

        tbb::task* execute() {
          while ( _task->run() ) {}
          delete _task;
          return nullptr;
        }
    };
  }
 }
}

#endif
