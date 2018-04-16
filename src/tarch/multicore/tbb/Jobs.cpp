#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/tbb/Jobs.h"


#include <vector>
#include <limits>





tarch::logging::Log tarch::multicore::jobs::internal::_log( "tarch::multicore::jobs::internal" );


tbb::atomic<int>                                     tarch::multicore::jobs::internal::_numberOfRunningBackgroundJobConsumerTasks(0);
tarch::multicore::jobs::internal::JobMap             tarch::multicore::jobs::internal::_pendingJobs;

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


void tarch::multicore::jobs::internal::BackgroundJobConsumerTask::enqueue() {
  _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(1);
  BackgroundJobConsumerTask* tbbTask = new (tbb::task::allocate_root(::backgroundTaskContext)) BackgroundJobConsumerTask(
    std::max( TBBMinimalNumberOfJobsPerBackgroundConsumerRun, static_cast<int>(internal::getJobQueue( internal::BackgroundJobsJobClassNumber ).jobs.unsafe_size())/tarch::multicore::Core::getInstance().getNumberOfThreads() )
  );
  tbb::task::enqueue(*tbbTask);
  ::backgroundTaskContext.set_priority(tbb::priority_low);
  logDebug( "enqueue()", "spawned new background consumer task" );
}


tbb::task* tarch::multicore::jobs::internal::BackgroundJobConsumerTask::execute() {
  processJobs(internal::BackgroundJobsJobClassNumber,_maxJobs);

  const int newNumberOfBackgroundJobs = internal::getJobQueue( internal::BackgroundJobsJobClassNumber ).jobs.unsafe_size();

  _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(-1);

  if (
	(newNumberOfBackgroundJobs>0 && _numberOfRunningBackgroundJobConsumerTasks==0)
  ) {
    enqueue();
  }
  return nullptr;
}


tarch::multicore::jobs::internal::JobQueue& tarch::multicore::jobs::internal::getJobQueue( int jobClass ) {
	if ( _pendingJobs.count(jobClass)==0 ) {
    JobMap::accessor    a;
    _pendingJobs.insert( a, jobClass );
	}
  JobMap::accessor c;
  _pendingJobs.find( c, jobClass );
  return c->second;
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
    internal::getJobQueue(jobClass).jobs.push(
      new JobWithoutCopyOfFunctorAndSemaphore(job, jobType, jobClass, semaphore )
    );

    logDebug( "spawnBlockingJob(...)", "enqueued job. tasks in this queue of class " << jobClass << "=" << internal::getJobQueue(jobClass).jobs.unsafe_size() );
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
        internal::getJobQueue( internal::BackgroundJobsJobClassNumber ).jobs.push(job);
        
        const int currentlyRunningBackgroundThreads = internal::_numberOfRunningBackgroundJobConsumerTasks;
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
  return internal::getJobQueue( internal::BackgroundJobsJobClassNumber ).jobs.unsafe_size() + internal::_numberOfRunningBackgroundJobConsumerTasks;
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
    internal::getJobQueue(job->getClass()).jobs.push(job);

    logDebug( "spawn(Job*)", "enqueued job of class " << job->getClass() );
  }
}


void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,jobType,jobClass) );
}


