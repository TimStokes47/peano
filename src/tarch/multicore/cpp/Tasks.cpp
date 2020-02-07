#include "tarch/Assertions.h"
#include "../Tasks.h"
#include "tarch/multicore/multicore.h"


#ifdef SharedCPP
#include <thread>
#include <queue>
#include <mutex>

namespace {
  std::queue<tarch::multicore::Task* > nonblockingTasks;
  std::mutex                           taskQueueMutex;
}



void tarch::multicore::yield() {
  std::this_thread::yield();
}


bool tarch::multicore::processPendingTasks(int maxTasks) {
  assertion(maxTasks>=0);
  bool result = false;
  while ( maxTasks>0 ) {
	// try to get a task and store in p
    Task* p = nullptr;
    taskQueueMutex.lock();
    if (not nonblockingTasks.empty()) {
      p = nonblockingTasks.front();
      nonblockingTasks.pop();
    }
    taskQueueMutex.unlock();

    // process task
    if (p!=nullptr) {
      bool requeue = p->run();
      if (requeue) {
        spawnTask( p );
      }
      else {
        delete p;
      }
      maxTasks--;
      result = true;
    }
    else {
      maxTasks=0;
    }
  }
  return result;
}


void tarch::multicore::spawnTask(Task*  job) {
  taskQueueMutex.lock();
  nonblockingTasks.push(job);
  taskQueueMutex.unlock();
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


int tarch::multicore::getNumberOfPendingTasks() {
  return nonblockingTasks.size();
}

#endif
