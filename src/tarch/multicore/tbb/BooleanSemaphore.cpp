#if defined(SharedTBB) || defined(SharedTBBInvade)
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/logging/Log.h"


#include <limits>


#include <tbb/tbb_machine.h>
#include <tbb/task.h>
#include <tbb/tbb_thread.h>


/**
 * There is no constructor for an atomic. I hence rely on the TBB documentation
 * saying "you can rely on zero initialization o initialize an atomic".
 */
tbb::atomic<int>   tarch::multicore::BooleanSemaphore::_pauseCounter;
const int          tarch::multicore::BooleanSemaphore::_pauseBeforeYield(32);
const int          tarch::multicore::BooleanSemaphore::_counterThresholdForWarning(std::numeric_limits<int>::max() - 20);


tarch::multicore::BooleanSemaphore::BooleanSemaphore() {
}


tarch::multicore::BooleanSemaphore::~BooleanSemaphore() {
}


void tarch::multicore::BooleanSemaphore::enterCriticalSection() {
  _mutex.lock();
}


void tarch::multicore::BooleanSemaphore::leaveCriticalSection() {
  _mutex.unlock();
}


void tarch::multicore::BooleanSemaphore::sendTaskToBack() {
  tbb::this_tbb_thread::yield();
}


void tarch::multicore::BooleanSemaphore::continuedWithTask() {
  _pauseCounter = 1;
}

#endif
