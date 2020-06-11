// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "tarch/multicore/multicore.h"
#if defined(SharedCPP)
  #include "cpp/IntegerSemaphore.h"
#elif defined(SharedTBB)
  #include "tbb/IntegerSemaphore.h"
#elif defined(SharedOMP)
  #include "omp/IntegerSemaphore.h"
#elif !defined(_TARCH_MULTICORE_INTEGER_SEMAPHORE_H_)
#define _TARCH_MULTICORE_INTEGER_SEMAPHORE_H_

#include <string>


namespace tarch {
  namespace multicore {
    class IntegerSemaphore;
    class IntegerLock;
  }
}


/**
 * Integer Semaphore
 *
 * The integer sempahore is a variant of a semaphore, where N threads have to
 * hit the semaphore. Only when all N threads have arrived, all of them
 * continue in one rush.
 *
 * Other names in literature for such a semaphore are counting semaphores.
 *
 * <h2> Usage </h2>
 *
 * - Create a static instance of IntegerSemaphore.
 * - The first one hitting the semaphore sets the number of threads it wants
 *   to wait for. This is done via setNumberOfThreadsToWaitFor(). Anybody
 *   else can reset this value at any time. The point is: The semaphore pools
 *   all incoming thread requests until the number of waiting threads is equal
 *   or bigger to the number of threads it should wait for.
 * - When we leave the semaphore it all actually works the other way round:
 *   All the threads leave the protected area around the same time.
 *
 * @author Tobias Weinzierl
 */
class tarch::multicore::IntegerSemaphore {
  private:
    friend class tarch::multicore::IntegerLock;

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
    IntegerSemaphore( const IntegerSemaphore& semaphore ) {}

    /**
     * You may not copy a semaphore
     */
    IntegerSemaphore& operator=( const IntegerSemaphore& semaphore ) {return *this;}
  public:
    /**
     * @param numberOfThreadsToWaitFor You have to set this to a value greater
     *     than 0. Either here or via setNumberOfThreadsToWaitFor(), but it has
     *     to be set before you issue any lock.
     */
    IntegerSemaphore(int numberOfThreadsToWaitFor = -1);
    ~IntegerSemaphore();

    /**
     * This operation is thread-safe in the sense that you can freely reset it
     * if you want.
     */
    void setNumberOfThreadsToWaitFor( int value );
};


#endif
