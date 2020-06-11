#include "tarch/multicore/multicore.h"


#if !defined(SharedMemoryParallelisation)
#include "tarch/multicore/IntegerSemaphore.h"
tarch::multicore::IntegerSemaphore::IntegerSemaphore(int value) {
}


tarch::multicore::IntegerSemaphore::~IntegerSemaphore() {
}


void tarch::multicore::IntegerSemaphore::enterCriticalSection() {
}


void tarch::multicore::IntegerSemaphore::leaveCriticalSection() {
}

#endif
