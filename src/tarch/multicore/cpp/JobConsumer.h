// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_JOB_CONSUMER_H_) && defined(SharedCPP)
#define _TARCH_MULTICORE_JOB_CONSUMER_H_


#include <atomic>


#include "tarch/logging/Log.h"


namespace tarch {
  namespace multicore {
    namespace internal {
      class JobConsumer;
      class JobConsumerController;
    }
  }
}



struct tarch::multicore::internal::JobConsumerController {
  private:
    /**
     * Boolean semaphore.
     */
    std::atomic_flag    spinLock;
  public:
    enum class State {
      Running,
      TerminateTriggered,
      Terminated,
      Suspended
    };

    JobConsumerController();
    ~JobConsumerController();

    State    state;

    void lock();
    void unlock();
};



/**
 * A job consumer.
 */
class tarch::multicore::internal::JobConsumer {
  private:
	static tarch::logging::Log _log;

	/**
	 * -1 if no pinning is required
	 */
	const int                _pinCore;
	JobConsumerController*   _controller;
	cpu_set_t*               _mask;

	bool processBackgroundJobs();

	static const int MinNumberOfBackgroundJobs;
  public:
	static void addMask(int core, cpu_set_t* mask);
	static void removeMask();

	constexpr static int NoPinning = -1;

	/**
	 * Determine the primary queue this job is responsible for. We just take the
	 * pin core modulo the job classes minus one, as the biggest job class is
	 * reserved for background tasks.
	 */
	JobConsumer(int pinCore, JobConsumerController* controller, cpu_set_t*  mask);

	~JobConsumer();

	void operator()();
};


#endif

