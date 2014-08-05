#include "peano/heap/AbstractHeap.h"


std::set< peano::heap::AbstractHeap* >  peano::heap::AbstractHeap::_registeredHeaps;
tarch::logging::Log                     peano::heap::AbstractHeap::_log( "peano::heap::AbstractHeap" );


void peano::heap::AbstractHeap::registerHeap( AbstractHeap* newHeap ) {
  _registeredHeaps.insert( newHeap );
}


void peano::heap::AbstractHeap::allHeapsStartToSendSynchronousData() {
  logInfo( "allHeapsStartToSendSynchronousData()", "start to send synchronous data" );
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).startToSendSynchronousData();
  }
}


void peano::heap::AbstractHeap::allHeapsStartToSendBoundaryData(bool isTraversalInverted) {
  logInfo( "allHeapsStartToSendSynchronousData()", "start to send boundary data" );
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).startToSendBoundaryData(isTraversalInverted);
  }
}


void peano::heap::AbstractHeap::allHeapsFinishedToSendSynchronousData() {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).finishedToSendSynchronousData();
  }
  logInfo( "allHeapsStartToSendSynchronousData()", "finished to send synchronous data" );
}


void peano::heap::AbstractHeap::allHeapsFinishedToSendBoundaryData(bool isTraversalInverted) {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).finishedToSendBoundaryData(isTraversalInverted);
  }
  logInfo( "allHeapsStartToSendSynchronousData()", "finished to send boundary data" );
}
