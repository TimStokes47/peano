#include "peano/heap/SendReceiveTask.h"

#include "tarch/la/VectorCompare.h"

tarch::logging::Log  peano::heap::SendReceiveTask<double>::_log( "peano::heap::SendReceiveTask<double>" );
tarch::logging::Log  peano::heap::SendReceiveTask<char>::_log(   "peano::heap::SendReceiveTask<char>" );


#ifdef Asserts
peano::heap::SendReceiveTask<double>::SendReceiveTask():
  _rank(-1),
  _data(0) {
}
#endif


bool peano::heap::SendReceiveTask<double>::fits(
  const tarch::la::Vector<DIMENSIONS, double>&  position,
  int                                           level
) const {
  #ifdef Asserts
  const double tolerance =
    tarch::la::NUMERICAL_ZERO_DIFFERENCE *
    std::max(
      1.0, std::max(
        tarch::la::maxAbs(_metaInformation.getPosition()), tarch::la::maxAbs(position)
      )
    );

  return
    (_metaInformation.getLevel() == -1) ||
    (
      _metaInformation.getLevel() == level &&
      tarch::la::equals(_metaInformation.getPosition(), position, tolerance)
    );
  #else
  return true;
  #endif
}


void peano::heap::SendReceiveTask<double>::setInvalid() {
   #if defined(Asserts)
  _metaInformation.setLevel(-1);
  #endif
  _metaInformation.setLength(0);
  _data = nullptr;
}


void peano::heap::SendReceiveTask<double>::freeMemory() {
  if (_freeDataPointer && _metaInformation.getLength()>0) {
    delete[] _data;
  }
}



void peano::heap::SendReceiveTask<double>::sendDataDirectlyFromBuffer(const double* const data) {
  assertion( _metaInformation.getLength()>0 );
  assertion( _data==0 );

  _freeDataPointer = false;
  _data            = const_cast< double* >( data );
}


void peano::heap::SendReceiveTask<double>::wrapData(const double* const data) {
  assertion( _metaInformation.getLength()>0 );
  assertion( _data==0 );

  _freeDataPointer = true;

  _data = new (std::nothrow) double[ _metaInformation.getLength() ];
  if (_data==nullptr) {
    logError( "wrapData(std::vector<double>)", "memory allocation for wrapped data failed. Terminate" );
    exit(-1);
  }
  for (int i=0; i<_metaInformation.getLength(); i++) {
    _data[i] = data[i];
  }
}


void peano::heap::SendReceiveTask<double>::triggerSend(int tag) {
  assertion( _data!=nullptr );
  assertion( _metaInformation.getLength()>0 );

  #ifdef Parallel
  const int result = MPI_Isend(
    _data, _metaInformation.getLength(), MPI_DOUBLE, _rank,
    tag,
    tarch::parallel::Node::getInstance().getCommunicator(), &_request
  );

  if ( result != MPI_SUCCESS ) {
    logError(
      "triggerSend(int)", "failed to send heap data to node "
      << _rank << ": " << tarch::parallel::MPIReturnValueToString(result)
    );
  }
  #else
  assertionMsg( false, "should not be called if compiled without -DParallel" );
  #endif
}


void peano::heap::SendReceiveTask<double>::triggerReceive(int tag) {
  assertion( _rank >= 0 );
  assertion( _data==0 );

  #ifdef Parallel
  logTraceInWith2Arguments( "triggerReceive(int)", tag, _metaInformation.toString() );
  _data = new (std::nothrow) double[ _metaInformation.getLength() ];
  if (_data==nullptr) {
    logError( "triggerReceive(int)", "memory allocation failed. Terminate" );
    exit(-1);
  }
  const int  result = MPI_Irecv(
    _data, _metaInformation.getLength(), MPI_DOUBLE,
    _rank, tag, tarch::parallel::Node::getInstance().getCommunicator(),
    &_request
  );
  if ( result != MPI_SUCCESS ) {
    logError(
      "triggerReceive()",
      "failed to receive heap data from node "
      << _rank << ": " << tarch::parallel::MPIReturnValueToString(result)
    );
  }
  logTraceOut( "triggerReceive(int)" );
  #else
  assertionMsg( false, "should not be called if compiled without -DParallel" );
  #endif
}


