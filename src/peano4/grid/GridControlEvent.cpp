#include "peano4/grid/GridControlEvent.h"

peano4::grid::GridControlEvent::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridControlEvent::PersistentRecords::PersistentRecords(const RefinementControl& refinementControl, const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width, const tarch::la::Vector<Dimensions,double>& h):
_refinementControl(refinementControl),
_offset(offset),
_width(width),
_h(h) {
   
}


 peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEvent::PersistentRecords::getRefinementControl() const  {
   return _refinementControl;
}



 void peano4::grid::GridControlEvent::PersistentRecords::setRefinementControl(const RefinementControl& refinementControl)  {
   _refinementControl = refinementControl;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::PersistentRecords::getOffset() const  {
   return _offset;
}



 void peano4::grid::GridControlEvent::PersistentRecords::setOffset(const tarch::la::Vector<Dimensions,double>& offset)  {
   _offset = (offset);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::PersistentRecords::getWidth() const  {
   return _width;
}



 void peano4::grid::GridControlEvent::PersistentRecords::setWidth(const tarch::la::Vector<Dimensions,double>& width)  {
   _width = (width);
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
_persistentRecords(persistentRecords._refinementControl, persistentRecords._offset, persistentRecords._width, persistentRecords._h) {
   
}


peano4::grid::GridControlEvent::GridControlEvent(const RefinementControl& refinementControl, const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width, const tarch::la::Vector<Dimensions,double>& h):
_persistentRecords(refinementControl, offset, width, h) {
   
}


peano4::grid::GridControlEvent::~GridControlEvent() { }


 peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEvent::getRefinementControl() const  {
   return _persistentRecords._refinementControl;
}



 void peano4::grid::GridControlEvent::setRefinementControl(const RefinementControl& refinementControl)  {
   _persistentRecords._refinementControl = refinementControl;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::getOffset() const  {
   return _persistentRecords._offset;
}



 void peano4::grid::GridControlEvent::setOffset(const tarch::la::Vector<Dimensions,double>& offset)  {
   _persistentRecords._offset = (offset);
}



 double peano4::grid::GridControlEvent::getOffset(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._offset[elementIndex];
   
}



 void peano4::grid::GridControlEvent::setOffset(int elementIndex, const double& offset)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._offset[elementIndex]= offset;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEvent::getWidth() const  {
   return _persistentRecords._width;
}



 void peano4::grid::GridControlEvent::setWidth(const tarch::la::Vector<Dimensions,double>& width)  {
   _persistentRecords._width = (width);
}



 double peano4::grid::GridControlEvent::getWidth(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._width[elementIndex];
   
}



 void peano4::grid::GridControlEvent::setWidth(int elementIndex, const double& width)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._width[elementIndex]= width;
   
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
   }
   return "undefined";
}

std::string peano4::grid::GridControlEvent::getRefinementControlMapping() {
   return "RefinementControl(Refine=0,Erase=1)";
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
   out << "offset:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getOffset(i) << ",";
   }
   out << getOffset(Dimensions-1) << "]";
   out << ",";
   out << "width:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getWidth(i) << ",";
   }
   out << getWidth(Dimensions-1) << "]";
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
      getOffset(),
      getWidth(),
      getH()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridControlEvent::_log( "peano4::grid::GridControlEvent" );
   
   MPI_Datatype peano4::grid::GridControlEvent::Datatype = 0;
   MPI_Datatype peano4::grid::GridControlEvent::FullDatatype = 0;
   
   
   void peano4::grid::GridControlEvent::initDatatype() {
      {
         GridControlEvent dummyGridControlEvent[16];
         
         #ifdef MPI2
         const int Attributes = 4;
         #else
         const int Attributes = 4+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //refinementControl
            , MPI_DOUBLE		 //offset
            , MPI_DOUBLE		 //width
            , MPI_DOUBLE		 //h
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //refinementControl
            , Dimensions		 //offset
            , Dimensions		 //width
            , Dimensions		 //h
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridControlEvent::Datatype );
         MPI_Type_commit( &GridControlEvent::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEvent::Datatype);
         MPI_Type_commit( &GridControlEvent::Datatype );
         #endif
         
      }
      {
         GridControlEvent dummyGridControlEvent[16];
         
         #ifdef MPI2
         const int Attributes = 4;
         #else
         const int Attributes = 4+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //refinementControl
            , MPI_DOUBLE		 //offset
            , MPI_DOUBLE		 //width
            , MPI_DOUBLE		 //h
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //refinementControl
            , Dimensions		 //offset
            , Dimensions		 //width
            , Dimensions		 //h
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEvent[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridControlEvent::FullDatatype );
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
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridControlEvent " 
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
         msg << "was not able to send message peano4::grid::GridControlEvent "  
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
           msg << "testing for finished send task for peano4::grid::GridControlEvent " 
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
             "peano4::grid::GridControlEvent", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridControlEvent", 
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
   
   
   
   void peano4::grid::GridControlEvent::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
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
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEvent failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridControlEvent from node " 
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
   
   
   
   bool peano4::grid::GridControlEvent::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridControlEvent::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::grid::GridControlEventPacked::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridControlEventPacked::PersistentRecords::PersistentRecords(const RefinementControl& refinementControl, const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width, const tarch::la::Vector<Dimensions,double>& h):
_refinementControl(refinementControl),
_offset(offset),
_width(width),
_h(h) {
   
}


 peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEventPacked::PersistentRecords::getRefinementControl() const  {
   return _refinementControl;
}



 void peano4::grid::GridControlEventPacked::PersistentRecords::setRefinementControl(const RefinementControl& refinementControl)  {
   _refinementControl = refinementControl;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::PersistentRecords::getOffset() const  {
   return _offset;
}



 void peano4::grid::GridControlEventPacked::PersistentRecords::setOffset(const tarch::la::Vector<Dimensions,double>& offset)  {
   _offset = (offset);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::PersistentRecords::getWidth() const  {
   return _width;
}



 void peano4::grid::GridControlEventPacked::PersistentRecords::setWidth(const tarch::la::Vector<Dimensions,double>& width)  {
   _width = (width);
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
_persistentRecords(persistentRecords._refinementControl, persistentRecords._offset, persistentRecords._width, persistentRecords._h) {
   
}


peano4::grid::GridControlEventPacked::GridControlEventPacked(const RefinementControl& refinementControl, const tarch::la::Vector<Dimensions,double>& offset, const tarch::la::Vector<Dimensions,double>& width, const tarch::la::Vector<Dimensions,double>& h):
_persistentRecords(refinementControl, offset, width, h) {
   
}


peano4::grid::GridControlEventPacked::~GridControlEventPacked() { }


 peano4::grid::GridControlEvent::RefinementControl peano4::grid::GridControlEventPacked::getRefinementControl() const  {
   return _persistentRecords._refinementControl;
}



 void peano4::grid::GridControlEventPacked::setRefinementControl(const RefinementControl& refinementControl)  {
   _persistentRecords._refinementControl = refinementControl;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::getOffset() const  {
   return _persistentRecords._offset;
}



 void peano4::grid::GridControlEventPacked::setOffset(const tarch::la::Vector<Dimensions,double>& offset)  {
   _persistentRecords._offset = (offset);
}



 double peano4::grid::GridControlEventPacked::getOffset(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._offset[elementIndex];
   
}



 void peano4::grid::GridControlEventPacked::setOffset(int elementIndex, const double& offset)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._offset[elementIndex]= offset;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridControlEventPacked::getWidth() const  {
   return _persistentRecords._width;
}



 void peano4::grid::GridControlEventPacked::setWidth(const tarch::la::Vector<Dimensions,double>& width)  {
   _persistentRecords._width = (width);
}



 double peano4::grid::GridControlEventPacked::getWidth(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._width[elementIndex];
   
}



 void peano4::grid::GridControlEventPacked::setWidth(int elementIndex, const double& width)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._width[elementIndex]= width;
   
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
   out << "offset:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getOffset(i) << ",";
   }
   out << getOffset(Dimensions-1) << "]";
   out << ",";
   out << "width:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getWidth(i) << ",";
   }
   out << getWidth(Dimensions-1) << "]";
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
      getOffset(),
      getWidth(),
      getH()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridControlEventPacked::_log( "peano4::grid::GridControlEventPacked" );
   
   MPI_Datatype peano4::grid::GridControlEventPacked::Datatype = 0;
   MPI_Datatype peano4::grid::GridControlEventPacked::FullDatatype = 0;
   
   
   void peano4::grid::GridControlEventPacked::initDatatype() {
      {
         GridControlEventPacked dummyGridControlEventPacked[16];
         
         #ifdef MPI2
         const int Attributes = 4;
         #else
         const int Attributes = 4+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //refinementControl
            , MPI_DOUBLE		 //offset
            , MPI_DOUBLE		 //width
            , MPI_DOUBLE		 //h
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //refinementControl
            , Dimensions		 //offset
            , Dimensions		 //width
            , Dimensions		 //h
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridControlEventPacked::Datatype );
         MPI_Type_commit( &GridControlEventPacked::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridControlEventPacked::Datatype);
         MPI_Type_commit( &GridControlEventPacked::Datatype );
         #endif
         
      }
      {
         GridControlEventPacked dummyGridControlEventPacked[16];
         
         #ifdef MPI2
         const int Attributes = 4;
         #else
         const int Attributes = 4+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //refinementControl
            , MPI_DOUBLE		 //offset
            , MPI_DOUBLE		 //width
            , MPI_DOUBLE		 //h
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //refinementControl
            , Dimensions		 //offset
            , Dimensions		 //width
            , Dimensions		 //h
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._refinementControl))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._offset[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._width[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridControlEventPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridControlEventPacked::FullDatatype );
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
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridControlEventPacked " 
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
         msg << "was not able to send message peano4::grid::GridControlEventPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridControlEventPacked " 
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
             "peano4::grid::GridControlEventPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridControlEventPacked", 
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
   
   
   
   void peano4::grid::GridControlEventPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
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
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridControlEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridControlEventPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridControlEventPacked from node " 
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
   
   
   
   bool peano4::grid::GridControlEventPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridControlEventPacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



