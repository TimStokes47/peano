#if defined(SharedTBB)

#include "ThreadIdMapper.h"

#include "tarch/Assertions.h"
#include "tarch/multicore/Lock.h"

#include <iostream>

tarch::multicore::ThreadIdMapper::ThreadIdMapper():
	 _semaphore(),
	 //_lock(_semaphore, false),
	 _nextId(0)
{}

tarch::multicore::ThreadIdMapper::~ThreadIdMapper() {
}

void tarch::multicore::ThreadIdMapper::on_scheduler_entry( bool ) {
	assertion(!isCurrThreadMapped());
	    insertCurrThread();
}

void tarch::multicore::ThreadIdMapper::on_scheduler_exit( bool ) {
	assertion(isCurrThreadMapped());
	deleteCurrThread();
}

int tarch::multicore::ThreadIdMapper::getThreadId() {
       
	std::thread::id currId= std::this_thread::get_id();

	if(!isCurrThreadMapped())
            return 0;
	return _mapToTid[currId];
}

void tarch::multicore::ThreadIdMapper::insertCurrThread() {
    assertion(!isCurrThreadMapped());
    std::thread::id currId= std::this_thread::get_id();

    tarch::multicore::Lock lock(_semaphore);
    if(_freeIds.empty())
    	_mapToTid.insert(std::make_pair(currId, _nextId++));
    else {
    	_mapToTid.insert(std::make_pair(currId, _freeIds.top()));
    	_freeIds.pop();
    }
    lock.free();
}

void tarch::multicore::ThreadIdMapper::deleteCurrThread() {
	std::thread::id currId= std::this_thread::get_id();

    tarch::multicore::Lock lock(_semaphore);
	int assignedId = getThreadId();
	_freeIds.push(assignedId);
	_mapToTid.erase(currId);
	lock.free();
}

bool tarch::multicore::ThreadIdMapper::isCurrThreadMapped() {
	std::thread::id currId= std::this_thread::get_id();
	return !(_mapToTid.find(currId)==_mapToTid.end());
}

#endif
