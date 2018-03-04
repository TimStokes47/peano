#ifdef SharedCPP

#include "tarch/multicore/cpp/JobQueue.h"
#include "tarch/multicore/Jobs.h"


#include <sstream>


tarch::logging::Log tarch::multicore::internal::JobQueue::_log( "tarch::multicore::internal::JobQueue" );
std::atomic<int>    tarch::multicore::internal::JobQueue::LatestQueueBefilled(0);


tarch::multicore::internal::JobQueue::JobQueue() {
  _numberOfPendingJobs = 0;
}


tarch::multicore::internal::JobQueue::~JobQueue() {
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

  if (_numberOfPendingJobs.load()>0) {
    _mutex.lock();

    // might have changed meanwhile
    maxNumberOfJobs = std::min( maxNumberOfJobs, static_cast<int>( _jobs.size() ));

    if (maxNumberOfJobs==0) {
      _mutex.unlock();
      return false;
    }
    else {
      // This is the first thing we do to avoid that others try to read from
      // the queue even if we are about to take all elements out.
      _numberOfPendingJobs.fetch_sub(maxNumberOfJobs);

      std::list< jobs::Job* >::iterator lastElementToBeProcessed  = _jobs.begin();
      for (int i=0; i<maxNumberOfJobs; i++) {
        lastElementToBeProcessed++;
      }
      std::list< jobs::Job* > localList;
      localList.splice( localList.begin(), _jobs, _jobs.begin(), lastElementToBeProcessed );

      logDebug( "processJobs(int)", "spliced " << maxNumberOfJobs << " job(s) from job queue and will process those now" );

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
  }
  else return false;
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


#endif
