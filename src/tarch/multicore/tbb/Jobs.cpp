#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"

#include <vector>
#include <tbb/task.h>
#include <tbb/concurrent_queue.h>
#include <tbb/parallel_invoke.h>
#include <tbb/tbb_machine.h>
#include <tbb/task.h>
#include <tbb/tbb_thread.h>
#include <tbb/task_group.h>


namespace {
  /**
   * Use this to launch all background with very low priority
   */
  tbb::task_group_context  _backgroundTaskContext;
  tbb::task_group_context  _jobTaskContext;

  /**
   * So we use the background task context above explicitly as we rely on
   * enqueue quite a lot. Whenever we however span a task directly, we do
   * spawn it into this task group using TBB's novel functor/lambda
   * interface.
   */
  ::tbb::task_group        _task_group;

  /**
   * Number of actively running background tasks.
   */
  tbb::atomic<int>         _numberOfRunningBackgroundThreads(0);

  /**
   * The active tasks
   */
  tbb::concurrent_queue<tarch::multicore::jobs::BackgroundJob*>  _backgroundJobs;
  tbb::concurrent_queue<tarch::multicore::jobs::Job*>            _jobs;


  tarch::logging::Log _log( "tarch::multicore" );


  /**
   * This is a task which consumes background jobs, as it invokes
   * processBackgroundJobs().
   */
  class BackgroundJobConsumerTask: public tbb::task {
    public:
      BackgroundJobConsumerTask() {}
      tbb::task* execute() {
        tarch::multicore::jobs::processBackgroundJobs();
        _numberOfRunningBackgroundThreads.fetch_and_add(-1);
        return nullptr;
      }
  };


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
        tarch::multicore::jobs::processJob(_jobClass);
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
}


void tarch::multicore::jobs::spawnBackgroundJob(BackgroundJob* task) {
  BackgroundJobType mode = task->getJobType();

  switch (mode) {
    case BackgroundJobType::ProcessImmediately:
      task->run();
      delete task;
      break;
    case BackgroundJobType::RunAsSoonAsPossible:
      {
        // This is basically an alternative for spawn introduced with newer TBB version
        _task_group.run(
          [task]() {
            task->run();
            delete task;
          }
        );
      }
      break;
    case BackgroundJobType::BackgroundJob:
      {
        _backgroundJobs.push(task);
        
        const int currentlyRunningBackgroundThreads = _numberOfRunningBackgroundThreads;
        if (
          currentlyRunningBackgroundThreads<BackgroundJob::_maxNumberOfRunningBackgroundThreads
        ) {
          logDebug( "kickOffBackgroundTask(BackgroundTask*)", "no consumer task running yet or long-running task dropped in; kick off" );
          _numberOfRunningBackgroundThreads.fetch_and_add(1);
          BackgroundJobConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) BackgroundJobConsumerTask();
          tbb::task::enqueue(*tbbTask);
          _backgroundTaskContext.set_priority(tbb::priority_low);
          logDebug( "kickOffBackgroundTask(BackgroundTask*)", "it is out now" );
        }
      }
      break;
    case BackgroundJobType::LongRunningBackgroundJob:
      {
        _backgroundJobs.push(task);
        
        _numberOfRunningBackgroundThreads.fetch_and_add(1);
        BackgroundJobConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) BackgroundJobConsumerTask();
        tbb::task::enqueue(*tbbTask);
        _backgroundTaskContext.set_priority(tbb::priority_low);
        logDebug( "kickOffBackgroundTask(BackgroundTask*)", "it is out now" );
      }
      break;
    case BackgroundJobType::PersistentBackgroundJob:
      // This is basically an alternative for spawn introduced with newer TBB version; internally does run
      _task_group.run(
        [task]() {
          task->run();
          delete task;
        }
      );
      break;
  }
}


