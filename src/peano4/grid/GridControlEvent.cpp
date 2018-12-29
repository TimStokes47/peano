#include "peano4/grid/GridControlEvent.h"

#if !defined(PeanoDebug)
   peano4::grid::GridControlEvent::PersistentRecords::PersistentRecords() {
      
   }
   
   
   peano4::grid::GridControlEvent::PersistentRecords::PersistentRecords(const RefinementControl& refinementControl):
   _refinementControl(refinementControl) {
      
   }
   
   
    peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEvent::PersistentRecords::getRefinementControl() const  {
      return _refinementControl;
   }
   
   
   
    void peano4::grid::GridControlEvent::PersistentRecords::setRefinementControl(const RefinementControl& refinementControl)  {
      _refinementControl = refinementControl;
   }
   
   
   peano4::grid::GridControlEvent::GridControlEvent() {
      
   }
   
   
   peano4::grid::GridControlEvent::GridControlEvent(const PersistentRecords& persistentRecords):
   _persistentRecords(persistentRecords._refinementControl) {
      
   }
   
   
   peano4::grid::GridControlEvent::GridControlEvent(const RefinementControl& refinementControl):
   _persistentRecords(refinementControl) {
      
   }
   
   
   peano4::grid::GridControlEvent::~GridControlEvent() { }
   
   
    peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEvent::getRefinementControl() const  {
      return _persistentRecords._refinementControl;
   }
   
   
   
    void peano4::grid::GridControlEvent::setRefinementControl(const RefinementControl& refinementControl)  {
      _persistentRecords._refinementControl = refinementControl;
   }
   
   
   std::string peano4::grid::GridControlEvent::toString(const RefinementControl& param) {
      switch (param) {
         case Refine: return "Refine";
         case Erase: return "Erase";
         case Keep: return "Keep";
      }
      return "undefined";
   }
   
   std::string peano4::grid::GridControlEvent::getRefinementControlMapping() {
      return "RefinementControl(Refine=0,Erase=1,Keep=2)";
   }
   
   
   std::string peano4::grid::GridControlEvent::toString() const {
      std::ostringstream stringstr;
      toString(stringstr);
      return stringstr.str();
   }
   
   void peano4::grid::GridControlEvent::toString (std::ostream& out) const {
      out << "("; 
      out << "refinementControl:" << toString(getRefinementControl());
      out <<  ")";
   }
   
   
   peano4::grid::GridControlEvent::PersistentRecords peano4::grid::GridControlEvent::getPersistentRecords() const {
      return _persistentRecords;
   }
   
   peano4::grid::GridControlEventPacked peano4::grid::GridControlEvent::convert() const{
      return GridControlEventPacked(
         getRefinementControl()
      );
   }
   
   #ifdef Parallel
      tarch::logging::Log peano4::grid::GridControlEvent::_log( "peano4::grid::GridControlEvent" );
      
      MPI_Datatype peano4::grid::GridControlEvent::Datatype = 0;
      MPI_Datatype peano4::grid::GridControlEvent::FullDatatype = 0;
      
      
      void peano4::grid::GridControlEvent::initDatatype() {
         {
            GridControlEvent dummyGridControlEvent[2];
            
            #ifdef MPI2
            const int Attributes = 1;
            #else
            const int Attributes = 2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //refinementControl
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //refinementControl
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridControlEvent)), i, disp[i], Attributes, sizeof(GridControlEvent));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), 		&disp[1] );
            disp[1] -= base;
            disp[1] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEvent::Datatype );
            MPI_Type_commit( &GridControlEvent::Datatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEvent::Datatype);
            MPI_Type_commit( &GridControlEvent::Datatype );
            #endif
            
         }
         {
            GridControlEvent dummyGridControlEvent[2];
            
            #ifdef MPI2
            const int Attributes = 1;
            #else
            const int Attributes = 2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //refinementControl
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //refinementControl
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridControlEvent)), i, disp[i], Attributes, sizeof(GridControlEvent));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), 		&disp[1] );
            disp[1] -= base;
            disp[1] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEvent::FullDatatype );
            MPI_Type_commit( &GridControlEvent::FullDatatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEvent::FullDatatype);
            MPI_Type_commit( &GridControlEvent::FullDatatype );
            #endif
            
         }
         
      }
      
      
      void peano4::grid::GridControlEvent::shutdownDatatype() {
         MPI_Type_free( &GridControlEvent::Datatype );
         MPI_Type_free( &GridControlEvent::FullDatatype );
         
      }
      
      void peano4::grid::GridControlEvent::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
         // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridControlEvent " 
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
         msg << "was not able to send message peano4::grid::GridControlEvent "  
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
           msg << "testing for finished send task for peano4::grid::GridControlEvent " 
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
             "peano4::grid::GridControlEvent", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridControlEvent", 
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
      
      
      
      void peano4::grid::GridControlEvent::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
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
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
      
      
      
      bool peano4::grid::GridControlEvent::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
      
      int peano4::grid::GridControlEvent::getSenderRank() const {
         assertion( _senderDestinationRank!=-1 );
         return _senderDestinationRank;
         
      }
   #endif
   
   
   peano4::grid::GridControlEventPacked::PersistentRecords::PersistentRecords() {
      
   }
   
   
   peano4::grid::GridControlEventPacked::PersistentRecords::PersistentRecords(const RefinementControl& refinementControl):
   _refinementControl(refinementControl) {
      
   }
   
   
    peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEventPacked::PersistentRecords::getRefinementControl() const  {
      return _refinementControl;
   }
   
   
   
    void peano4::grid::GridControlEventPacked::PersistentRecords::setRefinementControl(const RefinementControl& refinementControl)  {
      _refinementControl = refinementControl;
   }
   
   
   peano4::grid::GridControlEventPacked::GridControlEventPacked() {
      
   }
   
   
   peano4::grid::GridControlEventPacked::GridControlEventPacked(const PersistentRecords& persistentRecords):
   _persistentRecords(persistentRecords._refinementControl) {
      
   }
   
   
   peano4::grid::GridControlEventPacked::GridControlEventPacked(const RefinementControl& refinementControl):
   _persistentRecords(refinementControl) {
      
   }
   
   
   peano4::grid::GridControlEventPacked::~GridControlEventPacked() { }
   
   
    peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEventPacked::getRefinementControl() const  {
      return _persistentRecords._refinementControl;
   }
   
   
   
    void peano4::grid::GridControlEventPacked::setRefinementControl(const RefinementControl& refinementControl)  {
      _persistentRecords._refinementControl = refinementControl;
   }
   
   
   std::string peano4::grid::GridControlEventPacked::toString(const RefinementControl& param) {
      return peano4::grid::GridControlEvent::toString(param);
   }
   
   std::string peano4::grid::GridControlEventPacked::getRefinementControlMapping() {
      return peano4::grid::GridControlEvent::getRefinementControlMapping();
   }
   
   
   
   std::string peano4::grid::GridControlEventPacked::toString() const {
      std::ostringstream stringstr;
      toString(stringstr);
      return stringstr.str();
   }
   
   void peano4::grid::GridControlEventPacked::toString (std::ostream& out) const {
      out << "("; 
      out << "refinementControl:" << toString(getRefinementControl());
      out <<  ")";
   }
   
   
   peano4::grid::GridControlEventPacked::PersistentRecords peano4::grid::GridControlEventPacked::getPersistentRecords() const {
      return _persistentRecords;
   }
   
   peano4::grid::GridControlEvent peano4::grid::GridControlEventPacked::convert() const{
      return GridControlEvent(
         getRefinementControl()
      );
   }
   
   #ifdef Parallel
      tarch::logging::Log peano4::grid::GridControlEventPacked::_log( "peano4::grid::GridControlEventPacked" );
      
      MPI_Datatype peano4::grid::GridControlEventPacked::Datatype = 0;
      MPI_Datatype peano4::grid::GridControlEventPacked::FullDatatype = 0;
      
      
      void peano4::grid::GridControlEventPacked::initDatatype() {
         {
            GridControlEventPacked dummyGridControlEventPacked[2];
            
            #ifdef MPI2
            const int Attributes = 1;
            #else
            const int Attributes = 2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //refinementControl
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //refinementControl
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridControlEventPacked)), i, disp[i], Attributes, sizeof(GridControlEventPacked));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), 		&disp[1] );
            disp[1] -= base;
            disp[1] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEventPacked::Datatype );
            MPI_Type_commit( &GridControlEventPacked::Datatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEventPacked::Datatype);
            MPI_Type_commit( &GridControlEventPacked::Datatype );
            #endif
            
         }
         {
            GridControlEventPacked dummyGridControlEventPacked[2];
            
            #ifdef MPI2
            const int Attributes = 1;
            #else
            const int Attributes = 2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //refinementControl
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //refinementControl
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridControlEventPacked)), i, disp[i], Attributes, sizeof(GridControlEventPacked));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), 		&disp[1] );
            disp[1] -= base;
            disp[1] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEventPacked::FullDatatype );
            MPI_Type_commit( &GridControlEventPacked::FullDatatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEventPacked::FullDatatype);
            MPI_Type_commit( &GridControlEventPacked::FullDatatype );
            #endif
            
         }
         
      }
      
      
      void peano4::grid::GridControlEventPacked::shutdownDatatype() {
         MPI_Type_free( &GridControlEventPacked::Datatype );
         MPI_Type_free( &GridControlEventPacked::FullDatatype );
         
      }
      
      void peano4::grid::GridControlEventPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
         // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridControlEventPacked " 
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
         msg << "was not able to send message peano4::grid::GridControlEventPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridControlEventPacked " 
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
             "peano4::grid::GridControlEventPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridControlEventPacked", 
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
      
      
      
      void peano4::grid::GridControlEventPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
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
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
      
      
      
      bool peano4::grid::GridControlEventPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
      
      int peano4::grid::GridControlEventPacked::getSenderRank() const {
         assertion( _senderDestinationRank!=-1 );
         return _senderDestinationRank;
         
      }
   #endif
   
   
   #elif defined(PeanoDebug)
      peano4::grid::GridControlEvent::PersistentRecords::PersistentRecords() {
         
      }
      
      
      peano4::grid::GridControlEvent::PersistentRecords::PersistentRecords(const RefinementControl& refinementControl, const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h):
      _refinementControl(refinementControl),
      _level(level),
      _x(x),
      _h(h) {
         
      }
      
      
       peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEvent::PersistentRecords::getRefinementControl() const  {
         return _refinementControl;
      }
      
      
      
       void peano4::grid::GridControlEvent::PersistentRecords::setRefinementControl(const RefinementControl& refinementControl)  {
         _refinementControl = refinementControl;
      }
      
      
      
       int peano4::grid::GridControlEvent::PersistentRecords::getLevel() const  {
         return _level;
      }
      
      
      
       void peano4::grid::GridControlEvent::PersistentRecords::setLevel(const int& level)  {
         _level = level;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::PersistentRecords::getX() const  {
         return _x;
      }
      
      
      
       void peano4::grid::GridControlEvent::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _x = (x);
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::PersistentRecords::getH() const  {
         return _h;
      }
      
      
      
       void peano4::grid::GridControlEvent::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
         _h = (h);
      }
      
      
      peano4::grid::GridControlEvent::GridControlEvent() {
         
      }
      
      
      peano4::grid::GridControlEvent::GridControlEvent(const PersistentRecords& persistentRecords):
      _persistentRecords(persistentRecords._refinementControl, persistentRecords._level, persistentRecords._x, persistentRecords._h) {
         
      }
      
      
      peano4::grid::GridControlEvent::GridControlEvent(const RefinementControl& refinementControl, const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h):
      _persistentRecords(refinementControl, level, x, h) {
         
      }
      
      
      peano4::grid::GridControlEvent::~GridControlEvent() { }
      
      
       peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEvent::getRefinementControl() const  {
         return _persistentRecords._refinementControl;
      }
      
      
      
       void peano4::grid::GridControlEvent::setRefinementControl(const RefinementControl& refinementControl)  {
         _persistentRecords._refinementControl = refinementControl;
      }
      
      
      
       int peano4::grid::GridControlEvent::getLevel() const  {
         return _persistentRecords._level;
      }
      
      
      
       void peano4::grid::GridControlEvent::setLevel(const int& level)  {
         _persistentRecords._level = level;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::getX() const  {
         return _persistentRecords._x;
      }
      
      
      
       void peano4::grid::GridControlEvent::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _persistentRecords._x = (x);
      }
      
      
      
       double peano4::grid::GridControlEvent::getX(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         return _persistentRecords._x[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridControlEvent::setX(int elementIndex, const double& x)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         _persistentRecords._x[elementIndex]= x;
         
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::getH() const  {
         return _persistentRecords._h;
      }
      
      
      
       void peano4::grid::GridControlEvent::setH(const tarch::la::Vector<Dimensions,double>& h)  {
         _persistentRecords._h = (h);
      }
      
      
      
       double peano4::grid::GridControlEvent::getH(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         return _persistentRecords._h[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridControlEvent::setH(int elementIndex, const double& h)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         _persistentRecords._h[elementIndex]= h;
         
      }
      
      
      std::string peano4::grid::GridControlEvent::toString(const RefinementControl& param) {
         switch (param) {
            case Refine: return "Refine";
            case Erase: return "Erase";
            case Keep: return "Keep";
         }
         return "undefined";
      }
      
      std::string peano4::grid::GridControlEvent::getRefinementControlMapping() {
         return "RefinementControl(Refine=0,Erase=1,Keep=2)";
      }
      
      
      std::string peano4::grid::GridControlEvent::toString() const {
         std::ostringstream stringstr;
         toString(stringstr);
         return stringstr.str();
      }
      
      void peano4::grid::GridControlEvent::toString (std::ostream& out) const {
         out << "("; 
         out << "refinementControl:" << toString(getRefinementControl());
         out << ",";
         out << "level:" << getLevel();
         out << ",";
         out << "x:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(Dimensions-1) << "]";
         out << ",";
         out << "h:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getH(i) << ",";
   }
   out << getH(Dimensions-1) << "]";
         out <<  ")";
      }
      
      
      peano4::grid::GridControlEvent::PersistentRecords peano4::grid::GridControlEvent::getPersistentRecords() const {
         return _persistentRecords;
      }
      
      peano4::grid::GridControlEventPacked peano4::grid::GridControlEvent::convert() const{
         return GridControlEventPacked(
            getRefinementControl(),
            getLevel(),
            getX(),
            getH()
         );
      }
      
      #ifdef Parallel
         tarch::logging::Log peano4::grid::GridControlEvent::_log( "peano4::grid::GridControlEvent" );
         
         MPI_Datatype peano4::grid::GridControlEvent::Datatype = 0;
         MPI_Datatype peano4::grid::GridControlEvent::FullDatatype = 0;
         
         
         void peano4::grid::GridControlEvent::initDatatype() {
            {
               GridControlEvent dummyGridControlEvent[2];
               
               #ifdef MPI2
               const int Attributes = 4;
               #else
               const int Attributes = 5;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //refinementControl
                  , MPI_INT		 //level
                  , MPI_DOUBLE		 //x
                  , MPI_DOUBLE		 //h
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //refinementControl
                  , 1		 //level
                  , Dimensions		 //x
                  , Dimensions		 //h
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._level))), 		&disp[1] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._level))), 		&disp[1] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._x[0]))), 		&disp[2] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._x[0]))), 		&disp[2] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[3] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[3] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridControlEvent)), i, disp[i], Attributes, sizeof(GridControlEvent));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), 		&disp[4] );
               disp[4] -= base;
               disp[4] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEvent::Datatype );
               MPI_Type_commit( &GridControlEvent::Datatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEvent::Datatype);
               MPI_Type_commit( &GridControlEvent::Datatype );
               #endif
               
            }
            {
               GridControlEvent dummyGridControlEvent[2];
               
               #ifdef MPI2
               const int Attributes = 4;
               #else
               const int Attributes = 5;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //refinementControl
                  , MPI_INT		 //level
                  , MPI_DOUBLE		 //x
                  , MPI_DOUBLE		 //h
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //refinementControl
                  , 1		 //level
                  , Dimensions		 //x
                  , Dimensions		 //h
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._level))), 		&disp[1] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._level))), 		&disp[1] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._x[0]))), 		&disp[2] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._x[0]))), 		&disp[2] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[3] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[3] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridControlEvent)), i, disp[i], Attributes, sizeof(GridControlEvent));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), 		&disp[4] );
               disp[4] -= base;
               disp[4] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEvent::FullDatatype );
               MPI_Type_commit( &GridControlEvent::FullDatatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEvent::FullDatatype);
               MPI_Type_commit( &GridControlEvent::FullDatatype );
               #endif
               
            }
            
         }
         
         
         void peano4::grid::GridControlEvent::shutdownDatatype() {
            MPI_Type_free( &GridControlEvent::Datatype );
            MPI_Type_free( &GridControlEvent::FullDatatype );
            
         }
         
         void peano4::grid::GridControlEvent::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
            // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridControlEvent " 
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
         msg << "was not able to send message peano4::grid::GridControlEvent "  
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
           msg << "testing for finished send task for peano4::grid::GridControlEvent " 
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
             "peano4::grid::GridControlEvent", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridControlEvent", 
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
         
         
         
         void peano4::grid::GridControlEvent::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
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
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
         
         
         
         bool peano4::grid::GridControlEvent::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
         
         int peano4::grid::GridControlEvent::getSenderRank() const {
            assertion( _senderDestinationRank!=-1 );
            return _senderDestinationRank;
            
         }
      #endif
      
      
      peano4::grid::GridControlEventPacked::PersistentRecords::PersistentRecords() {
         
      }
      
      
      peano4::grid::GridControlEventPacked::PersistentRecords::PersistentRecords(const RefinementControl& refinementControl, const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h):
      _refinementControl(refinementControl) {
         setLevel(level);
         setX(x);
         setH(h);
         
      }
      
      
       peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEventPacked::PersistentRecords::getRefinementControl() const  {
         return _refinementControl;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::PersistentRecords::setRefinementControl(const RefinementControl& refinementControl)  {
         _refinementControl = refinementControl;
      }
      
      
      
       int peano4::grid::GridControlEventPacked::PersistentRecords::getLevel() const  {
         return _level;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::PersistentRecords::setLevel(const int& level)  {
         _level = level;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::PersistentRecords::getX() const  {
         return _x;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _x = (x);
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::PersistentRecords::getH() const  {
         return _h;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
         _h = (h);
      }
      
      
      peano4::grid::GridControlEventPacked::GridControlEventPacked() {
         
      }
      
      
      peano4::grid::GridControlEventPacked::GridControlEventPacked(const PersistentRecords& persistentRecords):
      _persistentRecords(persistentRecords._refinementControl, persistentRecords.getLevel(), persistentRecords.getX(), persistentRecords.getH()) {
         
      }
      
      
      peano4::grid::GridControlEventPacked::GridControlEventPacked(const RefinementControl& refinementControl, const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h):
      _persistentRecords(refinementControl, level, x, h) {
         
      }
      
      
      peano4::grid::GridControlEventPacked::~GridControlEventPacked() { }
      
      
       peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEventPacked::getRefinementControl() const  {
         return _persistentRecords._refinementControl;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::setRefinementControl(const RefinementControl& refinementControl)  {
         _persistentRecords._refinementControl = refinementControl;
      }
      
      
      
       int peano4::grid::GridControlEventPacked::getLevel() const  {
         return _persistentRecords._level;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::setLevel(const int& level)  {
         _persistentRecords._level = level;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::getX() const  {
         return _persistentRecords._x;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _persistentRecords._x = (x);
      }
      
      
      
       double peano4::grid::GridControlEventPacked::getX(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         return _persistentRecords._x[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridControlEventPacked::setX(int elementIndex, const double& x)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         _persistentRecords._x[elementIndex]= x;
         
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::getH() const  {
         return _persistentRecords._h;
      }
      
      
      
       void peano4::grid::GridControlEventPacked::setH(const tarch::la::Vector<Dimensions,double>& h)  {
         _persistentRecords._h = (h);
      }
      
      
      
       double peano4::grid::GridControlEventPacked::getH(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         return _persistentRecords._h[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridControlEventPacked::setH(int elementIndex, const double& h)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         _persistentRecords._h[elementIndex]= h;
         
      }
      
      
      std::string peano4::grid::GridControlEventPacked::toString(const RefinementControl& param) {
         return peano4::grid::GridControlEvent::toString(param);
      }
      
      std::string peano4::grid::GridControlEventPacked::getRefinementControlMapping() {
         return peano4::grid::GridControlEvent::getRefinementControlMapping();
      }
      
      
      
      std::string peano4::grid::GridControlEventPacked::toString() const {
         std::ostringstream stringstr;
         toString(stringstr);
         return stringstr.str();
      }
      
      void peano4::grid::GridControlEventPacked::toString (std::ostream& out) const {
         out << "("; 
         out << "refinementControl:" << toString(getRefinementControl());
         out << ",";
         out << "level:" << getLevel();
         out << ",";
         out << "x:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(Dimensions-1) << "]";
         out << ",";
         out << "h:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getH(i) << ",";
   }
   out << getH(Dimensions-1) << "]";
         out <<  ")";
      }
      
      
      peano4::grid::GridControlEventPacked::PersistentRecords peano4::grid::GridControlEventPacked::getPersistentRecords() const {
         return _persistentRecords;
      }
      
      peano4::grid::GridControlEvent peano4::grid::GridControlEventPacked::convert() const{
         return GridControlEvent(
            getRefinementControl(),
            getLevel(),
            getX(),
            getH()
         );
      }
      
      #ifdef Parallel
         tarch::logging::Log peano4::grid::GridControlEventPacked::_log( "peano4::grid::GridControlEventPacked" );
         
         MPI_Datatype peano4::grid::GridControlEventPacked::Datatype = 0;
         MPI_Datatype peano4::grid::GridControlEventPacked::FullDatatype = 0;
         
         
         void peano4::grid::GridControlEventPacked::initDatatype() {
            {
               GridControlEventPacked dummyGridControlEventPacked[2];
               
               #ifdef MPI2
               const int Attributes = 4;
               #else
               const int Attributes = 5;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //refinementControl
                  , MPI_INT		 //level
                  , MPI_DOUBLE		 //x
                  , MPI_DOUBLE		 //h
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //refinementControl
                  , 1		 //level
                  , Dimensions		 //x
                  , Dimensions		 //h
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._level))), 		&disp[1] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._level))), 		&disp[1] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._x[0]))), 		&disp[2] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._x[0]))), 		&disp[2] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[3] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[3] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridControlEventPacked)), i, disp[i], Attributes, sizeof(GridControlEventPacked));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), 		&disp[4] );
               disp[4] -= base;
               disp[4] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEventPacked::Datatype );
               MPI_Type_commit( &GridControlEventPacked::Datatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEventPacked::Datatype);
               MPI_Type_commit( &GridControlEventPacked::Datatype );
               #endif
               
            }
            {
               GridControlEventPacked dummyGridControlEventPacked[2];
               
               #ifdef MPI2
               const int Attributes = 4;
               #else
               const int Attributes = 5;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //refinementControl
                  , MPI_INT		 //level
                  , MPI_DOUBLE		 //x
                  , MPI_DOUBLE		 //h
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //refinementControl
                  , 1		 //level
                  , Dimensions		 //x
                  , Dimensions		 //h
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[0] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._level))), 		&disp[1] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._level))), 		&disp[1] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._x[0]))), 		&disp[2] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._x[0]))), 		&disp[2] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[3] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[3] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridControlEventPacked)), i, disp[i], Attributes, sizeof(GridControlEventPacked));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), 		&disp[4] );
               disp[4] -= base;
               disp[4] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridControlEventPacked::FullDatatype );
               MPI_Type_commit( &GridControlEventPacked::FullDatatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEventPacked::FullDatatype);
               MPI_Type_commit( &GridControlEventPacked::FullDatatype );
               #endif
               
            }
            
         }
         
         
         void peano4::grid::GridControlEventPacked::shutdownDatatype() {
            MPI_Type_free( &GridControlEventPacked::Datatype );
            MPI_Type_free( &GridControlEventPacked::FullDatatype );
            
         }
         
         void peano4::grid::GridControlEventPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
            // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridControlEventPacked " 
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
         msg << "was not able to send message peano4::grid::GridControlEventPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridControlEventPacked " 
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
             "peano4::grid::GridControlEventPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridControlEventPacked", 
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
         
         
         
         void peano4::grid::GridControlEventPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
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
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
         
         
         
         bool peano4::grid::GridControlEventPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
         
         int peano4::grid::GridControlEventPacked::getSenderRank() const {
            assertion( _senderDestinationRank!=-1 );
            return _senderDestinationRank;
            
         }
      #endif
      
      
      
   
#endif


