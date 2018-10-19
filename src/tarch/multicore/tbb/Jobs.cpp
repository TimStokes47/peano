#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/tbb/Jobs.h"

#include "peano/performanceanalysis/Analysis.h"

#include <vector>
#include <limits>
#include <cmath>

//#include <ittnotify.h>
#include "tarch/timing/Watch.h"

#ifdef USE_ITAC
#include "VT.h"
#endif


//__itt_domain* domain = __itt_domain_create("multicore.tbb.jobs");
//__itt_string_handle* handleTask = __itt_string_handle_create("process_task");

tarch::logging::Log tarch::multicore::jobs::internal::_log( "tarch::multicore::jobs::internal" );
tarch::logging::Log tarch::multicore::jobs::_log( "tarch::multicore::jobs" );


tbb::atomic<int>                              tarch::multicore::jobs::internal::_numberOfRunningJobConsumerTasks(0);
tarch::multicore::jobs::internal::JobQueue    tarch::multicore::jobs::internal::_pendingJobs[NumberOfJobQueues];
tbb::atomic<bool>                             tarch::multicore::jobs::internal::_bandwidthTasksAreProcessed(false);

int                      tarch::multicore::jobs::internal::_minimalNumberOfJobsPerConsumerRun(1);
int                      tarch::multicore::jobs::internal::_maximumNumberOfJobsPerConsumerRun(std::numeric_limits<int>::max());

tarch::multicore::jobs::HighPriorityTaskProcessing  tarch::multicore::jobs::internal::_processHighPriorityJobsAlwaysFirst(HighPriorityTaskProcessing::ProcessAllHighPriorityTasksInARush);


//
// This is a bug in Intel's TBB as documented on
//
// https://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/700057
//
// These task groups have to be static inside a cpp file.
//
static tbb::task_group_context  backgroundTaskContext(tbb::task_group_context::isolated);


void tarch::multicore::jobs::setMinMaxNumberOfJobsToConsumeInOneRush(int min, int max) {
  assertion2(min>=1,min,max);
  assertion2(max>=min,min,max);

  internal::_minimalNumberOfJobsPerConsumerRun = min;
  internal::_maximumNumberOfJobsPerConsumerRun = max;
}


void tarch::multicore::jobs::setHighPriorityJobBehaviour(HighPriorityTaskProcessing behaviour) {
  internal::_processHighPriorityJobsAlwaysFirst = behaviour;
}


tarch::multicore::jobs::internal::JobConsumerTask::JobConsumerTask(int maxJobs):
  _maxJobs(maxJobs) {
}


tarch::multicore::jobs::internal::JobConsumerTask::JobConsumerTask(const JobConsumerTask& copy):
  _maxJobs(copy._maxJobs) {
}


int tarch::multicore::jobs::internal::getNumberOfJobsPerConsumerRun( int jobClass ) {
  int result =
    static_cast<int>(std::round(tarch::multicore::jobs::internal::getJobQueue(jobClass).maxSize.load())+1) / tarch::multicore::Core::getInstance().getNumberOfThreads();

  result = std::max( std::max(internal::_minimalNumberOfJobsPerConsumerRun,1), result );
  result = std::min( internal::_maximumNumberOfJobsPerConsumerRun, result );

  return result;
}

#ifdef TBB_USE_THREADING_TOOLS
tbb::atomic<int>                    tarch::multicore::jobs::internal::JobConsumerTask::_numberOfConsumerRuns(0);
tbb::concurrent_hash_map<int,int>   tarch::multicore::jobs::internal::JobConsumerTask::_histogramOfHighPriorityTasks;
tbb::concurrent_hash_map<int,int>   tarch::multicore::jobs::internal::JobConsumerTask::_histogramOfBackgroundTasksProcessed;
tbb::concurrent_hash_map<int,int>   tarch::multicore::jobs::internal::JobConsumerTask::_histogramOfRunningConsumers;
tbb::concurrent_hash_map<int,int>   tarch::multicore::jobs::internal::JobConsumerTask::_histogramOfBackgroundTasks;
tbb::atomic<int>                    tarch::multicore::jobs::internal::JobConsumerTask::_numberOfHighBandwidthTasks(0);
tbb::atomic<int>                    tarch::multicore::jobs::internal::JobConsumerTask::_numberOfHighPriorityTasks(0);
tbb::atomic<int>                    tarch::multicore::jobs::internal::JobConsumerTask::_numberOfBackgroundTasks(0);
#endif


