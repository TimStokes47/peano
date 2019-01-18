#include "peano4/parallel/StartTraversalMessage.h"

peano4::parallel::StartTraversalMessage::PersistentRecords::PersistentRecords() {
   
}


peano4::parallel::StartTraversalMessage::PersistentRecords::PersistentRecords(const int& stepIdentifier):
_stepIdentifier(stepIdentifier) {
   
}


 int peano4::parallel::StartTraversalMessage::PersistentRecords::getStepIdentifier() const  {
   return _stepIdentifier;
}



 void peano4::parallel::StartTraversalMessage::PersistentRecords::setStepIdentifier(const int& stepIdentifier)  {
   _stepIdentifier = stepIdentifier;
}


peano4::parallel::StartTraversalMessage::StartTraversalMessage() {
   
}


peano4::parallel::StartTraversalMessage::StartTraversalMessage(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._stepIdentifier) {
   
}


peano4::parallel::StartTraversalMessage::StartTraversalMessage(const int& stepIdentifier):
_persistentRecords(stepIdentifier) {
   
}


peano4::parallel::StartTraversalMessage::~StartTraversalMessage() { }


 int peano4::parallel::StartTraversalMessage::getStepIdentifier() const  {
   return _persistentRecords._stepIdentifier;
}



 void peano4::parallel::StartTraversalMessage::setStepIdentifier(const int& stepIdentifier)  {
   _persistentRecords._stepIdentifier = stepIdentifier;
}




std::string peano4::parallel::StartTraversalMessage::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::parallel::StartTraversalMessage::toString (std::ostream& out) const {
   out << "("; 
   out << "stepIdentifier:" << getStepIdentifier();
   out <<  ")";
}


peano4::parallel::StartTraversalMessage::PersistentRecords peano4::parallel::StartTraversalMessage::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::parallel::StartTraversalMessagePacked peano4::parallel::StartTraversalMessage::convert() const{
   return StartTraversalMessagePacked(
      getStepIdentifier()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::parallel::StartTraversalMessage::_log( "peano4::parallel::StartTraversalMessage" );
   
   MPI_Datatype peano4::parallel::StartTraversalMessage::Datatype = 0;
   MPI_Datatype peano4::parallel::StartTraversalMessage::FullDatatype = 0;
   
   
   void peano4::parallel::StartTraversalMessage::initDatatype() {
      {
         StartTraversalMessage dummyStartTraversalMessage[2];
         
         #ifdef MPI2
         const int Attributes = 1;
         #else
         const int Attributes = 1+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
            MPI_LB,
            #endif
              MPI_INT		 //stepIdentifier
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //stepIdentifier
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         auto base = disp[0];
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &StartTraversalMessage::Datatype );
         MPI_Type_commit( &StartTraversalMessage::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StartTraversalMessage::Datatype);
         MPI_Type_commit( &StartTraversalMessage::Datatype );
         #endif
         
      }
      {
    	  // todo Blos so ne Idee
         StartTraversalMessage dummyStartTraversalMessage[10];
         
         #ifdef MPI2
         const int Attributes = 1;
         #else

         const int Attributes = 1+2;
         // todo
//         const int Attributes = 1+1;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
            MPI_LB,
            #endif
              MPI_INT		 //stepIdentifier
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
                 // todo
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //stepIdentifier
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         // todo
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]))), &disp[currentAddress]);
         #endif

         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[1]))), &disp[currentAddress]);
         #endif

         for (int i=1; i<Attributes; i++) {
            assertion1( disp[i] > disp[i-1], i );
         }

         // @todo
         #ifndef MPI2
         MPI_Aint  base;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessage[0]))), &base);
         for (int i=0; i<Attributes; i++) {

            disp[i] = disp[i] - base;

         }
         #endif

         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &StartTraversalMessage::FullDatatype );
         MPI_Type_commit( &StartTraversalMessage::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StartTraversalMessage::FullDatatype);
         MPI_Type_commit( &StartTraversalMessage::FullDatatype );
         // @todo
         logInfo( "initDatatype()", "size of entry=" << sizeof(StartTraversalMessage) << ", disp[0]=" << disp[0] << ", disp[-1]=" << disp[Attributes-1] );
         #endif
         
      }
      
   }
   
   
   void peano4::parallel::StartTraversalMessage::shutdownDatatype() {
      MPI_Type_free( &StartTraversalMessage::Datatype );
      MPI_Type_free( &StartTraversalMessage::FullDatatype );
      
   }
   
   void peano4::parallel::StartTraversalMessage::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::parallel::StartTraversalMessage " 
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result); 
         _log.error( "send(int)",msg.str() ); 
       } 
    } 
    break; 
   case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    {
      MPI_Request* sendRequestHandle = new MPI_Request(); 
      int          flag = 0; 
       int          result; 
       clock_t      timeOutWarning   = -1; 
       clock_t      timeOutShutdown  = -1; 
       bool         triggeredTimeoutWarning = false;  
       result = MPI_Isend(  
         this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination,  
         tag, tarch::mpi::Rank::getInstance().getCommunicator(), 
         sendRequestHandle  
       ); 
       if  (result!=MPI_SUCCESS) {  
         std::ostringstream msg;  
         msg << "was not able to send message peano4::parallel::StartTraversalMessage "  
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result);  
         _log.error( "send(int)",msg.str() );  
       }  
       result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
       while (!flag) { 
         if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
         result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
           std::ostringstream msg; 
           msg << "testing for finished send task for peano4::parallel::StartTraversalMessage " 
               << toString() 
               << " sent to node " << destination 
               << " failed: " << tarch::mpi::MPIReturnValueToString(result); 
           _log.error("send(int)", msg.str() ); 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
           (clock()>timeOutWarning) && 
           (!triggeredTimeoutWarning) 
         ) { 
           tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
             "peano4::parallel::StartTraversalMessage", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::parallel::StartTraversalMessage", 
             "send(int)", destination,tag,1 
           ); 
         } 
 	       tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
       } 
       delete sendRequestHandle; 
     }  
     break; 
   case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    assertionMsg(false,"should not be called"); 
    break; 
} 
 // ============================= 