std::string peano::heap::SendReceiveTask<double>::toString() const {
  std::ostringstream out;
  out << "(" << _metaInformation.toString() << ",rank=" << _rank << ",data=" << (_data==nullptr ? "no" : "yes") << ")";
  return out.str();
}




#ifdef Asserts
peano::heap::SendReceiveTask<char>::SendReceiveTask():
  _rank(-1),
  _data(0) {
}
#endif


bool peano::heap::SendReceiveTask<char>::fits(
  const tarch::la::Vector<DIMENSIONS, double>&  position,
  int                                           level
) const {
  #ifdef Asserts
  const double tolerance =
    tarch::la::NUMERICAL_ZERO_DIFFERENCE *
    std::max(
      1.0, std::max(
        tarch::la::maxAbs(_metaInformation.getPosition()), tarch::la::maxAbs(position)
      )
    );

  return
    (_metaInformation.getLevel() == -1) ||
    (
      _metaInformation.getLevel() == level &&
      tarch::la::equals(_metaInformation.getPosition(), position, tolerance)
    );
  #else
  return true;
  #endif
}


void peano::heap::SendReceiveTask<char>::setInvalid() {
   #if defined(Asserts)
  _metaInformation.setLevel(-1);
  #endif
  _metaInformation.setLength(0);
  _data = nullptr;
}


void peano::heap::SendReceiveTask<char>::freeMemory() {
  if (_freeDataPointer && _metaInformation.getLength()>0) {
    delete[] _data;
  }
}



void peano::heap::SendReceiveTask<char>::sendDataDirectlyFromBuffer(const char* const data) {
  assertion( _metaInformation.getLength()>0 );
  assertion( _data==0 );

  _freeDataPointer = false;
  _data            = const_cast< char* >( data );
}


void peano::heap::SendReceiveTask<char>::wrapData(const char* const data) {
  assertion( _metaInformation.getLength()>0 );
  assertion( _data==0 );

  _freeDataPointer = true;

  _data = new (std::nothrow) char[ _metaInformation.getLength() ];
  if (_data==nullptr) {
    logError( "wrapData(std::vector<char>)", "memory allocation for wrapped data failed. Terminate" );
    exit(-1);
  }
  for (int i=0; i<_metaInformation.getLength(); i++) {
    _data[i] = data[i];
  }
}


void peano::heap::SendReceiveTask<char>::triggerSend(int tag) {
  assertion( _data!=nullptr );
  assertion( _metaInformation.getLength()>0 );

  #ifdef Parallel
  const int result = MPI_Isend(
    _data, _metaInformation.getLength(), MPI_CHAR, _rank,
    tag,
    tarch::parallel::Node::getInstance().getCommunicator(), &_request
  );

  if ( result != MPI_SUCCESS ) {
    logError(
      "triggerSend(int)", "failed to send heap data to node "
      << _rank << ": " << tarch::parallel::MPIReturnValueToString(result)
    );
  }
  #else
  assertionMsg( false, "should not be called if compiled without -DParallel" );
  #endif
}


void peano::heap::SendReceiveTask<char>::triggerReceive(int tag) {
  assertion( _rank >= 0 );
  assertion( _data==0 );

  #ifdef Parallel
  logTraceInWith2Arguments( "triggerReceive(int)", tag, _metaInformation.toString() );
  _data = new (std::nothrow) char[ _metaInformation.getLength() ];
  if (_data==nullptr) {
    logError( "triggerReceive(int)", "memory allocation failed. Terminate" );
    exit(-1);
  }
  const int  result = MPI_Irecv(
    _data, _metaInformation.getLength(), MPI_CHAR,
    _rank, tag, tarch::parallel::Node::getInstance().getCommunicator(),
    &_request
  );
  if ( result != MPI_SUCCESS ) {
    logError(
      "triggerReceive()",
      "failed to receive heap data from node "
      << _rank << ": " << tarch::parallel::MPIReturnValueToString(result)
    );
  }
  logTraceOut( "triggerReceive(int)" );
  #else
  assertionMsg( false, "should not be called if compiled without -DParallel" );
  #endif
}


std::string peano::heap::SendReceiveTask<char>::toString() const {
  std::ostringstream out;
  out << "(" << _metaInformation.toString() << ",rank=" << _rank << ",data=" << (_data==nullptr ? "no" : "yes") << ")";
  return out.str();
}
