#include "IntegerMessage.h"


#include <sstream>


std::string tarch::mpi::IntegerMessage::toString() const {
  std::ostringstream out;
  out << "(";
  out << _value;
  out << ")";
  return out.str();
}


int   tarch::mpi::IntegerMessage::getValue() const {
  return _value;
}


void   tarch::mpi::IntegerMessage::setValue(int value) {
  _value = value;
}



#ifdef Parallel
void tarch::mpi::IntegerMessage::sendAndPollDanglingMessages(const tarch::mpi::IntegerMessage& message, int destination, int tag ) {
  tarch::mpi::IntegerMessage::send(
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
        tarch::mpi::Rank::getInstance().writeTimeOutWarning( "tarch::mpi::IntegerMessage", "sendAndPollDanglingMessages()",destination, tag );
        triggeredTimeoutWarning = true;
      }
      if (
        tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
        (clock()>timeOutShutdown)
      ) {
        tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( "tarch::mpi::IntegerMessage", "sendAndPollDanglingMessages()", destination, tag );
      }
      tarch::mpi::Rank::getInstance().receiveDanglingMessages();
    },
    tarch::mpi::Rank::getInstance().getCommunicator()
  );
}


void tarch::mpi::IntegerMessage::receiveAndPollDanglingMessages(tarch::mpi::IntegerMessage& message, int source, int tag ) {
  tarch::mpi::IntegerMessage::receive(
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
        tarch::mpi::Rank::getInstance().writeTimeOutWarning( "tarch::mpi::IntegerMessage", "receiveAndPollDanglingMessages()", source, tag );
        triggeredTimeoutWarning = true;
      }
      if (
        tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() &&
        (clock()>timeOutShutdown)
      ) {
        tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( "tarch::mpi::IntegerMessage", "receiveAndPollDanglingMessages()", source, tag );
      }
      tarch::mpi::Rank::getInstance().receiveDanglingMessages();
    },
    tarch::mpi::Rank::getInstance().getCommunicator()
  );
}
#endif

    
#ifdef Parallel

MPI_Datatype tarch::mpi::IntegerMessage::Datatype;

int tarch::mpi::IntegerMessage::getSenderRank() const {
  return _senderDestinationRank;
}


void tarch::mpi::IntegerMessage::initDatatype() {
  tarch::mpi::IntegerMessage  instances[2];
    
  MPI_Datatype subtypes[] = { MPI_INT };
    
  int blocklen[] = { 1 };

  const int NumberOfAttributes = 1;
    
  MPI_Aint  baseFirstInstance;
  MPI_Aint  baseSecondInstance;
  MPI_Get_address( &instances[0], &baseFirstInstance );
  MPI_Get_address( &instances[1], &baseSecondInstance );
  MPI_Aint  disp[ NumberOfAttributes ];
  int       currentAddress = 0;
  MPI_Get_address( &(instances[0]._value), &disp[currentAddress] );
  currentAddress++;

  MPI_Aint offset = disp[0] - baseFirstInstance;
  MPI_Aint extent = baseSecondInstance - baseFirstInstance - offset;
  for (int i=0; i<1; i++) {
    disp[i] = disp[i] - disp[0];
  }

  int errorCode = 0; 
  MPI_Datatype tmpType; 
  errorCode += MPI_Type_create_struct( NumberOfAttributes, blocklen, disp, subtypes, &tmpType );
  errorCode += MPI_Type_create_resized( tmpType, offset, extent, &Datatype );
  errorCode += MPI_Type_commit( &Datatype );
  if (errorCode) std::cerr << "error committing MPI datatype in " << __FILE__ << ":" << __LINE__ << std::endl;
}


void tarch::mpi::IntegerMessage::shutdownDatatype() {
  MPI_Type_free( &Datatype );
}


void tarch::mpi::IntegerMessage::send(const tarch::mpi::IntegerMessage& buffer, int destination, int tag, MPI_Comm communicator ) {
  MPI_Send( &buffer, 1, Datatype, destination, tag, communicator);
}


void tarch::mpi::IntegerMessage::receive(tarch::mpi::IntegerMessage& buffer, int source, int tag, MPI_Comm communicator ) {
  MPI_Status status;
  MPI_Recv( &buffer, 1, Datatype, source, tag, communicator, &status);
  buffer._senderDestinationRank = status.MPI_SOURCE;
}


void tarch::mpi::IntegerMessage::send(const tarch::mpi::IntegerMessage& buffer, int destination, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {
  MPI_Request sendRequestHandle; 
  int         flag = 0; 
  MPI_Isend( &buffer, 1, Datatype, destination, tag, communicator, &sendRequestHandle ); 
  MPI_Test( &sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  while (!flag) { 
    waitFunctor();
    MPI_Test( &sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
  }
}


void tarch::mpi::IntegerMessage::receive(tarch::mpi::IntegerMessage& buffer, int source, int tag, std::function<void()> waitFunctor, MPI_Comm communicator ) {   
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


bool tarch::mpi::IntegerMessage::isMessageInQueue(int tag, MPI_Comm communicator) {
  int  flag        = 0;
  MPI_Iprobe(
    MPI_ANY_SOURCE, tag,
    communicator, &flag, MPI_STATUS_IGNORE
  );
  return flag;
}

#endif
