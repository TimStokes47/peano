#include "peano/heap/AbstractHeap.h"


std::set< peano::heap::AbstractHeap* >  peano::heap::AbstractHeap::_registeredHeaps;
tarch::logging::Log                     peano::heap::AbstractHeap::_log( "peano::heap::AbstractHeap" );


void peano::heap::AbstractHeap::registerHeap( AbstractHeap* newHeap ) {
  _registeredHeaps.insert( newHeap );
}


void peano::heap::AbstractHeap::allHeapsStartToSendSynchronousData() {
  logTraceIn( "allHeapsStartToSendSynchronousData()" );
  for ( AbstractHeap* p : _registeredHeaps  ) {
    p->startToSendSynchronousData();
  }
  logTraceOut( "allHeapsStartToSendSynchronousData()" );
}


void peano::heap::AbstractHeap::allHeapsStartToSendBoundaryData(bool isTraversalInverted) {
  logTraceIn( "allHeapsStartToSendSynchronousData(bool)" );
  for ( AbstractHeap* p : _registeredHeaps  ) {
    p->startToSendBoundaryData(isTraversalInverted);
  }
  logTraceOut( "allHeapsStartToSendSynchronousData(bool)" );
}


void peano::heap::AbstractHeap::allHeapsFinishedToSendSynchronousData() {
  logTraceIn( "allHeapsStartToSendSynchronousData()" );
  for ( AbstractHeap* p : _registeredHeaps  ) {
    p->finishedToSendSynchronousData();
  }
  logTraceOut( "allHeapsStartToSendSynchronousData()" );
}


void peano::heap::AbstractHeap::allHeapsFinishedToSendBoundaryData(bool isTraversalInverted) {
  logTraceIn( "allHeapsStartToSendSynchronousData(bool)" );
  for ( AbstractHeap* p : _registeredHeaps  ) {
    p->finishedToSendBoundaryData(isTraversalInverted);
  }
  logTraceOut( "allHeapsStartToSendSynchronousData(bool)" );
}


void peano::heap::AbstractHeap::allHeapsDropReceivedBoundaryData() {
  logTraceIn( "allHeapsDropReceivedBoundaryData()" );
  for ( AbstractHeap* p : _registeredHeaps  ) {
    p->dropReceivedBoundaryData();
  }
  logTraceOut( "allHeapsDropReceivedBoundaryData()" );
}
