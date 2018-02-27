#ifdef SharedCPP

#include "tarch/multicore/cpp/JobQueue.h"
#include "tarch/multicore/Jobs.h"


#include <sstream>


tarch::logging::Log tarch::multicore::internal::JobQueue::_log( "tarch::multicore::internal::JobQueue" );


tarch::multicore::internal::JobQueue::JobQueue() {
  _numberOfPendingJobs = 0;
}


tarch::multicore::internal::JobQueue::~JobQueue() {

}


tarch::multicore::internal::JobQueue&  tarch::multicore::internal::JobQueue::getMPIReceiveQueue() {
  static tarch::multicore::internal::JobQueue queue;
  return queue;
}


tarch::multicore::internal::JobQueue&  tarch::multicore::internal::JobQueue::getBackgroundQueue() {
  static tarch::multicore::internal::JobQueue queue;
  return queue;
}


std::string tarch::multicore::internal::JobQueue::toString() {
  std::ostringstream msg;
  msg << "(no-of-background-tasks:" << getBackgroundQueue().getNumberOfPendingJobs();
  for (int i=0; i<MaxNormalJobQueues; i++) {
	msg << ",queue[" << i << "]:" << getStandardQueue(i).getNumberOfPendingJobs();
  }
  msg << ")";
  return msg.str();
}


bool tarch::multicore::internal::JobQueue::processJobs( int maxNumberOfJobs ) {
  #ifdef UseNaiveImplementation
  _mutex.lock();
  if ( !_jobs.empty() && maxNumberOfJobs>0 ) {
    maxNumberOfJobs--;
    result = true;
    jobs::Job* job = _jobs.front();
    _jobs.pop_front();
    bool reenqueue = job->run();
    if (reenqueue) {
      _jobs.push_back( job );
    }
    else {
      delete job;
      _numberOfPendingJobs.fetch_sub(1);
    }
  }
  _mutex.unlock();
  return result;
  #else
  _mutex.lock();
  maxNumberOfJobs = std::min( maxNumberOfJobs, static_cast<int>( _jobs.size() ));

  if (maxNumberOfJobs==0) {
    _mutex.unlock();
    return false;
  }
  else {
    std::list< jobs::Job* >::iterator lastElementToBeProcessed  = _jobs.begin();
    for (int i=0; i<maxNumberOfJobs; i++) {
      lastElementToBeProcessed++;
    }
    std::list< jobs::Job* > localList;
    localList.splice( localList.begin(), _jobs, _jobs.begin(), lastElementToBeProcessed );

    logDebug( "processJobs(int)", "spliced " << maxNumberOfJobs << " job(s) from job queue and will process those now" );

    _numberOfPendingJobs.fetch_sub(maxNumberOfJobs);
    _mutex.unlock();

    for (auto& p: localList) {
      bool reenqueue = p->run();
      if (reenqueue) {
        addJob( p );
      }
      else {
        delete p;
      }
    }

    return true;
  }
  #endif
}


void tarch::multicore::internal::JobQueue::addJob( jobs::Job* job ) {
  _mutex.lock();
  _jobs.push_back(job);
  _mutex.unlock();
  _numberOfPendingJobs.fetch_add(1);
}


void tarch::multicore::internal::JobQueue::addJobWithHighPriority( jobs::Job* job ) {
  _mutex.lock();
  _jobs.push_front(job);
  _mutex.unlock();
  _numberOfPendingJobs.fetch_add(1);
}


int tarch::multicore::internal::JobQueue::getNumberOfPendingJobs() const {
  return _numberOfPendingJobs.load();
}


tarch::multicore::internal::JobQueue& tarch::multicore::internal::JobQueue::getStandardQueue(int jobClass) {
  static JobQueue queues[MaxNormalJobQueues];
  return queues[ jobClass%MaxNormalJobQueues ];
}



#endif
