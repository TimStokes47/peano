#include "TreeManagementMessage.h"

peano4::parallel::TreeManagementMessage::PersistentRecords::PersistentRecords() {
   
}


peano4::parallel::TreeManagementMessage::PersistentRecords::PersistentRecords(const int& masterSpacetreeId, const int& workerSpacetreeId, const Action& action):
_masterSpacetreeId(masterSpacetreeId),
_workerSpacetreeId(workerSpacetreeId),
_action(action) {
   
}


 int peano4::parallel::TreeManagementMessage::PersistentRecords::getMasterSpacetreeId() const  {
   return _masterSpacetreeId;
}



 void peano4::parallel::TreeManagementMessage::PersistentRecords::setMasterSpacetreeId(const int& masterSpacetreeId)  {
   _masterSpacetreeId = masterSpacetreeId;
}



 int peano4::parallel::TreeManagementMessage::PersistentRecords::getWorkerSpacetreeId() const  {
   return _workerSpacetreeId;
}



 void peano4::parallel::TreeManagementMessage::PersistentRecords::setWorkerSpacetreeId(const int& workerSpacetreeId)  {
   _workerSpacetreeId = workerSpacetreeId;
}



 peano4::parallel::TreeManagementMessage::Action peano4::parallel::TreeManagementMessage::PersistentRecords::getAction() const  {
   return _action;
}



 void peano4::parallel::TreeManagementMessage::PersistentRecords::setAction(const Action& action)  {
   _action = action;
}


peano4::parallel::TreeManagementMessage::TreeManagementMessage() {
   
}


peano4::parallel::TreeManagementMessage::TreeManagementMessage(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._masterSpacetreeId, persistentRecords._workerSpacetreeId, persistentRecords._action) {
   
}


peano4::parallel::TreeManagementMessage::TreeManagementMessage(const int& masterSpacetreeId, const int& workerSpacetreeId, const Action& action):
_persistentRecords(masterSpacetreeId, workerSpacetreeId, action) {
   
}


peano4::parallel::TreeManagementMessage::~TreeManagementMessage() { }


 int peano4::parallel::TreeManagementMessage::getMasterSpacetreeId() const  {
   return _persistentRecords._masterSpacetreeId;
}



 void peano4::parallel::TreeManagementMessage::setMasterSpacetreeId(const int& masterSpacetreeId)  {
   _persistentRecords._masterSpacetreeId = masterSpacetreeId;
}



 int peano4::parallel::TreeManagementMessage::getWorkerSpacetreeId() const  {
   return _persistentRecords._workerSpacetreeId;
}



 void peano4::parallel::TreeManagementMessage::setWorkerSpacetreeId(const int& workerSpacetreeId)  {
   _persistentRecords._workerSpacetreeId = workerSpacetreeId;
}



 peano4::parallel::TreeManagementMessage::Action peano4::parallel::TreeManagementMessage::getAction() const  {
   return _persistentRecords._action;
}



 void peano4::parallel::TreeManagementMessage::setAction(const Action& action)  {
   _persistentRecords._action = action;
}


std::string peano4::parallel::TreeManagementMessage::toString(const Action& param) {
   switch (param) {
      case RequestNewRemoteTree: return "RequestNewRemoteTree";
      case CreateNewRemoteTree: return "CreateNewRemoteTree";
      case RemoveChildTreeFromBooksAsChildBecameEmpty: return "RemoveChildTreeFromBooksAsChildBecameEmpty";
      case JoinWithWorker: return "JoinWithWorker";
      case Acknowledgement: return "Acknowledgement";
   }
   return "undefined";
}

std::string peano4::parallel::TreeManagementMessage::getActionMapping() {
   return "Action(RequestNewRemoteTree=0,CreateNewRemoteTree=1,RemoveChildTreeFromBooksAsChildBecameEmpty=2,JoinWithWorker=3,Acknowledgement=4)";
}


