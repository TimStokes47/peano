#include "AutomatonState.h"

peano4::grid::AutomatonState::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::AutomatonState::PersistentRecords::PersistentRecords(const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& inverted, const std::bitset<Dimensions>& evenFlags, const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber):
_level(level),
_x(x),
_h(h),
_inverted(inverted),
_evenFlags(evenFlags),
_accessNumber(accessNumber) {
   
}


 int peano4::grid::AutomatonState::PersistentRecords::getLevel() const  {
   return _level;
}



 void peano4::grid::AutomatonState::PersistentRecords::setLevel(const int& level)  {
   _level = level;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonState::PersistentRecords::getX() const  {
   return _x;
}



 void peano4::grid::AutomatonState::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _x = (x);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonState::PersistentRecords::getH() const  {
   return _h;
}



 void peano4::grid::AutomatonState::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _h = (h);
}



 bool peano4::grid::AutomatonState::PersistentRecords::getInverted() const  {
   return _inverted;
}



 void peano4::grid::AutomatonState::PersistentRecords::setInverted(const bool& inverted)  {
   _inverted = inverted;
}



 std::bitset<Dimensions> peano4::grid::AutomatonState::PersistentRecords::getEvenFlags() const  {
   return _evenFlags;
}



 void peano4::grid::AutomatonState::PersistentRecords::setEvenFlags(const std::bitset<Dimensions>& evenFlags)  {
   _evenFlags = (evenFlags);
}



 tarch::la::Vector<DimensionsTimesTwo,short int> peano4::grid::AutomatonState::PersistentRecords::getAccessNumber() const  {
   return _accessNumber;
}



 void peano4::grid::AutomatonState::PersistentRecords::setAccessNumber(const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber)  {
   _accessNumber = (accessNumber);
}


peano4::grid::AutomatonState::AutomatonState() {
   
}


peano4::grid::AutomatonState::AutomatonState(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._level, persistentRecords._x, persistentRecords._h, persistentRecords._inverted, persistentRecords._evenFlags, persistentRecords._accessNumber) {
   
}


peano4::grid::AutomatonState::AutomatonState(const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& inverted, const std::bitset<Dimensions>& evenFlags, const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber):
_persistentRecords(level, x, h, inverted, evenFlags, accessNumber) {
   
}


peano4::grid::AutomatonState::~AutomatonState() { }


 int peano4::grid::AutomatonState::getLevel() const  {
   return _persistentRecords._level;
}



 void peano4::grid::AutomatonState::setLevel(const int& level)  {
   _persistentRecords._level = level;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonState::getX() const  {
   return _persistentRecords._x;
}



 void peano4::grid::AutomatonState::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _persistentRecords._x = (x);
}



 double peano4::grid::AutomatonState::getX(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._x[elementIndex];
   
}



 void peano4::grid::AutomatonState::setX(int elementIndex, const double& x)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._x[elementIndex]= x;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonState::getH() const  {
   return _persistentRecords._h;
}



 void peano4::grid::AutomatonState::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _persistentRecords._h = (h);
}



 double peano4::grid::AutomatonState::getH(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._h[elementIndex];
   
}



 void peano4::grid::AutomatonState::setH(int elementIndex, const double& h)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._h[elementIndex]= h;
   
}



 bool peano4::grid::AutomatonState::getInverted() const  {
   return _persistentRecords._inverted;
}



 void peano4::grid::AutomatonState::setInverted(const bool& inverted)  {
   _persistentRecords._inverted = inverted;
}



 std::bitset<Dimensions> peano4::grid::AutomatonState::getEvenFlags() const  {
   return _persistentRecords._evenFlags;
}



 void peano4::grid::AutomatonState::setEvenFlags(const std::bitset<Dimensions>& evenFlags)  {
   _persistentRecords._evenFlags = (evenFlags);
}



 bool peano4::grid::AutomatonState::getEvenFlags(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._evenFlags[elementIndex];
   
}



 void peano4::grid::AutomatonState::setEvenFlags(int elementIndex, const bool& evenFlags)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._evenFlags[elementIndex]= evenFlags;
   
}



 void peano4::grid::AutomatonState::flipEvenFlags(int elementIndex)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._evenFlags.flip(elementIndex);
}



 tarch::la::Vector<DimensionsTimesTwo,short int> peano4::grid::AutomatonState::getAccessNumber() const  {
   return _persistentRecords._accessNumber;
}



 void peano4::grid::AutomatonState::setAccessNumber(const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber)  {
   _persistentRecords._accessNumber = (accessNumber);
}



 short int peano4::grid::AutomatonState::getAccessNumber(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<DimensionsTimesTwo);
   return _persistentRecords._accessNumber[elementIndex];
   
}



 void peano4::grid::AutomatonState::setAccessNumber(int elementIndex, const short int& accessNumber)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<DimensionsTimesTwo);
   _persistentRecords._accessNumber[elementIndex]= accessNumber;
   
}




