#include "tarch/Assertions.h"
#include "tarch/multicore/RecursiveLock.h"
#include "tarch/multicore/RecursiveSemaphore.h"

#ifdef SharedOMP
#include "RecursiveSemaphore.h"

tarch::multicore::RecursiveSemaphore::RecursiveSemaphore() {
  omp_init_lock(&lock);
}


tarch::multicore::RecursiveSemaphore::~RecursiveSemaphore() {
}


void tarch::multicore::RecursiveSemaphore::enterCriticalSection() {
  omp_set_lock(&lock);
}


void tarch::multicore::RecursiveSemaphore::leaveCriticalSection() {
  omp_unset_lock(&lock);
}


bool tarch::multicore::RecursiveSemaphore::tryEnterCriticalSection() {
  return omp_test_lock(&lock);
}


#endif

