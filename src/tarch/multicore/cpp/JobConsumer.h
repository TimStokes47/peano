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
    std::atomic_flag    _spinLock;
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

	/**
	 * There are two different application areas of this constant.
	 *
	 * If we process background tasks, we always either process this many jobs
	 * or we process, if there are more than those, the number of jobs divided
	 * by the core count.
	 */
	static const int MinNumberOfJobs;
	static std::atomic<int> idleJobConsumers;
  public:
	static void addMask(int core, cpu_set_t* mask);
	static void removeMask();
	static bool isOneConsumerIdle();

	constexpr static int NoPinning = -1;

	/**
	 * Determine the primary queue this job is responsible for. We just take the
	 * pin core modulo the job classes minus one, as the biggest job class is
	 * reserved for background tasks.
	 */
	JobConsumer(int pinCore, JobConsumerController* controller, cpu_set_t*  mask);

	~JobConsumer();

	/**
	 * <h2> Sleep behaviour </h2>
	 *
	 * I frequently ran into the situation that some threads did starve as idle
	 * consumer threads (notably throughout grid constructor when not that much
	 * concurrency does exist) did block all semaphores. I thus found it useful
	 * to make consumers yield their thread if no work is available. We could
	 * also send them to sleep, but then getting the sleep time right is kind of
	 * black magic. So we stick to the yield.
	 *
	 * <h2> Idle consumers </h2>
	 *
	 * Technically, a consumer is idle if and only if it processes real jobs.
	 * If we do background jobs, we are indeed idle, as the job could actually
	 * do something else (something more meaningful).
	 */
	void operator()();
};


#endif

