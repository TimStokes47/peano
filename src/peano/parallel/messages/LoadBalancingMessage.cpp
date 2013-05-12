#include "peano/parallel/messages/LoadBalancingMessage.h"

peano::parallel::messages::LoadBalancingMessage::PersistentRecords::PersistentRecords() {
   
}


peano::parallel::messages::LoadBalancingMessage::PersistentRecords::PersistentRecords(const int& loadBalancingFlag, const bool& couldNotEraseDueToDecomposition):
_loadBalancingFlag(loadBalancingFlag),
_couldNotEraseDueToDecomposition(couldNotEraseDueToDecomposition) {
   
}


 int peano::parallel::messages::LoadBalancingMessage::PersistentRecords::getLoadBalancingFlag() const  {
   return _loadBalancingFlag;
}



 void peano::parallel::messages::LoadBalancingMessage::PersistentRecords::setLoadBalancingFlag(const int& loadBalancingFlag)  {
   _loadBalancingFlag = loadBalancingFlag;
}



 bool peano::parallel::messages::LoadBalancingMessage::PersistentRecords::getCouldNotEraseDueToDecomposition() const  {
   return _couldNotEraseDueToDecomposition;
}



 void peano::parallel::messages::LoadBalancingMessage::PersistentRecords::setCouldNotEraseDueToDecomposition(const bool& couldNotEraseDueToDecomposition)  {
   _couldNotEraseDueToDecomposition = couldNotEraseDueToDecomposition;
}


peano::parallel::messages::LoadBalancingMessage::LoadBalancingMessage() {
   
}


peano::parallel::messages::LoadBalancingMessage::LoadBalancingMessage(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._loadBalancingFlag, persistentRecords._couldNotEraseDueToDecomposition) {
   
}


peano::parallel::messages::LoadBalancingMessage::LoadBalancingMessage(const int& loadBalancingFlag, const bool& couldNotEraseDueToDecomposition):
_persistentRecords(loadBalancingFlag, couldNotEraseDueToDecomposition) {
   
}


peano::parallel::messages::LoadBalancingMessage::~LoadBalancingMessage() { }


 int peano::parallel::messages::LoadBalancingMessage::getLoadBalancingFlag() const  {
   return _persistentRecords._loadBalancingFlag;
}



 void peano::parallel::messages::LoadBalancingMessage::setLoadBalancingFlag(const int& loadBalancingFlag)  {
   _persistentRecords._loadBalancingFlag = loadBalancingFlag;
}



 bool peano::parallel::messages::LoadBalancingMessage::getCouldNotEraseDueToDecomposition() const  {
   return _persistentRecords._couldNotEraseDueToDecomposition;
}



 void peano::parallel::messages::LoadBalancingMessage::setCouldNotEraseDueToDecomposition(const bool& couldNotEraseDueToDecomposition)  {
   _persistentRecords._couldNotEraseDueToDecomposition = couldNotEraseDueToDecomposition;
}




std::string peano::parallel::messages::LoadBalancingMessage::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano::parallel::messages::LoadBalancingMessage::toString (std::ostream& out) const {
   out << "("; 
   out << "loadBalancingFlag:" << getLoadBalancingFlag();
   out << ",";
   out << "couldNotEraseDueToDecomposition:" << getCouldNotEraseDueToDecomposition();
   out <<  ")";
}


peano::parallel::messages::LoadBalancingMessage::PersistentRecords peano::parallel::messages::LoadBalancingMessage::getPersistentRecords() const {
   return _persistentRecords;
}

peano::parallel::messages::LoadBalancingMessagePacked peano::parallel::messages::LoadBalancingMessage::convert() const{
   return LoadBalancingMessagePacked(
      getLoadBalancingFlag(),
      getCouldNotEraseDueToDecomposition()
   );
}

