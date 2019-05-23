#include "STDVectorStack.h"


tarch::logging::Log peano4::stacks::STDVectorStack<double>::_log( "peano4::stacks::STDVectorStack<double>" );


peano4::stacks::STDVectorStack<double>::STDVectorStack():
  _data(),
  _currentElement(0),
  _ioMode(IOMode::None),
  _ioMPIRequest(nullptr) {
}


peano4::stacks::STDVectorStack<double>::~STDVectorStack() {
}


peano4::stacks::STDVectorStack<double>::STDVectorStack( const STDVectorStack<double>& stack ):
  _data(),
  _currentElement(0),
  _ioMode(IOMode::None),
  _ioMPIRequest(nullptr) {
  assertionMsg( stack._currentElement==0, "may not copy non-empty stack" );
  assertionMsg( stack._ioMPIRequest==nullptr, "may not copy sending/receiving stack" );
}


peano4::stacks::STDVectorStack<double>&  peano4::stacks::STDVectorStack<double>::operator=( const peano4::stacks::STDVectorStack<double>& stack ) {
  assertionEquals( stack._currentElement, 0 );
  assertionEquals( _currentElement, 0 );
  assertion( _data.empty() );
  return *this;
}


void peano4::stacks::STDVectorStack<double>::clone( const STDVectorStack<double>&  data ) {
  assertionEquals( _currentElement, 0 );
  _currentElement = data._currentElement;
  _data           = data._data;
}


int peano4::stacks::STDVectorStack<double>::PopBlockVertexStackView::size() const {
  return _size;
}


peano4::stacks::STDVectorStack<double>::PopBlockVertexStackView::PopBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<double>* stack):
  _size(size),
  _baseElement(base),
  _stack(stack) {
}


double peano4::stacks::STDVectorStack<double>::PopBlockVertexStackView::get(int index) {
  assertion2( index>=0, index, _size );
  assertion2( index<_size, index, _size );
  return _stack->_data[_baseElement+index];
}


std::string peano4::stacks::STDVectorStack<double>::PopBlockVertexStackView::toString() const {
  std::ostringstream msg;
  msg << "(size=" << _size
      << ",baseElement=" << _baseElement
      << ")";
  return msg.str();
}



peano4::stacks::STDVectorStack<double>::PushBlockVertexStackView::PushBlockVertexStackView(int size, int base, peano4::stacks::STDVectorStack<double>* stack):
  _size(size),
  _baseElement(base),
  _stack(stack) {
}


int peano4::stacks::STDVectorStack<double>::PushBlockVertexStackView::size() const {
  return _size;
}


double* peano4::stacks::STDVectorStack<double>::PushBlockVertexStackView::set(int index, const double& value) {
  assertion2( index>=0, index, _size );
  assertion2( index<_size, index, _size );
  _stack->_data[_baseElement+index] = value;
  return &(_stack->_data[_baseElement+index]);
}


#if PeanoDebug>=1
double peano4::stacks::STDVectorStack<double>::PushBlockVertexStackView::get(int index) const {
  assertion2( index>=0, index, _size );
  assertion2( index<_size, index, _size );
  return _stack->_data[_baseElement+index];
}
#endif


std::string peano4::stacks::STDVectorStack<double>::PushBlockVertexStackView::toString() const {
  std::ostringstream msg;
  msg << "(size=" << _size
      << ",baseElement=" << _baseElement
              << ")";
  return msg.str();
}


double peano4::stacks::STDVectorStack<double>::pop() {
  assertion(_currentElement>0);
  _currentElement--;
  return _data[_currentElement];
}


double& peano4::stacks::STDVectorStack<double>::top(int shift) {
  return _data[_currentElement-1-shift];
}


void peano4::stacks::STDVectorStack<double>::push( const double& element ) {
  if (_currentElement >= static_cast<int>(_data.size()) ) {
    assertion(_currentElement - static_cast<int>(_data.size()) <= 1 );
    _data.push_back(element);
  }
  else {
    _data[_currentElement] = element;
  }
  _currentElement++;
}


peano4::stacks::STDVectorStack<double>::PopBlockVertexStackView  peano4::stacks::STDVectorStack<double>::popBlock(int numberOfVertices) {
  _currentElement-=numberOfVertices;
  assertion( _currentElement>=0 );

  PopBlockVertexStackView result(numberOfVertices, _currentElement, this);
  return result;
}


