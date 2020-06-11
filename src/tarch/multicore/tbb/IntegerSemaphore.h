// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_INTEGER_SEMAPHORE_TBB_H_) && defined(SharedTBB)
#define _TARCH_MULTICORE_INTEGER_SEMAPHORE_TBB_H_


namespace tarch {
  namespace multicore {
    class IntegerSemaphore;
    class IntegerLock;
  }
}



#include <string>

#include <tbb/atomic.h>


class tarch::multicore::IntegerSemaphore {
  private:
    friend class tarch::multicore::IntegerLock;

    tbb::atomic<int>    _waitCounter;
    tbb::atomic<int>    _numberOfThreadsToWaitFor;

    void enterCriticalSection();
    void leaveCriticalSection();

    /**
     * You may not copy a semaphore
     */
    IntegerSemaphore( const IntegerSemaphore& semaphore ) {}

    /**
     * You may not copy a semaphore
     */
    IntegerSemaphore& operator=( const IntegerSemaphore& semaphore ) {return *this;}
  public:
    IntegerSemaphore(int numberOfThreadsToWaitFor = -1);
    ~IntegerSemaphore();

    void setNumberOfThreadsToWaitFor( int value );
};
#endif