#ifdef Parallel
   tarch::logging::Log peano::parallel::messages::LoadBalancingMessage::_log( "peano::parallel::messages::LoadBalancingMessage" );
   
   MPI_Datatype peano::parallel::messages::LoadBalancingMessage::Datatype = 0;
   MPI_Datatype peano::parallel::messages::LoadBalancingMessage::FullDatatype = 0;
   
   
   void peano::parallel::messages::LoadBalancingMessage::initDatatype() {
      {
         LoadBalancingMessage dummyLoadBalancingMessage[2];
         
         const int Attributes = 3;
         MPI_Datatype subtypes[Attributes] = {
            MPI_INT,		 //loadBalancingFlag
            MPI_CHAR,		 //couldNotEraseDueToDecomposition
            MPI_UB		 // end/displacement flag
         };
         
         int blocklen[Attributes] = {
            1,		 //loadBalancingFlag
            1,		 //couldNotEraseDueToDecomposition
            1		 // end/displacement flag
         };
         
         MPI_Aint     disp[Attributes];
         
         MPI_Aint base;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[0]))), &base);
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[0]._persistentRecords._loadBalancingFlag))), 		&disp[0] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[0]._persistentRecords._couldNotEraseDueToDecomposition))), 		&disp[1] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[1]._persistentRecords._loadBalancingFlag))), 		&disp[2] );
         
         for (int i=1; i<Attributes; i++) {
            assertion1( disp[i] > disp[i-1], i );
         }
         for (int i=0; i<Attributes; i++) {
            disp[i] -= base;
         }
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &LoadBalancingMessage::Datatype );
         MPI_Type_commit( &LoadBalancingMessage::Datatype );
         
      }
      {
         LoadBalancingMessage dummyLoadBalancingMessage[2];
         
         const int Attributes = 3;
         MPI_Datatype subtypes[Attributes] = {
            MPI_INT,		 //loadBalancingFlag
            MPI_CHAR,		 //couldNotEraseDueToDecomposition
            MPI_UB		 // end/displacement flag
         };
         
         int blocklen[Attributes] = {
            1,		 //loadBalancingFlag
            1,		 //couldNotEraseDueToDecomposition
            1		 // end/displacement flag
         };
         
         MPI_Aint     disp[Attributes];
         
         MPI_Aint base;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[0]))), &base);
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[0]._persistentRecords._loadBalancingFlag))), 		&disp[0] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[0]._persistentRecords._couldNotEraseDueToDecomposition))), 		&disp[1] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessage[1]._persistentRecords._loadBalancingFlag))), 		&disp[2] );
         
         for (int i=1; i<Attributes; i++) {
            assertion1( disp[i] > disp[i-1], i );
         }
         for (int i=0; i<Attributes; i++) {
            disp[i] -= base;
         }
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &LoadBalancingMessage::FullDatatype );
         MPI_Type_commit( &LoadBalancingMessage::FullDatatype );
         
      }
      
   }
   
   
   void peano::parallel::messages::LoadBalancingMessage::shutdownDatatype() {
      MPI_Type_free( &LoadBalancingMessage::Datatype );
      MPI_Type_free( &LoadBalancingMessage::FullDatatype );
      
   }
   
   void peano::parallel::messages::LoadBalancingMessage::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Request* sendRequestHandle = new MPI_Request();
      MPI_Status   status;
      int          flag = 0;
      int          result;
      
      clock_t      timeOutWarning   = -1;
      clock_t      timeOutShutdown  = -1;
      bool         triggeredTimeoutWarning = false;
      
      _senderDestinationRank = destination;
      
      if (exchangeOnlyAttributesMarkedWithParallelise) {
         result = MPI_Isend(
            this, 1, Datatype, destination,
            tag, tarch::parallel::Node::getInstance().getCommunicator(),
            sendRequestHandle
         );
         
      }
      else {
         result = MPI_Isend(
            this, 1, FullDatatype, destination,
            tag, tarch::parallel::Node::getInstance().getCommunicator(),
            sendRequestHandle
         );
         
      }
      if  (result!=MPI_SUCCESS) {
         std::ostringstream msg;
         msg << "was not able to send message peano::parallel::messages::LoadBalancingMessage "
         << toString()
         << " to node " << destination
         << ": " << tarch::parallel::MPIReturnValueToString(result);
         _log.error( "send(int)",msg.str() );
      }
      result = MPI_Test( sendRequestHandle, &flag, &status );
      while (!flag) {
         if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp();
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp();
         result = MPI_Test( sendRequestHandle, &flag, &status );
         if (result!=MPI_SUCCESS) {
            std::ostringstream msg;
            msg << "testing for finished send task for peano::parallel::messages::LoadBalancingMessage "
            << toString()
            << " sent to node " << destination
            << " failed: " << tarch::parallel::MPIReturnValueToString(result);
            _log.error("send(int)", msg.str() );
         }
         
         // deadlock aspect
         if (
            tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() &&
            (clock()>timeOutWarning) &&
            (!triggeredTimeoutWarning)
         ) {
            tarch::parallel::Node::getInstance().writeTimeOutWarning(
            "peano::parallel::messages::LoadBalancingMessage",
            "send(int)", destination,tag,1
            );
            triggeredTimeoutWarning = true;
         }
         if (
            tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() &&
            (clock()>timeOutShutdown)
         ) {
            tarch::parallel::Node::getInstance().triggerDeadlockTimeOut(
            "peano::parallel::messages::LoadBalancingMessage",
            "send(int)", destination,tag,1
            );
         }
         tarch::parallel::Node::getInstance().receiveDanglingMessages();
      }
      
      delete sendRequestHandle;
      #ifdef Debug
      _log.debug("send(int,int)", "sent " + toString() );
      #endif
      
   }
   
   
   
   void peano::parallel::messages::LoadBalancingMessage::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Request* sendRequestHandle = new MPI_Request();
      MPI_Status   status;
      int          flag = 0;
      int          result;
      
      clock_t      timeOutWarning   = -1;
      clock_t      timeOutShutdown  = -1;
      bool         triggeredTimeoutWarning = false;
      
      if (exchangeOnlyAttributesMarkedWithParallelise) {
         result = MPI_Irecv(
            this, 1, Datatype, source, tag,
            tarch::parallel::Node::getInstance().getCommunicator(), sendRequestHandle
         );
         
      }
      else {
         result = MPI_Irecv(
            this, 1, FullDatatype, source, tag,
            tarch::parallel::Node::getInstance().getCommunicator(), sendRequestHandle
         );
         
      }
      if ( result != MPI_SUCCESS ) {
         std::ostringstream msg;
         msg << "failed to start to receive peano::parallel::messages::LoadBalancingMessage from node "
         << source << ": " << tarch::parallel::MPIReturnValueToString(result);
         _log.error( "receive(int)", msg.str() );
      }
      
      result = MPI_Test( sendRequestHandle, &flag, &status );
      while (!flag) {
         if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp();
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp();
         result = MPI_Test( sendRequestHandle, &flag, &status );
         if (result!=MPI_SUCCESS) {
            std::ostringstream msg;
            msg << "testing for finished receive task for peano::parallel::messages::LoadBalancingMessage failed: "
            << tarch::parallel::MPIReturnValueToString(result);
            _log.error("receive(int)", msg.str() );
         }
         
         // deadlock aspect
         if (
            tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() &&
            (clock()>timeOutWarning) &&
            (!triggeredTimeoutWarning)
         ) {
            tarch::parallel::Node::getInstance().writeTimeOutWarning(
            "peano::parallel::messages::LoadBalancingMessage",
            "receive(int)", source,tag,1
            );
            triggeredTimeoutWarning = true;
         }
         if (
            tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() &&
            (clock()>timeOutShutdown)
         ) {
            tarch::parallel::Node::getInstance().triggerDeadlockTimeOut(
            "peano::parallel::messages::LoadBalancingMessage",
            "receive(int)", source,tag,1
            );
         }
         tarch::parallel::Node::getInstance().receiveDanglingMessages();
      }
      
      delete sendRequestHandle;
      
      _senderDestinationRank = status.MPI_SOURCE;
      #ifdef Debug
      _log.debug("receive(int,int)", "received " + toString() ); 
      #endif
      
   }
   
   
   
   bool peano::parallel::messages::LoadBalancingMessage::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Status status;
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::parallel::Node::getInstance().getCommunicator(), &flag, &status
      );
      if (flag) {
         int  messageCounter;
         if (exchangeOnlyAttributesMarkedWithParallelise) {
            MPI_Get_count(&status, Datatype, &messageCounter);
         }
         else {
            MPI_Get_count(&status, FullDatatype, &messageCounter);
         }
         return messageCounter > 0;
      }
      else return false;
      
   }
   
   int peano::parallel::messages::LoadBalancingMessage::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords::PersistentRecords() {
   
}


peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords::PersistentRecords(const int& loadBalancingFlag, const bool& couldNotEraseDueToDecomposition):
_loadBalancingFlag(loadBalancingFlag),
_couldNotEraseDueToDecomposition(couldNotEraseDueToDecomposition) {
   
}


 int peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords::getLoadBalancingFlag() const  {
   return _loadBalancingFlag;
}



 void peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords::setLoadBalancingFlag(const int& loadBalancingFlag)  {
   _loadBalancingFlag = loadBalancingFlag;
}



 bool peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords::getCouldNotEraseDueToDecomposition() const  {
   return _couldNotEraseDueToDecomposition;
}



 void peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords::setCouldNotEraseDueToDecomposition(const bool& couldNotEraseDueToDecomposition)  {
   _couldNotEraseDueToDecomposition = couldNotEraseDueToDecomposition;
}


peano::parallel::messages::LoadBalancingMessagePacked::LoadBalancingMessagePacked() {
   
}


peano::parallel::messages::LoadBalancingMessagePacked::LoadBalancingMessagePacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._loadBalancingFlag, persistentRecords._couldNotEraseDueToDecomposition) {
   
}


peano::parallel::messages::LoadBalancingMessagePacked::LoadBalancingMessagePacked(const int& loadBalancingFlag, const bool& couldNotEraseDueToDecomposition):
_persistentRecords(loadBalancingFlag, couldNotEraseDueToDecomposition) {
   
}


