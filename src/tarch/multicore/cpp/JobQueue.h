// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_CPP_JOB_QUEUE_H_) && defined(SharedCPP)
#define _TARCH_MULTICORE_CPP_JOB_QUEUE_H_


#include <list>
#include <atomic>
#include <mutex>
#include <stack>


#include "tarch/logging/Log.h"


namespace tarch {
  namespace multicore {
    namespace jobs {
      /**
       * Forward declaration
       */
      class Job;
    }

    namespace internal {
      class JobQueue;
    }
  }
}


#define JobQueueUsesSpinLockInsteadOfMutex


class tarch::multicore::internal::JobQueue {
  private:
	static tarch::logging::Log _log;

	std::list< jobs::Job* > _jobs;

	std::atomic<int>  _numberOfPendingJobs;

    #ifdef JobQueueUsesSpinLockInsteadOfMutex
    std::atomic_flag    _spinLock;
    #else
    std::mutex        _mutex;
    #endif

	JobQueue();

	static std::atomic<int>  LatestQueueBefilled;

//	static std::stack<int>   latestQueuesBefilled;
//	static std::mutex        latestQueueMutex;
  public:
	static constexpr int     MaxNormalJobQueues = 8;

	static inline void setLatestQueueBefilled(int jobClass) {
	  LatestQueueBefilled = jobClass;
	}

	static inline int  getLatestQueueBefilled() {
	  return internal::JobQueue::LatestQueueBefilled.load();
	}

	static inline void latestQueueBefilledIsEmpty() {}

	~JobQueue();

	static inline JobQueue& getBackgroundQueue() __attribute__((always_inline)) {
      static tarch::multicore::internal::JobQueue queue;
      return queue;
    }

	static inline JobQueue& getMPIReceiveQueue() __attribute__((always_inline)) {
	  static tarch::multicore::internal::JobQueue queue;
	  return queue;
	}

	static inline JobQueue& getStandardQueue(int jobClass) __attribute__((always_inline)) {
	  static JobQueue queues[MaxNormalJobQueues];
	  return queues[ jobClass%MaxNormalJobQueues ];
	}

	bool processJobs( int maxNumberOfJobs );

	void addJob( jobs::Job* job );
	void addJobWithHighPriority( jobs::Job* job );

	int inline getNumberOfPendingJobs() const __attribute__((always_inline)) {
	  return _numberOfPendingJobs.load();
	}

	static std::string toString();
};

#endif
