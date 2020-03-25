#include "IntegerMessage.h"

tarch::mpi::IntegerMessage::PersistentRecords::PersistentRecords() {
   
}


tarch::mpi::IntegerMessage::IntegerMessage() {
   
}


tarch::mpi::IntegerMessage::IntegerMessage(const PersistentRecords& persistentRecords):
_persistentRecords() {
   
}


tarch::mpi::IntegerMessage::IntegerMessage(const int& value):
_value(value) {
   
}

tarch::mpi::IntegerMessage::~IntegerMessage() { }


 int tarch::mpi::IntegerMessage::getValue() const  {
   return _value;
}



 void tarch::mpi::IntegerMessage::setValue(const int& value)  {
   _value = value;
}




std::string tarch::mpi::IntegerMessage::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void tarch::mpi::IntegerMessage::toString (std::ostream& out) const {
   out << "("; 
   out << "value:" << getValue();
   out <<  ")";
}


tarch::mpi::IntegerMessage::PersistentRecords tarch::mpi::IntegerMessage::getPersistentRecords() const {
   return _persistentRecords;
}

tarch::mpi::IntegerMessagePacked tarch::mpi::IntegerMessage::convert() const{
   return IntegerMessagePacked(
      getValue()
   );
}

#ifdef Parallel
   tarch::logging::Log tarch::mpi::IntegerMessage::_log( "tarch::mpi::IntegerMessage" );
   
   MPI_Datatype tarch::mpi::IntegerMessage::Datatype = 0;
   MPI_Datatype tarch::mpi::IntegerMessage::FullDatatype = 0;
   
   
   void tarch::mpi::IntegerMessage::initDatatype() {
      {
         logTraceIn( "initDatatype()" );
         // braucht mer net
         IntegerMessage anotherMessage;
         IntegerMessage dummyIntegerMessage[2];

         logWarning( "initDataype()", "anotherMessage=" << (&anotherMessage) );
         logWarning( "initDataype()", "anotherMessage=" << (&dummyIntegerMessage[0]) );

         const int Attributes = 1;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //value
            
         };
         
         int blocklen[Attributes] = {
              1		 //value
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessage[0]._value))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessage[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         // umbauen
         for (int i=Attributes-1; i>=0; i--) {
         
           // base
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         //errorCode += MPI_Type_commit( &tmpType );  war eh ein debug
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessage[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         //errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &IntegerMessage::Datatype );
         errorCode += MPI_Type_create_resized( tmpType, 0, 12, &IntegerMessage::Datatype );
         errorCode += MPI_Type_commit( &IntegerMessage::Datatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );

         // wichtig! => nett, aber bringt nix, weil er polymorphic ist
         assertion( not std::is_polymorphic<IntegerMessage>::value );

         logWarning(
           "initDatatype()", "typeOffset=" << typeOffset << ", typeExtent=" << typeExtent <<
           ", disp[0]=" << disp[0] << ", sizeof(IntegerMessage)=" << sizeof(IntegerMessage) <<
           ", sizeof(int)=" << sizeof(int)
         );
         logTraceOut( "initDatatype()" );
         
      }
      {
         logTraceIn( "initDatatype()" );
         IntegerMessage dummyIntegerMessage[2];
         
         const int Attributes = 1;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //value
            
         };
         
         int blocklen[Attributes] = {
              1		 //value
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessage[0]._value))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessage[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessage[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &IntegerMessage::FullDatatype );
         errorCode += MPI_Type_commit( &IntegerMessage::FullDatatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         logTraceOut( "initDatatype()" );
         
      }
      
   }
   
   
   void tarch::mpi::IntegerMessage::shutdownDatatype() {
      logTraceIn( "shutdownDatatype()" );
      MPI_Type_free( &IntegerMessage::Datatype );
      MPI_Type_free( &IntegerMessage::FullDatatype );
      logTraceOut( "shutdownDatatype()" );
      
   }
   
   void tarch::mpi::IntegerMessage::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 

   int* tmp = (int*)this;
   volatile IntegerMessage tmpSendCopy( *this );
   tmpSendCopy._value = _value;

   logWarning( "send(...)", *(tmp+0) << " x " << *(tmp+1) << " x " << *(tmp+2) );

switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send((const void*)&tmpSendCopy, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator());
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message tarch::mpi::IntegerMessage " 
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
         msg << "was not able to send message tarch::mpi::IntegerMessage "  
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
           msg << "testing for finished send task for tarch::mpi::IntegerMessage " 
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
             "tarch::mpi::IntegerMessage", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "tarch::mpi::IntegerMessage", 
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
   
   
   
   void tarch::mpi::IntegerMessage::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
     int* tmp = (int*)this;
     logWarning( "receive(...)", "before receive: " << *(tmp+0) << " x " << *(tmp+1) << " x " << *(tmp+2) );

     MPI_Status status;
     volatile IntegerMessage tmpReceiveCopy;
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv( (void*)&tmpReceiveCopy, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE );
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive tarch::mpi::IntegerMessage from rank " 
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
        msg << "failed to start to receive tarch::mpi::IntegerMessage from rank " 
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
            "tarch::mpi::IntegerMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::IntegerMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::IntegerMessage failed: " 
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
        msg << "testing for finished receive task for tarch::mpi::IntegerMessage failed: " 
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
            "tarch::mpi::IntegerMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::IntegerMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::IntegerMessage failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive tarch::mpi::IntegerMessage from rank " 
            << source << ": " << tarch::mpi::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
    }
    break; 
  } 
