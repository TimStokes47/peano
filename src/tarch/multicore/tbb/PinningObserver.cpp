#if defined(SharedTBB)
#include "tarch/multicore/tbb/PinningObserver.h"
#include <sched.h>
#include <sys/resource.h>


tarch::logging::Log tarch::multicore::PinningObserver::_log( "tarch::multicore::PinningObserver" );


tarch::multicore::PinningObserver::PinningObserver(int pinningStep):
  _pinningStep( pinningStep ),
  _mask( nullptr ) {
  const int MaxNumberOfSupportedCPUs = 16*1024;
  for ( _ncpus = sizeof(cpu_set_t)/CHAR_BIT; _ncpus < MaxNumberOfSupportedCPUs; _ncpus <<= 1 ) {
    _mask = CPU_ALLOC( _ncpus );
    if ( !_mask ) break;
    const size_t size = CPU_ALLOC_SIZE( _ncpus );
    CPU_ZERO_S( size, _mask );
    const int err = sched_getaffinity( 0, size, _mask );
    if ( !err ) break;

    CPU_FREE( _mask );
    _mask = NULL;
    if ( errno != EINVAL )  break;
  }
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
  }
  else {
    struct rlimit l;
    getrlimit(RLIMIT_STACK, &l);
    logInfo( "PinningObserver()", "Set thread affinity: thread " << thr_idx << " is pinned to hardware thread " << mapped_idx << ", stack size is " << l.rlim_cur );
  }

  CPU_FREE( target_mask );
}


void tarch::multicore::PinningObserver::on_scheduler_entry( bool ) {
  ++_numThreads;

  if ( _mask ) {
    pinCurrentThread();
  }
}


void tarch::multicore::PinningObserver::on_scheduler_exit( bool ) {
  --_numThreads;
}


int tarch::multicore::PinningObserver::getNumberOfRegisteredThreads() {
  return _numThreads;
}



#endif
