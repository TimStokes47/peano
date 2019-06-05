#if !defined(_TARCH_MULTICORE_TBB_PERFORMANCEMONITOR_H_) && defined(SharedTBB)  && defined(Parallel)
#define _TARCH_MULTICORE_TBB_PERFORMANCEMONITOR_H_

#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"

#include <mpi.h>


namespace tarch {
  namespace multicore {
    class PerformanceMonitor;
    //class PerformanceMonitorJob;
  }
}

class tarch::multicore::PerformanceMonitor {
private:
    static tarch::logging::Log _log;
    PerformanceMonitor();
    bool _isStarted;
    int *_currentNumBackgroundTasks;
    int *_currentNumBackgroundTasksBuffer;
    MPI_Comm _mpi_comm;
    MPI_Request _gather_request;
    tarch::multicore::BooleanSemaphore _semaphore;

    void progressGather();
    void postGather();

    bool isGloballyTerminated();
public:
	static PerformanceMonitor& getInstance();
	virtual ~PerformanceMonitor();

	void start();
	void stop();
	void setCurrentNumberOfBGTasks(int num);
	void run();

};


/*class tarch::multicore::PerformanceMonitorJob {
private:
    //static tarch::logging::Log _log;
	//bool test;
public:
    PerformanceMonitorJob();
    //virtual ~PerformanceMonitorJob();

    bool operator()();
};*/


#endif
