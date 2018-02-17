#include "tarch/multicore/Jobs.h"
#include "tarch/Assertions.h"


#include "tarch/multicore/MulticoreDefinitions.h"

#include <thread>


int tarch::multicore::jobs::BackgroundJob::_maxNumberOfRunningBackgroundThreads( std::thread::hardware_concurrency() );


tarch::multicore::jobs::BackgroundJob::BackgroundJob( tarch::multicore::jobs::BackgroundJobType jobType ):
  _jobType(
    (
      ( _maxNumberOfRunningBackgroundThreads==DontUseAnyBackgroundJobs )
      ||
      ( _maxNumberOfRunningBackgroundThreads==ProcessNormalBackgroundJobsImmediately)
	)
	? BackgroundJobType::ProcessImmediately : jobType
  ) {
}


tarch::multicore::jobs::BackgroundJob::~BackgroundJob() {}


bool tarch::multicore::jobs::BackgroundJob::isLongRunning() const {
  return _jobType==BackgroundJobType::LongRunningBackgroundJob;
}


tarch::multicore::jobs::BackgroundJobType tarch::multicore::jobs::BackgroundJob::getJobType() const {
  return _jobType;
}


void tarch::multicore::jobs::BackgroundJob::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads>=ProcessNormalBackgroundJobsImmediately);
  _maxNumberOfRunningBackgroundThreads = maxNumberOfRunningBackgroundThreads;
}



tarch::multicore::jobs::Job::Job( bool isTask, int jobClass ):
  _isTask(isTask),
  _jobClass(jobClass) {
}


tarch::multicore::jobs::Job::~Job() {
}


bool tarch::multicore::jobs::Job::isTask() const {
  return _isTask;
}


int tarch::multicore::jobs::Job::getClass() const {
  return _jobClass;
}



tarch::multicore::jobs::GenericJobWithCopyOfFunctor::GenericJobWithCopyOfFunctor(const std::function<void()>& functor, bool isTask, int jobClass  ):
  Job(isTask,jobClass),
  _functor(functor)  {
}


void tarch::multicore::jobs::GenericJobWithCopyOfFunctor::run() {
  _functor();
}


tarch::multicore::jobs::GenericJobWithCopyOfFunctor::~GenericJobWithCopyOfFunctor() {
}


tarch::multicore::jobs::GenericJobWithoutCopyOfFunctor::GenericJobWithoutCopyOfFunctor(std::function<void()>& functor, bool isTask, int jobClass ):
  Job(isTask,jobClass),
  _functor(functor)  {
}


void tarch::multicore::jobs::GenericJobWithoutCopyOfFunctor::run() {
  _functor();
}


tarch::multicore::jobs::GenericJobWithoutCopyOfFunctor::~GenericJobWithoutCopyOfFunctor() {
}


tarch::multicore::jobs::GenericBackgroundJobWithCopyOfFunctor::GenericBackgroundJobWithCopyOfFunctor(const std::function<bool()>& functor, BackgroundJobType jobType ):
  BackgroundJob(jobType),
  _functor(functor)  {
}


bool tarch::multicore::jobs::GenericBackgroundJobWithCopyOfFunctor::run() {
  return _functor();
}


tarch::multicore::jobs::GenericBackgroundJobWithCopyOfFunctor::~GenericBackgroundJobWithCopyOfFunctor() {
}



#ifndef SharedMemoryParallelisation

void tarch::multicore::jobs::spawnBackgroundJob(BackgroundJob* task) {
  task->run();
  delete task;
}


bool tarch::multicore::jobs::processBackgroundJobs() {
  return false;
}


int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return 0;
}


void tarch::multicore::jobs::spawn(Job*  job) {
  job->run();
  delete job;
}


void tarch::multicore::jobs::spawn(std::function<void()>& job, bool isTask, int jobClass) {
  job();
}


int tarch::multicore::jobs::getNumberOfPendingJobs() {
  return 0;
}



bool tarch::multicore::jobs::processJobs(int jobClass) {
  return false;
}


bool tarch::multicore::jobs::processJobs() {
  return false;
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>& job0,
  std::function<void()>& job1,
	 bool                    isTask0,
	 bool                    isTask1,
	 int                     jobClass0,
	 int                     jobClass1
) {
  job0();
  job1();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>& job0,
  std::function<void()>& job1,
  std::function<void()>& job2,
	 bool                    isTask0,
	 bool                    isTask1,
	 bool                    isTask2,
	 int                     jobClass0,
	 int                     jobClass1,
	 int                     jobClass2
) {
  job0();
  job1();
  job2();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>& job0,
  std::function<void()>& job1,
  std::function<void()>& job2,
  std::function<void()>& job3,
	 bool                    isTask0,
	 bool                    isTask1,
	 bool                    isTask2,
	 bool                    isTask3,
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
  job0();
  job1();
  job2();
  job3();
  job4();
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>& job0,
  std::function<void()>& job1,
  std::function<void()>& job2,
  std::function<void()>& job3,
  std::function<void()>& job4,
  std::function<void()>& job5,
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
  job0();
  job1();
  job2();
  job3();
  job4();
  job5();
}


#endif

