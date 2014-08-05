#include "peano/heap/AbstractHeap.h"


std::set< peano::heap::AbstractHeap* >  peano::heap::AbstractHeap::_registeredHeaps;


void peano::heap::AbstractHeap::registerHeap( AbstractHeap* newHeap ) {
  _registeredHeaps.insert( newHeap );
}


void peano::heap::AbstractHeap::allHeapsStartToSendSynchronousData() {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).startToSendSynchronousData();
  }
}


void peano::heap::AbstractHeap::allHeapsStartToSendBoundaryData(bool isTraversalInverted) {
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
}


void peano::heap::AbstractHeap::allHeapsFinishedToSendBoundaryData(bool isTraversalInverted) {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).finishedToSendBoundaryData(isTraversalInverted);
  }
}
