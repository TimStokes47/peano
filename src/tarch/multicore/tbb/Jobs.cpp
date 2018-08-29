#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/tbb/Jobs.h"


#include <vector>
#include <limits>





tarch::logging::Log tarch::multicore::jobs::internal::_log( "tarch::multicore::jobs::internal" );


tbb::atomic<int>                              tarch::multicore::jobs::internal::_numberOfRunningBackgroundJobConsumerTasks(0);
tarch::multicore::jobs::internal::JobQueue    tarch::multicore::jobs::internal::_pendingJobs[NumberOfJobQueues];
tbb::atomic<int>                              tarch::multicore::jobs::internal::MaxSizeOfBackgroundQueue;



//
// This is a bug in Intel's TBB as documented on
//
// https://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/700057
//
// These task groups have to be static inside a cpp file.
//
static tbb::task_group_context  backgroundTaskContext(tbb::task_group_context::isolated);
static tbb::task_group_context  importantTaskContext(tbb::task_group_context::isolated);


tarch::multicore::jobs::internal::BackgroundJobConsumerTask::BackgroundJobConsumerTask(int maxJobs):
  _maxJobs(maxJobs) {
}


tarch::multicore::jobs::internal::BackgroundJobConsumerTask::BackgroundJobConsumerTask(const BackgroundJobConsumerTask& copy):
  _maxJobs(copy._maxJobs) {
}


int tarch::multicore::jobs::internal::getMinimalNumberOfJobsPerBackgroundConsumerRun() {
  const int MinimumNumberOfJobsPerConsumer = 1;
  return std::max(
    MinimumNumberOfJobsPerConsumer,
	tarch::multicore::jobs::internal::MaxSizeOfBackgroundQueue.load() / tarch::multicore::Core::getInstance().getNumberOfThreads()
  );
}


void tarch::multicore::jobs::internal::BackgroundJobConsumerTask::enqueue() {
  _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(1);
  BackgroundJobConsumerTask* tbbTask = new (tbb::task::allocate_root(::backgroundTaskContext)) BackgroundJobConsumerTask(
	getMinimalNumberOfJobsPerBackgroundConsumerRun()
  );
  tbb::task::enqueue(*tbbTask);
  ::backgroundTaskContext.set_priority(tbb::priority_low);
}


tbb::task* tarch::multicore::jobs::internal::BackgroundJobConsumerTask::execute() {
  _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(-1);

  processJobs(internal::BackgroundJobsJobClassNumber,_maxJobs);

  return nullptr;
}


tarch::multicore::jobs::internal::JobQueue& tarch::multicore::jobs::internal::getJobQueue( int jobClass ) {
  assertion( jobClass>=0 );

  return _pendingJobs[jobClass % NumberOfJobQueues];
}


void tarch::multicore::jobs::internal::insertJob( int jobClass, Job* job ) {
  #if defined(TBBUsesLocalQueueWhenProcessingJobs)
  internal::getJobQueue(jobClass).mutex.lock();
  internal::getJobQueue(jobClass).jobs.push_back( job );
  internal::getJobQueue(jobClass).mutex.unlock();
  #else
  internal::getJobQueue(jobClass).push( job );
  #endif
}


int  tarch::multicore::jobs::internal::getJobQueueSize( int jobClass ) {
  #if defined(TBBUsesLocalQueueWhenProcessingJobs)
  //internal::getJobQueue( jobClass ).mutex.lock();
  int result = static_cast<int>(internal::getJobQueue( jobClass ).jobs.size());
  //internal::getJobQueue( jobClass ).mutex.unlock();
  return result;
  #else
  return static_cast<int>(internal::getJobQueue( jobClass ).unsafe_size());
  #endif
}


void tarch::multicore::jobs::internal::spawnBlockingJob(
  std::function<bool()>&  job,
  JobType                 jobType,
  int                     jobClass,
  tbb::atomic<int>&       semaphore
) {
  if ( jobType!=JobType::Job ) {
    job();
    semaphore.fetch_and_add(-1);
  }
  else {
	insertJob(
	  jobClass,
	  new JobWithoutCopyOfFunctorAndSemaphore(job, jobType, jobClass, semaphore )
	);
  }
}



void tarch::multicore::jobs::terminateAllPendingBackgroundConsumerJobs() {
  backgroundTaskContext.cancel_group_execution();
}



