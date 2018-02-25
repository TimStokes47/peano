#include "tarch/multicore/Jobs.h"
#include "tarch/Assertions.h"


#include "tarch/multicore/MulticoreDefinitions.h"

#include <thread>



#if defined(SharedCPP)


#include <atomic>

#include "tarch/multicore/Jobs.h"


namespace {
  /**
   * The spawn and wait routines fire their job and then have to wait for all
   * jobs to be processed. They do this through an integer atomic that they
   * count down to zero, i.e. the atomic stores how many jobs are still
   * pending.
   */
  class JobWithoutCopyOfFunctorAndSemaphore: public tarch::multicore::jobs::Job {
    private:
      std::function<bool()>&   _functor;
      std::atomic<int>&        _semaphore;
    public:
      JobWithoutCopyOfFunctorAndSemaphore(std::function<bool()>& functor, tarch::multicore::jobs::JobType jobType, int jobClass, std::atomic<int>& semaphore ):
        Job(jobType,jobClass),
        _functor(functor),
        _semaphore(semaphore) {
      }

      bool run() override {
        bool result = _functor();
        if (!result) _semaphore.fetch_add(-1, std::memory_order_relaxed);
        return result;
      }

      virtual ~JobWithoutCopyOfFunctorAndSemaphore() {}
  };
}

#include "JobQueue.h"


void tarch::multicore::jobs::spawnBackgroundJob(Job* job) {
  switch (job->getJobType()) {
     case JobType::ProcessImmediately:
       while (job->run()) {};
       delete job;
       break;
     case JobType::RunTaskAsSoonAsPossible:
       internal::JobQueue::getBackgroundQueue().addJobWithHighPriority(job);
       break;
     case JobType::Task:
     case JobType::Job:
       internal::JobQueue::getBackgroundQueue().addJob(job);
       break;
   }
}


bool tarch::multicore::jobs::processBackgroundJobs() {
  return internal::JobQueue::getBackgroundQueue().processJobs( std::max(1,getNumberOfWaitingBackgroundJobs()/2) );
}


int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return internal::JobQueue::getBackgroundQueue().getNumberOfPendingJobs();
}


void tarch::multicore::jobs::spawn(Job*  job) {
  if ( job->isTask() ) {
	internal::JobQueue::getStandardQueue(job->getClass()).addJobWithHighPriority(job);
  }
  else {
	internal::JobQueue::getStandardQueue(job->getClass()).addJob(job);
  }
}


void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,jobType,jobClass) );
}


int tarch::multicore::jobs::getNumberOfPendingJobs() {
  int result = 0;
  for (int i=0; i<internal::JobQueue::MaxNormalJobQueues; i++) {
    result += internal::JobQueue::getStandardQueue(i).getNumberOfPendingJobs();
  }
  return result;
}


bool tarch::multicore::jobs::processJobs(int jobClass, int maxNumberOfJobs) {
  return internal::JobQueue::getStandardQueue(jobClass).processJobs(maxNumberOfJobs);
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  JobType                 jobType0,
  JobType                 jobType1,
  int                     jobClass0,
  int                     jobClass1
) {
  std::atomic<int>  semaphore(2);

  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job0, jobType0, jobClass0, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job1, jobType1, jobClass1, semaphore ) );

  while (semaphore>0) {
    processJobs(jobClass0,1);
    processJobs(jobClass1,1);
  }
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
	 int                     jobClass0,
	 int                     jobClass1,
	 int                     jobClass2
) {
  job0();
  job1();
  job2();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
  JobType                    jobType3,
	 int                     jobClass0,
	 int                     jobClass1,
	 int                     jobClass2,
	 int                     jobClass3
) {
  job0();
  job1();
  job2();
  job3();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  std::function<bool()>& job4,
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
  job0();
  job1();
  job2();
  job3();
  job4();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  std::function<bool()>& job4,
  std::function<bool()>& job5,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
  JobType                    jobType3,
  JobType                    jobType4,
  JobType                    jobType5,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4,
  int                     jobClass5
) {
  job0();
  job1();
  job2();
  job3();
  job4();
  job5();
}


#endif

