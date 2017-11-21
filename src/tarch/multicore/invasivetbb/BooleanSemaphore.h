// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_BOOLEAN_SEMAPHORE_TBB_H_) && defined(SharedTBBInvade)
#define _TARCH_MULTICORE_BOOLEAN_SEMAPHORE_TBB_H_


namespace tarch {
  namespace multicore {
    class BooleanSemaphore;
    class Lock;
  }
}



#include <string>
#include <tbb/spin_mutex.h>


class tarch::multicore::BooleanSemaphore {
  private:
    friend class tarch::multicore::Lock;

    tbb::spin_mutex          _mutex;

    /**
     * Waits until I can enter the critical section.
     */
    void enterCriticalSection();

    /**
     * Tells the semaphore that it is about to leave.
     */
    void leaveCriticalSection();

    /**
     * You may not copy a semaphore
     */
    BooleanSemaphore( const BooleanSemaphore& semaphore ) {}

    /**
     * You may not copy a semaphore
     */
    BooleanSemaphore& operator=( const BooleanSemaphore& semaphore ) {return *this;}
  public:
    BooleanSemaphore();
    ~BooleanSemaphore();

    /**
     * Send task to background
     *
     * We found that yielding alone does not work for the invasive TBB version
     * though I'm not 100% clear why. It seems to deadlock, but the deadlocks
     * show up if and only if we have tree splitting and we have a significantly
     * large grid. So we may as well assume that the system is not deadlocking
     * but the background tasks are starving. So what I do in this version is
     * that I work through the background tasks before I yield.
     *
     * This is not a particularly nice implementation.
     *
     * @todo All the background stuff should go into the multicore/tarch part
     *       of the code. It does not belong into datatraversal.
     */
    static void sendTaskToBack();

};

#endif