void tarch::multicore::jobs::internal::JobConsumerTask::enqueue() {
  _numberOfRunningJobConsumerTasks.fetch_and_add(1);
  JobConsumerTask* tbbTask = new (tbb::task::allocate_root(::backgroundTaskContext)) JobConsumerTask(
    internal::getNumberOfJobsPerConsumerRun(BackgroundTasksJobClassNumber)
  );

  tbb::task::enqueue(*tbbTask);
  ::backgroundTaskContext.set_priority(tbb::priority_low);
}


tbb::task* tarch::multicore::jobs::internal::JobConsumerTask::execute() {

#ifdef USE_ITAC
    static int event_execute = -1;
    std::string event_execute_name = "execute_consumer";
    if(event_execute == -1)
        int ierr = VT_funcdef(event_execute_name.c_str(), VT_NOCLASS, &event_execute);
    VT_begin(event_execute);
#endif


  if ( _bandwidthTasksAreProcessed.compare_and_swap(true,false) ) {
    processJobs(internal::HighBandwidthTasksJobClassNumber,std::numeric_limits<int>::max());
	_bandwidthTasksAreProcessed = false;
  }

  bool hasProcessedJobs = false;

  #ifdef TBB_USE_THREADING_TOOLS
  _numberOfConsumerRuns++;

  tbb::concurrent_hash_map<int,int>::accessor p;

  _histogramOfHighPriorityTasks.insert(p,getJobQueueSize(internal::HighPriorityTasksJobClassNumber));
  p->second += 1;
  p.release();

  _histogramOfBackgroundTasks.insert(p,getJobQueueSize(internal::BackgroundTasksJobClassNumber));
  p->second += 1;
  p.release();

  _histogramOfBackgroundTasksProcessed.insert(p,_maxJobs);
  p->second += 1;
  p.release();

  _histogramOfRunningConsumers.insert(p,_numberOfRunningJobConsumerTasks.load());
  p->second += 1;
  p.release();
  #endif

  switch (_processHighPriorityJobsAlwaysFirst) {
    case HighPriorityTaskProcessing::ProcessAllHighPriorityTasksInARush:
      hasProcessedJobs |= processJobs(internal::HighPriorityTasksJobClassNumber,std::numeric_limits<int>::max());
      hasProcessedJobs |= processJobs(internal::BackgroundTasksJobClassNumber,_maxJobs);
      break;
    case HighPriorityTaskProcessing::ProcessAllHighPriorityTasksInARushAndRunBackgroundTasksOnlyIfNoHighPriorityTasksAreLeft:
      hasProcessedJobs |= processJobs(internal::HighPriorityTasksJobClassNumber,std::numeric_limits<int>::max());
      if (!hasProcessedJobs) {
        hasProcessedJobs |= processJobs(internal::BackgroundTasksJobClassNumber,_maxJobs);
      }
      break;
    case HighPriorityTaskProcessing::ProcessOneHighPriorityTasksAtATime:
      hasProcessedJobs |= processJobs(internal::HighPriorityTasksJobClassNumber,1);
      hasProcessedJobs |= processJobs(internal::BackgroundTasksJobClassNumber,_maxJobs);
      break;
    case HighPriorityTaskProcessing::ProcessOneHighPriorityTasksAtATimeAndRunBackgroundTasksOnlyIfNoHighPriorityTasksAreLeft:
      hasProcessedJobs |= processJobs(internal::HighPriorityTasksJobClassNumber,1);
      if (!hasProcessedJobs) {
        hasProcessedJobs |= processJobs(internal::BackgroundTasksJobClassNumber,_maxJobs);
      }
      break;
    case HighPriorityTaskProcessing::MapHighPriorityTasksToRealTBBTasks:
      assertion( internal::getJobQueueSize(HighPriorityTasksJobClassNumber)==0 );
      hasProcessedJobs |= processJobs(internal::BackgroundTasksJobClassNumber,_maxJobs);
      break;
    case HighPriorityTaskProcessing::MapHighPriorityAndBackgroundTasksToRealTBBTasks:
      assertion( internal::getJobQueueSize(HighPriorityTasksJobClassNumber)==0 );
      assertion( internal::getJobQueueSize(BackgroundTasksJobClassNumber)==0 );
      break;
  }

  const int oldNumberOfConsumerTasks = _numberOfRunningJobConsumerTasks.fetch_and_add(-1);

  tbb::task* result = nullptr;

/*
  if (hasProcessedJobs) {
	if (
      oldNumberOfConsumerTasks<Job::_maxNumberOfRunningBackgroundThreads
	  and
	  internal::getJobQueueSize(internal::BackgroundTasksJobClassNumber) >= internal::_minimalNumberOfJobsPerConsumerRun
    ) {
      enqueue();
      enqueue();
	}
	else if (
      oldNumberOfConsumerTasks>1
	  and
	  internal::getJobQueueSize(internal::BackgroundTasksJobClassNumber) < internal::_minimalNumberOfJobsPerConsumerRun
    ) {
      // enqueue();
      // enqueue();
      // starve
	}
	else {
      enqueue();
	}
  }
*/

  if (
    oldNumberOfConsumerTasks<Job::_maxNumberOfRunningBackgroundThreads
    and
    internal::getJobQueueSize(internal::BackgroundTasksJobClassNumber) >= internal::_minimalNumberOfJobsPerConsumerRun
  ) {
    result = spawnFollowUpConsumerAsDirectChild(2);
  }
  else if (
    oldNumberOfConsumerTasks>1
    and
	internal::getJobQueueSize(internal::BackgroundTasksJobClassNumber) < internal::_minimalNumberOfJobsPerConsumerRun
  ) {
    // starve
  }
  // It is very important not to directly spawn a consumer here, as otherwise
  // consumers tend to run into endless loops or somehow are not scheduled.
  // Might be due to the background thing.
  else if (
    oldNumberOfConsumerTasks==1
  ) {
    enqueue();
  }

  if (oldNumberOfConsumerTasks==1) {
    internal::getJobQueue(internal::BackgroundTasksJobClassNumber).maxSize    = internal::getJobQueue(internal::BackgroundTasksJobClassNumber).maxSize*0.9;
    internal::getJobQueue(internal::HighPriorityTasksJobClassNumber).maxSize  = internal::getJobQueue(internal::HighPriorityTasksJobClassNumber).maxSize*0.9;
    internal::getJobQueue(internal::HighBandwidthTasksJobClassNumber).maxSize = internal::getJobQueue(internal::HighBandwidthTasksJobClassNumber).maxSize*0.9;
  }

#ifdef USE_ITAC
  VT_end(event_execute);
#endif
  return result;
}


