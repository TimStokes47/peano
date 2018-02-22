#include "tarch/multicore/Jobs.h"
#include "tarch/Assertions.h"


#include "tarch/multicore/MulticoreDefinitions.h"

#include <thread>



#if defined(SharedCPP)

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