peano4::stacks::STDVectorStack<double>::PushBlockVertexStackView  peano4::stacks::STDVectorStack<double>::pushBlock(int numberOfVertices) {
  PushBlockVertexStackView result(numberOfVertices, _currentElement, this);

  _currentElement+=numberOfVertices;

  if (static_cast<int>(_data.size())<_currentElement) {
  	_data.resize(_currentElement);
  }
  return result;
}


int peano4::stacks::STDVectorStack<double>::size() const {
  return _currentElement;
}


bool peano4::stacks::STDVectorStack<double>::empty() const {
  return _currentElement==0;
}


void peano4::stacks::STDVectorStack<double>::clear() {
  _currentElement = 0;
}


void peano4::stacks::STDVectorStack<double>::startSend(int rank, int tag) {
  #ifdef Parallel
  assertion( _ioMode==IOMode::None );
  _ioMode = IOMode::MPISend;
  _ioTag  = tag;
  _ioRank = rank;

  assertion( _ioMPIRequest == nullptr );
  _ioMPIRequest = new MPI_Request;
  int result = MPI_Isend( _data.data(), _data.size(), MPI_DOUBLE, _ioRank, _ioTag, tarch::mpi::Rank::getInstance().getCommunicator(), _ioMPIRequest);
  if  (result!=MPI_SUCCESS) {
    std::ostringstream msg;
    logError( "startSend(int,int)", "was not able to send to node " << rank << " on tag " << tag
      << ": " << tarch::mpi::MPIReturnValueToString(result)
    );
  }
  #endif
}


void peano4::stacks::STDVectorStack<double>::startReceive(int rank, int tag, int numberOfElements) {
  #ifdef Parallel
  assertion( _ioMode==IOMode::None );
  _ioMode = IOMode::MPIReceive;
  _ioTag  = tag;
  _ioRank = rank;

  _data.resize(numberOfElements);
  _currentElement = numberOfElements;

  assertion( _ioMPIRequest == nullptr );
  _ioMPIRequest = new MPI_Request;
  int result = MPI_Irecv( _data.data(), _data.size(), MPI_DOUBLE, _ioRank, _ioTag, tarch::mpi::Rank::getInstance().getCommunicator(), _ioMPIRequest);
  if  (result!=MPI_SUCCESS) {
    logError( "startReceive(int,int)", "was not able to receive " << numberOfElements << " values from node " << rank << " on tag " << tag
      << ": " << tarch::mpi::MPIReturnValueToString(result)
    );
  }
  #endif
}


bool peano4::stacks::STDVectorStack<double>::isSendingOrReceiving() const {
  return _ioMode==IOMode::MPISend or _ioMode==IOMode::MPIReceive;
}


void peano4::stacks::STDVectorStack<double>::finishSendOrReceive() {
  #ifdef Parallel
  assertion( _ioMode==IOMode::MPISend or _ioMode==IOMode::MPIReceive );
  assertion( _ioMPIRequest!=nullptr );

  _ioMode = IOMode::None;

  int          flag = 0;
  int          result;
  clock_t      timeOutWarning   = -1;
  clock_t      timeOutShutdown  = -1;
  bool         triggeredTimeoutWarning = false;
  result = MPI_Test( _ioMPIRequest, &flag, MPI_STATUS_IGNORE );
  while (!flag) {
    if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp();
    if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp();
    result = MPI_Test( _ioMPIRequest, &flag, MPI_STATUS_IGNORE );
    if (result!=MPI_SUCCESS) {
      logError("finishSendOrReceive()", "failed" );
    }
    if (
      tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() &&
      (clock()>timeOutWarning) &&
      (!triggeredTimeoutWarning)
    ) {
      tarch::mpi::Rank::getInstance().writeTimeOutWarning(
        "peano4::stacks::STDVectorStack<double>",
        "finishSendOrReceive()", _ioRank,_ioTag, _data.size()
       );
       triggeredTimeoutWarning = true;
     }
     if (
       tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
       (clock()>timeOutShutdown)
     ) {
       tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut(
         "peano4::stacks::STDVectorStack<double>",
         "finishSendOrReceive()", _ioRank,_ioTag, _data.size()
       );
     }
    tarch::mpi::Rank::getInstance().receiveDanglingMessages();
  }
  delete _ioMPIRequest;
  _ioMPIRequest = nullptr;
  #endif
}


void peano4::stacks::STDVectorStack<double>::reverse() {
  std::reverse(std::begin(_data), std::end(_data));
}


std::string peano4::stacks::STDVectorStack<double>::toString() const {
  std::ostringstream msg;
  msg << "(";
  msg << "size:" << size();
  #if PeanoDebug>0
  for (auto& p: _data) {
    msg << "," << p;
  }
  #endif
  msg << ")";
  return msg.str();
}
