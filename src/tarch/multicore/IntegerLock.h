// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_INTEGER_LOCK_H_
#define _TARCH_MULTICORE_INTEGER_LOCK_H_


namespace tarch {
  namespace multicore {
    class IntegerSemaphore;
    class IntegerLock;
  }
}


/**
 * Create a lock around a boolean semaphore region
 *
 * @see tarch::multicore::IntegerSemaphore
 */
class tarch::multicore::IntegerLock {
  private:
    IntegerSemaphore&  _semaphore;
    bool               _lockIsAquired;
  public:
    IntegerLock( tarch::multicore::IntegerSemaphore& semaphore, bool aquireLockImmediately = true );
    ~IntegerLock();

    void lock();
    void free();
};

#endif
