// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined( _TARCH_MULTICORE_CPP_JOB_QUEUE_H_) && defined(SharedCPP)
#define _TARCH_MULTICORE_CPP_JOB_QUEUE_H_


#include <list>
#include <atomic>
#include <mutex>


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



class tarch::multicore::internal::JobQueue {
  private:
	static tarch::logging::Log _log;

	std::list< jobs::Job* > _jobs;

	std::atomic<int>  _numberOfPendingJobs;

	std::mutex        _mutex;

	JobQueue();

  public:
	static constexpr int MaxNormalJobQueues = 8;

	~JobQueue();

	static JobQueue& getBackgroundQueue();
	static JobQueue& getStandardQueue(int jobClass);

	bool processJobs( int maxNumberOfJobs );

	void addJob( jobs::Job* job );
	void addJobWithHighPriority( jobs::Job* job );

	int getNumberOfPendingJobs() const;

	static std::string toString();
};

#endif
