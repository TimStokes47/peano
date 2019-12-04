#include "tarch/Assertions.h"
#include "../Tasks.h"
#include "tarch/multicore/MulticoreDefinitions.h"


#ifdef SharedCPP
#include <thread>
#include <queue>

namespace {
  std::queue<tarch::multicore::Task* > backgroundJobs;
}



void tarch::multicore::yield() {
  std::this_thread::yield();
}


bool tarch::multicore::processPendingTasks() {
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
