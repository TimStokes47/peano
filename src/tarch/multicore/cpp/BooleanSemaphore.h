// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_BOOLEAN_SEMAPHORE_H_ ) && defined(SharedCPP)
#define _TARCH_MULTICORE_BOOLEAN_SEMAPHORE_H_

#include <string>


namespace tarch {
  namespace multicore {
    class BooleanSemaphore;
    class Lock;
  }
}

class tarch::multicore::BooleanSemaphore {
  private:
	std::mutex   _mutex;

    friend class tarch::multicore::Lock;

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
};
#endif


#endif
