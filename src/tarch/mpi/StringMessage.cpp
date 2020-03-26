#include "StringMessage.h"



#include <sstream>
#include <algorithm>



std::string tarch::mpi::StringMessage::toString() const {
  std::ostringstream out;
  out << "(";
  out << getData();
  out << ")";
  return out.str();
}


std::string   tarch::mpi::StringMessage::getData() const {

  std::ostringstream result;
  for (int i=0; i<_dataLength; i++) {
    result << static_cast<char>(_data[i]);
  }
  return result.str();
}


void   tarch::mpi::StringMessage::setData(const std::string& value) {

  _dataLength = value.length();
  for (int i=0; i<value.length(); i++) {
    _data[i] = value.data()[i];
  }
    
}



#ifdef Parallel
void tarch::mpi::StringMessage::sendAndPollDanglingMessages(const tarch::mpi::StringMessage& message, int destination, int tag ) {
  tarch::mpi::StringMessage::send(
    message, destination, tag,
    [&]() {
      int  timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp();
      int  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp();
      bool triggeredTimeoutWarning = false;
      if (
        tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() &&
        (clock()>timeOutWarning) &&
        (!triggeredTimeoutWarning)
      ) {
        tarch::mpi::Rank::getInstance().writeTimeOutWarning( "tarch::mpi::StringMessage", "sendAndPollDanglingMessages()",destination, tag );
        triggeredTimeoutWarning = true;
      }
      if (
        tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
        (clock()>timeOutShutdown)
      ) {
        tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( "tarch::mpi::StringMessage", "sendAndPollDanglingMessages()", destination, tag );
      }
      tarch::mpi::Rank::getInstance().receiveDanglingMessages();
    },
    tarch::mpi::Rank::getInstance().getCommunicator()
  );
}


void tarch::mpi::StringMessage::receiveAndPollDanglingMessages(tarch::mpi::StringMessage& message, int source, int tag ) {
  tarch::mpi::StringMessage::receive(
    message, source, tag,
    [&]() {
      int  timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp();
      int  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp();
      bool triggeredTimeoutWarning = false;
      if (
        tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() &&
        (clock()>timeOutWarning) &&
        (!triggeredTimeoutWarning)
      ) {
        tarch::mpi::Rank::getInstance().writeTimeOutWarning( "tarch::mpi::StringMessage", "receiveAndPollDanglingMessages()", source, tag );
        triggeredTimeoutWarning = true;
      }
      if (
        tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
        (clock()>timeOutShutdown)
      ) {
        tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( "tarch::mpi::StringMessage", "receiveAndPollDanglingMessages()", source, tag );
      }
      tarch::mpi::Rank::getInstance().receiveDanglingMessages();
    },
    tarch::mpi::Rank::getInstance().getCommunicator()
  );
}
#endif

    
#ifdef Parallel

MPI_Datatype tarch::mpi::StringMessage::Datatype;

int tarch::mpi::StringMessage::getSenderRank() const {
  return _senderDestinationRank;
}


void tarch::mpi::StringMessage::initDatatype() {
  tarch::mpi::StringMessage  instances[2];
    
  MPI_Datatype subtypes[] = { MPI_CHAR, MPI_INT };
    
  int blocklen[] = { 80, 1 };

  const int NumberOfAttributes = 2;
    
  MPI_Aint  baseFirstInstance;
  MPI_Aint  baseSecondInstance;
  MPI_Get_address( &instances[0], &baseFirstInstance );
  MPI_Get_address( &instances[1], &baseSecondInstance );
  MPI_Aint  disp[ NumberOfAttributes ];
  int       currentAddress = 0;
  MPI_Get_address( &(instances[0]._data[80]), &disp[currentAddress] );
  currentAddress++;
  MPI_Get_address( &(instances[0]._dataLength), &disp[currentAddress] );
  currentAddress++;

  MPI_Aint offset = disp[0] - baseFirstInstance;
  MPI_Aint extent = baseSecondInstance - baseFirstInstance - offset;
  for (int i=1-1; i>=0; i--) {
    disp[i] = disp[i] - disp[0];
  }

  int errorCode = 0; 
  MPI_Datatype tmpType; 
  errorCode += MPI_Type_create_struct( NumberOfAttributes, blocklen, disp, subtypes, &tmpType );
  errorCode += MPI_Type_create_resized( tmpType, offset, extent, &Datatype );
  errorCode += MPI_Type_commit( &Datatype );
  if (errorCode) std::cerr << "error committing MPI datatype in " << __FILE__ << ":" << __LINE__ << std::endl;
}


void tarch::mpi::StringMessage::shutdownDatatype() {
  MPI_Type_free( &Datatype );
}


void tarch::mpi::StringMessage::send(const tarch::mpi::StringMessage& buffer, int destination, int tag, MPI_Comm communicator ) {
  MPI_Send( &buffer, 1, Datatype, destination, tag, communicator);
}


void tarch::mpi::StringMessage::receive(tarch::mpi::StringMessage& buffer, int source, int tag, MPI_Comm communicator ) {
  MPI_Status status;
  MPI_Recv( &buffer, 1, Datatype, source, tag, communicator, &status);
  buffer._senderDestinationRank = status.MPI_SOURCE;
}


void tarch::mpi::StringMessage::send(const tarch::mpi::StringMessage& buffer, int destination, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {
  MPI_Request sendRequestHandle; 
  int         flag = 0; 
  MPI_Isend( &buffer, 1, Datatype, destination, tag, communicator, &sendRequestHandle ); 
  MPI_Test( &sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( &sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  }
}


void tarch::mpi::StringMessage::receive(tarch::mpi::StringMessage& buffer, int source, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {   
  MPI_Status  status;
  MPI_Request receiveRequestHandle; 
  int         flag = 0; 
  MPI_Irecv( &buffer, 1, Datatype, source, tag, communicator, &receiveRequestHandle ); 
  MPI_Test( &receiveRequestHandle, &flag, &status ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( &receiveRequestHandle, &flag, &status ); 
  }
  buffer._senderDestinationRank = status.MPI_SOURCE;
}


bool tarch::mpi::StringMessage::isMessageInQueue(int tag, MPI_Comm communicator) {
  int  flag        = 0;
  MPI_Iprobe(
    MPI_ANY_SOURCE, tag,
    communicator, &flag, MPI_STATUS_IGNORE
  );
  return flag;
}

#endif
