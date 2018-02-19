#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/tbb/Jobs.h"


#include <vector>
#include <tbb/task.h>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_invoke.h>
#include <tbb/tbb_machine.h>
#include <tbb/task.h>
#include <tbb/tbb_thread.h>
#include <tbb/task_group.h>
#include <tbb/concurrent_hash_map.h>
#include <limits>


namespace {
  /**
   * Number of actively running background consumer tasks.
   *
   * @see BackgroundJobConsumerTask
   */
  tbb::atomic<int>         _numberOfRunningBackgroundJobConsumerTasks(0);

  /**
   * This queue holds jobs that should be processed in the background. Consumer
   * tasks then read jobs from this queue and process them.
   */
  tbb::concurrent_queue<tarch::multicore::jobs::BackgroundJob*>  _backgroundJobs;

  /**
   * Work around for future versions where I might want to augment each
   * individual job queue.
   */
  struct JobQueue {
    tbb::concurrent_queue<tarch::multicore::jobs::Job*> jobs;
  };

  /**
   * There are different classes of jobs. See Job class description.
   * Per job class, there is one queue.
   */
  typedef tbb::concurrent_hash_map< int, JobQueue >  JobMap;
  JobMap     _pendingJobs;


  tarch::logging::Log _log( "tarch::multicore" );

  /**
   * Return job queue for one type of job. Does not hold for background jobs.
   * They are a completely different beast. If a job queue for one class does
   * not exist yet, it is created, i.e. there's a lazy creation mechanism
   * implemented here.
   */
  JobQueue& getJobQueue( int jobClass ) {
	if ( _pendingJobs.count(jobClass)==0 ) {
      JobMap::accessor    a;
      _pendingJobs.insert( a, jobClass );
	}
    JobMap::accessor c;
    _pendingJobs.find( c, jobClass );
    return c->second;
  }

  /**
   * This is a task which consumes background jobs, as it invokes
   * processJobs(). It is tied to one particular job
   * class.
   */
  class JobConsumerTask: public tbb::task {
    private:
      const int   _jobClass;
      const bool  _checkOtherJobClassesToo;
    public:
      JobConsumerTask(int jobClass, bool checkOtherJobClassesToo):
        _jobClass(jobClass),
        _checkOtherJobClassesToo(checkOtherJobClassesToo) {
      }
      
      
      tbb::task* execute() {
        tarch::multicore::jobs::processJobs(_jobClass);
        return nullptr;
      }
  };
  
  /**
   * The spawn and wait routines fire their job and then have to wait for all
   * jobs to be processed. They do this through an integer atomic that they
   * count down to zero, i.e. the atomic stores how many jobs are still
   * pending.
   */
  class JobWithoutCopyOfFunctorAndSemaphore: public tarch::multicore::jobs::Job {
    private:
      std::function<void()>&   _functor;
      tbb::atomic<int>&        _semaphore;
    public:
      JobWithoutCopyOfFunctorAndSemaphore(std::function<void()>& functor, tbb::atomic<int>& semaphore, bool isTask, int jobClass ):
       Job(isTask,jobClass),
       _functor(functor),
       _semaphore(semaphore) {
      }
           
      void run() override {
        _functor();
        #ifdef Asserts
        int result = _semaphore.fetch_and_add(-1);
        assertion( result>=1 );
        #else
        _semaphore.fetch_and_add(-1);
        #endif
      }

      virtual ~JobWithoutCopyOfFunctorAndSemaphore() {}
  };
  
  /**
   * Maps one job onto a TBB task. Is used if Peano's job component is asked
   * to process a job and this job is a task, i.e. has no incoming and outgoing
   * dependencies. In this case, it wraps a TBB task around the job and spawns
   * or enqueues it. The wrapper takes over the responsibility to delete the
   * job instance in the end.
   */
  class TBBJobWrapper: public tbb::task {
    private:
      tarch::multicore::jobs::Job*        _job;
    public:
      TBBJobWrapper( tarch::multicore::jobs::Job* job ):
        _job(job) {
      }
           