void tarch::multicore::jobs::spawnBackgroundJob(Job* job) {
  switch (job->getJobType()) {
    case JobType::ProcessImmediately:
      while (job->run()) {};
      delete job;
      break;
    case JobType::RunTaskAsSoonAsPossible:
      {
        internal::TBBJobWrapper* tbbTask = new(tbb::task::allocate_root(importantTaskContext)) internal::TBBJobWrapper(job);
        // we may not use spawn as spawn relies on the current context and thus kills us if this context is already freed
        //        tbb::task::spawn(*tbbTask);
        tbb::task::enqueue(*tbbTask);
        importantTaskContext.set_priority(tbb::priority_high);
      }
      break;
    case JobType::MPIReceiveTask:
    case JobType::Task:
    case JobType::Job:
      {
    	internal::insertJob(internal::BackgroundJobsJobClassNumber,job);

        const int oldSize = std::max(internal::MaxSizeOfBackgroundQueue.load(),2);
        internal::MaxSizeOfBackgroundQueue = std::max(oldSize-1,internal::getJobQueueSize(internal::BackgroundJobsJobClassNumber));

        const int currentlyRunningBackgroundThreads = internal::_numberOfRunningBackgroundJobConsumerTasks.load();
        if (
          currentlyRunningBackgroundThreads<Job::_maxNumberOfRunningBackgroundThreads
        ) {
          internal::BackgroundJobConsumerTask::enqueue();
        }
      }
      break;
  }
}



int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return internal::getJobQueueSize( internal::BackgroundJobsJobClassNumber ) + internal::_numberOfRunningBackgroundJobConsumerTasks.load();
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
    internal::TBBJobWrapper* tbbTask = new(tbb::task::allocate_root(importantTaskContext)) internal::TBBJobWrapper(job);
    tbb::task::enqueue(*tbbTask);
  }
  else {
	internal::insertJob( job->getClass(), job );
  }
}


void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,jobType,jobClass) );
}


/**
 * If we are handling background jobs and if there are still jobs left once
 * we've done our share, then we spawn a new background task consumer job.
 * This is done for cases where the master processes background jobs and there
 * are plenty of them left. In such a case, the master might be totally blocked
 * by the background jobs (if processJobs() is called in a while loop) and
 * should actually use further tasks instead.
 */
bool tarch::multicore::jobs::processJobs(int jobClass, int maxNumberOfJobs) {
  bool result = false;

  #if defined(TBBUsesLocalQueueWhenProcessingJobs)
    std::list<tarch::multicore::jobs::Job*> localJobs;

    internal::getJobQueue(jobClass).mutex.lock();
    auto firstIteration = internal::getJobQueue(jobClass).jobs.begin();
    auto lastIteration  = internal::getJobQueue(jobClass).jobs.begin();
    maxNumberOfJobs = std::min(
      maxNumberOfJobs,
	  static_cast<int>( internal::getJobQueue(jobClass).jobs.size() )
	);
    std::advance( lastIteration, maxNumberOfJobs );
    internal::getJobQueue(jobClass).jobs.splice(
      localJobs.begin(), localJobs,
      firstIteration, lastIteration
    );
    internal::getJobQueue(jobClass).mutex.unlock();

    result = !localJobs.empty();

    for (
      std::list<tarch::multicore::jobs::Job*>::iterator p = localJobs.begin();
      p!=localJobs.end();
      p++
	) {
      #ifdef TBBPrefetchesJobData
      if (p!=std::prev(localJobs.end())) {
        std::list<tarch::multicore::jobs::Job*>::iterator prefetch = p;
        std::advance(prefetch,1);
        (*prefetch)->prefetchData();
      }
      #endif

      bool reschedule = (*p)->run();
      if (reschedule) {
        internal::insertJob(jobClass,*p);
      }
      else {
        delete *p;
      }
    }
  #elif defined(TBBPrefetchesJobData)
    Job* myTask           = nullptr;
    Job* prefetchedTask   = nullptr;
    bool gotOne           = internal::getJobQueue(jobClass).try_pop(myTask);
    bool prefetchedOne    = internal::getJobQueue(jobClass).try_pop(prefetchedTask);

    // consistency rollover in case somebody did sqeeze in-between the two
    // try_pops.
    if (prefetchedOne and !gotOne) {
      prefetchedOne  = false;
      gotOne         = true;
      myTask         = prefetchedTask;
      prefetchedTask = nullptr;
    }

    while (gotOne) {
      result = true;

      if (prefetchedOne) {
    	prefetchedTask->prefetchData();
      }

      bool reschedule = myTask->run();
      if (reschedule) {
        internal::getJobQueue(jobClass).push(myTask);
        maxNumberOfJobs--;
      }
      else {
        delete myTask;
        maxNumberOfJobs--;
      }

      if ( maxNumberOfJobs>1 and prefetchedOne ) {
        gotOne        = prefetchedOne;
        myTask        = prefetchedTask;
        prefetchedOne = internal::getJobQueue(jobClass).try_pop(prefetchedTask);
      }
      else if ( prefetchedOne ) {
        gotOne         = prefetchedOne;
        myTask         = prefetchedTask;
        prefetchedOne  = false;
        prefetchedTask = nullptr;
      }
      else {
        gotOne = false;
      }
    }
  #else
    Job* myTask   = nullptr;
    bool gotOne   = internal::getJobQueue(jobClass).try_pop(myTask);
    while (gotOne) {
      result = true;

      bool reschedule = myTask->run();
      if (reschedule) {
        internal::getJobQueue(jobClass).push(myTask);
        maxNumberOfJobs--;
      }
      else {
        delete myTask;
        maxNumberOfJobs--;
      }
      if ( maxNumberOfJobs>0 ) {
        gotOne = internal::getJobQueue(jobClass).try_pop(myTask);
      }
      else {
        gotOne = false;
      }
    }
  #endif

  if(
    jobClass==internal::BackgroundJobsJobClassNumber
    and
    internal::getJobQueueSize(jobClass)>0
    and
    internal::_numberOfRunningBackgroundJobConsumerTasks.load()==0) {
    internal::BackgroundJobConsumerTask::enqueue();
  }

  return result;
}


