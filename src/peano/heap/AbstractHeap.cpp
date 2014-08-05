#include "peano/heap/AbstractHeap.h"


std::set< peano::heap::AbstractHeap* >  peano::heap::AbstractHeap::_registeredHeaps;


void peano::heap::AbstractHeap::registerHeap( AbstractHeap* newHeap ) {
  _registeredHeaps.insert( newHeap );
}


void peano::heap::AbstractHeap::heapsStartToSendSynchronousData() {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).startToSendSynchronousData();
  }
}


void peano::heap::AbstractHeap::heapsStartToSendBoundaryData(bool isTraversalInverted) {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).startToSendBoundaryData(isTraversalInverted);
  }
}


void peano::heap::AbstractHeap::heapsFinishToSendSynchronousData() {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).finishedToSendSynchronousData();
  }
}


void peano::heap::AbstractHeap::heapsFinishToSendBoundaryData(bool isTraversalInverted) {
  for (
    std::set< peano::heap::AbstractHeap* >::iterator p = _registeredHeaps.begin();
    p != _registeredHeaps.end();
    p++
  ) {
    (**p).finishedToSendBoundaryData(isTraversalInverted);
  }
}
