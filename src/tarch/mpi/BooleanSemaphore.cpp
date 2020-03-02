#include "BooleanSemaphore.h"
#include "Rank.h"


#include "tarch/Assertions.h"
#include "tarch/multicore/Lock.h"
#include "tarch/services/ServiceRepository.h"


tarch::logging::Log  tarch::mpi::BooleanSemaphore::_log( "tarch::mpi::BooleanSemaphore" );


void tarch::mpi::BooleanSemaphore::enterCriticalSection() {
  BooleanSemaphoreService::getInstance().acquireLock(_semaphoreNumber);
}


void tarch::mpi::BooleanSemaphore::leaveCriticalSection() {
  BooleanSemaphoreService::getInstance().releaseLock(_semaphoreNumber);
}


tarch::mpi::BooleanSemaphore::BooleanSemaphore( const std::string& identifier ):
  _semaphoreNumber( BooleanSemaphoreService::getInstance().getSemaphoreNumber()) {
}


tarch::mpi::BooleanSemaphore::~BooleanSemaphore() {
}


tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::BooleanSemaphoreService():
  // Don't give out 0 as 0 can't be inverted into -0 to distinguish locks and frees
  _semaphoreCounter(1),
  _semaphoreTag(tarch::mpi::Rank::reserveFreeTag("global semaphores")) {
  tarch::services::ServiceRepository::getInstance().addService( this, "tarch::mpi::BooleanSemaphore::BooleanSemaphoreService" );
}


void tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::serveLockRequests() {
  assertion(tarch::mpi::Rank::getInstance().isGlobalMaster());

  #ifdef Parallel
  bool servedLockRequest = true;
  while (servedLockRequest) {
    servedLockRequest = false;
    {
      tarch::multicore::Lock lock(_mapAccessSemaphore);
      for (auto request = _pendingLockRequests.begin(); request != _pendingLockRequests.end(); ) {
        addMapEntryLazily(request->second);
        if ( not _map[request->second] ) {
          servedLockRequest = true;
          _pendingLockRequests.erase(request);
          _map[request->second] = true;
          // @todo Debug
          logInfo( "receiveDanglingMessages()", "locked sempahore " << request->second << " for rank " << request->first );
          MPI_Send( &(request->second), 1, MPI_INT, request->first, _semaphoreTag, tarch::mpi::Rank::getInstance().getCommunicator());
          request--;
        }
        else {
          request++;
        }
      }
    }
  }
  #endif
}



void tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::receiveDanglingMessages() {
  if (tarch::mpi::Rank::getInstance().isGlobalMaster()) {
    #ifdef Parallel
    int flag = false;
    MPI_Status status;
    MPI_Iprobe(MPI_ANY_SOURCE, _semaphoreTag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, &status);

    if (flag) {
      int number;
      logInfo( "receiveDanglingMessages()", "there's a pending message from " << status.MPI_SOURCE );
      MPI_Recv( &number, 1, MPI_INT, status.MPI_SOURCE, _semaphoreTag, tarch::mpi::Rank::getInstance().getCommunicator(), MPI_STATUS_IGNORE);
      assertion(number!=0);

      // @todo Debug
      logInfo( "receiveDanglingMessages()", "received number " << number << " from rank " << status.MPI_SOURCE );
      if (number>0) {
        tarch::multicore::Lock lock(_mapAccessSemaphore);
        std::pair<int,int> newEntry( status.MPI_SOURCE, number );
        _pendingLockRequests.push_back(newEntry);
      }
      else {
        releaseLock(-number);
      }
    }

    serveLockRequests();
    #endif
  }
}


tarch::mpi::BooleanSemaphore::BooleanSemaphoreService& tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::getInstance() {
  static BooleanSemaphoreService singleton;
  return singleton;
}


int tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::getSemaphoreNumber() {
  _semaphoreCounter++;
  return _semaphoreCounter-1;
}


void tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::addMapEntryLazily(int number) {
  if ( _map.count(number)==0 ) {
    _map.insert( std::pair<int,bool>(number,false) );
  }
}


void tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::acquireLock( int number ) {
  if ( tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    {
      tarch::multicore::Lock lock(_mapAccessSemaphore);
      addMapEntryLazily(number);
    }

    bool gotLock = false;
    while (not gotLock) {
      {
        tarch::multicore::Lock lock(_mapAccessSemaphore);
        if (_map[number]==false) {
          gotLock = true;
          _map[number] = true;
        }
      }
      // only check if not successful, so someone else has the chance to release
      // a lock
      if (not gotLock) {
        receiveDanglingMessages();
      }
    }
    // @todo Debug
    logInfo( "acquireLock()", "successfully acquired lock " << number << " for global master");
  }
  else {
    #ifdef Parallel
    // @todo Debug
    logInfo( "acquireLock()", "have to acquire lock on global master " << tarch::mpi::Rank::getGlobalMasterRank() << " and thus send master a " << number );
           
    MPI_Send( &number, 1, MPI_INT, tarch::mpi::Rank::getGlobalMasterRank(), _semaphoreTag, tarch::mpi::Rank::getInstance().getCommunicator() );

    // @todo Debug
    logInfo( "acquireLock()", "wait for confirmation from global master rank" );
    MPI_Request request;
    MPI_Irecv( &number, 1, MPI_INT, tarch::mpi::Rank::getGlobalMasterRank(), _semaphoreTag, tarch::mpi::Rank::getInstance().getCommunicator(), &request );
    int flag = 0;
    while ( not flag ) {
      receiveDanglingMessages();
      MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
    }
    #else
    assertionMsg( false, "may not happen" );
    #endif
  }
}


void tarch::mpi::BooleanSemaphore::BooleanSemaphoreService::releaseLock( int number ) {
  if ( tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    {
      tarch::multicore::Lock lock(_mapAccessSemaphore);
      assertion( _map.count(number)==1 );
      assertion( _map[number]==true );
      _map[number]=false;

      // @todo Debug
      logInfo( "acquireLock()", "successfully released lock " << number );
    }

    // It is important to unlock the semaphore, as the serve process itself will reaquire it.
    serveLockRequests();
  }
  else {
    #ifdef Parallel
    number = -number;
    // @todo Debug
    logInfo( "releaseLock()", "send global master " << number << " to release global lock" );
   
    MPI_Send( &number, 1, MPI_INT, tarch::mpi::Rank::getGlobalMasterRank(), _semaphoreTag, tarch::mpi::Rank::getInstance().getCommunicator() );
    #else
    assertionMsg( false, "may not happen" );
    #endif
  }
}