peano::parallel::messages::LoadBalancingMessagePacked::~LoadBalancingMessagePacked() { }


 int peano::parallel::messages::LoadBalancingMessagePacked::getLoadBalancingFlag() const  {
   return _persistentRecords._loadBalancingFlag;
}



 void peano::parallel::messages::LoadBalancingMessagePacked::setLoadBalancingFlag(const int& loadBalancingFlag)  {
   _persistentRecords._loadBalancingFlag = loadBalancingFlag;
}



 bool peano::parallel::messages::LoadBalancingMessagePacked::getCouldNotEraseDueToDecomposition() const  {
   return _persistentRecords._couldNotEraseDueToDecomposition;
}



 void peano::parallel::messages::LoadBalancingMessagePacked::setCouldNotEraseDueToDecomposition(const bool& couldNotEraseDueToDecomposition)  {
   _persistentRecords._couldNotEraseDueToDecomposition = couldNotEraseDueToDecomposition;
}




std::string peano::parallel::messages::LoadBalancingMessagePacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano::parallel::messages::LoadBalancingMessagePacked::toString (std::ostream& out) const {
   out << "("; 
   out << "loadBalancingFlag:" << getLoadBalancingFlag();
   out << ",";
   out << "couldNotEraseDueToDecomposition:" << getCouldNotEraseDueToDecomposition();
   out <<  ")";
}


peano::parallel::messages::LoadBalancingMessagePacked::PersistentRecords peano::parallel::messages::LoadBalancingMessagePacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano::parallel::messages::LoadBalancingMessage peano::parallel::messages::LoadBalancingMessagePacked::convert() const{
   return LoadBalancingMessage(
      getLoadBalancingFlag(),
      getCouldNotEraseDueToDecomposition()
   );
}