      tbb::task* execute() {
        _job->run();
        delete _job;
        return nullptr;
      }
  };

  /**
   * Same as TBBJobWrapper but for background jobs. Usually, background jobs
   * are enqueued and then processed one by one by a background job consumer.
   * There are however background jobs which should be done asap. Those guys
   * are directly mapped onto a TBB task.
   */
  class TBBBackgroundJobWrapper: public tbb::task {
    private:
      tarch::multicore::jobs::BackgroundJob*        _job;
    public:
 	  TBBBackgroundJobWrapper( tarch::multicore::jobs::BackgroundJob* job ):
        _job(job) {
      }

      tbb::task* execute() {
        _job->run();
        delete _job;
        return nullptr;
      }
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
    std::function<void()>&  job,
    tbb::atomic<int>&       semaphore,
    bool                    isTask,
    int                     jobClass
  ) {
    if ( isTask ) {
      job();
      semaphore.fetch_and_add(-1);
    }
    else {
      getJobQueue(jobClass).jobs.push(
        new JobWithoutCopyOfFunctorAndSemaphore(job, semaphore, isTask, jobClass )
      );

      logDebug( "spawnBlockingJob(...)", "enqueued job. tasks in this queue of class " << jobClass << "=" << getJobQueue(jobClass).jobs.unsafe_size() );
    }
  }


  /**
   * Process background tasks. If there is a large number of background tasks
   * pending, we do not process all of them but only up to maxJobs. The reason
   * is simple: background job consumer tasks are enqueued with low priority.
   * Whenever TBB threads become idle, they steal those consumer tasks and thus
   * start to process the jobs. However, these consumer tasks now should not do
   * all of the jobs, as we otherwise run risk that the (more importan) actual
   * implementation has to wait for the background jobs to be finished. Thus,
   * this routine does only up to a certain number of jobs.
   */
  bool processNumberOfBackgroundJobs(int maxJobs) {
    logDebug( "processNumberOfBackgroundJobs()", "background consumer task becomes awake" );

    tarch::multicore::jobs::BackgroundJob* myTask = nullptr;
    bool gotOne = _backgroundJobs.try_pop(myTask);
    bool result = false;
    while (gotOne && maxJobs>0) {
      logDebug( "processNumberOfBackgroundJobs()", "consumer task found job to do" );
      const bool reschedule = myTask->run();
      const bool taskHasBeenLongRunning = myTask->isLongRunning();
      if (reschedule) {
        _backgroundJobs.push( myTask );
      }
      else {
        delete myTask;
      }
      maxJobs--;
      result = true;
      if ( maxJobs>0 && !taskHasBeenLongRunning ) {
        gotOne = _backgroundJobs.try_pop(myTask);
      }
    }

    logDebug( "processNumberOfBackgroundJobs()", "background task consumer is done and kills itself" );

    return result;
  }

  /**
   * This is a task which consumes background jobs, as it invokes
   * processBackgroundJobs(). Typically, I make such a job consume up to
   * half of the available background jobs, before it then stops the
   * processing. When it stops and finds out that there would still
   * have been more jobs to process, then it enqueues another consumer task
   * to continue to work on the jobs at a later point.
   */
  class BackgroundJobConsumerTask: public tbb::task {
    private:
	  const int _maxJobs;
      BackgroundJobConsumerTask(int maxJobs):
        _maxJobs(maxJobs) {
      }
    public:
      static tbb::task_group_context  backgroundTaskContext;

      static void enqueue() {
        _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(1);
        BackgroundJobConsumerTask* tbbTask = new(tbb::task::allocate_root(backgroundTaskContext)) BackgroundJobConsumerTask(
          std::max( 1, static_cast<int>(_backgroundJobs.unsafe_size())/2 )
        );
        tbb::task::enqueue(*tbbTask);
        backgroundTaskContext.set_priority(tbb::priority_low);
      }

