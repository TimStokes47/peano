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
 * As Peano uses the enqueue routine frequently, our invasive TBB version
 * invades at least one additional core always. That is, you will always see
 * two non-idle ranks.
 *
 * Please ensure that isInitialised() or any other routine is called early
 * throughout the user cycle. In Peano, peano::initSharedMemoryEnvironment()
 * does this job.
 *
 * @author Tobias Weinzierl
 */
class tarch::multicore::Core {
  private:
    SHMInvadeRoot*    _invadeRoot;
    //SHMInvade*        _basicInvasion;

    /**
     * As the class is a singleton, this constructor is protected and you
     * solely are allowed to create an object indirectly through getIntance().
     * The constructor first cleans up the shared memory environment and then
     * invades the node with the first MinThreads threads.
     */
    Core();

    static tarch::logging::Log  _log;

    /**
     * We never allocate fewer than two cores per MPI ranks. One core is a
     * trivial lower bound. Yet, it makes sense to set it to zero. If the
     * invasion is tied to one core and Peano enqueues a background task, then
     * the system will deadlock.
     */
    static constexpr int MinThreads = 2;
  public:
    static constexpr int UseDefaultNumberOfThreads = SHMInvade::AllCoresConcurrencyLevel;

    /**
     * Destructor
     */
    ~Core();

    /**
     * @return Singleton instance
     */
    static Core& getInstance();

    /**
     * Configure the invasion level.
     *
     * The core's configure tells the core how many cores shall be used at
     * least/as baseline. You may call it over and over with different arguments
     * which induces a change of the invasion base level.
     *
     * @param numberOfThreads Number of threads that the code wants to use at
     *        least. This
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
     * This query has to be triggered after all MPI ranks are up and running.
     *
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
