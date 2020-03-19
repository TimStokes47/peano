#include "StringMessage.h"

tarch::mpi::StringMessage::PersistentRecords::PersistentRecords() {
   
}


tarch::mpi::StringMessage::StringMessage() {
   
}


tarch::mpi::StringMessage::StringMessage(const PersistentRecords& persistentRecords):
_persistentRecords() {
   
}


tarch::mpi::StringMessage::StringMessage(const int& length, const tarch::la::Vector<MPI_MAX_NAME_STRING,short int>& data):
_length(length),
_data(data) {
   
}

tarch::mpi::StringMessage::~StringMessage() { }


 int tarch::mpi::StringMessage::getLength() const  {
   return _length;
}



 void tarch::mpi::StringMessage::setLength(const int& length)  {
   _length = length;
}



 tarch::la::Vector<MPI_MAX_NAME_STRING,short int> tarch::mpi::StringMessage::getData() const  {
   return _data;
}



 void tarch::mpi::StringMessage::setData(const tarch::la::Vector<MPI_MAX_NAME_STRING,short int>& data)  {
   _data = (data);
}



 short int tarch::mpi::StringMessage::getData(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<MPI_MAX_NAME_STRING);
   return _data[elementIndex];
   
}



 void tarch::mpi::StringMessage::setData(int elementIndex, const short int& data)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<MPI_MAX_NAME_STRING);
   _data[elementIndex]= data;
   
}




std::string tarch::mpi::StringMessage::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void tarch::mpi::StringMessage::toString (std::ostream& out) const {
   out << "("; 
   out << "length:" << getLength();
   out << ",";
   out << "data:[";
   for (int i = 0; i < MPI_MAX_NAME_STRING-1; i++) {
      out << getData(i) << ",";
   }
   out << getData(MPI_MAX_NAME_STRING-1) << "]";
   out <<  ")";
}


tarch::mpi::StringMessage::PersistentRecords tarch::mpi::StringMessage::getPersistentRecords() const {
   return _persistentRecords;
}

tarch::mpi::StringMessagePacked tarch::mpi::StringMessage::convert() const{
   return StringMessagePacked(
      getLength(),
      getData()
   );
}

#ifdef Parallel
   tarch::logging::Log tarch::mpi::StringMessage::_log( "tarch::mpi::StringMessage" );
   
   MPI_Datatype tarch::mpi::StringMessage::Datatype = 0;
   MPI_Datatype tarch::mpi::StringMessage::FullDatatype = 0;
   
   
   void tarch::mpi::StringMessage::initDatatype() {
      {
         StringMessage dummyStringMessage[16];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 2+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //length
            , MPI_SHORT		 //data
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //length
            , MPI_MAX_NAME_STRING		 //data
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._length))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._length))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._data[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._data[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]))), &base);
         #endif
         #ifdef MPI2
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         #else
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #endif
         int errorCode = 0; 
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &StringMessage::Datatype );
         errorCode += MPI_Type_commit( &StringMessage::Datatype );
         errorCode += MPI_Type_free(&tmpType);
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StringMessage::Datatype);
         int errorCode = MPI_Type_commit( &StringMessage::Datatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      {
         StringMessage dummyStringMessage[16];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 2+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //length
            , MPI_SHORT		 //data
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //length
            , MPI_MAX_NAME_STRING		 //data
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._length))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._length))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._data[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]._data[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[0]))), &base);
         #endif
         #ifdef MPI2
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         #else
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #endif
         int errorCode = 0; 
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessage[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &StringMessage::FullDatatype );
         errorCode += MPI_Type_commit( &StringMessage::FullDatatype );
         errorCode += MPI_Type_free(&tmpType);
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StringMessage::FullDatatype);
         int errorCode = MPI_Type_commit( &StringMessage::FullDatatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      
   }
   
   
   void tarch::mpi::StringMessage::shutdownDatatype() {
      MPI_Type_free( &StringMessage::Datatype );
      MPI_Type_free( &StringMessage::FullDatatype );
      
   }
   
   void tarch::mpi::StringMessage::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message tarch::mpi::StringMessage " 
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
         msg << "was not able to send message tarch::mpi::StringMessage "  
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
           msg << "testing for finished send task for tarch::mpi::StringMessage " 
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
             "tarch::mpi::StringMessage", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "tarch::mpi::StringMessage", 
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
   
   
   
   void tarch::mpi::StringMessage::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive tarch::mpi::StringMessage from rank " 
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
        msg << "failed to start to receive tarch::mpi::StringMessage from rank " 
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
            "tarch::mpi::StringMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::StringMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::StringMessage failed: " 
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
        msg << "testing for finished receive task for tarch::mpi::StringMessage failed: " 
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
            "tarch::mpi::StringMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::StringMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::StringMessage failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive tarch::mpi::StringMessage from rank " 
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
   
   
   
   bool tarch::mpi::StringMessage::isMessageInQueue(int tag) {
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE
      );
      return flag;
   }
   
   int tarch::mpi::StringMessage::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


tarch::mpi::StringMessagePacked::PersistentRecords::PersistentRecords() {
   
}


tarch::mpi::StringMessagePacked::StringMessagePacked() {
   
}


