// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "tarch/multicore/MulticoreDefinitions.h"

#if !defined(_TARCH_MULTICORE_RECURSIVE_SEMAPHORE_H_) && defined(SharedTBB)
#define _TARCH_MULTICORE_RECURSIVE_SEMAPHORE_H_


#include <tbb/recursive_mutex.h>
//#include <mutex>


namespace tarch {
  namespace multicore {
    class RecursiveSemaphore;
    class RecursiveLock;
  }
}


/**
 * Recursive Semaphore
 *
 * A recursive semaphore is a boolean semphore that one thread (the first one)
 * can lock an arbitrary number of times.
 *
 * <h2> C++17 vs. new TBB versions </h2>
 *
 * New TBB versions have deprecated the tbb::recursive_mutex class in favour of
 * the C++ version. I however experienced segfault every now and then when I
 * tried to use the C++ variant. For the time being, I'll thus stick with the
 * TBB version.
 *
 *
 * @author Tobias Weinzierl
 */
class tarch::multicore::RecursiveSemaphore {
  private:
    friend class tarch::multicore::RecursiveLock;

    tbb::recursive_mutex          _recursiveMutex;
    //std::recursive_mutex          _recursiveMutex;


    bool tryEnterCriticalSection();
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
    RecursiveSemaphore( const RecursiveSemaphore& semaphore ) {}

    /**
     * You may not copy a semaphore
     */
    RecursiveSemaphore& operator=( const RecursiveSemaphore& semaphore ) {return *this;}
  public:
    RecursiveSemaphore();
    ~RecursiveSemaphore();
};


#endif