tarch::multicore::jobs::internal::JobConsumerTask*  tarch::multicore::jobs::internal::JobConsumerTask::spawnFollowUpConsumerAsDirectChild(int count) {
  assertion(count>=1);

  set_ref_count(count);
  recycle_as_continuation();

  JobConsumerTask* tbbTask = nullptr;
  for (int i=0; i<count; i++) {
    JobConsumerTask* tbbTask = new (tbb::task::allocate_child()) JobConsumerTask(
     internal::getNumberOfJobsPerConsumerRun(BackgroundTasksJobClassNumber)
    );
    _numberOfRunningJobConsumerTasks.fetch_and_add(1);
  }

  return tbbTask;
}


tarch::multicore::jobs::internal::JobQueue& tarch::multicore::jobs::internal::getJobQueue( int jobClass ) {
  assertion( jobClass>=0 );

  return _pendingJobs[jobClass % NumberOfJobQueues];
}


void tarch::multicore::jobs::internal::insertJob( int jobClass, Job* job ) {
  internal::getJobQueue(jobClass).jobs.push( job );
}


int  tarch::multicore::jobs::internal::getJobQueueSize( int jobClass ) {
  return static_cast<int>(internal::getJobQueue( jobClass ).jobs.unsafe_size());
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


void tarch::multicore::jobs::plotStatistics() {
  #ifdef TBB_USE_THREADING_TOOLS
  static tarch::logging::Log _log("tarch::multicore::jobs");
  logInfo( "plotStatistics()", "total no of consumer runs=" << internal::JobConsumerTask::_numberOfConsumerRuns.load() );
  logInfo( "plotStatistics()", "total no of high bandwidth tasks=" << internal::JobConsumerTask::_numberOfHighBandwidthTasks.load() );
  logInfo( "plotStatistics()", "total no of high priority tasks=" << internal::JobConsumerTask::_numberOfHighPriorityTasks.load() );
  logInfo( "plotStatistics()", "total no of background tasks=" << internal::JobConsumerTask::_numberOfBackgroundTasks.load() );
  for (auto p: internal::JobConsumerTask::_histogramOfHighPriorityTasks) {
    logInfo( "plotStatistics()", "no of high priority tasks[" << p.first << "]=" << p.second );
  }
  for (auto p: internal::JobConsumerTask::_histogramOfBackgroundTasks) {
    logInfo( "plotStatistics()", "no of background tasks available per consumer run[" << p.first << "]=" << p.second );
  }
  for (auto p: internal::JobConsumerTask::_histogramOfBackgroundTasksProcessed) {
    logInfo( "plotStatistics()", "no of background tasks processed per consumer run[" << p.first << "]=" << p.second );
  }
  for (auto p: internal::JobConsumerTask::_histogramOfRunningConsumers) {
    logInfo( "plotStatistics()", "no of running consumers[" << p.first << "]=" << p.second );
  }
  #endif

  logInfo( "plotStatistics()", "max no of background tasks in queue=" << tarch::multicore::jobs::internal::getJobQueue(internal::BackgroundTasksJobClassNumber).maxSize.load() << " (sliding average)");
  logInfo( "plotStatistics()", "max no of high priority tasks in queue=" << tarch::multicore::jobs::internal::getJobQueue(internal::HighPriorityTasksJobClassNumber).maxSize.load() << " (sliding average)" );
  logInfo( "plotStatistics()", "max no of high bandwidth tasks in queue=" << tarch::multicore::jobs::internal::getJobQueue(internal::HighBandwidthTasksJobClassNumber).maxSize.load() << " (sliding average)" );
}



int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return internal::getJobQueueSize( internal::BackgroundTasksJobClassNumber ) + internal::_numberOfRunningJobConsumerTasks.load();
}


