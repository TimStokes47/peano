#include "peano4/parallel/TreeEntry.h"

peano4::parallel::TreeEntry::PersistentRecords::PersistentRecords() {
   
}


peano4::parallel::TreeEntry::PersistentRecords::PersistentRecords(const int& id, const int& master):
_id(id),
_master(master) {
   
}


 int peano4::parallel::TreeEntry::PersistentRecords::getId() const  {
   return _id;
}



 void peano4::parallel::TreeEntry::PersistentRecords::setId(const int& id)  {
   _id = id;
}



 int peano4::parallel::TreeEntry::PersistentRecords::getMaster() const  {
   return _master;
}



 void peano4::parallel::TreeEntry::PersistentRecords::setMaster(const int& master)  {
   _master = master;
}


peano4::parallel::TreeEntry::TreeEntry() {
   
}


peano4::parallel::TreeEntry::TreeEntry(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._id, persistentRecords._master) {
   
}


peano4::parallel::TreeEntry::TreeEntry(const int& id, const int& master):
_persistentRecords(id, master) {
   
}


peano4::parallel::TreeEntry::~TreeEntry() { }


 int peano4::parallel::TreeEntry::getId() const  {
   return _persistentRecords._id;
}



 void peano4::parallel::TreeEntry::setId(const int& id)  {
   _persistentRecords._id = id;
}



 int peano4::parallel::TreeEntry::getMaster() const  {
   return _persistentRecords._master;
}



 void peano4::parallel::TreeEntry::setMaster(const int& master)  {
   _persistentRecords._master = master;
}




std::string peano4::parallel::TreeEntry::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::parallel::TreeEntry::toString (std::ostream& out) const {
   out << "("; 
   out << "id:" << getId();
   out << ",";
   out << "master:" << getMaster();
   out <<  ")";
}


