#if !defined(_TARCH_MULTICORE_PINNING_OBSERVER_) && defined(SharedTBB)
#define _TARCH_MULTICORE_PINNING_OBSERVER_


#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <tbb/task_arena.h>
#include <tbb/task_scheduler_observer.h>
#include <tbb/atomic.h>

#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>
#include <tbb/tick_count.h>

#include "tarch/logging/Log.h"

namespace tarch {
  namespace multicore {
    class PinningObserver;
  }
}


/**
 * This implementations follows very closely
 *
 * https://software.intel.com/en-us/blogs/2013/10/31/applying-intel-threading-building-blocks-observers-for-thread-affinity-on-intel
 *
 * @author Leonhard Rannabauer
 * @author Tobias Weinzierl
 */
class tarch::multicore::PinningObserver: public tbb::task_scheduler_observer {
  private:
    static tarch::logging::Log  _log;

    const int        _pinningStep;

    /**
     * Masking being available to process. This is basically a bitfield which
     * holds an entry for each core (hardware thread) the present application
     * is allowed to run on. If you run multiple MPI ranks for example, this
     * is a subset of the actual cores available on a node. The fild is
     * initialised in the constructor.
     */
    cpu_set_t*       _mask;

    int              _ncpus;

    tbb::atomic<int> _threadIndex;

    /**
     * How many threads have been registered through callback
     */
    tbb::atomic<int> _numThreads;

  public:
    /**
     * @todo ncpus should be replaced by the concurrency operation from C++14
     *
     * @see _mask
     */
    PinningObserver( int pinningStep=1 );
    virtual ~PinningObserver();


/*
    : pinning_step(pinning_step), thread_index();

  {
*/


/*
  override void on_scheduler_entry( bool ) {
    if ( !mask ) return;

    const size_t size = CPU_ALLOC_SIZE( ncpus );
    const int num_cpus = CPU_COUNT_S( size, mask );
    int thr_idx =  tbb::task_arena::current_thread_index();
    thr_idx %= num_cpus; // To limit unique number in [0; num_cpus-1] range
    // Place threads with specified step
    int cpu_idx = 0;
    for ( int i = 0, offset = 0; i<thr_idx; ++i ) {
      cpu_idx += pinning_step;
      if ( cpu_idx >= num_cpus )
        cpu_idx = ++offset;
    }


    // Find index of 'cpu_idx'-th bit equal to 1
    int mapped_idx = -1;
    while ( cpu_idx >= 0 ) {
      if ( CPU_ISSET_S( ++mapped_idx, size, mask ) )
        --cpu_idx;
    }

    cpu_set_t *target_mask = CPU_ALLOC( ncpus );
    CPU_ZERO_S( size, target_mask );
    CPU_SET_S( mapped_idx, size, target_mask );
    const int err = sched_setaffinity( 0, size, target_mask );

if ( err ) {
  logInfo( "PinningObserver()","Failed to set thread affinity!\n");
  exit( EXIT_FAILURE );
 }
 else {
  logInfo( "PinningObserver()", "Set thread affinity: Thread " << thr_idx << ": CPU " << mapped_idx);
 }

 CPU_FREE( target_mask );
}

~pinning_observer() {
  if ( mask )
    CPU_FREE( mask );
}
*/

  void on_scheduler_entry( bool ) override;
  void on_scheduler_exit( bool ) override;


  int getNumberOfRegisteredThreads();

};

#endif

