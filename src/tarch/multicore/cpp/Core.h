// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_CORE_H_ ) && defined(SharedCPP)
#define _TARCH_MULTICORE_CORE_H_


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
	int    _numberOfThreads;
    Core();
  public:
    static constexpr int UseDefaultNumberOfThreads = 0;

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

    void pinThreads(bool value);
};


#endif