std::string peano4::grid::AutomatonState::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::AutomatonState::toString (std::ostream& out) const {
   out << "("; 
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
   out << ",";
   out << "inverted:" << getInverted();
   out << ",";
   out << "evenFlags:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getEvenFlags(i) << ",";
   }
   out << getEvenFlags(Dimensions-1) << "]";
   out << ",";
   out << "accessNumber:[";
   for (int i = 0; i < DimensionsTimesTwo-1; i++) {
      out << getAccessNumber(i) << ",";
   }
   out << getAccessNumber(DimensionsTimesTwo-1) << "]";
   out <<  ")";
}


peano4::grid::AutomatonState::PersistentRecords peano4::grid::AutomatonState::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::AutomatonStatePacked peano4::grid::AutomatonState::convert() const{
   return AutomatonStatePacked(
      getLevel(),
      getX(),
      getH(),
      getInverted(),
      getEvenFlags(),
      getAccessNumber()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::AutomatonState::_log( "peano4::grid::AutomatonState" );
   
   MPI_Datatype peano4::grid::AutomatonState::Datatype = 0;
   MPI_Datatype peano4::grid::AutomatonState::FullDatatype = 0;
   
   
   void peano4::grid::AutomatonState::initDatatype() {
      {
         logTraceIn( "initDatatype()" );
         AutomatonState dummyAutomatonState[2];
         
         const int Attributes = 3;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //level
            , MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            
         };
         
         int blocklen[Attributes] = {
              1		 //level
            , Dimensions		 //x
            , Dimensions		 //h
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._level))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &AutomatonState::Datatype );
         errorCode += MPI_Type_commit( &AutomatonState::Datatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         logTraceOut( "initDatatype()" );
         
      }
      {
         logTraceIn( "initDatatype()" );
         AutomatonState dummyAutomatonState[2];
         
         const int Attributes = 6;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //level
            , MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_BYTE		 //inverted
            , MPI_BYTE		 //evenFlags
            , MPI_SHORT		 //accessNumber
            
         };
         
         int blocklen[Attributes] = {
              1		 //level
            , Dimensions		 //x
            , Dimensions		 //h
            , 1		 //inverted
            , Dimensions		 //evenFlags
            , DimensionsTimesTwo		 //accessNumber
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._level))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._inverted))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._evenFlags))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]._persistentRecords._accessNumber[0]))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonState[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &AutomatonState::FullDatatype );
         errorCode += MPI_Type_commit( &AutomatonState::FullDatatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         logTraceOut( "initDatatype()" );
         
      }
      
   }
   
   
   void peano4::grid::AutomatonState::shutdownDatatype() {
      logTraceIn( "shutdownDatatype()" );
      MPI_Type_free( &AutomatonState::Datatype );
      MPI_Type_free( &AutomatonState::FullDatatype );
      logTraceOut( "shutdownDatatype()" );
      
   }
   
   void peano4::grid::AutomatonState::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::AutomatonState " 
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
         msg << "was not able to send message peano4::grid::AutomatonState "  
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
           msg << "testing for finished send task for peano4::grid::AutomatonState " 
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
             "peano4::grid::AutomatonState", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::AutomatonState", 
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
   
   
   
   void peano4::grid::AutomatonState::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::AutomatonState from rank " 
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
        msg << "failed to start to receive peano4::grid::AutomatonState from rank " 
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
            "peano4::grid::AutomatonState", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::AutomatonState", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::AutomatonState failed: " 
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
        msg << "testing for finished receive task for peano4::grid::AutomatonState failed: " 
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
            "peano4::grid::AutomatonState", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::AutomatonState", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::AutomatonState failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::AutomatonState from rank " 
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
   
   
   
   bool peano4::grid::AutomatonState::isMessageInQueue(int tag) {
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE
      );
      return flag;
   }
   
   int peano4::grid::AutomatonState::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::grid::AutomatonStatePacked::PersistentRecords::PersistentRecords() {
   if ((Dimensions+1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((Dimensions+1 < (8 * sizeof(short int))));
   
}


peano4::grid::AutomatonStatePacked::PersistentRecords::PersistentRecords(const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& inverted, const std::bitset<Dimensions>& evenFlags, const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber):
_level(level),
_x(x),
_h(h),
_accessNumber(accessNumber) {
   setInverted(inverted);
   setEvenFlags(evenFlags);
   if ((Dimensions+1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((Dimensions+1 < (8 * sizeof(short int))));
   
}


 int peano4::grid::AutomatonStatePacked::PersistentRecords::getLevel() const  {
   return _level;
}



 void peano4::grid::AutomatonStatePacked::PersistentRecords::setLevel(const int& level)  {
   _level = level;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonStatePacked::PersistentRecords::getX() const  {
   return _x;
}



 void peano4::grid::AutomatonStatePacked::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _x = (x);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonStatePacked::PersistentRecords::getH() const  {
   return _h;
}



 void peano4::grid::AutomatonStatePacked::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _h = (h);
}



 bool peano4::grid::AutomatonStatePacked::PersistentRecords::getInverted() const  {
   short int mask = 1 << (0);
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   return (tmp != 0);
}



 void peano4::grid::AutomatonStatePacked::PersistentRecords::setInverted(const bool& inverted)  {
   short int mask = 1 << (0);
   _packedRecords0 = static_cast<short int>( inverted ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
}



 std::bitset<Dimensions> peano4::grid::AutomatonStatePacked::PersistentRecords::getEvenFlags() const  {
   short int mask = (short int) (1 << (Dimensions)) - 1 ;
   mask = static_cast<short int>(mask << (1));
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (1));
   std::bitset<Dimensions> result = tmp;
   return result;
}



 void peano4::grid::AutomatonStatePacked::PersistentRecords::setEvenFlags(const std::bitset<Dimensions>& evenFlags)  {
   short int mask = (short int) (1 << (Dimensions)) - 1 ;
   mask = static_cast<short int>(mask << (1));
   _packedRecords0 = static_cast<short int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<short int>(_packedRecords0 | evenFlags.to_ulong() << (1));
}



 tarch::la::Vector<DimensionsTimesTwo,short int> peano4::grid::AutomatonStatePacked::PersistentRecords::getAccessNumber() const  {
   return _accessNumber;
}



 void peano4::grid::AutomatonStatePacked::PersistentRecords::setAccessNumber(const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber)  {
   _accessNumber = (accessNumber);
}


peano4::grid::AutomatonStatePacked::AutomatonStatePacked() {
   if ((Dimensions+1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((Dimensions+1 < (8 * sizeof(short int))));
   
}


peano4::grid::AutomatonStatePacked::AutomatonStatePacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._level, persistentRecords._x, persistentRecords._h, persistentRecords.getInverted(), persistentRecords.getEvenFlags(), persistentRecords._accessNumber) {
   if ((Dimensions+1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((Dimensions+1 < (8 * sizeof(short int))));
   
}


peano4::grid::AutomatonStatePacked::AutomatonStatePacked(const int& level, const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& inverted, const std::bitset<Dimensions>& evenFlags, const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber):
_persistentRecords(level, x, h, inverted, evenFlags, accessNumber) {
   if ((Dimensions+1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((Dimensions+1 < (8 * sizeof(short int))));
   
}


peano4::grid::AutomatonStatePacked::~AutomatonStatePacked() { }


 int peano4::grid::AutomatonStatePacked::getLevel() const  {
   return _persistentRecords._level;
}



 void peano4::grid::AutomatonStatePacked::setLevel(const int& level)  {
   _persistentRecords._level = level;
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonStatePacked::getX() const  {
   return _persistentRecords._x;
}



 void peano4::grid::AutomatonStatePacked::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _persistentRecords._x = (x);
}



 double peano4::grid::AutomatonStatePacked::getX(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._x[elementIndex];
   
}



 void peano4::grid::AutomatonStatePacked::setX(int elementIndex, const double& x)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._x[elementIndex]= x;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::AutomatonStatePacked::getH() const  {
   return _persistentRecords._h;
}



 void peano4::grid::AutomatonStatePacked::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _persistentRecords._h = (h);
}



 double peano4::grid::AutomatonStatePacked::getH(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._h[elementIndex];
   
}



 void peano4::grid::AutomatonStatePacked::setH(int elementIndex, const double& h)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._h[elementIndex]= h;
   
}



 bool peano4::grid::AutomatonStatePacked::getInverted() const  {
   short int mask = 1 << (0);
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
}



 void peano4::grid::AutomatonStatePacked::setInverted(const bool& inverted)  {
   short int mask = 1 << (0);
   _persistentRecords._packedRecords0 = static_cast<short int>( inverted ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
}



 std::bitset<Dimensions> peano4::grid::AutomatonStatePacked::getEvenFlags() const  {
   short int mask = (short int) (1 << (Dimensions)) - 1 ;
   mask = static_cast<short int>(mask << (1));
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (1));
   std::bitset<Dimensions> result = tmp;
   return result;
}



 void peano4::grid::AutomatonStatePacked::setEvenFlags(const std::bitset<Dimensions>& evenFlags)  {
   short int mask = (short int) (1 << (Dimensions)) - 1 ;
   mask = static_cast<short int>(mask << (1));
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 | evenFlags.to_ulong() << (1));
}



 bool peano4::grid::AutomatonStatePacked::getEvenFlags(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   int mask = 1 << (1);
   mask = mask << elementIndex;
   return (_persistentRecords._packedRecords0& mask);
}



 void peano4::grid::AutomatonStatePacked::setEvenFlags(int elementIndex, const bool& evenFlags)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   assertion(!evenFlags || evenFlags==1);
   int shift        = 1 + elementIndex; 
   short int mask         = 1     << (shift);
   short int shiftedValue = evenFlags << (shift);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 |  shiftedValue;
}



 void peano4::grid::AutomatonStatePacked::flipEvenFlags(int elementIndex)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   short int mask = 1 << (1);
   mask = mask << elementIndex;
   _persistentRecords._packedRecords0^= mask;
}



 tarch::la::Vector<DimensionsTimesTwo,short int> peano4::grid::AutomatonStatePacked::getAccessNumber() const  {
   return _persistentRecords._accessNumber;
}



 void peano4::grid::AutomatonStatePacked::setAccessNumber(const tarch::la::Vector<DimensionsTimesTwo,short int>& accessNumber)  {
   _persistentRecords._accessNumber = (accessNumber);
}



 short int peano4::grid::AutomatonStatePacked::getAccessNumber(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<DimensionsTimesTwo);
   return _persistentRecords._accessNumber[elementIndex];
   
}



 void peano4::grid::AutomatonStatePacked::setAccessNumber(int elementIndex, const short int& accessNumber)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<DimensionsTimesTwo);
   _persistentRecords._accessNumber[elementIndex]= accessNumber;
   
}




std::string peano4::grid::AutomatonStatePacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::AutomatonStatePacked::toString (std::ostream& out) const {
   out << "("; 
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
   out << ",";
   out << "inverted:" << getInverted();
   out << ",";
   out << "evenFlags:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getEvenFlags(i) << ",";
   }
   out << getEvenFlags(Dimensions-1) << "]";
   out << ",";
   out << "accessNumber:[";
   for (int i = 0; i < DimensionsTimesTwo-1; i++) {
      out << getAccessNumber(i) << ",";
   }
   out << getAccessNumber(DimensionsTimesTwo-1) << "]";
   out <<  ")";
}


