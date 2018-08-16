#if defined(SharedTBB)
#include "tarch/multicore/tbb/PinningObserver.h"
#include "tarch/Assertions.h"

#include <sys/resource.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>


tarch::logging::Log tarch::multicore::PinningObserver::_log( "tarch::multicore::PinningObserver" );


tarch::multicore::PinningObserver::PinningObserver():
  _availableCores(0),
  _mutex() {
}


void tarch::multicore::PinningObserver::observe(bool toggle) {
  assertion( _availableCores.count()==0 );

  // reconstruct Unix mask
  cpu_set_t* mask;
  for ( _numberOfCPUs = sizeof(cpu_set_t)/CHAR_BIT; _numberOfCPUs < MaxCores; _numberOfCPUs <<= 1 ) {
    mask = CPU_ALLOC( _numberOfCPUs );
    if ( !mask ) break;
    const size_t size = CPU_ALLOC_SIZE( _numberOfCPUs );
    CPU_ZERO_S( size, mask );
    const int err = sched_getaffinity( 0, size, mask );
    if ( !err ) break;

    CPU_FREE( mask );
    mask = NULL;
    if ( errno != EINVAL )  break;
  }
<<<<<<< HEAD


  // Convert into bitset to make it more C++ish
  if ( !mask ) {
    logError( "PinningObserver()","Failed to obtain process affinity mask");
=======
  if ( !_mask ) {
    logWarning( "PinningObserver()","Failed to obtain process affinity mask");
  }
}


tarch::multicore::PinningObserver::~PinningObserver() {
  if ( _mask != nullptr ) {
    CPU_FREE( _mask );
  }
}


void tarch::multicore::PinningObserver::pinCurrentThread() {
  const size_t size = CPU_ALLOC_SIZE( _ncpus );
  const int num_cpus = CPU_COUNT_S( size, _mask );
  int thr_idx =  tbb::task_arena::current_thread_index();
  thr_idx %= num_cpus; // To limit unique number in [0; num_cpus-1] range
  // Place threads with specified step
  int cpu_idx = 0;
  for ( int i = 0, offset = 0; i<thr_idx; ++i ) {
    cpu_idx += _pinningStep;
    if ( cpu_idx >= num_cpus )
      cpu_idx = ++offset;
  }


  // Find index of 'cpu_idx'-th bit equal to 1
  int mapped_idx = -1;
  while ( cpu_idx >= 0 ) {
    if ( CPU_ISSET_S( ++mapped_idx, size, _mask ) )
      --cpu_idx;
  }

  cpu_set_t *target_mask = CPU_ALLOC( _ncpus );
  CPU_ZERO_S( size, target_mask );
  CPU_SET_S( mapped_idx, size, target_mask );
  const int err = sched_setaffinity( 0, size, target_mask );

  if ( err ) {
    logError( "PinningObserver()","Failed to set thread affinity!");
    exit( EXIT_FAILURE );
>>>>>>> branch 'master' of https://gitlab.lrz.de/gi26det/Peano.git
  }
  else {
<<<<<<< HEAD
    for (long i = 0; i < MaxCores; i++) {
      if (CPU_ISSET(i, mask)) {
        _availableCores[i] = true;
      }
    }
    logInfo( "PinningObserver()", "Available process mask: " << _availableCores );
    CPU_FREE( mask );
=======
    struct rlimit l;
    getrlimit(RLIMIT_STACK, &l);
    logInfo( "PinningObserver()", "Set thread affinity: thread " << thr_idx << " is pinned to hardware thread " << mapped_idx << ", stack size is " << l.rlim_cur );
>>>>>>> branch 'master' of https://gitlab.lrz.de/gi26det/Peano.git
  }

  // Call base class implementation
  tbb::task_scheduler_observer::observe(toggle);
}


tarch::multicore::PinningObserver::~PinningObserver() {
}


void tarch::multicore::PinningObserver::on_scheduler_entry( bool ) {
  _mutex.lock();

  if (_availableCores.count()==0) {
	logError( "on_scheduler_entry(bool)", "too many threads, i.e. no idle core available anymore" );
  }
  else {
    int targetCore = 0;
    for (int i=0; i<MaxCores; i++) {
      if (_availableCores[i]) {
    	targetCore = i;
        _availableCores[i] = false;
        i = MaxCores;
      }
    }

	cpu_set_t*   target_mask = CPU_ALLOC( _numberOfCPUs );
    const size_t size        = CPU_ALLOC_SIZE( _numberOfCPUs );
    CPU_ZERO_S( size, target_mask );
    CPU_SET_S( targetCore, size, target_mask );
	const int err = sched_setaffinity( 0, size, target_mask );

    if ( err ) {
      logInfo( "on_scheduler_entry(bool)", "pinning new thread to core " << targetCore << " failed with error code " << err );
    }
    else {
      const int   currentCore     = sched_getcpu();
   	  logInfo( "on_scheduler_entry(bool)", "pinned new thread currently running on core " << currentCore << " to core " << targetCore );
    }

    CPU_FREE( target_mask );
  }

  _mutex.unlock();
}


void tarch::multicore::PinningObserver::on_scheduler_exit( bool ) {
  _mutex.lock();

  const int   currentCore     = sched_getcpu();
  if (_availableCores[currentCore]) {
    logWarning( "on_scheduler_exit(bool)", "thread running on core " << currentCore << " exits though core is already marked as idle" );
  }
  else {
    _availableCores[currentCore] = true;
    logInfo( "on_scheduler_exit(bool)", "thread exits and frees core " << currentCore );
  }

  _mutex.unlock();
}



#endif