/**
 * <h2> Flag checkWhetherToLaunchAJobConsumer </h2>
 *
 * We do spawn a consumer if this flag is set and if there are no other
 * consumers around. Obviously, this behaviour never launches more than
 * one consumer (for background and high priority tasks). The trick is
 * that enqueue() itself checks for further spawns. This way, we have a
 * tree spawn pattern and ensure that the main thread usually spawns only
 * one consumer task.
 */
void tarch::multicore::jobs::spawn(Job*  job) {
  bool checkWhetherToLaunchAJobConsumer = false;

  switch (job->getJobType()) {
	case JobType::ProcessImmediately:
	  while (job->run()) {};
	  delete job;
	  break;
	case JobType::RunTaskAsSoonAsPossible:
	  if (
        internal::_processHighPriorityJobsAlwaysFirst==HighPriorityTaskProcessing::MapHighPriorityTasksToRealTBBTasks
		or
        internal::_processHighPriorityJobsAlwaysFirst==HighPriorityTaskProcessing::MapHighPriorityAndBackgroundTasksToRealTBBTasks
      ) {
	    internal::TBBWrapperAroundJob* tbbTask = new (tbb::task::allocate_root(::backgroundTaskContext)) internal::TBBWrapperAroundJob(
	      job
        );
	    tbb::task::enqueue(*tbbTask);
        checkWhetherToLaunchAJobConsumer = false;
	  }
	  else {
        internal::insertJob(internal::HighPriorityTasksJobClassNumber,job);
        checkWhetherToLaunchAJobConsumer = true;
        internal::getJobQueue(internal::HighPriorityTasksJobClassNumber).maxSize =
          std::max(
            static_cast<double>(internal::getJobQueueSize(internal::HighPriorityTasksJobClassNumber)),
            internal::getJobQueue(internal::HighPriorityTasksJobClassNumber).maxSize.load()
          );
  	  }
      #ifdef TBB_USE_THREADING_TOOLS
      tarch::multicore::jobs::internal::JobConsumerTask::_numberOfHighPriorityTasks++;
      #endif
      break;
    case JobType::BandwidthBoundTask:
      internal::insertJob(internal::HighBandwidthTasksJobClassNumber,job);
      checkWhetherToLaunchAJobConsumer = true;
      #ifdef TBB_USE_THREADING_TOOLS
      tarch::multicore::jobs::internal::JobConsumerTask::_numberOfHighBandwidthTasks++;
      #endif
      internal::getJobQueue(internal::HighBandwidthTasksJobClassNumber).maxSize =
        std::max(
          static_cast<double>(internal::getJobQueueSize(internal::HighBandwidthTasksJobClassNumber)),
		  internal::getJobQueue(internal::HighBandwidthTasksJobClassNumber).maxSize.load()
        );
      break;
    case JobType::BackgroundTask:
      if (
        internal::_processHighPriorityJobsAlwaysFirst==HighPriorityTaskProcessing::MapHighPriorityAndBackgroundTasksToRealTBBTasks
      ) {
        internal::TBBWrapperAroundJob* tbbTask = new (tbb::task::allocate_root(::backgroundTaskContext)) internal::TBBWrapperAroundJob(
  	      job
        );
  	    tbb::task::enqueue(*tbbTask);
        checkWhetherToLaunchAJobConsumer = false;
  	  }
  	  else {
        internal::insertJob(internal::BackgroundTasksJobClassNumber,job);
        checkWhetherToLaunchAJobConsumer = true;
        internal::getJobQueue(internal::BackgroundTasksJobClassNumber).maxSize =
          std::max(
            static_cast<double>(internal::getJobQueueSize(internal::BackgroundTasksJobClassNumber)),
		    internal::getJobQueue(internal::BackgroundTasksJobClassNumber).maxSize.load()
          );
        #ifdef TBB_USE_THREADING_TOOLS
        tarch::multicore::jobs::internal::JobConsumerTask::_numberOfBackgroundTasks++;
        #endif
  	  }
      break;
    case JobType::Job:
      internal::insertJob(job->getClass(),job);
      internal::getJobQueue(job->getClass()).maxSize =
        std::max(
          static_cast<double>(internal::getJobQueueSize(job->getClass())),
		  internal::getJobQueue(job->getClass()).maxSize.load()
        );
      break;
  }

  if (checkWhetherToLaunchAJobConsumer) {
    if (Job::_maxNumberOfRunningBackgroundThreads==UseDefaultNumberOFBackgroundJobs) {
      Job::_maxNumberOfRunningBackgroundThreads = std::max(tarch::multicore::Core::getInstance().getNumberOfThreads()-1,1);
    }

    const int currentlyRunningBackgroundThreads = internal::_numberOfRunningJobConsumerTasks.load();
    if (
      currentlyRunningBackgroundThreads==0
    ) {
      internal::JobConsumerTask::enqueue();
    }
  }
}


