#include "tarch/multicore/RecursiveSemaphore.h"


#ifdef SharedTBB

tarch::multicore::RecursiveSemaphore::RecursiveSemaphore():
  _recursiveMutex() {
}


tarch::multicore::RecursiveSemaphore::~RecursiveSemaphore() {
}

bool tarch::multicore::RecursiveSemaphore::tryEnterCriticalSection() {
  return _recursiveMutex.try_lock();
}

void tarch::multicore::RecursiveSemaphore::enterCriticalSection() {
  _recursiveMutex.lock();
}


void tarch::multicore::RecursiveSemaphore::leaveCriticalSection() {
  _recursiveMutex.unlock();
}

#endif
