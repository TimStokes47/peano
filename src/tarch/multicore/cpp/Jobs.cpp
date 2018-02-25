#include "tarch/multicore/Jobs.h"
#include "tarch/Assertions.h"


#include "tarch/multicore/MulticoreDefinitions.h"

#include <thread>



#if defined(SharedCPP)


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


//
// @todo
//
void tarch::multicore::jobs::spawn(Job*  job) {
  while ( job->run() ) {};
  delete job;
}


void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  while ( job() ) {};
}


int tarch::multicore::jobs::getNumberOfPendingJobs() {
  return 0;
}



bool tarch::multicore::jobs::processJobs(int jobClass, int maxNumberOfJobs) {
  return false;
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  JobType                    isTask0,
	 JobType                    isTask1,
	 int                     jobClass0,
	 int                     jobClass1
) {
  job0();
  job1();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  JobType                    isTask0,
  JobType                    isTask1,
  JobType                    isTask2,
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
  JobType                    isTask0,
  JobType                    isTask1,
  JobType                    isTask2,
  JobType                    isTask3,
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
  JobType                    isTask0,
	 JobType                    isTask1,
	 JobType                    isTask2,
	 JobType                    isTask3,
	 JobType                    isTask4,
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
  JobType                    isTask0,
  JobType                    isTask1,
  JobType                    isTask2,
  JobType                    isTask3,
  JobType                    isTask4,
  JobType                    isTask5,
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

