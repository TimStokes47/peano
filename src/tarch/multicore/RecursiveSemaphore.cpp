#include "tarch/multicore/RecursiveSemaphore.h"


#ifndef SharedMemoryParallelisation


tarch::multicore::RecursiveSemaphore::RecursiveSemaphore() {
}


tarch::multicore::RecursiveSemaphore::~RecursiveSemaphore() {
}

bool tarch::multicore::RecursiveSemaphore::tryEnterCriticalSection() {
  return true;
}

void tarch::multicore::RecursiveSemaphore::enterCriticalSection() {
}


void tarch::multicore::RecursiveSemaphore::leaveCriticalSection() {
}

#endif
