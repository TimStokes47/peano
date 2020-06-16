#include "../multicore.h"


#if defined(SharedTBB)
#include "tarch/multicore/IntegerSemaphore.h"
#include "tarch/Assertions.h"


#include "../Tasks.h"



tarch::multicore::IntegerSemaphore::IntegerSemaphore(int numberOfThreadsToWaitFor):
  _waitCounter(0),
  _numberOfThreadsToWaitFor(numberOfThreadsToWaitFor) {
}


tarch::multicore::IntegerSemaphore::~IntegerSemaphore() {
}


void tarch::multicore::IntegerSemaphore::enterCriticalSection() {
  assertion(_numberOfThreadsToWaitFor>0);

  int waitingThreads = _waitCounter.fetch_and_increment() + 1;

  while (waitingThreads<_numberOfThreadsToWaitFor) {
    tarch::multicore::yield();
    waitingThreads = _waitCounter;
  }
}


void tarch::multicore::IntegerSemaphore::leaveCriticalSection() {
  int waitingThreads = _waitCounter.fetch_and_decrement() - 1;

  while (waitingThreads>0) {
    tarch::multicore::yield();
    waitingThreads = _waitCounter;
  }
}


void tarch::multicore::IntegerSemaphore::setNumberOfThreadsToWaitFor( int value ) {
  _numberOfThreadsToWaitFor = value;
}


#endif