tarch::mpi::StringMessagePacked::StringMessagePacked(const PersistentRecords& persistentRecords):
_persistentRecords() {
   
}


tarch::mpi::StringMessagePacked::StringMessagePacked(const int& length, const tarch::la::Vector<MPI_MAX_NAME_STRING,short int>& data):
_length(length),
_data(data) {
   
}

tarch::mpi::StringMessagePacked::~StringMessagePacked() { }


 int tarch::mpi::StringMessagePacked::getLength() const  {
   return _length;
}



 void tarch::mpi::StringMessagePacked::setLength(const int& length)  {
   _length = length;
}



 tarch::la::Vector<MPI_MAX_NAME_STRING,short int> tarch::mpi::StringMessagePacked::getData() const  {
   return _data;
}



 void tarch::mpi::StringMessagePacked::setData(const tarch::la::Vector<MPI_MAX_NAME_STRING,short int>& data)  {
   _data = (data);
}



 short int tarch::mpi::StringMessagePacked::getData(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<MPI_MAX_NAME_STRING);
   return _data[elementIndex];
   
}



 void tarch::mpi::StringMessagePacked::setData(int elementIndex, const short int& data)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<MPI_MAX_NAME_STRING);
   _data[elementIndex]= data;
   
}




std::string tarch::mpi::StringMessagePacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void tarch::mpi::StringMessagePacked::toString (std::ostream& out) const {
   out << "("; 
   out << "length:" << getLength();
   out << ",";
   out << "data:[";
   for (int i = 0; i < MPI_MAX_NAME_STRING-1; i++) {
      out << getData(i) << ",";
   }
   out << getData(MPI_MAX_NAME_STRING-1) << "]";
   out <<  ")";
}


tarch::mpi::StringMessagePacked::PersistentRecords tarch::mpi::StringMessagePacked::getPersistentRecords() const {
   return _persistentRecords;
}

tarch::mpi::StringMessage tarch::mpi::StringMessagePacked::convert() const{
   return StringMessage(
      getLength(),
      getData()
   );
}

#ifdef Parallel
   tarch::logging::Log tarch::mpi::StringMessagePacked::_log( "tarch::mpi::StringMessagePacked" );
   
   MPI_Datatype tarch::mpi::StringMessagePacked::Datatype = 0;
   MPI_Datatype tarch::mpi::StringMessagePacked::FullDatatype = 0;
   
   
   void tarch::mpi::StringMessagePacked::initDatatype() {
      {
         StringMessagePacked dummyStringMessagePacked[16];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 2+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //length
            , MPI_SHORT		 //data
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //length
            , MPI_MAX_NAME_STRING		 //data
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._length))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._length))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._data[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._data[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]))), &base);
         #endif
         #ifdef MPI2
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         #else
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #endif
         int errorCode = 0; 
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &StringMessagePacked::Datatype );
         errorCode += MPI_Type_commit( &StringMessagePacked::Datatype );
         errorCode += MPI_Type_free(&tmpType);
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StringMessagePacked::Datatype);
         int errorCode = MPI_Type_commit( &StringMessagePacked::Datatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      {
         StringMessagePacked dummyStringMessagePacked[16];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 2+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //length
            , MPI_SHORT		 //data
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //length
            , MPI_MAX_NAME_STRING		 //data
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._length))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._length))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._data[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]._data[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[0]))), &base);
         #endif
         #ifdef MPI2
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         #else
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #endif
         int errorCode = 0; 
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyStringMessagePacked[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &StringMessagePacked::FullDatatype );
         errorCode += MPI_Type_commit( &StringMessagePacked::FullDatatype );
         errorCode += MPI_Type_free(&tmpType);
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &StringMessagePacked::FullDatatype);
         int errorCode = MPI_Type_commit( &StringMessagePacked::FullDatatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      
   }
   
   
   void tarch::mpi::StringMessagePacked::shutdownDatatype() {
      MPI_Type_free( &StringMessagePacked::Datatype );
      MPI_Type_free( &StringMessagePacked::FullDatatype );
      
   }
   
   void tarch::mpi::StringMessagePacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message tarch::mpi::StringMessagePacked " 
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
         msg << "was not able to send message tarch::mpi::StringMessagePacked "  
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
           msg << "testing for finished send task for tarch::mpi::StringMessagePacked " 
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
             "tarch::mpi::StringMessagePacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "tarch::mpi::StringMessagePacked", 
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
   
   
   
   void tarch::mpi::StringMessagePacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive tarch::mpi::StringMessagePacked from rank " 
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
        msg << "failed to start to receive tarch::mpi::StringMessagePacked from rank " 
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
            "tarch::mpi::StringMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::StringMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::StringMessagePacked failed: " 
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
        msg << "testing for finished receive task for tarch::mpi::StringMessagePacked failed: " 
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
            "tarch::mpi::StringMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "tarch::mpi::StringMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for tarch::mpi::StringMessagePacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive tarch::mpi::StringMessagePacked from rank " 
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
   
   
   
   bool tarch::mpi::StringMessagePacked::isMessageInQueue(int tag) {
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE
      );
      return flag;
   }
   
   int tarch::mpi::StringMessagePacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