void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,jobType,jobClass) );
}


bool tarch::multicore::jobs::processBackgroundJobs(int maxNumberOfJobs) {
  return processJobs(internal::BackgroundTasksJobClassNumber, maxNumberOfJobs);
}


bool tarch::multicore::jobs::processHighPriorityJobs(int maxNumberOfJobs ) {
  return processJobs(internal::HighPriorityTasksJobClassNumber, maxNumberOfJobs);
}


bool tarch::multicore::jobs::processHighBandwidthJobs(int maxNumberOfJobs ) {
  return processJobs(internal::HighBandwidthTasksJobClassNumber, maxNumberOfJobs);
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
  if (internal::getJobQueue(jobClass).jobs.empty() ) {
    return false;
  }
  else {
    bool result = false;

    #if defined(TBBPrefetchesJobData)
    Job* myTask           = nullptr;
    Job* prefetchedTask   = nullptr;
    bool gotOne           = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
    bool prefetchedOne    = internal::getJobQueue(jobClass).jobs.try_pop(prefetchedTask);

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
        internal::getJobQueue(jobClass).jobs.push(myTask);
        maxNumberOfJobs--;
      }
      else {
        delete myTask;
        maxNumberOfJobs--;
      }

      if ( maxNumberOfJobs>1 and prefetchedOne ) {
        gotOne        = prefetchedOne;
        myTask        = prefetchedTask;
        prefetchedOne = internal::getJobQueue(jobClass).jobs.try_pop(prefetchedTask);
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
    bool gotOne   = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
    while (gotOne) {
      result = true;

      bool reschedule = myTask->run();
      if (reschedule) {
        internal::getJobQueue(jobClass).jobs.push(myTask);
        maxNumberOfJobs--;
      }
      else {
        delete myTask;
        maxNumberOfJobs--;
      }
      if ( maxNumberOfJobs>0 ) {
        gotOne = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
      }
      else {
        gotOne = false;
      }
    }
    #endif

    return result;
  }
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
	  internal::getJobQueueSize( internal::BackgroundTasksJobClassNumber ) / internal::getNumberOfJobsPerConsumerRun(internal::BackgroundTasksJobClassNumber)
	);

  const int maxAdditionalBackgroundThreads =
    std::max(
      tarch::multicore::Core::getInstance().getNumberOfThreads()-1 - internal::_numberOfRunningJobConsumerTasks.load(),
      0
    );
  
  const int additionalBackgroundThreads = std::min( maxBusyConsumerTasks, maxAdditionalBackgroundThreads );

  #ifdef Asserts
  if (additionalBackgroundThreads>0) {
    static tarch::logging::Log _log( "tarch::multicore::jobs" );
    logInfo(
      "startToProcessBackgroundJobs()",
      "spawn another " << additionalBackgroundThreads << " background job consumer tasks ("
        << internal::_numberOfRunningJobConsumerTasks.load() << " task(s) already running)"
    );
  }
  #endif

  for (int i=0; i<additionalBackgroundThreads; i++) {
    internal::JobConsumerTask::enqueue();
  }
}


bool tarch::multicore::jobs::finishToProcessBackgroundJobs() {
  processJobs( internal::BackgroundTasksJobClassNumber, internal::getNumberOfJobsPerConsumerRun(internal::BackgroundTasksJobClassNumber) );

  return internal::getJobQueueSize(internal::BackgroundTasksJobClassNumber)>0;
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
  msg << "no-of-running-bg-consumer-tasks=" << _numberOfRunningJobConsumerTasks.load();
  for (int i=0; i<NumberOfJobQueues; i++) {
	msg << ",queue[" << i
        << "]=" << internal::getJobQueueSize(i) << " job(s); max="
		<< internal::getJobQueue(i).maxSize;
  }
  return msg.str();
}

#endif