#ifdef Parallel
   tarch::logging::Log peano::parallel::messages::LoadBalancingMessagePacked::_log( "peano::parallel::messages::LoadBalancingMessagePacked" );
   
   MPI_Datatype peano::parallel::messages::LoadBalancingMessagePacked::Datatype = 0;
   MPI_Datatype peano::parallel::messages::LoadBalancingMessagePacked::FullDatatype = 0;
   
   
   void peano::parallel::messages::LoadBalancingMessagePacked::initDatatype() {
      {
         LoadBalancingMessagePacked dummyLoadBalancingMessagePacked[2];
         
         const int Attributes = 3;
         MPI_Datatype subtypes[Attributes] = {
            MPI_INT,		 //loadBalancingFlag
            MPI_CHAR,		 //couldNotEraseDueToDecomposition
            MPI_UB		 // end/displacement flag
         };
         
         int blocklen[Attributes] = {
            1,		 //loadBalancingFlag
            1,		 //couldNotEraseDueToDecomposition
            1		 // end/displacement flag
         };
         
         MPI_Aint     disp[Attributes];
         
         MPI_Aint base;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[0]))), &base);
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[0]._persistentRecords._loadBalancingFlag))), 		&disp[0] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[0]._persistentRecords._couldNotEraseDueToDecomposition))), 		&disp[1] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[1]._persistentRecords._loadBalancingFlag))), 		&disp[2] );
         
         for (int i=1; i<Attributes; i++) {
            assertion1( disp[i] > disp[i-1], i );
         }
         for (int i=0; i<Attributes; i++) {
            disp[i] -= base;
         }
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &LoadBalancingMessagePacked::Datatype );
         MPI_Type_commit( &LoadBalancingMessagePacked::Datatype );
         
      }
      {
         LoadBalancingMessagePacked dummyLoadBalancingMessagePacked[2];
         
         const int Attributes = 3;
         MPI_Datatype subtypes[Attributes] = {
            MPI_INT,		 //loadBalancingFlag
            MPI_CHAR,		 //couldNotEraseDueToDecomposition
            MPI_UB		 // end/displacement flag
         };
         
         int blocklen[Attributes] = {
            1,		 //loadBalancingFlag
            1,		 //couldNotEraseDueToDecomposition
            1		 // end/displacement flag
         };
         
         MPI_Aint     disp[Attributes];
         
         MPI_Aint base;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[0]))), &base);
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[0]._persistentRecords._loadBalancingFlag))), 		&disp[0] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[0]._persistentRecords._couldNotEraseDueToDecomposition))), 		&disp[1] );
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyLoadBalancingMessagePacked[1]._persistentRecords._loadBalancingFlag))), 		&disp[2] );
         
         for (int i=1; i<Attributes; i++) {
            assertion1( disp[i] > disp[i-1], i );
         }
         for (int i=0; i<Attributes; i++) {
            disp[i] -= base;
         }
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &LoadBalancingMessagePacked::FullDatatype );
         MPI_Type_commit( &LoadBalancingMessagePacked::FullDatatype );
         
      }
      
   }
   
   
   void peano::parallel::messages::LoadBalancingMessagePacked::shutdownDatatype() {
      MPI_Type_free( &LoadBalancingMessagePacked::Datatype );
      MPI_Type_free( &LoadBalancingMessagePacked::FullDatatype );
      
   }
   
   void peano::parallel::messages::LoadBalancingMessagePacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Request* sendRequestHandle = new MPI_Request();
      MPI_Status   status;
      int          flag = 0;
      int          result;
      
      clock_t      timeOutWarning   = -1;
      clock_t      timeOutShutdown  = -1;
      bool         triggeredTimeoutWarning = false;
      
      _senderDestinationRank = destination;
      
      if (exchangeOnlyAttributesMarkedWithParallelise) {
         result = MPI_Isend(
            this, 1, Datatype, destination,
            tag, tarch::parallel::Node::getInstance().getCommunicator(),
            sendRequestHandle
         );
         
      }
      else {
         result = MPI_Isend(
            this, 1, FullDatatype, destination,
            tag, tarch::parallel::Node::getInstance().getCommunicator(),
            sendRequestHandle
         );
         
      }
      if  (result!=MPI_SUCCESS) {
         std::ostringstream msg;
         msg << "was not able to send message peano::parallel::messages::LoadBalancingMessagePacked "
         << toString()
         << " to node " << destination
         << ": " << tarch::parallel::MPIReturnValueToString(result);
         _log.error( "send(int)",msg.str() );
      }
      result = MPI_Test( sendRequestHandle, &flag, &status );
      while (!flag) {
         if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp();
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp();
         result = MPI_Test( sendRequestHandle, &flag, &status );
         if (result!=MPI_SUCCESS) {
            std::ostringstream msg;
            msg << "testing for finished send task for peano::parallel::messages::LoadBalancingMessagePacked "
            << toString()
            << " sent to node " << destination
            << " failed: " << tarch::parallel::MPIReturnValueToString(result);
            _log.error("send(int)", msg.str() );
         }
         
         // deadlock aspect
         if (
            tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() &&
            (clock()>timeOutWarning) &&
            (!triggeredTimeoutWarning)
         ) {
            tarch::parallel::Node::getInstance().writeTimeOutWarning(
            "peano::parallel::messages::LoadBalancingMessagePacked",
            "send(int)", destination,tag,1
            );
            triggeredTimeoutWarning = true;
         }
         if (
            tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() &&
            (clock()>timeOutShutdown)
         ) {
            tarch::parallel::Node::getInstance().triggerDeadlockTimeOut(
            "peano::parallel::messages::LoadBalancingMessagePacked",
            "send(int)", destination,tag,1
            );
         }
         tarch::parallel::Node::getInstance().receiveDanglingMessages();
      }
      
      delete sendRequestHandle;
      #ifdef Debug
      _log.debug("send(int,int)", "sent " + toString() );
      #endif
      
   }
   
   
   
   void peano::parallel::messages::LoadBalancingMessagePacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Request* sendRequestHandle = new MPI_Request();
      MPI_Status   status;
      int          flag = 0;
      int          result;
      
      clock_t      timeOutWarning   = -1;
      clock_t      timeOutShutdown  = -1;
      bool         triggeredTimeoutWarning = false;
      
      if (exchangeOnlyAttributesMarkedWithParallelise) {
         result = MPI_Irecv(
            this, 1, Datatype, source, tag,
            tarch::parallel::Node::getInstance().getCommunicator(), sendRequestHandle
         );
         
      }
      else {
         result = MPI_Irecv(
            this, 1, FullDatatype, source, tag,
            tarch::parallel::Node::getInstance().getCommunicator(), sendRequestHandle
         );
         
      }
      if ( result != MPI_SUCCESS ) {
         std::ostringstream msg;
         msg << "failed to start to receive peano::parallel::messages::LoadBalancingMessagePacked from node "
         << source << ": " << tarch::parallel::MPIReturnValueToString(result);
         _log.error( "receive(int)", msg.str() );
      }
      
      result = MPI_Test( sendRequestHandle, &flag, &status );
      while (!flag) {
         if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp();
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp();
         result = MPI_Test( sendRequestHandle, &flag, &status );
         if (result!=MPI_SUCCESS) {
            std::ostringstream msg;
            msg << "testing for finished receive task for peano::parallel::messages::LoadBalancingMessagePacked failed: "
            << tarch::parallel::MPIReturnValueToString(result);
            _log.error("receive(int)", msg.str() );
         }
         
         // deadlock aspect
         if (
            tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() &&
            (clock()>timeOutWarning) &&
            (!triggeredTimeoutWarning)
         ) {
            tarch::parallel::Node::getInstance().writeTimeOutWarning(
            "peano::parallel::messages::LoadBalancingMessagePacked",
            "receive(int)", source,tag,1
            );
            triggeredTimeoutWarning = true;
         }
         if (
            tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() &&
            (clock()>timeOutShutdown)
         ) {
            tarch::parallel::Node::getInstance().triggerDeadlockTimeOut(
            "peano::parallel::messages::LoadBalancingMessagePacked",
            "receive(int)", source,tag,1
            );
         }
         tarch::parallel::Node::getInstance().receiveDanglingMessages();
      }
      
      delete sendRequestHandle;
      
      _senderDestinationRank = status.MPI_SOURCE;
      #ifdef Debug
      _log.debug("receive(int,int)", "received " + toString() ); 
      #endif
      
   }
   
   
   
   bool peano::parallel::messages::LoadBalancingMessagePacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Status status;
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::parallel::Node::getInstance().getCommunicator(), &flag, &status
      );
      if (flag) {
         int  messageCounter;
         if (exchangeOnlyAttributesMarkedWithParallelise) {
            MPI_Get_count(&status, Datatype, &messageCounter);
         }
         else {
            MPI_Get_count(&status, FullDatatype, &messageCounter);
         }
         return messageCounter > 0;
      }
      else return false;
      
   }
   
   int peano::parallel::messages::LoadBalancingMessagePacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



