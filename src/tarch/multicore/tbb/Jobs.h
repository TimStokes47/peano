#ifdef SharedTBB


#include "tarch/Assertions.h"


#include <tbb/task.h>
#include <tbb/parallel_invoke.h>
#include <tbb/tbb_machine.h>
#include <tbb/task.h>
#include <tbb/tbb_thread.h>
#include <tbb/task_group.h>
#include <tbb/concurrent_hash_map.h>


#if !defined(TBBPrefetchesJobData) and !defined(noTBBPrefetchesJobData)
  #define TBBPrefetchesJobData
#endif


#include <tbb/concurrent_queue.h>


namespace tarch {
  namespace multicore {
    namespace jobs {
      void terminateAllPendingBackgroundConsumerJobs();

      enum class HighPriorityTaskProcessing {
  		ProcessAllHighPriorityTasksInARush,
  		ProcessAllHighPriorityTasksInARushAndRunBackgroundTasksOnlyIfNoHighPriorityTasksAreLeft,
  		ProcessOneHighPriorityTasksAtATime,
  		ProcessOneHighPriorityTasksAtATimeAndRunBackgroundTasksOnlyIfNoHighPriorityTasksAreLeft,
  		MapHighPriorityTasksToRealTBBTasks
      };

      /**
       * Configure TBB runtime.
       */
      void setMinMaxNumberOfJobsToConsumeInOneRush(int min=1, int max=std::numeric_limits<int>::max());
      void setHighPriorityJobBehaviour(HighPriorityTaskProcessing behaviour);

      namespace internal {
        /**
         * Number of actively running background consumer tasks.
         *
         * @see BackgroundJobConsumerTask
         */
        extern tbb::atomic<int>            _numberOfRunningJobConsumerTasks;

        extern int                         _minimalNumberOfJobsPerConsumerRun;
        extern int                         _maximumNumberOfJobsPerConsumerRun;
        extern HighPriorityTaskProcessing  _processHighPriorityJobsAlwaysFirst;

        constexpr int NumberOfJobQueues = 32;
        struct JobQueue {
          tbb::concurrent_queue<tarch::multicore::jobs::Job*>   jobs;

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
          tbb::atomic<int>         maxSize;
        };
        extern JobQueue _pendingJobs[NumberOfJobQueues];

        constexpr int BackgroundTasksJobClassNumber    = NumberOfJobQueues-1;
        constexpr int HighPriorityTasksJobClassNumber  = NumberOfJobQueues-2;
        constexpr int HighBandwidthTasksJobClassNumber = NumberOfJobQueues-3;

        /**
         * Each consumer should roughly process MaxSizeOfBackgroundQueue
         * jobs divided by the number of threads. A consumer should at least
         * process one job.
         */
        int getNumberOfJobsPerConsumerRun( int jobClass );

        extern tarch::logging::Log _log;

        extern tbb::atomic<bool> _bandwidthTasksAreProcessed;

        /**
         * Return job queue for one type of job. Does not hold for background jobs.
         * They are a completely different beast. If a job queue for one class does
         * not exist yet, it is created, i.e. there's a lazy creation mechanism
         * implemented here.
         */
        JobQueue& getJobQueue( int jobClass );

        void insertJob( int jobClass, Job* job );
        int  getJobQueueSize( int jobClass );

        /**
         * The spawn and wait routines fire their job and then have to wait for all
         * jobs to be processed. They do this through an integer atomic that they
         * count down to zero, i.e. the atomic stores how many jobs are still
         * pending.
         */
       class JobWithoutCopyOfFunctorAndSemaphore: public tarch::multicore::jobs::Job {
          private:
            std::function<bool()>&   _functor;
            tbb::atomic<int>&        _semaphore;
          public:
            JobWithoutCopyOfFunctorAndSemaphore(std::function<bool()>& functor, JobType jobType, int jobClass, tbb::atomic<int>& semaphore ):
             Job(jobType,jobClass),
             _functor(functor),
             _semaphore(semaphore) {
            }

