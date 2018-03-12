#include "tarch/multicore/RecursiveSemaphore.h"
#include "tarch/multicore/Core.h"

#include "Jobs.h"


tarch::multicore::RecursiveSemaphore::RecursiveSemaphore():
  _threadIdSemaphore(),
  _dataSemaphore(),
  _threadId(0) {
}


tarch::multicore::RecursiveSemaphore::~RecursiveSemaphore() {
}


bool tarch::multicore::RecursiveSemaphore::enterCriticalSection() {
  _threadIdSemaphore.enterCriticalSection();
  const bool IHaveAlreadyLocked = _threadId==getThreadNumber();
  _threadIdSemaphore.leaveCriticalSection();

  bool result = false;
  if (IHaveAlreadyLocked) {
    result = false;
  }
  else {
    //  if (_threadId==std::thread::id(0)) {
    result = true;
    _dataSemaphore.enterCriticalSection();

    _threadIdSemaphore.enterCriticalSection();
     _threadId = getThreadNumber();
    _threadIdSemaphore.leaveCriticalSection();
  }

  return result;
}


void tarch::multicore::RecursiveSemaphore::leaveCriticalSection() {
  _threadIdSemaphore.enterCriticalSection();
  _threadId = std::thread::id(0);
  _threadIdSemaphore.leaveCriticalSection();

  _dataSemaphore.leaveCriticalSection();
}