peano4::parallel::TreeEntry::PersistentRecords peano4::parallel::TreeEntry::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::parallel::TreeEntryPacked peano4::parallel::TreeEntry::convert() const{
   return TreeEntryPacked(
      getId(),
      getMaster()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::parallel::TreeEntry::_log( "peano4::parallel::TreeEntry" );
   
   MPI_Datatype peano4::parallel::TreeEntry::Datatype = 0;
   MPI_Datatype peano4::parallel::TreeEntry::FullDatatype = 0;
   
   
   void peano4::parallel::TreeEntry::initDatatype() {
      {
         TreeEntry dummyTreeEntry[2];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 3;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //id
            , MPI_INT		 //master
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //id
            , 1		 //master
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._id))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._id))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._master))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._master))), 		&disp[1] );
         #endif
         #ifdef MPI2
         for (int i=1; i<Attributes; i++) {
         #else
         for (int i=1; i<Attributes-1; i++) {
         #endif
            assertion1( disp[i] > disp[i-1], i );
         }
         #ifdef MPI2
         for (int i=0; i<Attributes; i++) {
         #else
         for (int i=0; i<Attributes-1; i++) {
         #endif
            disp[i] = disp[i] - base; // should be MPI_Aint_diff(disp[i], base); but this is not supported by most MPI-2 implementations
            assertion4(disp[i]<static_cast<int>(sizeof(TreeEntry)), i, disp[i], Attributes, sizeof(TreeEntry));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[1]))), 		&disp[2] );
         disp[2] -= base;
         disp[2] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &TreeEntry::Datatype );
         MPI_Type_commit( &TreeEntry::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeEntry::Datatype);
         MPI_Type_commit( &TreeEntry::Datatype );
         #endif
         
      }
      {
         TreeEntry dummyTreeEntry[2];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 3;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //id
            , MPI_INT		 //master
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //id
            , 1		 //master
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._id))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._id))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._master))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[0]._persistentRecords._master))), 		&disp[1] );
         #endif
         #ifdef MPI2
         for (int i=1; i<Attributes; i++) {
         #else
         for (int i=1; i<Attributes-1; i++) {
         #endif
            assertion1( disp[i] > disp[i-1], i );
         }
         #ifdef MPI2
         for (int i=0; i<Attributes; i++) {
         #else
         for (int i=0; i<Attributes-1; i++) {
         #endif
            disp[i] = disp[i] - base; // should be MPI_Aint_diff(disp[i], base); but this is not supported by most MPI-2 implementations
            assertion4(disp[i]<static_cast<int>(sizeof(TreeEntry)), i, disp[i], Attributes, sizeof(TreeEntry));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntry[1]))), 		&disp[2] );
         disp[2] -= base;
         disp[2] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &TreeEntry::FullDatatype );
         MPI_Type_commit( &TreeEntry::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeEntry::FullDatatype);
         MPI_Type_commit( &TreeEntry::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::parallel::TreeEntry::shutdownDatatype() {
      MPI_Type_free( &TreeEntry::Datatype );
      MPI_Type_free( &TreeEntry::FullDatatype );
      
   }
   
   void peano4::parallel::TreeEntry::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::parallel::TreeEntry " 
             << toString() 
             << " to node " << destination 
             << ": " << tarch::parallel::MPIReturnValueToString(result); 
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
         tag, tarch::parallel::Node::getInstance().getCommunicator(), 
         sendRequestHandle  
       ); 
       if  (result!=MPI_SUCCESS) {  
         std::ostringstream msg;  
         msg << "was not able to send message peano4::parallel::TreeEntry "  
             << toString() 
             << " to node " << destination 
             << ": " << tarch::parallel::MPIReturnValueToString(result);  
         _log.error( "send(int)",msg.str() );  
       }  
       result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
       while (!flag) { 
         if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp(); 
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp(); 
         result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
           std::ostringstream msg; 
           msg << "testing for finished send task for peano4::parallel::TreeEntry " 
               << toString() 
               << " sent to node " << destination 
               << " failed: " << tarch::parallel::MPIReturnValueToString(result); 
           _log.error("send(int)", msg.str() ); 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() && 
           (clock()>timeOutWarning) && 
           (!triggeredTimeoutWarning) 
         ) { 
           tarch::parallel::Node::getInstance().writeTimeOutWarning( 
             "peano4::parallel::TreeEntry", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::parallel::TreeEntry", 
             "send(int)", destination,tag,1 
           ); 
         } 
 	       tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
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
   
   
   
   void peano4::parallel::TreeEntry::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
