#include "tarch/multicore/MulticoreDefinitions.h"


#if !defined(SharedMemoryParallelisation)
#include "tarch/multicore/BooleanSemaphore.h"
tarch::multicore::BooleanSemaphore::BooleanSemaphore() {
}


tarch::multicore::BooleanSemaphore::~BooleanSemaphore() {
}


void tarch::multicore::BooleanSemaphore::enterCriticalSection() {
}


void tarch::multicore::BooleanSemaphore::leaveCriticalSection() {
}


#endif
