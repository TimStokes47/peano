#include "tarch/Assertions.h"

#include "tarch/multicore/multicore.h"

#ifdef SharedOMP

#include <thread>
#include <queue>
#include "../Tasks.h"

namespace {
  std::queue<tarch::multicore::Task* > nonblockingTasks;
}

bool tarch::multicore::processPendingTasks(int maxTasks) {
  if (nonblockingTasks.empty()) {
    return false;
  }
  else {
    while ( maxTasks>0 and !nonblockingTasks.empty() ) {
      Task* p = nonblockingTasks.front();
      nonblockingTasks.pop();
      while (p->run()) {};
      delete p;
      maxTasks--;
    }
    return true;
  }
}


void tarch::multicore::spawnTask(Task*  job) {
  nonblockingTasks.push(job);
}


void tarch::multicore::spawnAndWait(
  const std::vector< Task* >&  tasks
) {
#pragma omp parallel for shared(tasks) schedule(dynamic, 1)
  for (auto& p: tasks) {
    while (p->run()) {}
    delete p;
  }
}


/**
 * This has to be the standard C++ implementation. Even if multithreading is
 * disabled, the code might logically run with multiple threads, and then we
 * have to switch the thread context from time to time.
 */
void tarch::multicore::yield() {
  std::this_thread::yield();
}


int tarch::multicore::getNumberOfPendingTasks() {
  return nonblockingTasks.size();
}

#endif
