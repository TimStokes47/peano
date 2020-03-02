// Copyright (C) 2009 Technische Universitaet Muenchen
// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPI_BOOLEAN_SEMAPHORE_H_
#define _TARCH_MPI_BOOLEAN_SEMAPHORE_H_


#include "tarch/mpi/mpi.h"
#include "tarch/logging/Log.h"
#include "tarch/services/Service.h"
#include "tarch/multicore/BooleanSemaphore.h"

#include <string>
#include <map>
#include <vector>


namespace tarch {
  namespace mpi {
    class BooleanSemaphore;
    class Lock;
  }
}


/**
 * Boolean Semaphore
 *
 * A boolean semaphore is a global atomic variable.
 *
 *
 * @author Tobias Weinzierl
 */
class tarch::mpi::BooleanSemaphore {
  private:
    class BooleanSemaphoreService: public tarch::services::Service {
      private:
        int                  _semaphoreCounter;
        int                  _semaphoreTag;

        tarch::multicore::BooleanSemaphore  _mapSemaphore;
        std::map<int,bool>                  _semaphoreMap;

        BooleanSemaphoreService();

      public:
        void receiveDanglingMessages() override;

        /**
         * Don't use this routine. It returns the global semaphore and is only used
         * on the master/by the core.
         */
        static BooleanSemaphoreService& getInstance();

        int getSemaphoreNumber();

        void acquireLock( int number );
        void releaseLock( int number );
    };

    friend class tarch::mpi::Lock;

    static tarch::logging::Log  _log;

    const int  _semaphoreNumber;

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
    BooleanSemaphore( const BooleanSemaphore& semaphore ) = delete;

    /**
     * You may not copy a semaphore
     */
    BooleanSemaphore& operator=( const BooleanSemaphore& semaphore ) = delete;

  public:
    /**
     * Register a new global semaphore. Please note that global semaphores
     * have to be static!
     *
     * @identifier Unique identifier. I usually use the fully qualified classname.
     *   The string however is only used for bookkeeping reasons. There's no
     *   semantics behind the string (it even doesn't have to be unique though I
     *   do not encourage passing non-meaningful arguments).
     */
    BooleanSemaphore( const std::string& identifier );

    ~BooleanSemaphore();
};


#endif