/**
 * This routine is typically invoked by user codes to ensure that all
 * background jobs have finished before the user code continues. It is not
 * modelling what happens throughout the computation.
 *
 * As some background jobs might reschedule themselves
 * again - see their operator() - we quickly check how many jobs are
 * still pending. The code is given an upper bound how many consumer tasks to
 * use, to we issue the difference. But obviously if and only if there are still
 * jobs pending.
 *
 * In a next step, we check whether there's still a number of background tasks
 * pending, and we handle those. Yet, we leave a few for the still pending
 * background tasks.
 */
void tarch::multicore::jobs::startToProcessBackgroundJobs() {
  const int maxBusyConsumerTasks = 
    std::max( 
      0, 
	  internal::getJobQueueSize( internal::BackgroundJobsJobClassNumber ) / internal::getMinimalNumberOfJobsPerBackgroundConsumerRun()
	);

  const int maxAdditionalBackgroundThreads =
    std::max(
      tarch::multicore::Core::getInstance().getNumberOfThreads()-1 - internal::_numberOfRunningBackgroundJobConsumerTasks.load(),
      0
    );
  
  const int additionalBackgroundThreads = std::min( maxBusyConsumerTasks, maxAdditionalBackgroundThreads );

  #ifdef Asserts
  if (additionalBackgroundThreads>0) {
    static tarch::logging::Log _log( "tarch::multicore::jobs" );
    logInfo(
      "startToProcessBackgroundJobs()",
      "spawn another " << additionalBackgroundThreads << " background job consumer tasks ("
        << internal::_numberOfRunningBackgroundJobConsumerTasks.load() << " task(s) already running)"
    );
  }
  #endif

  for (int i=0; i<additionalBackgroundThreads; i++) {
    internal::BackgroundJobConsumerTask::enqueue();
  }
}