// end injected snippet/aspect 
// ============================= 

      
   }
   
   
   
   void peano4::parallel::StartTraversalMessage::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
MPI_Status status; 
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::StartTraversalMessage from node " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    } 
    break; 
  case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    { 
      int          flag = 0; 
      int          result; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      MPI_Request* sendRequestHandle = new MPI_Request(); 
       result = MPI_Irecv( 
        this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, 
        tarch::mpi::Rank::getInstance().getCommunicator(), sendRequestHandle 
      ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::StartTraversalMessage from node " 
             << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
      result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::parallel::StartTraversalMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::StartTraversalMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::StartTraversalMessage failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      delete sendRequestHandle; 
    }    break; 
  case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    {
      int flag; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      int result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
       if (result!=MPI_SUCCESS) { 
        std::ostringstream msg; 
        msg << "testing for finished receive task for peano4::parallel::StartTraversalMessage failed: " 
            << tarch::mpi::MPIReturnValueToString(result); 
        _log.error("receive(int)", msg.str() ); 
      } 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::parallel::StartTraversalMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::StartTraversalMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::StartTraversalMessage failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::StartTraversalMessage from node " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    }
    break; 
  } 
// =========================== 
// end injected snippet/aspect 
// =========================== 

      
     _senderDestinationRank = source==MPI_ANY_SOURCE ? status.MPI_SOURCE : source;
   }
   
   
   
   bool peano4::parallel::StartTraversalMessage::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Status status;
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, &status
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
   
   int peano4::parallel::StartTraversalMessage::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::parallel::StartTraversalMessagePacked::PersistentRecords::PersistentRecords() {
   
}


peano4::parallel::StartTraversalMessagePacked::PersistentRecords::PersistentRecords(const int& stepIdentifier):
_stepIdentifier(stepIdentifier) {
   
}


 int peano4::parallel::StartTraversalMessagePacked::PersistentRecords::getStepIdentifier() const  {
   return _stepIdentifier;
}



 void peano4::parallel::StartTraversalMessagePacked::PersistentRecords::setStepIdentifier(const int& stepIdentifier)  {
   _stepIdentifier = stepIdentifier;
}


peano4::parallel::StartTraversalMessagePacked::StartTraversalMessagePacked() {
   
}


peano4::parallel::StartTraversalMessagePacked::StartTraversalMessagePacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._stepIdentifier) {
   
}


peano4::parallel::StartTraversalMessagePacked::StartTraversalMessagePacked(const int& stepIdentifier):
_persistentRecords(stepIdentifier) {
   
}


peano4::parallel::StartTraversalMessagePacked::~StartTraversalMessagePacked() { }


 int peano4::parallel::StartTraversalMessagePacked::getStepIdentifier() const  {
   return _persistentRecords._stepIdentifier;
}



 void peano4::parallel::StartTraversalMessagePacked::setStepIdentifier(const int& stepIdentifier)  {
   _persistentRecords._stepIdentifier = stepIdentifier;
}




std::string peano4::parallel::StartTraversalMessagePacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::parallel::StartTraversalMessagePacked::toString (std::ostream& out) const {
   out << "("; 
   out << "stepIdentifier:" << getStepIdentifier();
   out <<  ")";
}


