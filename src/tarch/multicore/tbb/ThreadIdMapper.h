

#if !defined(_TARCH_MULTICORE_THREADIDMAPPER_H_)  && defined(SharedTBB)
#define _TARCH_MULTICORE_THREADIDMAPPER_H_

#include "tarch/multicore/BooleanSemaphore.h"

#include <tbb/task_scheduler_observer.h>

#include <map>
#include <stack>
#include <thread>

namespace tarch {
  namespace multicore{
	class ThreadIdMapper;
  }
}

class tarch::multicore::ThreadIdMapper: public tbb::task_scheduler_observer {
  private:
    //static tarch::logging::Log  _log;
	tarch::multicore::BooleanSemaphore _semaphore;
	//tarch::multicore::Lock _lock;
	std::map<std::thread::id, int> _mapToTid;
	std::stack<int> _freeIds;
	int _nextId;

	bool isCurrThreadMapped();
	void deleteCurrThread();
	void insertCurrThread();
	int getNextId();

  public:
    ThreadIdMapper( );
    virtual ~ThreadIdMapper();

    void on_scheduler_entry( bool ) override;
    void on_scheduler_exit( bool ) override;

    int getThreadId();
};

#endif /* TARCH_MULTICORE_THREADIDMAPPER_H_ */
