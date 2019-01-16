#include "tarch/Assertions.h"


#include <thread>
#include <queue>
#include "Tasks.h"
#include "MulticoreDefinitions.h"


tarch::multicore::TaskWithCopyOfFunctor::TaskWithCopyOfFunctor( const std::function<bool()>& taskFunctor ):
  _taskFunctor(taskFunctor)  {
}


bool tarch::multicore::TaskWithCopyOfFunctor::run() {
  return _taskFunctor();
}


void tarch::multicore::TaskWithCopyOfFunctor::prefetch() {
}


tarch::multicore::TaskWithoutCopyOfFunctor::TaskWithoutCopyOfFunctor( std::function<bool()>& taskFunctor ):
  _taskFunctor(taskFunctor)  {
}


bool tarch::multicore::TaskWithoutCopyOfFunctor::run() {
  return _taskFunctor();
}


void tarch::multicore::TaskWithoutCopyOfFunctor::prefetch() {
}



#ifndef SharedMemoryParallelisation

namespace {
  std::queue<tarch::multicore::Task* > backgroundJobs;
}


bool tarch::multicore::processPendingTasks() {
  // Note: Only invoked if no shared memory parallelisation activated. If
  // TBB/C++/OpenMP are enabled, the routine of the respective subfolder is
  // invoked
  if (backgroundJobs.empty()) {
    return false;
  }
  else {
    while ( !backgroundJobs.empty() ) {
      Task* p = backgroundJobs.front();
      backgroundJobs.pop();
      while (p->run()) {};
      delete p;
    }
    return true;
  }
}


void tarch::multicore::spawnTask(Task*  job) {
  backgroundJobs.push(job);
}


void tarch::multicore::spawnHighBandwidthTask(Task*  job) {
  backgroundJobs.push(job);
}


void tarch::multicore::spawnHighPriorityTask(Task*  job) {
  while( job->run() ) {};
  delete job;
}


void tarch::multicore::spawnAndWait(
  const std::vector< Task* >&  tasks
) {
  for (auto& p: tasks) {
    while (p->run()) {}
    delete p;
  }
}



/*
void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  job();
}
*/



/*

void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  JobType                 jobType0,
  JobType                 jobType1,
  int                     jobClass0,
  int                     jobClass1
) {
  while (job0()) {};
  while (job1()) {};
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
  while (job0()) {};
  while (job1()) {};
  while (job2()) {};
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
  while (job0()) {};
  while (job1()) {};
  while (job2()) {};
  while (job3()) {};
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
  while (job0()) {};
  while (job1()) {};
  while (job2()) {};
  while (job3()) {};
  while (job4()) {};
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
  while (job0()) {};
  while (job1()) {};
  while (job2()) {};
  while (job3()) {};
  while (job4()) {};
  while (job5()) {};
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  std::function<bool()>& job4,
  std::function<bool()>& job5,
  std::function<bool()>& job6,
  std::function<bool()>& job7,
  std::function<bool()>& job8,
  std::function<bool()>& job9,
  std::function<bool()>& job10,
  std::function<bool()>& job11,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
  JobType                    jobType3,
  JobType                    jobType4,
  JobType                    jobType5,
  JobType                    jobType6,
  JobType                    jobType7,
  JobType                    jobType8,
  JobType                    jobType9,
  JobType                    jobType10,
  JobType                    jobType11,
	 int                     jobClass0,
	 int                     jobClass1,
	 int                     jobClass2,
	 int                     jobClass3,
	 int                     jobClass4,
	 int                     jobClass5,
	 int                     jobClass6,
	 int                     jobClass7,
	 int                     jobClass8,
	 int                     jobClass9,
	 int                     jobClass10,
	 int                     jobClass11
) {
  while (job0()) {};
  while (job1()) {};
  while (job2()) {};
  while (job3()) {};
  while (job4()) {};
  while (job5()) {};
  while (job6()) {};
  while (job7()) {};
  while (job8()) {};
  while (job9()) {};
  while (job10()) {};
  while (job11()) {};
}
*/

#endif