peano4::parallel::StartTraversalMessagePacked::PersistentRecords peano4::parallel::StartTraversalMessagePacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::parallel::StartTraversalMessage peano4::parallel::StartTraversalMessagePacked::convert() const{
   return StartTraversalMessage(
      getStepIdentifier()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::parallel::StartTraversalMessagePacked::_log( "peano4::parallel::StartTraversalMessagePacked" );
   
   MPI_Datatype peano4::parallel::StartTraversalMessagePacked::Datatype = 0;
   MPI_Datatype peano4::parallel::StartTraversalMessagePacked::FullDatatype = 0;
   
   
   void peano4::parallel::StartTraversalMessagePacked::initDatatype() {
      {
         StartTraversalMessagePacked dummyStartTraversalMessagePacked[2];
         
         #ifdef MPI2
         const int Attributes = 1;
         #else
         const int Attributes = 1+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
            MPI_LB,
            #endif
              MPI_INT		 //stepIdentifier
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //stepIdentifier
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         auto base = disp[0];
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &StartTraversalMessagePacked::Datatype );
         MPI_Type_commit( &StartTraversalMessagePacked::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StartTraversalMessagePacked::Datatype);
         MPI_Type_commit( &StartTraversalMessagePacked::Datatype );
         #endif
         
      }
      {
         StartTraversalMessagePacked dummyStartTraversalMessagePacked[2];
         
         #ifdef MPI2
         const int Attributes = 1;
         #else
         const int Attributes = 1+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
            MPI_LB,
            #endif
              MPI_INT		 //stepIdentifier
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //stepIdentifier
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[0]._persistentRecords._stepIdentifier))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStartTraversalMessagePacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         auto base = disp[0];
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &StartTraversalMessagePacked::FullDatatype );
         MPI_Type_commit( &StartTraversalMessagePacked::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StartTraversalMessagePacked::FullDatatype);
         MPI_Type_commit( &StartTraversalMessagePacked::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::parallel::StartTraversalMessagePacked::shutdownDatatype() {
      MPI_Type_free( &StartTraversalMessagePacked::Datatype );
      MPI_Type_free( &StartTraversalMessagePacked::FullDatatype );
      
   }
   
   void peano4::parallel::StartTraversalMessagePacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::parallel::StartTraversalMessagePacked " 
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result); 
         _log.error( "send(int)",msg.str() ); 
       } 
    } 
    break; 
   case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    {
      MPI_Request* sendRequestHandle = new MPI_Request(); 
      int          flag = 0; 
       int          result; 
       clock_t      timeOutWarning   = -1; 
       clock_t      timeOutShutdown  = -1; 
       bool         triggeredTimeoutWarning = false;  
       result = MPI_Isend(  
         this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination,  
         tag, tarch::mpi::Rank::getInstance().getCommunicator(), 
         sendRequestHandle  
       ); 
       if  (result!=MPI_SUCCESS) {  
         std::ostringstream msg;  
         msg << "was not able to send message peano4::parallel::StartTraversalMessagePacked "  
             << toString() 
             << " to node " << destination 
             << ": " << tarch::mpi::MPIReturnValueToString(result);  
         _log.error( "send(int)",msg.str() );  
       }  
       result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
       while (!flag) { 
         if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
         result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
           std::ostringstream msg; 
           msg << "testing for finished send task for peano4::parallel::StartTraversalMessagePacked " 
               << toString() 
               << " sent to node " << destination 
               << " failed: " << tarch::mpi::MPIReturnValueToString(result); 
           _log.error("send(int)", msg.str() ); 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
           (clock()>timeOutWarning) && 
           (!triggeredTimeoutWarning) 
         ) { 
           tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
             "peano4::parallel::StartTraversalMessagePacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::parallel::StartTraversalMessagePacked", 
             "send(int)", destination,tag,1 
           ); 
         } 
 	       tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
       } 
       delete sendRequestHandle; 
     }  
     break; 
   case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    assertionMsg(false,"should not be called"); 
    break; 
} 
 // ============================= 
// end injected snippet/aspect 
// ============================= 

      
   }
   
   
   
   void peano4::parallel::StartTraversalMessagePacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
MPI_Status status; 
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::StartTraversalMessagePacked from node " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    } 
    break; 
  case ExchangeMode::NonblockingWithPollingLoopOverTests: 
    { 
      int          flag = 0; 
      int          result; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      MPI_Request* sendRequestHandle = new MPI_Request(); 
       result = MPI_Irecv( 
        this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, 
        tarch::mpi::Rank::getInstance().getCommunicator(), sendRequestHandle 
      ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::StartTraversalMessagePacked from node " 
             << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
      result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::parallel::StartTraversalMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::StartTraversalMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::StartTraversalMessagePacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      delete sendRequestHandle; 
    }    break; 
  case ExchangeMode::LoopOverProbeWithBlockingReceive: 
    {
      int flag; 
      clock_t      timeOutWarning   = -1; 
      clock_t      timeOutShutdown  = -1; 
      bool         triggeredTimeoutWarning = false; 
      int result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
       if (result!=MPI_SUCCESS) { 
        std::ostringstream msg; 
        msg << "testing for finished receive task for peano4::parallel::StartTraversalMessagePacked failed: " 
            << tarch::mpi::MPIReturnValueToString(result); 
        _log.error("receive(int)", msg.str() ); 
      } 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::mpi::Rank::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::mpi::Rank::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::mpi::Rank::getInstance().writeTimeOutWarning( 
            "peano4::parallel::StartTraversalMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::StartTraversalMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::StartTraversalMessagePacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::StartTraversalMessagePacked from node " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    }
    break; 
  } 
// =========================== 
// end injected snippet/aspect 
// =========================== 

      
     _senderDestinationRank = source==MPI_ANY_SOURCE ? status.MPI_SOURCE : source;
   }
   
   
   
   bool peano4::parallel::StartTraversalMessagePacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
      MPI_Status status;
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, &status
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
   
   int peano4::parallel::StartTraversalMessagePacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