            bool run() override {
              bool result = _functor();
              if (!result) _semaphore.fetch_and_add(-1);
              return result;
            }

            virtual ~JobWithoutCopyOfFunctorAndSemaphore() {}
        };

       class JobWithCopyOfFunctorAndSemaphore: public tarch::multicore::jobs::Job {
          private:
            std::function<bool()>   _functor;
            tbb::atomic<int>&       _semaphore;
          public:
            JobWithCopyOfFunctorAndSemaphore(std::function<bool()>& functor, JobType jobType, int jobClass, tbb::atomic<int>& semaphore ):
             Job(jobType,jobClass),
             _functor(functor),
             _semaphore(semaphore) {
            }

            bool run() override {
              bool result = _functor();
              if (!result) _semaphore.fetch_and_add(-1);
              return result;
            }

            virtual ~JobWithCopyOfFunctorAndSemaphore() {}
        };

        /**
         * Helper function of the for loops and the parallel task invocations.
         *
         * Primarily invoked by the spawnAndWait routines. A spawn and wait routine always
         * realises the same pattern:
         *
         * - create an atomic set to the number of concurrent jobs (they are
         *   concurrent but might depend on each other).
         * - open a parallel section
         * -- invoke spawnBlockingJob() for each job, i.e. start to do something in parallel
         * -- if a job is a real task, it will be executed straightaway and we decrease the atomic
         * -- otherwise, we enqueue it in the job queues
         * - trigger the job consumer tasks
         * - wait until all jobs have terminated, i.e. the atomic counter equals 0
         *
         * As we call this helper within a parallel section, it makes sense to run all real
         * tasks immediately. It does not make sense to wait. If we have a non-task,
         * we enqueue it and we return. Originally, I thought it might be clever to
         * trigger a consumer task. But this is not that clever actually: If a parallel
         * section triggers k tasks (which in turn might spawn new subtasks) on a
         * machine with less than k hardware threads (l < k), then it might happen that
         * these l tasks all rely on input from one of the remaining k-l tasks. the
         * waits typically enter a busy loop where they try to process further tasks.
         * We might end up with a deadlock, as the original jobs of the parallel section
         * that insert the k-l jobs into their respective queue haven't been started up
         * yet. The system deadlocks as TBB does process jobs depth-first.
         *
         * The solution is rather straightforward consequently: A parallel for has to
         * spawn all of its tasks though spawnBlockingJob. All of these invocations will
         * insert jobs into the queues - besides the real tasks which can be handled
         * straight away as they, by definition, do not rely on input data while they are
         * running. Once all the jobs are enqueued (spawned), we actually kick off the
         * processing TBB tasks, i.e. the consumer tasks. Here, we can be overambitious -
         * if one of these guys finds its queues empty, it terminates immediately.
         */
        void spawnBlockingJob(
          std::function<bool()>&  job,
          JobType                 isTask,
          int                     jobClass,
          tbb::atomic<int>&       semaphore
        );


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

            #ifdef TBB_USE_THREADING_TOOLS
            static tbb::atomic<int>  _numberOfConsumerRuns;
            static tbb::atomic<int>  _totalNumberOfBackgroundTasks;
            static tbb::atomic<int>  _totalNumberOfBackgroundTasks;
            #endif

          public:
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

        class TBBWrapperAroundJob: public tbb::task {
          private:
        	Job*  _job;
          public:
        	TBBWrapperAroundJob( Job* job ):
        	  _job(job) {
        	}

            tbb::task* execute() {
              while ( _job->run() ) {}
              delete _job;
	      return nullptr;
            }
        };

        /**
         * Implementation details: The queue seems to need an & traversal
         * operator, otherwise I have experienced deadlocks.
         */
        std::string report();
      }
    }
  }
}

#endif

