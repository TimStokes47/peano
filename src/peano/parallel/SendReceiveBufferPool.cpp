#include "peano/parallel/SendReceiveBufferPool.h"


#include "tarch/parallel/Node.h"
#include "tarch/parallel/NodePool.h"
#include "tarch/Assertions.h"
#include "tarch/timing/Watch.h"
#include "tarch/mpianalysis/Analyser.h"


#include "tarch/services/ServiceFactory.h"
registerService(peano::parallel::SendReceiveBufferPool)


tarch::logging::Log peano::parallel::SendReceiveBufferPool::_log( "peano::parallel::SendReceiveBufferPool" );


#ifdef Parallel
peano::parallel::SendReceiveBufferPool::SendReceiveBufferPool():
  _iterationManagementTag(MPI_ANY_TAG),
  _iterationDataTag(MPI_ANY_TAG),
  _bufferSize(0) {
  _iterationManagementTag = tarch::parallel::Node::getInstance().reserveFreeTag("SendReceiveBufferPool[it-mgmt]");
  _iterationDataTag       = tarch::parallel::Node::getInstance().reserveFreeTag("SendReceiveBufferPool[it-data]");
}
#else
peano::parallel::SendReceiveBufferPool::SendReceiveBufferPool():
  _iterationManagementTag(-1),
  _iterationDataTag(-1),
  _bufferSize(0) {
}
#endif


peano::parallel::SendReceiveBufferPool::~SendReceiveBufferPool() {
  for (std::map<int,SendReceiveBuffer*>::iterator p = _map.begin(); p!=_map.end(); p++ ) {
    std::cerr << "encountered open buffer for destination " << p->first << ". Would be nicer to call terminate on SendReceiveBufferPool." << std::endl;
    delete p->second;
  }
}


peano::parallel::SendReceiveBufferPool& peano::parallel::SendReceiveBufferPool::getInstance() {
  static peano::parallel::SendReceiveBufferPool singleton;
  return singleton;
}



int peano::parallel::SendReceiveBufferPool::getIterationManagementTag() const {
  #ifdef Parallel
  assertion( _iterationManagementTag!=MPI_ANY_TAG );
  #endif
  return _iterationManagementTag;
}


int peano::parallel::SendReceiveBufferPool::getIterationDataTag() const {
  #ifdef Parallel
  assertion( _iterationDataTag!=MPI_ANY_TAG );
  #endif
  return _iterationDataTag;
}


void peano::parallel::SendReceiveBufferPool::receiveDanglingMessages() {
  for (std::map<int,SendReceiveBuffer*>::iterator p = _map.begin(); p!=_map.end(); p++ ) {
    p->second->receivePageIfAvailable();
  }
}


void peano::parallel::SendReceiveBufferPool::terminate() {
  for (std::map<int,SendReceiveBuffer*>::iterator p = _map.begin(); p!=_map.end(); p++ ) {
    assertion1(  p->first >= 0, tarch::parallel::Node::getInstance().getRank() );
    assertion1( _map.count(p->first) == 1, tarch::parallel::Node::getInstance().getRank() );
    assertionEquals2( p->second->getNumberOfReceivedMessages(), 0, p->first, tarch::parallel::Node::getInstance().getRank() );
    delete p->second;
  }

  _map.clear();
}


void peano::parallel::SendReceiveBufferPool::restart() {
  assertion1( _map.empty(), tarch::parallel::Node::getInstance().getRank() );
}


void peano::parallel::SendReceiveBufferPool::releaseMessages() {
  logTraceIn( "releaseMessages()" );

  tarch::timing::Watch watchTotal( "peano::parallel::SendReceiveBufferPool", "releaseMessages()", false );
  tarch::timing::Watch watchSend( "peano::parallel::SendReceiveBufferPool", "releaseMessages()", false );

  for ( std::map<int,SendReceiveBuffer*>::const_reverse_iterator p = _map.rbegin(); p != _map.rend(); p++ ) {
    p->second->releaseSentMessages();
  }
  watchSend.stopTimer();
  for ( std::map<int,SendReceiveBuffer*>::const_reverse_iterator p = _map.rbegin(); p != _map.rend(); p++ ) {
    p->second->releaseReceivedMessages(true);
  }

  watchTotal.stopTimer();

  logInfo(
    "releaseMessages()",
    "rank had to wait from data "
    << "(" << watchTotal.getCPUTicks() << "," << watchTotal.getCPUTime() << "," << watchTotal.getCalendarTime() << ")"
    << ", hereof "
    << "(" << watchSend.getCPUTicks() << "," << watchSend.getCPUTime() << "," << watchSend.getCalendarTime() << ") "
    << "for send [cpu ticks, cpu time, calendar time]"
  );

  logTraceOut( "releaseMessages()" );
}


void peano::parallel::SendReceiveBufferPool::setBufferSize( int bufferSize ) {
  #ifdef Parallel
  assertion1( _map.empty(), tarch::parallel::Node::getInstance().getRank() );
  assertion2( bufferSize>0, bufferSize, tarch::parallel::Node::getInstance().getRank() );

  _bufferSize = bufferSize;
  #endif
}