// =========================== 
// end injected snippet/aspect 
// =========================== 

  _value = tmpReceiveCopy._value;
  // *this = tmpReceiveCopy;
logWarning( "receive(...)", "after receive: " << *(tmp+0) << " x " << *(tmp+1) << " x " << *(tmp+2) );

     _senderDestinationRank = source==MPI_ANY_SOURCE ? status.MPI_SOURCE : source;
   }
   
   
   
   bool tarch::mpi::IntegerMessage::isMessageInQueue(int tag) {
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE
      );
      return flag;
   }
   
   int tarch::mpi::IntegerMessage::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


tarch::mpi::IntegerMessagePacked::PersistentRecords::PersistentRecords() {
   
}


tarch::mpi::IntegerMessagePacked::IntegerMessagePacked() {
   
}


tarch::mpi::IntegerMessagePacked::IntegerMessagePacked(const PersistentRecords& persistentRecords):
_persistentRecords() {
   
}


tarch::mpi::IntegerMessagePacked::IntegerMessagePacked(const int& value):
_value(value) {
   
}

tarch::mpi::IntegerMessagePacked::~IntegerMessagePacked() { }


 int tarch::mpi::IntegerMessagePacked::getValue() const  {
   return _value;
}



 void tarch::mpi::IntegerMessagePacked::setValue(const int& value)  {
   _value = value;
}




std::string tarch::mpi::IntegerMessagePacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void tarch::mpi::IntegerMessagePacked::toString (std::ostream& out) const {
   out << "("; 
   out << "value:" << getValue();
   out <<  ")";
}


tarch::mpi::IntegerMessagePacked::PersistentRecords tarch::mpi::IntegerMessagePacked::getPersistentRecords() const {
   return _persistentRecords;
}

tarch::mpi::IntegerMessage tarch::mpi::IntegerMessagePacked::convert() const{
   return IntegerMessage(
      getValue()
   );
}

#ifdef Parallel
   tarch::logging::Log tarch::mpi::IntegerMessagePacked::_log( "tarch::mpi::IntegerMessagePacked" );
   
   MPI_Datatype tarch::mpi::IntegerMessagePacked::Datatype = 0;
   MPI_Datatype tarch::mpi::IntegerMessagePacked::FullDatatype = 0;
   
   
   void tarch::mpi::IntegerMessagePacked::initDatatype() {
      {
         logTraceIn( "initDatatype()" );
         IntegerMessagePacked dummyIntegerMessagePacked[2];
         
         const int Attributes = 1;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //value
            
         };
         
         int blocklen[Attributes] = {
              1		 //value
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessagePacked[0]._value))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessagePacked[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessagePacked[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &IntegerMessagePacked::Datatype );
         errorCode += MPI_Type_commit( &IntegerMessagePacked::Datatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );

         logWarning(
           "initDatatype()", "typeOffset=" << typeOffset << ", typeExtent=" << typeExtent <<
           ", disp[0]" << disp[0]
         );
         logTraceOut( "initDatatype()" );
         
      }
      {
         logTraceIn( "initDatatype()" );
         IntegerMessagePacked dummyIntegerMessagePacked[2];
         
         const int Attributes = 1;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //value
            
         };
         
         int blocklen[Attributes] = {
              1		 //value
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessagePacked[0]._value))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessagePacked[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyIntegerMessagePacked[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &IntegerMessagePacked::FullDatatype );
         errorCode += MPI_Type_commit( &IntegerMessagePacked::FullDatatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         logTraceOut( "initDatatype()" );
         
      }
      
   }
   
   
   void tarch::mpi::IntegerMessagePacked::shutdownDatatype() {
      logTraceIn( "shutdownDatatype()" );
      MPI_Type_free( &IntegerMessagePacked::Datatype );
      MPI_Type_free( &IntegerMessagePacked::FullDatatype );
      logTraceOut( "shutdownDatatype()" );
      
   }
   
   void tarch::mpi::IntegerMessagePacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message tarch::mpi::IntegerMessagePacked " 
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
         msg << "was not able to send message tarch::mpi::IntegerMessagePacked "  
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
           msg << "testing for finished send task for tarch::mpi::IntegerMessagePacked " 
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
             "tarch::mpi::IntegerMessagePacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "tarch::mpi::IntegerMessagePacked", 
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
   
   
   
   void tarch::mpi::IntegerMessagePacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive tarch::mpi::IntegerMessagePacked from rank " 
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
        msg << "failed to start to receive tarch::mpi::IntegerMessagePacked from rank " 
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
            "tarch::mpi::IntegerMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::IntegerMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::IntegerMessagePacked failed: " 
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
        msg << "testing for finished receive task for tarch::mpi::IntegerMessagePacked failed: " 
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
            "tarch::mpi::IntegerMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::IntegerMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::IntegerMessagePacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive tarch::mpi::IntegerMessagePacked from rank " 
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
   
   
   
   bool tarch::mpi::IntegerMessagePacked::isMessageInQueue(int tag) {
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE
      );
      return flag;
   }
   
   int tarch::mpi::IntegerMessagePacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