MPI_Status status; 
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeEntry from node " 
            << source << ": " << tarch::parallel::MPIReturnValueToString(result); 
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
        tarch::parallel::Node::getInstance().getCommunicator(), sendRequestHandle 
      ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeEntry from node " 
             << source << ": " << tarch::parallel::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
      result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::parallel::Node::getInstance().writeTimeOutWarning( 
            "peano4::parallel::TreeEntry", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeEntry", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeEntry failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
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
      int result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
       if (result!=MPI_SUCCESS) { 
        std::ostringstream msg; 
        msg << "testing for finished receive task for peano4::parallel::TreeEntry failed: " 
            << tarch::parallel::MPIReturnValueToString(result); 
        _log.error("receive(int)", msg.str() ); 
      } 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::parallel::Node::getInstance().writeTimeOutWarning( 
            "peano4::parallel::TreeEntry", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeEntry", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeEntry failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeEntry from node " 
            << source << ": " << tarch::parallel::MPIReturnValueToString(result); 
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
   
   
   
   bool peano4::parallel::TreeEntry::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::parallel::TreeEntry::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::parallel::TreeEntryPacked::PersistentRecords::PersistentRecords() {
   
}


peano4::parallel::TreeEntryPacked::PersistentRecords::PersistentRecords(const int& id, const int& master):
_id(id),
_master(master) {
   
}


 int peano4::parallel::TreeEntryPacked::PersistentRecords::getId() const  {
   return _id;
}



 void peano4::parallel::TreeEntryPacked::PersistentRecords::setId(const int& id)  {
   _id = id;
}



 int peano4::parallel::TreeEntryPacked::PersistentRecords::getMaster() const  {
   return _master;
}



 void peano4::parallel::TreeEntryPacked::PersistentRecords::setMaster(const int& master)  {
   _master = master;
}


peano4::parallel::TreeEntryPacked::TreeEntryPacked() {
   
}


peano4::parallel::TreeEntryPacked::TreeEntryPacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._id, persistentRecords._master) {
   
}


peano4::parallel::TreeEntryPacked::TreeEntryPacked(const int& id, const int& master):
_persistentRecords(id, master) {
   
}


peano4::parallel::TreeEntryPacked::~TreeEntryPacked() { }


 int peano4::parallel::TreeEntryPacked::getId() const  {
   return _persistentRecords._id;
}



 void peano4::parallel::TreeEntryPacked::setId(const int& id)  {
   _persistentRecords._id = id;
}



 int peano4::parallel::TreeEntryPacked::getMaster() const  {
   return _persistentRecords._master;
}



 void peano4::parallel::TreeEntryPacked::setMaster(const int& master)  {
   _persistentRecords._master = master;
}




std::string peano4::parallel::TreeEntryPacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::parallel::TreeEntryPacked::toString (std::ostream& out) const {
   out << "("; 
   out << "id:" << getId();
   out << ",";
   out << "master:" << getMaster();
   out <<  ")";
}


peano4::parallel::TreeEntryPacked::PersistentRecords peano4::parallel::TreeEntryPacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::parallel::TreeEntry peano4::parallel::TreeEntryPacked::convert() const{
   return TreeEntry(
      getId(),
      getMaster()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::parallel::TreeEntryPacked::_log( "peano4::parallel::TreeEntryPacked" );
   
   MPI_Datatype peano4::parallel::TreeEntryPacked::Datatype = 0;
   MPI_Datatype peano4::parallel::TreeEntryPacked::FullDatatype = 0;
   
   
   void peano4::parallel::TreeEntryPacked::initDatatype() {
      {
         TreeEntryPacked dummyTreeEntryPacked[2];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 3;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //id
            , MPI_INT		 //master
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //id
            , 1		 //master
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._id))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._id))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._master))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._master))), 		&disp[1] );
         #endif
         #ifdef MPI2
         for (int i=1; i<Attributes; i++) {
         #else
         for (int i=1; i<Attributes-1; i++) {
         #endif
            assertion1( disp[i] > disp[i-1], i );
         }
         #ifdef MPI2
         for (int i=0; i<Attributes; i++) {
         #else
         for (int i=0; i<Attributes-1; i++) {
         #endif
            disp[i] = disp[i] - base; // should be MPI_Aint_diff(disp[i], base); but this is not supported by most MPI-2 implementations
            assertion4(disp[i]<static_cast<int>(sizeof(TreeEntryPacked)), i, disp[i], Attributes, sizeof(TreeEntryPacked));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[1]))), 		&disp[2] );
         disp[2] -= base;
         disp[2] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &TreeEntryPacked::Datatype );
         MPI_Type_commit( &TreeEntryPacked::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeEntryPacked::Datatype);
         MPI_Type_commit( &TreeEntryPacked::Datatype );
         #endif
         
      }
      {
         TreeEntryPacked dummyTreeEntryPacked[2];
         
         #ifdef MPI2
         const int Attributes = 2;
         #else
         const int Attributes = 3;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //id
            , MPI_INT		 //master
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //id
            , 1		 //master
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._id))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._id))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._master))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[0]._persistentRecords._master))), 		&disp[1] );
         #endif
         #ifdef MPI2
         for (int i=1; i<Attributes; i++) {
         #else
         for (int i=1; i<Attributes-1; i++) {
         #endif
            assertion1( disp[i] > disp[i-1], i );
         }
         #ifdef MPI2
         for (int i=0; i<Attributes; i++) {
         #else
         for (int i=0; i<Attributes-1; i++) {
         #endif
            disp[i] = disp[i] - base; // should be MPI_Aint_diff(disp[i], base); but this is not supported by most MPI-2 implementations
            assertion4(disp[i]<static_cast<int>(sizeof(TreeEntryPacked)), i, disp[i], Attributes, sizeof(TreeEntryPacked));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyTreeEntryPacked[1]))), 		&disp[2] );
         disp[2] -= base;
         disp[2] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &TreeEntryPacked::FullDatatype );
         MPI_Type_commit( &TreeEntryPacked::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &TreeEntryPacked::FullDatatype);
         MPI_Type_commit( &TreeEntryPacked::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::parallel::TreeEntryPacked::shutdownDatatype() {
      MPI_Type_free( &TreeEntryPacked::Datatype );
      MPI_Type_free( &TreeEntryPacked::FullDatatype );
      
   }
   
   void peano4::parallel::TreeEntryPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::parallel::TreeEntryPacked " 
             << toString() 
             << " to node " << destination 
             << ": " << tarch::parallel::MPIReturnValueToString(result); 
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
         tag, tarch::parallel::Node::getInstance().getCommunicator(), 
         sendRequestHandle  
       ); 
       if  (result!=MPI_SUCCESS) {  
         std::ostringstream msg;  
         msg << "was not able to send message peano4::parallel::TreeEntryPacked "  
             << toString() 
             << " to node " << destination 
             << ": " << tarch::parallel::MPIReturnValueToString(result);  
         _log.error( "send(int)",msg.str() );  
       }  
       result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
       while (!flag) { 
         if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp(); 
         if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp(); 
         result = MPI_Test( sendRequestHandle, &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
           std::ostringstream msg; 
           msg << "testing for finished send task for peano4::parallel::TreeEntryPacked " 
               << toString() 
               << " sent to node " << destination 
               << " failed: " << tarch::parallel::MPIReturnValueToString(result); 
           _log.error("send(int)", msg.str() ); 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() && 
           (clock()>timeOutWarning) && 
           (!triggeredTimeoutWarning) 
         ) { 
           tarch::parallel::Node::getInstance().writeTimeOutWarning( 
             "peano4::parallel::TreeEntryPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::parallel::TreeEntryPacked", 
             "send(int)", destination,tag,1 
           ); 
         } 
 	       tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
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
   
   
   
   void peano4::parallel::TreeEntryPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
MPI_Status status; 
switch (mode) { 
  case ExchangeMode::Blocking: 
    { 
      const int   result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeEntryPacked from node " 
            << source << ": " << tarch::parallel::MPIReturnValueToString(result); 
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
        tarch::parallel::Node::getInstance().getCommunicator(), sendRequestHandle 
      ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeEntryPacked from node " 
             << source << ": " << tarch::parallel::MPIReturnValueToString(result); 
        _log.error( "receive(int)", msg.str() ); 
      } 
      result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::parallel::Node::getInstance().writeTimeOutWarning( 
            "peano4::parallel::TreeEntryPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeEntryPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeEntryPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
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
      int result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
       if (result!=MPI_SUCCESS) { 
        std::ostringstream msg; 
        msg << "testing for finished receive task for peano4::parallel::TreeEntryPacked failed: " 
            << tarch::parallel::MPIReturnValueToString(result); 
        _log.error("receive(int)", msg.str() ); 
      } 
      while (!flag) { 
        if (timeOutWarning==-1)   timeOutWarning   = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp(); 
        if (timeOutShutdown==-1)  timeOutShutdown  = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp(); 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutWarningEnabled() && 
          (clock()>timeOutWarning) && 
          (!triggeredTimeoutWarning) 
        ) { 
          tarch::parallel::Node::getInstance().writeTimeOutWarning( 
            "peano4::parallel::TreeEntryPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::parallel::TreeEntryPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::parallel::TreeEntryPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::parallel::TreeEntryPacked from node " 
            << source << ": " << tarch::parallel::MPIReturnValueToString(result); 
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
   
   
   
   bool peano4::parallel::TreeEntryPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::parallel::TreeEntryPacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