      tbb::task* execute() {
        processNumberOfBackgroundJobs(_maxJobs);
        _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(-1);
        if (!_backgroundJobs.empty()) {
          enqueue();
        }
        return nullptr;
      }
  };


  tbb::task_group_context  BackgroundJobConsumerTask::backgroundTaskContext;
}


void tarch::multicore::jobs::terminateAllPendingBackgroundConsumerJobs() {
  BackgroundJobConsumerTask::backgroundTaskContext.cancel_group_execution();
}


void tarch::multicore::jobs::spawnBackgroundJob(BackgroundJob* job) {
  BackgroundJobType mode = job->getJobType();

  switch (mode) {
    case BackgroundJobType::ProcessImmediately:
      job->run();
      delete job;
      break;
    case BackgroundJobType::IsTaskAndRunAsSoonAsPossible:
      {
        TBBBackgroundJobWrapper* tbbTask = new(tbb::task::allocate_root()) TBBBackgroundJobWrapper(job);
        tbb::task::spawn(*tbbTask);
      }
      break;
    case BackgroundJobType::BackgroundJob:
      {
        _backgroundJobs.push(job);
        
        const int currentlyRunningBackgroundThreads = _numberOfRunningBackgroundJobConsumerTasks;
        if (
          currentlyRunningBackgroundThreads<BackgroundJob::_maxNumberOfRunningBackgroundThreads
        ) {
          logDebug( "kickOffBackgroundTask(BackgroundTask*)", "no consumer task running yet or long-running task dropped in; kick off" );
          BackgroundJobConsumerTask::enqueue();
        }
      }
      break;
    case BackgroundJobType::LongRunningBackgroundJob:
      {
        _backgroundJobs.push(job);
        BackgroundJobConsumerTask::enqueue();
      }
      break;
  }
}


/**
 * This routine is typically invoked by user codes to ensure that all
 * background jobs have finished before the user code continues. We have however
 * to take into account that some background jobs might reschedule themselves
 * again as they are persistent. Therefore, we quickly check how many jobs are
 * still pending. Then we add the number of running background jobs (as those
 * guys might reschedule themselves again, so we try to be on the same side).
 * Finally, we process that many jobs that are in the queue and tell the
 * calling routine whether we've done any.
 */
bool tarch::multicore::jobs::processBackgroundJobs() {
  const int numberOfBackgroundJobs = _backgroundJobs.unsafe_size() + _numberOfRunningBackgroundJobConsumerTasks + 1;

/*
  #ifdef Asserts
  logInfo( "processBackgroundJobs()", "process " << numberOfBackgroundJobs << " job(s) as there are " << _backgroundJobs.unsafe_size()
    << " jobs pending and " << _numberOfRunningBackgroundJobConsumerTasks << " consumer tasks running right now"
  );
  #endif
*/

  return processNumberOfBackgroundJobs(numberOfBackgroundJobs);
}


int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return _backgroundJobs.unsafe_size();
}


/**
 * Spawn means a thread fires a new job and wants to continue itself.
 *
 * <h2> The spawned job is a task </h2>
 *
 * That means that the new job has no dependencies on any other job. It is
 * thus convenient to launch a real TBB task for it.
 *
 * <h2> The spawned job is not a task </h2>
 *
 * We enqueue it. We may not immediately spawn a job consumer task, as this
 * might mean that TBB might immediately start to consume the job and halt the
 * current thread. This is not what we want: We want to continue with the
 * calling thread immediately.
 */
void tarch::multicore::jobs::spawn(Job*  job) {
  if ( job->isTask() ) {
    logDebug( "spawn(Job*)", "job is a task, so issue TBB task immediately that handles job" );
    TBBJobWrapper* tbbTask = new(tbb::task::allocate_root()) TBBJobWrapper(job);
    tbb::task::spawn(*tbbTask);
  }
  else {
    getJobQueue(job->getClass()).jobs.push(job);

    logDebug( "spawn(Job*)", "enqueued job of class " << job->getClass() );
  }
}


