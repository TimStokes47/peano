#include "tarch/Assertions.h"
#include "tarch/multicore/RecursiveLock.h"
#include "tarch/multicore/RecursiveSemaphore.h"

#ifdef SharedOMP
#include "RecursiveSemaphore.h"

tarch::multicore::RecursiveSemaphore::RecursiveSemaphore() {
}


tarch::multicore::RecursiveSemaphore::~RecursiveSemaphore() {
}


void tarch::multicore::RecursiveSemaphore::enterCriticalSection() {
}


void tarch::multicore::RecursiveSemaphore::leaveCriticalSection() {
}

#endif

