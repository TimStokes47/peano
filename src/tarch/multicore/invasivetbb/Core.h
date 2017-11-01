// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined(_TARCH_MULTICORE_TBB_CORE_H_) && defined(SharedTBBInvade)
#define _TARCH_MULTICORE_TBB_CORE_H_


#include "tarch/logging/Log.h"
#include "tarch/multicore/MulticoreDefinitions.h"


#if defined(Asserts)
  #if !defined(SHM_INVADE_DEBUG) || SHM_INVADE_DEBUG<2
    #warning It is recommended to set compile flag SHM_INVADE_DEBUG to 2 or bigger if you compile Peano with Asserts
  #endif
#endif

#if defined(Debug)
  #if !defined(SHM_INVADE_DEBUG) || SHM_INVADE_DEBUG<4
    #warning It is recommended to set compile flag SHM_INVADE_DEBUG to 4 or bigger if you compile Peano with Debug
  #endif
#endif

#include "SHMInvade.hpp"


namespace tarch {
  namespace multicore {
    class Core;
  }
}


/**
 * TBB Core for our invasive wrapper.
 *
 * This class is a singleton.
 *
 * @author Tobias Weinzierl
 */
class tarch::multicore::Core {
  private:
    SHMInvadeRoot     _invadeRoot;

    Core();

    static tarch::logging::Log  _log;
  public:
    /**
     * Destructor
     */
    ~Core();

    /**
     * @return Singleton instance
     */
    static Core& getInstance();

    /**
     * Configure the whole thing. If numberOfThreads equals 0, the core is
     * using the number of standard threads.
     *
     *
     * @param numberOfThreads Numer of threads that shall be used. This
     *        parameter either is greater than zero (which defines the number
     *        of threads) or it equals DefaultNumberOfThreads which means that the code should
     *        use the default number of threads.
     */
    void configure( int numberOfThreads );

    /**
     * Shutdown parallel environment.
     */
    void shutDown();

    /**
     * @return Shared memory environment is up and running.
     */
    bool isInitialised() const;

    /**
     * Returns the number of threads that is used by TBB. This routine usually
     * is not of interest at all as TBB should do all the thread management
     * automatically. However, the user interface plots some information on the
     * number of threads used, and sometimes I found it useful.
     *
     * @return Number of threads available.
     */
    int getNumberOfThreads() const;
};


#endif