void tarch::multicore::jobs::spawn(std::function<void()>& job, bool isTask, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,isTask,jobClass) );
}


/**
 * @see processJobs()
 */
int tarch::multicore::jobs::getNumberOfPendingJobs() {
  int result = 0;
  logDebug( "processJobs()", "there are " << _pendingJobs.size() << " class queues" );
  for (auto& p: _pendingJobs) {
	result += p.second.jobs.unsafe_size();
  }
  return result;
}


bool tarch::multicore::jobs::processJobs(int jobClass) {
  logDebug( "processJobs()", "search for jobs of class " << jobClass );

  Job* myTask   = nullptr;
  bool gotOne   = getJobQueue(jobClass).jobs.try_pop(myTask);
  bool result   = false;
  while (gotOne) {
    result   = true;
    logDebug( "processJob(int)", "start to process job of class " << jobClass );
    myTask->run();
    delete myTask;
    logDebug(
      "processJob(int)", "job of class " << jobClass << " complete, there are still " <<
	  getJobQueue(jobClass).jobs.unsafe_size() <<
	  " jobs of this class pending"
	);
    gotOne = getJobQueue(jobClass).jobs.try_pop(myTask);
  }

  return result;
}


/**
 * Work way through the individual queues. Ensure that queues in turn do not 
 * invoke processJobs() again, i.e. pass in false as argument, as we otherwise
 * obtain endless cascadic recursion. The routine should not spawn new tasks on
 * its own, as it is itself used by the job consumer tasks.
 *
 * <h2> Implementation </h2>
 *
 * It is absolutely essential that one uses auto&. With a copy/read-only
 * reference, the code crashes if someone insert stuff concurrently.
 *
 * <h2> Danger </h2>
 *
 * If you run with many job classes, i.e. 'tasks' that depend on each other, then
 * invoking this routine is dangerous. It bears the risk that you spawn more and
 * more jobs that depend on another job and you thus run into a situation, where
 * all TBB tasks process one particular job type.
 */
bool tarch::multicore::jobs::processJobs() {
  bool result = false;

  for (auto& p: _pendingJobs) {
	result |= processJobs(p.first);
  }
  
  return result;
}




/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,  
  bool                    isTask0,
  bool                    isTask1,
  int                     jobClass0,
  int                     jobClass1
) {
  tbb::atomic<int>  semaphore(2);
  
  tbb::parallel_invoke(
    [&] () -> void {
      spawnBlockingJob( job0, semaphore, isTask0, jobClass0 );
    },
    [&] () -> void {
      spawnBlockingJob( job1, semaphore, isTask1, jobClass1 );
    }
  );


  while (semaphore>0) {
    tbb::parallel_invoke(
      [&] () -> void {
        processJobs(jobClass0);
      },
      [&] () -> void {
        processJobs(jobClass1);
      }
    );
  }
}



/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2
) {
  tbb::atomic<int>  semaphore(3);

  tbb::parallel_invoke(
    [&] () -> void {
      spawnBlockingJob( job0, semaphore, isTask0, jobClass0 );
    },
    [&] () -> void {
      spawnBlockingJob( job1, semaphore, isTask1, jobClass1 );
    },
    [&] () -> void {
      spawnBlockingJob( job2, semaphore, isTask2, jobClass2 );
    }
  );

  while (semaphore>0) {
    tbb::parallel_invoke(
      [&] () -> void {
        processJobs(jobClass0);
      },
      [&] () -> void {
        processJobs(jobClass1);
      },
      [&] () -> void {
        processJobs(jobClass2);
      }
    );
  }
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  std::function<void()>&  job3,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  bool                    isTask3,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3
) {
  tbb::atomic<int>  semaphore(4);
  
  tbb::parallel_invoke(
    [&] () -> void {
      spawnBlockingJob( job0, semaphore, isTask0, jobClass0 );
    },
    [&] () -> void {
      spawnBlockingJob( job1, semaphore, isTask1, jobClass1 );
    },
    [&] () -> void {
      spawnBlockingJob( job2, semaphore, isTask2, jobClass2 );
    },
    [&] () -> void {
      spawnBlockingJob( job3, semaphore, isTask3, jobClass3 );
    }
  );
  
  while (semaphore>0) {
    tbb::parallel_invoke(
      [&] () -> void {
        processJobs(jobClass0);
      },
      [&] () -> void {
        processJobs(jobClass1);
      },
      [&] () -> void {
        processJobs(jobClass2);
      },
      [&] () -> void {
        processJobs(jobClass3);
      }
    );
  }
}