peano4::grid::AutomatonStatePacked::PersistentRecords peano4::grid::AutomatonStatePacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::AutomatonState peano4::grid::AutomatonStatePacked::convert() const{
   return AutomatonState(
      getLevel(),
      getX(),
      getH(),
      getInverted(),
      getEvenFlags(),
      getAccessNumber()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::AutomatonStatePacked::_log( "peano4::grid::AutomatonStatePacked" );
   
   MPI_Datatype peano4::grid::AutomatonStatePacked::Datatype = 0;
   MPI_Datatype peano4::grid::AutomatonStatePacked::FullDatatype = 0;
   
   
   void peano4::grid::AutomatonStatePacked::initDatatype() {
      {
         logTraceIn( "initDatatype()" );
         AutomatonStatePacked dummyAutomatonStatePacked[2];
         
         const int Attributes = 3;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //level
            , MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            
         };
         
         int blocklen[Attributes] = {
              1		 //level
            , Dimensions		 //x
            , Dimensions		 //h
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._level))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &AutomatonStatePacked::Datatype );
         errorCode += MPI_Type_commit( &AutomatonStatePacked::Datatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         logTraceOut( "initDatatype()" );
         
      }
      {
         logTraceIn( "initDatatype()" );
         AutomatonStatePacked dummyAutomatonStatePacked[2];
         
         const int Attributes = 5;
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //level
            , MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_SHORT		 //accessNumber
            , MPI_SHORT		 //_packedRecords0
            
         };
         
         int blocklen[Attributes] = {
              1		 //level
            , Dimensions		 //x
            , Dimensions		 //h
            , DimensionsTimesTwo		 //accessNumber
            , 1		 //_packedRecords0
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = 0;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._level))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._accessNumber[0]))), 		&disp[currentAddress] );
         currentAddress++;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         currentAddress++;
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[0]))), &base);
         MPI_Aint typeOffset = disp[0] - base;
         for (int i=Attributes-1; i>=0; i--) {
         
            disp[i] = disp[i] - disp[0];
            
         }
         int errorCode = 0; 
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyAutomatonStatePacked[1]))), &typeExtent);
         typeExtent = typeExtent - base - typeOffset;
         errorCode += MPI_Type_create_resized( tmpType, typeOffset, typeExtent, &AutomatonStatePacked::FullDatatype );
         errorCode += MPI_Type_commit( &AutomatonStatePacked::FullDatatype );
         // errorCode += MPI_Type_free(&tmpType);
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         logTraceOut( "initDatatype()" );
         
      }
      
   }
   
   
   void peano4::grid::AutomatonStatePacked::shutdownDatatype() {
      logTraceIn( "shutdownDatatype()" );
      MPI_Type_free( &AutomatonStatePacked::Datatype );
      MPI_Type_free( &AutomatonStatePacked::FullDatatype );
      logTraceOut( "shutdownDatatype()" );
      
   }
   
   void peano4::grid::AutomatonStatePacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::AutomatonStatePacked " 
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
         msg << "was not able to send message peano4::grid::AutomatonStatePacked "  
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
           msg << "testing for finished send task for peano4::grid::AutomatonStatePacked " 
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
             "peano4::grid::AutomatonStatePacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::AutomatonStatePacked", 
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
   
   
   
   void peano4::grid::AutomatonStatePacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::AutomatonStatePacked from rank " 
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
        msg << "failed to start to receive peano4::grid::AutomatonStatePacked from rank " 
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
            "peano4::grid::AutomatonStatePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::AutomatonStatePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::AutomatonStatePacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::AutomatonStatePacked failed: " 
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
            "peano4::grid::AutomatonStatePacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::AutomatonStatePacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::AutomatonStatePacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::AutomatonStatePacked from rank " 
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
   
   
   
   bool peano4::grid::AutomatonStatePacked::isMessageInQueue(int tag) {
      int  flag        = 0;
      MPI_Iprobe(
         MPI_ANY_SOURCE, tag,
         tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE
      );
      return flag;
   }
   
   int peano4::grid::AutomatonStatePacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



