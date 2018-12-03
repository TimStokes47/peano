#include "tarch/Assertions.h"


#include <thread>
#include <queue>
#include "../Tasks.h"
#include "tarch/multicore/MulticoreDefinitions.h"


#ifdef SharedTBB

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
  std::thread t[ tasks.size() ];

  for (int i = 0; i < tasks.size(); ++i) {
	t[i] = std::thread( [&,i]()->void {
      while (tasks[i]->run()) {}
      delete tasks[i];
	});
  }

  for (int i = 0; i < tasks.size(); ++i) {
	t[i].join();
  }
}


#endif
