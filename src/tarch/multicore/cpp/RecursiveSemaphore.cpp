#include "RecursiveSemaphore.h"


#ifdef SharedCPP

tarch::multicore::RecursiveSemaphore::RecursiveSemaphore() {
}


tarch::multicore::RecursiveSemaphore::~RecursiveSemaphore() {
}


void tarch::multicore::RecursiveSemaphore::enterCriticalSection() {
  _mutex.lock();
}


void tarch::multicore::RecursiveSemaphore::leaveCriticalSection() {
  _mutex.unlock();
}

#endif