bool tarch::multicore::jobs::finishToProcessBackgroundJobs() {
  processJobs( internal::BackgroundJobsJobClassNumber, internal::getMinimalNumberOfJobsPerBackgroundConsumerRun() );

  return internal::getJobQueueSize(internal::BackgroundJobsJobClassNumber)>0;
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  JobType                 jobType0,
  JobType                 jobType1,
  int                     jobClass0,
  int                     jobClass1
) {

/*
  This version is serial and works
  --------------------------------

  job0();
  job1();
*/


/*
   This version is parallel and makes TBB crash
  ---------------------------------------------

   tbb::atomic<int>  semaphore(2);
  
  tbb::parallel_invoke(
    [&] () -> void {
	  internal::spawnBlockingJob( job0, semaphore, jobType0, jobClass0 );
    },
    [&] () -> void {
    	internal::spawnBlockingJob( job1, semaphore, jobType1, jobClass1 );
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
  }*/

  tbb::atomic<int>  semaphore(2);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  //g.wait();

  g.run( [&]() { processJobs(jobClass0); });
  g.run( [&]() { processJobs(jobClass1); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    processJobs(jobClass0);
    processJobs(jobClass1); 

    // There are several gaming companies that report that busy loops/spinning
    // over atomics did deadlock in their case (the atomic never was updated)
    // if they didn't use a yield. In our case, some applications seem to
    // exhibit exactly the same behaviour, so I decided to yield here.
    tbb::this_tbb_thread::yield();

    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(3xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 );
      deadlockCounter = 0;
    }
    #endif
  }
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  std::function<bool()>&  job2,
  JobType                 jobType0,
  JobType                 jobType1,
  JobType                 jobType2,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2
) {
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  //g.wait();

  g.run( [&]() { processJobs(jobClass0); });
  g.run( [&]() { processJobs(jobClass1); });
  g.run( [&]() { processJobs(jobClass2); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    processJobs(jobClass0);
    processJobs(jobClass1);
    processJobs(jobClass2);

    tbb::this_tbb_thread::yield();

    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(3xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 );
      deadlockCounter = 0;
    }
    #endif
  }
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  std::function<bool()>&  job2,
  std::function<bool()>&  job3,
  JobType                 jobType0,
  JobType                 jobType1,
  JobType                 jobType2,
  JobType                 jobType3,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3
) {
  tbb::atomic<int>  semaphore(4);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, jobType3, jobClass3, semaphore ); });
  //g.wait();

  g.run( [&]() { processJobs(jobClass0); });
  g.run( [&]() { processJobs(jobClass1); });
  g.run( [&]() { processJobs(jobClass2); });
  g.run( [&]() { processJobs(jobClass3); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    processJobs(jobClass0);
    processJobs(jobClass1);
    processJobs(jobClass2);
    processJobs(jobClass3);

    tbb::this_tbb_thread::yield();

    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(4xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 << ", " << jobClass3 );
      deadlockCounter = 0;
    }
    #endif
  }
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  std::function<bool()>&  job2,
  std::function<bool()>&  job3,
  std::function<bool()>&  job4,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
  JobType                    jobType3,
  JobType                    jobType4,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4
) {
  tbb::atomic<int>  semaphore(5);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, jobType3, jobClass3, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job4, jobType4, jobClass4, semaphore ); });
  //g.wait();

  g.run( [&]() { processJobs(jobClass0); });
  g.run( [&]() { processJobs(jobClass1); });
  g.run( [&]() { processJobs(jobClass2); });
  g.run( [&]() { processJobs(jobClass3); });
  g.run( [&]() { processJobs(jobClass4); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    processJobs(jobClass0); 
    processJobs(jobClass1); 
    processJobs(jobClass2); 
    processJobs(jobClass3); 
    processJobs(jobClass4); 

    tbb::this_tbb_thread::yield();

    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(5xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 << ", " << jobClass3 << ", " << jobClass4 );
      deadlockCounter = 0;
    }
    #endif
  }
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  std::function<bool()>&  job2,
  std::function<bool()>&  job3,
  std::function<bool()>&  job4,
  std::function<bool()>&  job5,
  JobType                 jobType0,
  JobType                 jobType1,
  JobType                 jobType2,
  JobType                 jobType3,
  JobType                 jobType4,
  JobType                 jobType5,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4,
  int                     jobClass5
) {
  tbb::atomic<int>  semaphore(6);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, jobType3, jobClass3, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job4, jobType4, jobClass4, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job5, jobType5, jobClass5, semaphore ); });
  //g.wait();

  g.run( [&]() { processJobs(jobClass0); });
  g.run( [&]() { processJobs(jobClass1); });
  g.run( [&]() { processJobs(jobClass2); });
  g.run( [&]() { processJobs(jobClass3); });
  g.run( [&]() { processJobs(jobClass4); });
  g.run( [&]() { processJobs(jobClass5); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    processJobs(jobClass0); 
    processJobs(jobClass1); 
    processJobs(jobClass2); 
    processJobs(jobClass3); 
    processJobs(jobClass4);
    processJobs(jobClass5);

    tbb::this_tbb_thread::yield();

    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(6xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 << ", " << jobClass3 << ", " << jobClass4 << ", " << jobClass5 );
      deadlockCounter = 0;
    }
    #endif
  }
}


std::string tarch::multicore::jobs::internal::report() {
  std::ostringstream msg;
  msg << "job-system-status: background-queue-no=" << internal::BackgroundJobsJobClassNumber
	  << ", no-of-running-bg-consumer-tasks=" << _numberOfRunningBackgroundJobConsumerTasks.load();
  for (int i=0; i<NumberOfJobQueues; i++) {
	msg << ",queue[" << i
        << "]=" << internal::getJobQueueSize(i) << " job(s)";
  }
  return msg.str();
}

#endif
