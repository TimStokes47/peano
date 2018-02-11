#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"

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
   * Number of actively running background tasks.
   */
  tbb::atomic<int>         _numberOfRunningBackgroundJobConsumerTasks(0);

  /**
   * The active tasks
   */
  tbb::concurrent_queue<tarch::multicore::jobs::BackgroundJob*>  _backgroundJobs;

  struct JobQueue {
    tbb::concurrent_queue<tarch::multicore::jobs::Job*> jobs;
  };

  typedef tbb::concurrent_hash_map< int, JobQueue >  JobMap;

  JobMap     _pendingJobs;


  tarch::logging::Log _log( "tarch::multicore" );



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
   * processBackgroundJobs(). It is tied to one particular job 
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
    

  class TBBFunctorWrapper: public tbb::task {
    private:
      std::function<void()>&   _functor;
      tbb::atomic<int>&        _semaphore;
    public:
      TBBFunctorWrapper( std::function<void()>&  functor, tbb::atomic<int>& semaphore ):
        _functor(functor),
        _semaphore(semaphore) {
      }
           
      tbb::task* execute() {
        _functor();
        #ifdef Asserts
        int result = _semaphore.fetch_and_add(-1);
        assertion( result>=1 );
        #else
        _semaphore.fetch_and_add(-1);
        #endif
        return nullptr;
      }
  };

  /**
   * Helper function of the for loops and the parallel task invocations.
   *
   * Primarily invoked by the spawnAndWait routines.
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

      // @todo revise comments: Here, we may neither spawn nor enqueue as this causes deadlocks; other producer tasks might then not be active,
      //       while these guys start off immediately and then wait for their dependencies
      // JobConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) JobConsumerTask(jobClass,false);
      // tbb::task::enqueue(*tbbTask);
      // _backgroundTaskContext.set_priority(tbb::priority_high);
    }
  }

  bool processNumberOfBackgroundJobs(int maxJobs) {
    logDebug( "processNumberOfBackgroundJobs()", "background consumer task becomes awake" );

    tarch::multicore::jobs::BackgroundJob* myTask = nullptr;
    bool gotOne = _backgroundJobs.try_pop(myTask);
    bool result = false;
    while (gotOne && maxJobs>0) {
      logDebug( "processNumberOfBackgroundJobs()", "consumer task found job to do" );
      myTask->run();
      const bool taskHasBeenLongRunning = myTask->isLongRunning();
      delete myTask;
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
   * processBackgroundJobs().
   */
  class BackgroundJobConsumerTask: public tbb::task {
    private:
	  const int _maxJobs;
      BackgroundJobConsumerTask(int maxJobs):
        _maxJobs(maxJobs) {
      }
    public:
      static void enqueue() {
        static tbb::task_group_context  backgroundTaskContext;
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
        //static tbb::task_group_context  backgroundTaskContext;
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
    case BackgroundJobType::PersistentBackgroundJob:
      {
   	    static tbb::task_group_context  backgroundTaskContext;
        TBBBackgroundJobWrapper* tbbTask = new(tbb::task::allocate_root(backgroundTaskContext)) TBBBackgroundJobWrapper(job);
/*
    	xxxx
      TBBBackgroundJobWrapper* tbbTask = new(tbb::task::allocate_root()) TBBBackgroundJobWrapper(job);
*/
        tbb::task::enqueue(*tbbTask);
        backgroundTaskContext.set_priority(tbb::priority_low);
      }
      break;
  }
}


bool tarch::multicore::jobs::processBackgroundJobs() {
  bool result = false;

  while (_numberOfRunningBackgroundJobConsumerTasks>0) {
	result |= processNumberOfBackgroundJobs(std::numeric_limits<int>::max());
  }

  return result;
}




int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return _backgroundJobs.unsafe_size();
}


/**
 * Spawn means a thread fires a new job and wants to coninueu itself.
 * There is no parent-child relation established directly.
 *
 * <h2> The spawned job is a task </h2>
 *
 * That means that the new job has no dependencies on any other job. It is
 * thus convenient to launch a real TBB task for it.
 *
 * <h2> The spawned job is not a task </h2>
 *
 * We enqueue it. We may not immediately.
 */
void tarch::multicore::jobs::spawn(Job*  job) {
  if ( job->isTask() ) {
    logDebug( "spawn(Job*)", "job is a task, so issue TBB task immediately that handles job" );
    //static tbb::task_group_context  jobTaskContext;
    // @todo Change withotu context
    TBBJobWrapper* tbbTask = new(tbb::task::allocate_root()) TBBJobWrapper(job);
    tbb::task::spawn(*tbbTask);
  }
  else {
    getJobQueue(job->getClass()).jobs.push(job);

    logDebug( "spawn(Job*)", "enqueued job of class " << job->getClass() );

    // Vorsicht hier
    //JobConsumerTask* tbbTask = new(tbb::task::allocate_root(_jobTaskContext)) JobConsumerTask(job->getClass(),false);
    //tbb::task::spawn(*tbbTask);
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
  result |= processBackgroundJobs();
  
  return result;
}



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