std::string peano4::parallel::TreeManagementMessage::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::parallel::TreeManagementMessage::toString (std::ostream& out) const {
   out << "("; 
   out << "masterSpacetreeId:" << getMasterSpacetreeId();
   out << ",";
   out << "workerSpacetreeId:" << getWorkerSpacetreeId();
   out << ",";
   out << "action:" << toString(getAction());
   out <<  ")";
}


peano4::parallel::TreeManagementMessage::PersistentRecords peano4::parallel::TreeManagementMessage::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::parallel::TreeManagementMessagePacked peano4::parallel::TreeManagementMessage::convert() const{
   return TreeManagementMessagePacked(
      getMasterSpacetreeId(),
      getWorkerSpacetreeId(),
      getAction()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::parallel::TreeManagementMessage::_log( "peano4::parallel::TreeManagementMessage" );
   
   MPI_Datatype peano4::parallel::TreeManagementMessage::Datatype = 0;
   MPI_Datatype peano4::parallel::TreeManagementMessage::FullDatatype = 0;
   
   
   void peano4::parallel::TreeManagementMessage::initDatatype() {
      {
         TreeManagementMessage dummyTreeManagementMessage[16];
         
         #ifdef MPI2
         const int Attributes = 3;
         #else
         const int Attributes = 3+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //masterSpacetreeId
            , MPI_INT		 //workerSpacetreeId
            , MPI_INT		 //action
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //masterSpacetreeId
            , 1		 //workerSpacetreeId
            , 1		 //action
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._action))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._action))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &TreeManagementMessage::Datatype );
         errorCode += MPI_Type_commit( &TreeManagementMessage::Datatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeManagementMessage::Datatype);
         errorCode += MPI_Type_commit( &TreeManagementMessage::Datatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      {
         TreeManagementMessage dummyTreeManagementMessage[16];
         
         #ifdef MPI2
         const int Attributes = 3;
         #else
         const int Attributes = 3+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //masterSpacetreeId
            , MPI_INT		 //workerSpacetreeId
            , MPI_INT		 //action
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //masterSpacetreeId
            , 1		 //workerSpacetreeId
            , 1		 //action
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._action))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]._persistentRecords._action))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessage[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &TreeManagementMessage::FullDatatype );
         errorCode += MPI_Type_commit( &TreeManagementMessage::FullDatatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeManagementMessage::FullDatatype);
         errorCode += MPI_Type_commit( &TreeManagementMessage::FullDatatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      
   }
   
   
   void peano4::parallel::TreeManagementMessage::shutdownDatatype() {
      MPI_Type_free( &TreeManagementMessage::Datatype );
      MPI_Type_free( &TreeManagementMessage::FullDatatype );
      
   }
   
   void peano4::parallel::TreeManagementMessage::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::parallel::TreeManagementMessage " 
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
         msg << "was not able to send message peano4::parallel::TreeManagementMessage "  
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
           msg << "testing for finished send task for peano4::parallel::TreeManagementMessage " 
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
             "peano4::parallel::TreeManagementMessage", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::parallel::TreeManagementMessage", 
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
   
   
   
   void peano4::parallel::TreeManagementMessage::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::parallel::TreeManagementMessage from rank " 
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
        msg << "failed to start to receive peano4::parallel::TreeManagementMessage from rank " 
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
            "peano4::parallel::TreeManagementMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeManagementMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeManagementMessage failed: " 
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
        msg << "testing for finished receive task for peano4::parallel::TreeManagementMessage failed: " 
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
            "peano4::parallel::TreeManagementMessage", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeManagementMessage", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeManagementMessage failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeManagementMessage from rank " 
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
   
   
   
   bool peano4::parallel::TreeManagementMessage::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::parallel::TreeManagementMessage::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::parallel::TreeManagementMessagePacked::PersistentRecords::PersistentRecords() {
   if ((3 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((3 < (8 * sizeof(short int))));
   
}


peano4::parallel::TreeManagementMessagePacked::PersistentRecords::PersistentRecords(const int& masterSpacetreeId, const int& workerSpacetreeId, const Action& action):
_masterSpacetreeId(masterSpacetreeId),
_workerSpacetreeId(workerSpacetreeId) {
   setAction(action);
   if ((3 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((3 < (8 * sizeof(short int))));
   
}


 int peano4::parallel::TreeManagementMessagePacked::PersistentRecords::getMasterSpacetreeId() const  {
   return _masterSpacetreeId;
}



 void peano4::parallel::TreeManagementMessagePacked::PersistentRecords::setMasterSpacetreeId(const int& masterSpacetreeId)  {
   _masterSpacetreeId = masterSpacetreeId;
}



 int peano4::parallel::TreeManagementMessagePacked::PersistentRecords::getWorkerSpacetreeId() const  {
   return _workerSpacetreeId;
}



 void peano4::parallel::TreeManagementMessagePacked::PersistentRecords::setWorkerSpacetreeId(const int& workerSpacetreeId)  {
   _workerSpacetreeId = workerSpacetreeId;
}



 peano4::parallel::TreeManagementMessage::Action peano4::parallel::TreeManagementMessagePacked::PersistentRecords::getAction() const  {
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 4));
   return (Action) tmp;
}



 void peano4::parallel::TreeManagementMessagePacked::PersistentRecords::setAction(const Action& action)  {
   assertion((action >= 0 && action <= 4));
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   _packedRecords0 = static_cast<short int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<short int>(_packedRecords0 | static_cast<short int>(action) << (0));
}


peano4::parallel::TreeManagementMessagePacked::TreeManagementMessagePacked() {
   if ((3 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((3 < (8 * sizeof(short int))));
   
}


peano4::parallel::TreeManagementMessagePacked::TreeManagementMessagePacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._masterSpacetreeId, persistentRecords._workerSpacetreeId, persistentRecords.getAction()) {
   if ((3 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((3 < (8 * sizeof(short int))));
   
}


peano4::parallel::TreeManagementMessagePacked::TreeManagementMessagePacked(const int& masterSpacetreeId, const int& workerSpacetreeId, const Action& action):
_persistentRecords(masterSpacetreeId, workerSpacetreeId, action) {
   if ((3 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((3 < (8 * sizeof(short int))));
   
}


peano4::parallel::TreeManagementMessagePacked::~TreeManagementMessagePacked() { }


 int peano4::parallel::TreeManagementMessagePacked::getMasterSpacetreeId() const  {
   return _persistentRecords._masterSpacetreeId;
}



 void peano4::parallel::TreeManagementMessagePacked::setMasterSpacetreeId(const int& masterSpacetreeId)  {
   _persistentRecords._masterSpacetreeId = masterSpacetreeId;
}



 int peano4::parallel::TreeManagementMessagePacked::getWorkerSpacetreeId() const  {
   return _persistentRecords._workerSpacetreeId;
}



 void peano4::parallel::TreeManagementMessagePacked::setWorkerSpacetreeId(const int& workerSpacetreeId)  {
   _persistentRecords._workerSpacetreeId = workerSpacetreeId;
}



 peano4::parallel::TreeManagementMessage::Action peano4::parallel::TreeManagementMessagePacked::getAction() const  {
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 4));
   return (Action) tmp;
}



 void peano4::parallel::TreeManagementMessagePacked::setAction(const Action& action)  {
   assertion((action >= 0 && action <= 4));
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 | static_cast<short int>(action) << (0));
}


std::string peano4::parallel::TreeManagementMessagePacked::toString(const Action& param) {
   return peano4::parallel::TreeManagementMessage::toString(param);
}

std::string peano4::parallel::TreeManagementMessagePacked::getActionMapping() {
   return peano4::parallel::TreeManagementMessage::getActionMapping();
}



std::string peano4::parallel::TreeManagementMessagePacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::parallel::TreeManagementMessagePacked::toString (std::ostream& out) const {
   out << "("; 
   out << "masterSpacetreeId:" << getMasterSpacetreeId();
   out << ",";
   out << "workerSpacetreeId:" << getWorkerSpacetreeId();
   out << ",";
   out << "action:" << toString(getAction());
   out <<  ")";
}


peano4::parallel::TreeManagementMessagePacked::PersistentRecords peano4::parallel::TreeManagementMessagePacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::parallel::TreeManagementMessage peano4::parallel::TreeManagementMessagePacked::convert() const{
   return TreeManagementMessage(
      getMasterSpacetreeId(),
      getWorkerSpacetreeId(),
      getAction()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::parallel::TreeManagementMessagePacked::_log( "peano4::parallel::TreeManagementMessagePacked" );
   
   MPI_Datatype peano4::parallel::TreeManagementMessagePacked::Datatype = 0;
   MPI_Datatype peano4::parallel::TreeManagementMessagePacked::FullDatatype = 0;
   
   
   void peano4::parallel::TreeManagementMessagePacked::initDatatype() {
      {
         TreeManagementMessagePacked dummyTreeManagementMessagePacked[16];
         
         #ifdef MPI2
         const int Attributes = 3;
         #else
         const int Attributes = 3+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //masterSpacetreeId
            , MPI_INT		 //workerSpacetreeId
            , MPI_SHORT		 //_packedRecords0
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //masterSpacetreeId
            , 1		 //workerSpacetreeId
            , 1		 //_packedRecords0
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &TreeManagementMessagePacked::Datatype );
         errorCode += MPI_Type_commit( &TreeManagementMessagePacked::Datatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeManagementMessagePacked::Datatype);
         errorCode += MPI_Type_commit( &TreeManagementMessagePacked::Datatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      {
         TreeManagementMessagePacked dummyTreeManagementMessagePacked[16];
         
         #ifdef MPI2
         const int Attributes = 3;
         #else
         const int Attributes = 3+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //masterSpacetreeId
            , MPI_INT		 //workerSpacetreeId
            , MPI_SHORT		 //_packedRecords0
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //masterSpacetreeId
            , 1		 //workerSpacetreeId
            , 1		 //_packedRecords0
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._masterSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._workerSpacetreeId))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeManagementMessagePacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &TreeManagementMessagePacked::FullDatatype );
         errorCode += MPI_Type_commit( &TreeManagementMessagePacked::FullDatatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeManagementMessagePacked::FullDatatype);
         errorCode += MPI_Type_commit( &TreeManagementMessagePacked::FullDatatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      
   }
   
   
   void peano4::parallel::TreeManagementMessagePacked::shutdownDatatype() {
      MPI_Type_free( &TreeManagementMessagePacked::Datatype );
      MPI_Type_free( &TreeManagementMessagePacked::FullDatatype );
      
   }
   
   void peano4::parallel::TreeManagementMessagePacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::parallel::TreeManagementMessagePacked " 
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
         msg << "was not able to send message peano4::parallel::TreeManagementMessagePacked "  
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
           msg << "testing for finished send task for peano4::parallel::TreeManagementMessagePacked " 
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
             "peano4::parallel::TreeManagementMessagePacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::parallel::TreeManagementMessagePacked", 
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
   
   
   
   void peano4::parallel::TreeManagementMessagePacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::parallel::TreeManagementMessagePacked from rank " 
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
        msg << "failed to start to receive peano4::parallel::TreeManagementMessagePacked from rank " 
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
            "peano4::parallel::TreeManagementMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeManagementMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeManagementMessagePacked failed: " 
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
        msg << "testing for finished receive task for peano4::parallel::TreeManagementMessagePacked failed: " 
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
            "peano4::parallel::TreeManagementMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeManagementMessagePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeManagementMessagePacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeManagementMessagePacked from rank " 
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
   
   
   
   bool peano4::parallel::TreeManagementMessagePacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::parallel::TreeManagementMessagePacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



