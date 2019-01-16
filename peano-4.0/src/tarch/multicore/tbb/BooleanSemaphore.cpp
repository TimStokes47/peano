#include "tarch/multicore/BooleanSemaphore.h"

#include "Tasks.h"


// This implementation is valid iff neither OpenMP nor TBBs nor any other
// shared memory parallelisation are active

#if defined(SharedTBB)

tarch::multicore::BooleanSemaphore::BooleanSemaphore() {
}


tarch::multicore::BooleanSemaphore::~BooleanSemaphore() {
}


void tarch::multicore::BooleanSemaphore::enterCriticalSection() {
}


void tarch::multicore::BooleanSemaphore::leaveCriticalSection() {
}


#endif