bool tarch::multicore::jobs::processJobs(int jobClass, int maxNumberOfJobs) {
  logDebug( "processJobs()", "search for jobs of class " << jobClass );

  // Ensure that we do not redo all re-enqueued jobs within the same while loop
  maxNumberOfJobs = std::min(
    maxNumberOfJobs,
	static_cast<int>( internal::getJobQueue(jobClass).jobs.unsafe_size() )
  );

  Job* myTask   = nullptr;
  bool gotOne   = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
  bool result   = false;
  while (gotOne) {
    logDebug( "processJob(int)", "start to process job of class " << jobClass );
    bool reschedule = myTask->run();
    if (reschedule) {
      internal::getJobQueue(jobClass).jobs.push(myTask);
    }
    else {
      delete myTask;
    }
    maxNumberOfJobs--;
    result   = true;
    logDebug(
      "processJob(int)", "job of class " << jobClass << " complete, there are still " <<
	  internal::getJobQueue(jobClass).jobs.unsafe_size() <<
	  " jobs of this class pending"
	);
    if ( maxNumberOfJobs>0 ) {
      gotOne = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
    }
    else {
      gotOne = false;
    }
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
bool tarch::multicore::jobs::processBackgroundJobs() {
  const int additionalBackgroundThreads =
    std::min(
      Job::_maxNumberOfRunningBackgroundThreads - internal::_numberOfRunningBackgroundJobConsumerTasks,
	  static_cast<int>( internal::getJobQueue( internal::BackgroundJobsJobClassNumber ).jobs.unsafe_size() )
	);

  #ifdef Asserts
  if (additionalBackgroundThreads>0) {
    static tarch::logging::Log _log( "tarch::multicore::jobs" );
    logInfo(
      "processBackgroundJobs()",
      "spawn another " << additionalBackgroundThreads << " background job consumer tasks ("
  	  << internal::_numberOfRunningBackgroundJobConsumerTasks << " task(s) already running)"
    );
  }
  #endif

  for (int i=0; i<additionalBackgroundThreads; i++) {
    internal::BackgroundJobConsumerTask::enqueue();
  }

  // New: Not used for SC
  processJobs( internal::BackgroundJobsJobClassNumber, TBBMinimalNumberOfJobsPerBackgroundConsumerRun );

  return !internal::getJobQueue( internal::BackgroundJobsJobClassNumber ).jobs.empty();
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
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(3xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 );
      deadlockCounter = 0;
    }
    #endif
    g.wait();
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
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(3xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 );
      deadlockCounter = 0;
    }
    #endif
    g.wait(); // Bin mer hier net sicher
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
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, jobType3, jobClass3, semaphore ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { processJobs(jobClass3); });
    //g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(4xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 << ", " << jobClass3 );
      deadlockCounter = 0;
    }
    #endif
    g.wait();
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
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, jobType3, jobClass3, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job4, jobType4, jobClass4, semaphore ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { processJobs(jobClass3); });
    g.run( [&]() { processJobs(jobClass4); });
    //g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(5xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 << ", " << jobClass3 << ", " << jobClass4 );
      deadlockCounter = 0;
    }
    #endif
    g.wait();
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
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, jobType0, jobClass0, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, jobType1, jobClass1, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, jobType2, jobClass2, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, jobType3, jobClass3, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job4, jobType4, jobClass4, semaphore ); });
  g.run( [&]() { internal::spawnBlockingJob( job5, jobType5, jobClass5, semaphore ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore.load()>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { processJobs(jobClass3); });
    g.run( [&]() { processJobs(jobClass4); });
    g.run( [&]() { processJobs(jobClass5); });
    //g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>65536) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(6xJob,...)", internal::report() );
      logInfo( "spawnAndWait(...)", "job-classes: " << jobClass0 << ", " << jobClass1 << ", " << jobClass2 << ", " << jobClass3 << ", " << jobClass4 << ", " << jobClass5 );
      deadlockCounter = 0;
    }
    #endif
    g.wait();
  }
}


std::string tarch::multicore::jobs::internal::report() {
  std::ostringstream msg;
  msg << "job-system-status: background-queue-no=" << internal::BackgroundJobsJobClassNumber
	  << ", no-of-running-bg-consumer-tasks=" << _numberOfRunningBackgroundJobConsumerTasks;
  for (auto& p: _pendingJobs) {
	msg << ",queue[" << p.first
        << "]=" << p.second.jobs.unsafe_size() << " job(s)";
  }
  typedef tbb::concurrent_hash_map< int, JobQueue >  JobMap;
  extern JobMap     _pendingJobs;

  return msg.str();
}

#endif
