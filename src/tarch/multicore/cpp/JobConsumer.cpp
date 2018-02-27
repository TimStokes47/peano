#ifdef SharedCPP

#include "tarch/Assertions.h"
#include "tarch/multicore/cpp/JobConsumer.h"
#include "tarch/multicore/cpp/JobQueue.h"
#include "tarch/multicore/Core.h"

#include <thread>
#include <sched.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdint>
#include <climits>

tarch::logging::Log   tarch::multicore::internal::JobConsumer::_log( "tarch::multicore::internal::JobConsumer" );
const int tarch::multicore::internal::JobConsumer::MinNumberOfBackgroundJobs = 16;


tarch::multicore::internal::JobConsumer::JobConsumer(int pinCore, JobConsumerController* controller, cpu_set_t*  mask):
  _pinCore(pinCore),
  _controller(controller),
  _mask(mask) {
}


tarch::multicore::internal::JobConsumer::~JobConsumer() {
}


bool tarch::multicore::internal::JobConsumer::processBackgroundJobs() {
  const int  numberOfJobs  = internal::JobQueue::getBackgroundQueue().getNumberOfPendingJobs();
  static int numberOfCores = std::max(2,tarch::multicore::Core::getInstance().getNumberOfThreads());
  if (numberOfJobs>=MinNumberOfBackgroundJobs) {
    logDebug( "operator()", "consumer task (pin=" << _pinCore << ") processes " << numberOfJobs << " background jobs" );
    internal::JobQueue::getBackgroundQueue().processJobs( std::max(MinNumberOfBackgroundJobs,numberOfJobs/numberOfCores) );
    return true;
  }
  else return false;
}


bool tarch::multicore::internal::JobConsumer::processMPIReceiveJobs() {
  #ifdef Parallel
  const int  numberOfJobs  = internal::JobQueue::getBackgroundQueue().getNumberOfPendingJobs();

  if (numberOfJobs>=MinNumberOfBackgroundJobs) {
    int result = 0;
    MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &result, MPI_STATUS_IGNORE);

    if (result) {
      #ifdef Asserts
      logInfo( "processMPIReceiveJobs()", "consumer task (pin=" << _pinCore << ") processes MPI receive background jobs" );
      #endif
      internal::JobQueue::getBackgroundQueue().processJobs( numberOfJobs );
      return true;
    }
  }

  return false;
  #else
  return false;
  #endif
}


void tarch::multicore::internal::JobConsumer::operator()() {
  if (_pinCore!=NoPinning) {
	addMask(_pinCore,_mask);
  }

  JobConsumerController::State state = JobConsumerController::State::Running;
  while (state!=JobConsumerController::State::TerminateTriggered) {
	switch (state) {
      case JobConsumerController::State::Running:
        {
          bool foundJob = true;
          while (foundJob) {
            foundJob = false;
            for (int i=0; i<internal::JobQueue::MaxNormalJobQueues; i++) {
              const int jobs = internal::JobQueue::getStandardQueue(i).getNumberOfPendingJobs();
              foundJob |= jobs>0;
              if (jobs>0) {
              	logDebug( "operator()", "consumer task (pin=" << _pinCore << ") grabbed " << jobs << " job(s) from class " << i );
                internal::JobQueue::getStandardQueue(i).processJobs( std::numeric_limits<int>::max() );
              }
            }
          }
          processMPIReceiveJobs();
          processBackgroundJobs();
        }
    	break;
      case JobConsumerController::State::TerminateTriggered:
    	logInfo( "operator()", "found terminate-triggered flag" );
        break;
      case JobConsumerController::State::Terminated:
      	assertionMsg(false, "should not enter" );
        break;
      case JobConsumerController::State::Suspended:
    	assertionMsg(false, "not supported yet" );
    	break;
	}

    _controller->lock();
	state = _controller->state;
	_controller->unlock();
  }

  if (_pinCore!=NoPinning) {
    removeMask();
  }

  #ifdef Asserts
  logInfo( "operator()", "job consumer is going down" );
  #endif

  _controller->lock();
  _controller->state = JobConsumerController::State::Terminated;
  _controller->unlock();
}


void tarch::multicore::internal::JobConsumer::addMask(int core, cpu_set_t* mask) {
  const int    ncpus = std::thread::hardware_concurrency();
  const size_t size = CPU_ALLOC_SIZE( ncpus );
  const int    num_cpus = CPU_COUNT_S( size, mask );
  const int    pinningStep = 1;
  int thr_idx =  core % num_cpus; // To limit unique number in [0; num_cpus-1] range

  // Place threads with specified step
  int cpu_idx = 0;
  for ( int i = 0, offset = 0; i<thr_idx; ++i ) {
    cpu_idx += pinningStep;
    if ( cpu_idx >= num_cpus ) {
      cpu_idx = ++offset;
    }
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
    logError( "addMask()","failed to set thread affinity!");
    exit( EXIT_FAILURE );
  }
  else {
    struct rlimit l;
    getrlimit(RLIMIT_STACK, &l);
    logInfo( "addMask()", "consumer task (pin=" << core << ") was mapped onto hardware thread " << thr_idx << " and got pinned to CPU " << mapped_idx << ", stack size is " << l.rlim_cur );
  }
  CPU_FREE( target_mask );
}


void tarch::multicore::internal::JobConsumer::removeMask() {

}


tarch::multicore::internal::JobConsumerController::JobConsumerController():
  spinLock( ATOMIC_FLAG_INIT ) {
  lock();
  state = State::Running;
  unlock();
}


tarch::multicore::internal::JobConsumerController::~JobConsumerController() {
  assertion( state==State::Terminated );
}


void tarch::multicore::internal::JobConsumerController::lock() {
  while (spinLock.test_and_set(std::memory_order_acquire)); // spin
}


void tarch::multicore::internal::JobConsumerController::unlock() {
  spinLock.clear(std::memory_order_release);
}

#endif

