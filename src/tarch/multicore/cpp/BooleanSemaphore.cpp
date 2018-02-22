#include "tarch/multicore/BooleanSemaphore.h"


// This implementation is valid iff neither OpenMP nor TBBs nor any other
// shared memory parallelisation are active

#if defined(SharedCPP)

tarch::multicore::BooleanSemaphore::BooleanSemaphore():
  _mutex() {
}


tarch::multicore::BooleanSemaphore::~BooleanSemaphore() {
}


void tarch::multicore::BooleanSemaphore::enterCriticalSection() {
  _mutex.lock();
}


void tarch::multicore::BooleanSemaphore::leaveCriticalSection() {
  _mutex.unlock();
}


#endif
