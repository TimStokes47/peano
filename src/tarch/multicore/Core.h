// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_CORE_H_
#define _TARCH_MULTICORE_CORE_H_


#include <thread>
#include "tarch/logging/Log.h"


namespace tarch {
  namespace multicore {
    class Core;
  }
}


/**
 * Core
 *
 * Any shared memory implementation has to provide a singleton Core. Its full
 * qualified name is tarch::multicore::Core. If no shared memory variant is
 * switched on, this is the default Core implementation that does nothing.
 *
 * @author Tobias Weinzierl
 */
class tarch::multicore::Core {
  private:
    /**
     * Logging device
     */
    static tarch::logging::Log  _log;

    Core();

    int _numberOfThreads;
  public:
    /**
     * The default is what the system management typically gives you. So if
     * you run four ranks on a 24 core node, then each MPI rank will get 6
     * threads if you choose this constant.
     *
     * Multiply with two to exploit hyperthreading.
     */
    static constexpr int UseDefaultNumberOfThreads         = 0;

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
     * @param numberOfThreads Number of threads that shall be used. This
     *        parameter either is greater than zero (which defines the number
     *        of threads) or it equals DefaultNumberOfThreads which means that the code should
     *        use the default number of threads.
     */
    void configure( int numberOfThreads = UseDefaultNumberOfThreads, int maxNumberOfConcurrentBackgroundTasks = UseDefaultNumberOfThreads, int maxNumberOfConcurrentBandwidthBoundTasks = UseDefaultNumberOfThreads );

    /**
     * Shutdown parallel environment.
     */
    void shutDown();

    /**
     * @return Shared memory environment is up and runnning.
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

    /**
     * This is a real string which identifies the (OS) thread.
     */
    std::string getThreadId() const;

    /**
     * @return Physical core the process is running on
     */
    int getCoreNumber() const;

    /**
     *
     */
    static int getNumberOfUnmaskedThreads();
};


#endif