/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>& job0,
  std::function<void()>& job1,
  std::function<void()>& job2,
  std::function<void()>& job3,
  std::function<void()>& job4,
	 bool                    isTask0,
	 bool                    isTask1,
	 bool                    isTask2,
	 bool                    isTask3,
	 bool                    isTask4,
	 int                     jobClass0,
	 int                     jobClass1,
	 int                     jobClass2,
	 int                     jobClass3,
	 int                     jobClass4
) {
  tbb::atomic<int>  semaphore(5);
  
  tbb::parallel_invoke(
    [&] () -> void {
      spawnBlockingJob( job0, semaphore, isTask0, jobClass0 );
    },
    [&] () -> void {
      spawnBlockingJob( job1, semaphore, isTask1, jobClass1 );
    },
    [&] () -> void {
      spawnBlockingJob( job2, semaphore, isTask2, jobClass2 );
    },
    [&] () -> void {
      spawnBlockingJob( job3, semaphore, isTask3, jobClass3 );
    },
    [&] () -> void {
      spawnBlockingJob( job4, semaphore, isTask4, jobClass4 );
    }
  );
  

  while (semaphore>0) {
    tbb::parallel_invoke(
      [&] () -> void {
        processJobs(jobClass0);
      },
      [&] () -> void {
        processJobs(jobClass1);
      },
      [&] () -> void {
        processJobs(jobClass2);
      },
      [&] () -> void {
        processJobs(jobClass3);
      },
      [&] () -> void {
        processJobs(jobClass4);
      }
    );
  }
}



/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  std::function<void()>&  job3,
  std::function<void()>&  job4,
  std::function<void()>&  job5,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  bool                    isTask3,
  bool                    isTask4,
  bool                    isTask5,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4,
  int                     jobClass5
) {
  tbb::atomic<int>  semaphore(6);
  
  tbb::parallel_invoke(
    [&] () -> void {
      spawnBlockingJob( job0, semaphore, isTask0, jobClass0 );
    },
    [&] () -> void {
      spawnBlockingJob( job1, semaphore, isTask1, jobClass1 );
    },
    [&] () -> void {
      spawnBlockingJob( job2, semaphore, isTask2, jobClass2 );
    },
    [&] () -> void {
      spawnBlockingJob( job3, semaphore, isTask3, jobClass3 );
    },
    [&] () -> void {
      spawnBlockingJob( job4, semaphore, isTask4, jobClass4 );
    },
    [&] () -> void {
      spawnBlockingJob( job5, semaphore, isTask5, jobClass5 );
    }
  );
  
  while (semaphore>0) {
    tbb::parallel_invoke(
      [&] () -> void {
        processJobs(jobClass0);
      },
      [&] () -> void {
        processJobs(jobClass1);
      },
      [&] () -> void {
        processJobs(jobClass2);
      },
      [&] () -> void {
        processJobs(jobClass3);
      },
      [&] () -> void {
        processJobs(jobClass4);
      },
      [&] () -> void {
        processJobs(jobClass5);
      }
    );
  }
}


#endif