bool tarch::multicore::jobs::processBackgroundJobs() {
  logDebug( "execute()", "background consumer task becomes awake" );

  BackgroundJob* myTask = nullptr;
  bool gotOne = _backgroundJobs.try_pop(myTask);
  bool result = false;
  while (gotOne) {
    logDebug( "execute()", "consumer task found job to do" );
    myTask->run();
    const bool taskHasBeenLongRunning = myTask->isLongRunning();
    delete myTask;
    gotOne = taskHasBeenLongRunning ? false : _backgroundJobs.try_pop(myTask);
    result = true;
  }

  logDebug( "execute()", "background task consumer is done and kills itself" );

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
 * We enqueue it first of all.
 */
void tarch::multicore::jobs::spawn(Job*  job) {
  if ( job->isTask() ) {
    logDebug( "spawn(Job*)", "job is a task, so issue TBB task immediately that handles job" );
    TBBJobWrapper* tbbTask = new(tbb::task::allocate_root(_jobTaskContext)) TBBJobWrapper(job);
    tbb::task::spawn(*tbbTask);
  }
  else {
    _jobs.push(job);

    logDebug( "spawn(Job*)", "enqueued job and issue consumer TBB task" );

    JobConsumerTask* tbbTask = new(tbb::task::allocate_root(_jobTaskContext)) JobConsumerTask(job->getClass(),false);
    tbb::task::spawn(*tbbTask);
  }
}



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
 * straightaway as they, by definition, do not rely on input data while they are 
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
    _jobs.push(
      new JobWithoutCopyOfFunctorAndSemaphore(job, semaphore, isTask, jobClass )
    );

    logDebug( "spawnBlockingJob(Job*)", "enqueued job and issue consumer TBB task" );
  }
}



void tarch::multicore::jobs::spawn(std::function<void()>& job, bool isTask, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,isTask,jobClass) );
}


int tarch::multicore::jobs::getNumberOfPendingJobs() {
  return _jobs.unsafe_size();
}


bool tarch::multicore::jobs::processJob(int jobClass) {
  logDebug( "processJobs()", "search for jobs of class " << jobClass );

  Job* myTask   = nullptr;
  bool gotOne   = _jobs.try_pop(myTask);
  bool result   = false;
  bool foundOne = false;
  while (gotOne) {
    result   = true;
    foundOne = myTask->getClass()==jobClass;
    myTask->run();
    delete myTask;
    if (!foundOne) {
      gotOne = _jobs.try_pop(myTask);
    }
    else {
      gotOne = false;
    }
  }
  
  return result;
}


/**
 * Work way through the individual queues. Ensure that queues in turn do not 
 * invoke processJobs() again, i.e. pass in false as argument, as we otherwise
 * obtain endless cascadic recursion. The routine should not spawn new tasks on
 * its own, as it is itself used by the job consumer tasks.
 */
bool tarch::multicore::jobs::processJobs() {
  Job* myTask   = nullptr;
  bool gotOne   = _jobs.try_pop(myTask);
  bool result   = false;
  while (gotOne) {
    result   = true;
    myTask->run();
    delete myTask;
    gotOne = _jobs.try_pop(myTask);
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

  logDebug( "spawnAndWait(...)", "have handed both jobs over to system. Wait for them to terminate" );

  while (semaphore>0) {
    processJobs();    
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
  
  logDebug( "spawnAndWait(...)", "have handed both jobs over to runtime system. Wait for them to terminate" );

  if (semaphore>0) {
    tbb::task_group g;
    g.run( [&]() {processJob(jobClass0); } );
    g.run( [&]() {processJob(jobClass1); } );
    g.run( [&]() {processJob(jobClass2); } );
    g.wait();
  }

  while (semaphore>0) {
    processJobs();    
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
  
  logDebug( "spawnAndWait(...)", "have handed both jobs over to runtime system. Wait for them to terminate" );
  
  while (semaphore>0) {
    processJobs();    
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
  
  logDebug( "spawnAndWait(...)", "have handed both jobs over to runtime system. Wait for them to terminate" );
  
  while (semaphore>0) {
    processJobs();    
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
  
  logDebug( "spawnAndWait(...)", "have handed both jobs over to runtime system. Wait for them to terminate" );

  while (semaphore>0) {
    processJobs();    
  }
}



#endif
