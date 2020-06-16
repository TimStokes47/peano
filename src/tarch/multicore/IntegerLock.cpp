#include "tarch/Assertions.h"
#include "tarch/multicore/IntegerLock.h"
#include "tarch/multicore/IntegerSemaphore.h"


tarch::multicore::IntegerLock::IntegerLock( tarch::multicore::IntegerSemaphore& semaphore, bool aquireLockImmediately ):
  _semaphore(semaphore),
  _lockIsAquired(false) {
  if (aquireLockImmediately) {
    lock();
  }
}


tarch::multicore::IntegerLock::~IntegerLock() {
  if (_lockIsAquired) {
    free();
  }
}


void tarch::multicore::IntegerLock::lock() {
  assertion( !_lockIsAquired );
  _semaphore.enterCriticalSection();
  _lockIsAquired = true;
}


void tarch::multicore::IntegerLock::free() {
  assertion( _lockIsAquired );
  _semaphore.leaveCriticalSection();
  _lockIsAquired = false;
}

