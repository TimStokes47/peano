#include "peano4/GridVertex.h"

#if defined(PEANO_DEBUG)
   peano4::GridVertex::PersistentRecords::PersistentRecords() {
      
   }
   
   
   peano4::GridVertex::PersistentRecords::PersistentRecords(const State& state, const int& adjacentRanks, const tarch::la::Vector<DIMENSIONS,double>& x, const int& level):
   _state(state),
   _adjacentRanks(adjacentRanks),
   _x(x),
   _level(level) {
      
   }
   
   
    peano4::GridVertex::State peano4::GridVertex::PersistentRecords::getState() const  {
      return _state;
   }
   
   
   
    void peano4::GridVertex::PersistentRecords::setState(const State& state)  {
      _state = state;
   }
   
   
   
    int peano4::GridVertex::PersistentRecords::getAdjacentRanks() const  {
      return _adjacentRanks;
   }
   
   
   
    void peano4::GridVertex::PersistentRecords::setAdjacentRanks(const int& adjacentRanks)  {
      _adjacentRanks = adjacentRanks;
   }
   
   
   
    tarch::la::Vector<DIMENSIONS,double> peano4::GridVertex::PersistentRecords::getX() const  {
      return _x;
   }
   
   
   
    void peano4::GridVertex::PersistentRecords::setX(const tarch::la::Vector<DIMENSIONS,double>& x)  {
      _x = (x);
   }
   
   
   
    int peano4::GridVertex::PersistentRecords::getLevel() const  {
      return _level;
   }
   
   
   
    void peano4::GridVertex::PersistentRecords::setLevel(const int& level)  {
      _level = level;
   }
   
   
   peano4::GridVertex::GridVertex() {
      
   }
   
   
   peano4::GridVertex::GridVertex(const PersistentRecords& persistentRecords):
   _persistentRecords(persistentRecords._state, persistentRecords._adjacentRanks, persistentRecords._x, persistentRecords._level) {
      
   }
   
   
   peano4::GridVertex::GridVertex(const State& state, const int& adjacentRanks, const tarch::la::Vector<DIMENSIONS,double>& x, const int& level):
   _persistentRecords(state, adjacentRanks, x, level) {
      
   }
   
   
   peano4::GridVertex::GridVertex(const State& state, const int& adjacentRanks, const int& adjacentRanksOfPreviousIteration, const tarch::la::Vector<DIMENSIONS,double>& x, const int& level):
   _persistentRecords(state, adjacentRanks, x, level),_adjacentRanksOfPreviousIteration(adjacentRanksOfPreviousIteration) {
      
   }
   
   peano4::GridVertex::~GridVertex() { }
   
   
    peano4::GridVertex::State peano4::GridVertex::getState() const  {
      return _persistentRecords._state;
   }
   
   
   
    void peano4::GridVertex::setState(const State& state)  {
      _persistentRecords._state = state;
   }
   
   
   
    int peano4::GridVertex::getAdjacentRanks() const  {
      return _persistentRecords._adjacentRanks;
   }
   
   
   
    void peano4::GridVertex::setAdjacentRanks(const int& adjacentRanks)  {
      _persistentRecords._adjacentRanks = adjacentRanks;
   }
   
   
   
    int peano4::GridVertex::getAdjacentRanksOfPreviousIteration() const  {
      return _adjacentRanksOfPreviousIteration;
   }
   
   
   
    void peano4::GridVertex::setAdjacentRanksOfPreviousIteration(const int& adjacentRanksOfPreviousIteration)  {
      _adjacentRanksOfPreviousIteration = adjacentRanksOfPreviousIteration;
   }
   
   
   
    tarch::la::Vector<DIMENSIONS,double> peano4::GridVertex::getX() const  {
      return _persistentRecords._x;
   }
   
   
   
    void peano4::GridVertex::setX(const tarch::la::Vector<DIMENSIONS,double>& x)  {
      _persistentRecords._x = (x);
   }
   
   
   
    double peano4::GridVertex::getX(int elementIndex) const  {
      assertion(elementIndex>=0);
      assertion(elementIndex<DIMENSIONS);
      return _persistentRecords._x[elementIndex];
      
   }
   
   
   
    void peano4::GridVertex::setX(int elementIndex, const double& x)  {
      assertion(elementIndex>=0);
      assertion(elementIndex<DIMENSIONS);
      _persistentRecords._x[elementIndex]= x;
      
   }
   
   
   
    int peano4::GridVertex::getLevel() const  {
      return _persistentRecords._level;
   }
   
   
   
    void peano4::GridVertex::setLevel(const int& level)  {
      _persistentRecords._level = level;
   }
   
   
   std::string peano4::GridVertex::toString(const State& param) {
      switch (param) {
         case HangingVertex: return "HangingVertex";
         case Unrefined: return "Unrefined";
         case Refined: return "Refined";
         case RefinementTriggered: return "RefinementTriggered";
         case Refining: return "Refining";
         case EraseTriggered: return "EraseTriggered";
         case Erasing: return "Erasing";
      }
      return "undefined";
   }
   
   std::string peano4::GridVertex::getStateMapping() {
      return "State(HangingVertex=0,Unrefined=1,Refined=2,RefinementTriggered=3,Refining=4,EraseTriggered=5,Erasing=6)";
   }
   
   
   std::string peano4::GridVertex::toString() const {
      std::ostringstream stringstr;
      toString(stringstr);
      return stringstr.str();
   }
   
   void peano4::GridVertex::toString (std::ostream& out) const {
      out << "("; 
      out << "state:" << toString(getState());
      out << ",";
      out << "adjacentRanks:" << getAdjacentRanks();
      out << ",";
      out << "adjacentRanksOfPreviousIteration:" << getAdjacentRanksOfPreviousIteration();
      out << ",";
      out << "x:[";
   for (int i = 0; i < DIMENSIONS-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(DIMENSIONS-1) << "]";
      out << ",";
      out << "level:" << getLevel();
      out <<  ")";
   }
   
   
   peano4::GridVertex::PersistentRecords peano4::GridVertex::getPersistentRecords() const {
      return _persistentRecords;
   }
   
   peano4::GridVertexPacked peano4::GridVertex::convert() const{
      return GridVertexPacked(
         getState(),
         getAdjacentRanks(),
         getAdjacentRanksOfPreviousIteration(),
         getX(),
         getLevel()
      );
   }
   
   #ifdef Parallel
      tarch::logging::Log peano4::GridVertex::_log( "peano4::GridVertex" );
      
      MPI_Datatype peano4::GridVertex::Datatype = 0;
      MPI_Datatype peano4::GridVertex::FullDatatype = 0;
      
      
      void peano4::GridVertex::initDatatype() {
         {
            GridVertex dummyGridVertex[2];
            
            #ifdef MPI2
            const int Attributes = 3;
            #else
            const int Attributes = 4;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //state
               , MPI_DOUBLE		 //x
               , MPI_INT		 //level
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //state
               , DIMENSIONS		 //x
               , 1		 //level
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[1] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[1] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[2] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[2] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridVertex)), i, disp[i], Attributes, sizeof(GridVertex));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), 		&disp[3] );
            disp[3] -= base;
            disp[3] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertex::Datatype );
            MPI_Type_commit( &GridVertex::Datatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::Datatype);
            MPI_Type_commit( &GridVertex::Datatype );
            #endif
            
         }
         {
            GridVertex dummyGridVertex[2];
            
            #ifdef MPI2
            const int Attributes = 5;
            #else
            const int Attributes = 6;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //state
               , MPI_INT		 //adjacentRanks
               , MPI_DOUBLE		 //x
               , MPI_INT		 //level
               , MPI_INT		 //adjacentRanksOfPreviousIteration
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //state
               , 1		 //adjacentRanks
               , DIMENSIONS		 //x
               , 1		 //level
               , 1		 //adjacentRanksOfPreviousIteration
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks))), 		&disp[1] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks))), 		&disp[1] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[2] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[2] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[3] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[3] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._adjacentRanksOfPreviousIteration))), 		&disp[4] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._adjacentRanksOfPreviousIteration))), 		&disp[4] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridVertex)), i, disp[i], Attributes, sizeof(GridVertex));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), 		&disp[5] );
            disp[5] -= base;
            disp[5] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertex::FullDatatype );
            MPI_Type_commit( &GridVertex::FullDatatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::FullDatatype);
            MPI_Type_commit( &GridVertex::FullDatatype );
            #endif
            
         }
         
      }
      
      
      void peano4::GridVertex::shutdownDatatype() {
         MPI_Type_free( &GridVertex::Datatype );
         MPI_Type_free( &GridVertex::FullDatatype );
         
      }
      
      void peano4::GridVertex::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
         // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::GridVertex " 
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
         msg << "was not able to send message peano4::GridVertex "  
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
           msg << "testing for finished send task for peano4::GridVertex " 
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
             "peano4::GridVertex", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::GridVertex", 
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
      
      
      
      void peano4::GridVertex::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::GridVertex from node " 
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
        msg << "failed to start to receive peano4::GridVertex from node " 
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
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertex failed: " 
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
        msg << "testing for finished receive task for peano4::GridVertex failed: " 
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
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertex failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::GridVertex from node " 
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
      
      
      
      bool peano4::GridVertex::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
      
      int peano4::GridVertex::getSenderRank() const {
         assertion( _senderDestinationRank!=-1 );
         return _senderDestinationRank;
         
      }
   #endif
   
   
   peano4::GridVertexPacked::PersistentRecords::PersistentRecords() {
      if ((3 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((3 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::GridVertexPacked::PersistentRecords::PersistentRecords(const State& state, const int& adjacentRanks, const tarch::la::Vector<DIMENSIONS,double>& x, const int& level):
   _adjacentRanks(adjacentRanks),
   _x(x),
   _level(level) {
      setState(state);
      if ((3 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((3 < (8 * sizeof(short int))));
      
   }
   
   
    peano4::GridVertex::State peano4::GridVertexPacked::PersistentRecords::getState() const  {
      short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 6));
   return (State) tmp;
   }
   
   
   
    void peano4::GridVertexPacked::PersistentRecords::setState(const State& state)  {
      assertion((state >= 0 && state <= 6));
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   _packedRecords0 = static_cast<short int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<short int>(_packedRecords0 | static_cast<short int>(state) << (0));
   }
   
   
   
    int peano4::GridVertexPacked::PersistentRecords::getAdjacentRanks() const  {
      return _adjacentRanks;
   }
   
   
   
    void peano4::GridVertexPacked::PersistentRecords::setAdjacentRanks(const int& adjacentRanks)  {
      _adjacentRanks = adjacentRanks;
   }
   
   
   
    tarch::la::Vector<DIMENSIONS,double> peano4::GridVertexPacked::PersistentRecords::getX() const  {
      return _x;
   }
   
   
   
    void peano4::GridVertexPacked::PersistentRecords::setX(const tarch::la::Vector<DIMENSIONS,double>& x)  {
      _x = (x);
   }
   
   
   
    int peano4::GridVertexPacked::PersistentRecords::getLevel() const  {
      return _level;
   }
   
   
   
    void peano4::GridVertexPacked::PersistentRecords::setLevel(const int& level)  {
      _level = level;
   }
   
   
   peano4::GridVertexPacked::GridVertexPacked() {
      if ((3 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((3 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::GridVertexPacked::GridVertexPacked(const PersistentRecords& persistentRecords):
   _persistentRecords(persistentRecords.getState(), persistentRecords._adjacentRanks, persistentRecords._x, persistentRecords._level) {
      if ((3 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((3 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::GridVertexPacked::GridVertexPacked(const State& state, const int& adjacentRanks, const tarch::la::Vector<DIMENSIONS,double>& x, const int& level):
   _persistentRecords(state, adjacentRanks, x, level) {
      if ((3 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((3 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::GridVertexPacked::GridVertexPacked(const State& state, const int& adjacentRanks, const int& adjacentRanksOfPreviousIteration, const tarch::la::Vector<DIMENSIONS,double>& x, const int& level):
   _persistentRecords(state, adjacentRanks, x, level),_adjacentRanksOfPreviousIteration(adjacentRanksOfPreviousIteration) {
      if ((3 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((3 < (8 * sizeof(short int))));
      
   }
   
   peano4::GridVertexPacked::~GridVertexPacked() { }
   
   
    peano4::GridVertex::State peano4::GridVertexPacked::getState() const  {
      short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 6));
   return (State) tmp;
   }
   
   
   
    void peano4::GridVertexPacked::setState(const State& state)  {
      assertion((state >= 0 && state <= 6));
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 | static_cast<short int>(state) << (0));
   }
   
   
   
    int peano4::GridVertexPacked::getAdjacentRanks() const  {
      return _persistentRecords._adjacentRanks;
   }
   
   
   
    void peano4::GridVertexPacked::setAdjacentRanks(const int& adjacentRanks)  {
      _persistentRecords._adjacentRanks = adjacentRanks;
   }
   
   
   
    int peano4::GridVertexPacked::getAdjacentRanksOfPreviousIteration() const  {
      return _adjacentRanksOfPreviousIteration;
   }
   
   
   
    void peano4::GridVertexPacked::setAdjacentRanksOfPreviousIteration(const int& adjacentRanksOfPreviousIteration)  {
      _adjacentRanksOfPreviousIteration = adjacentRanksOfPreviousIteration;
   }
   
   
   
    tarch::la::Vector<DIMENSIONS,double> peano4::GridVertexPacked::getX() const  {
      return _persistentRecords._x;
   }
   
   
   
    void peano4::GridVertexPacked::setX(const tarch::la::Vector<DIMENSIONS,double>& x)  {
      _persistentRecords._x = (x);
   }
   
   
   
    double peano4::GridVertexPacked::getX(int elementIndex) const  {
      assertion(elementIndex>=0);
      assertion(elementIndex<DIMENSIONS);
      return _persistentRecords._x[elementIndex];
      
   }
   
   
   
    void peano4::GridVertexPacked::setX(int elementIndex, const double& x)  {
      assertion(elementIndex>=0);
      assertion(elementIndex<DIMENSIONS);
      _persistentRecords._x[elementIndex]= x;
      
   }
   
   
   
    int peano4::GridVertexPacked::getLevel() const  {
      return _persistentRecords._level;
   }
   
   
   
    void peano4::GridVertexPacked::setLevel(const int& level)  {
      _persistentRecords._level = level;
   }
   
   
   std::string peano4::GridVertexPacked::toString(const State& param) {
      return peano4::GridVertex::toString(param);
   }
   
   std::string peano4::GridVertexPacked::getStateMapping() {
      return peano4::GridVertex::getStateMapping();
   }
   
   
   
   std::string peano4::GridVertexPacked::toString() const {
      std::ostringstream stringstr;
      toString(stringstr);
      return stringstr.str();
   }
   
   void peano4::GridVertexPacked::toString (std::ostream& out) const {
      out << "("; 
      out << "state:" << toString(getState());
      out << ",";
      out << "adjacentRanks:" << getAdjacentRanks();
      out << ",";
      out << "adjacentRanksOfPreviousIteration:" << getAdjacentRanksOfPreviousIteration();
      out << ",";
      out << "x:[";
   for (int i = 0; i < DIMENSIONS-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(DIMENSIONS-1) << "]";
      out << ",";
      out << "level:" << getLevel();
      out <<  ")";
   }
   
   
   peano4::GridVertexPacked::PersistentRecords peano4::GridVertexPacked::getPersistentRecords() const {
      return _persistentRecords;
   }
   
   peano4::GridVertex peano4::GridVertexPacked::convert() const{
      return GridVertex(
         getState(),
         getAdjacentRanks(),
         getAdjacentRanksOfPreviousIteration(),
         getX(),
         getLevel()
      );
   }
   
   #ifdef Parallel
      tarch::logging::Log peano4::GridVertexPacked::_log( "peano4::GridVertexPacked" );
      
      MPI_Datatype peano4::GridVertexPacked::Datatype = 0;
      MPI_Datatype peano4::GridVertexPacked::FullDatatype = 0;
      
      
      void peano4::GridVertexPacked::initDatatype() {
         {
            GridVertexPacked dummyGridVertexPacked[2];
            
            #ifdef MPI2
            const int Attributes = 3;
            #else
            const int Attributes = 4;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_DOUBLE		 //x
               , MPI_INT		 //level
               , MPI_SHORT		 //_packedRecords0
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 DIMENSIONS		 //x
               , 1		 //level
               , 1		 //_packedRecords0
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[0] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[1] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[1] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[2] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[2] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridVertexPacked)), i, disp[i], Attributes, sizeof(GridVertexPacked));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), 		&disp[3] );
            disp[3] -= base;
            disp[3] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertexPacked::Datatype );
            MPI_Type_commit( &GridVertexPacked::Datatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::Datatype);
            MPI_Type_commit( &GridVertexPacked::Datatype );
            #endif
            
         }
         {
            GridVertexPacked dummyGridVertexPacked[2];
            
            #ifdef MPI2
            const int Attributes = 5;
            #else
            const int Attributes = 6;
            #endif
            MPI_Datatype subtypes[Attributes] = {
                 MPI_INT		 //adjacentRanks
               , MPI_DOUBLE		 //x
               , MPI_INT		 //level
               , MPI_SHORT		 //_packedRecords0
               , MPI_INT		 //adjacentRanksOfPreviousIteration
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
                 1		 //adjacentRanks
               , DIMENSIONS		 //x
               , 1		 //level
               , 1		 //_packedRecords0
               , 1		 //adjacentRanksOfPreviousIteration
               #ifndef MPI2
               , 1
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            MPI_Aint  base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks))), 		&disp[0] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks))), 		&disp[0] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[1] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[1] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[2] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[2] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[3] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[3] );
            #endif
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._adjacentRanksOfPreviousIteration))), 		&disp[4] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._adjacentRanksOfPreviousIteration))), 		&disp[4] );
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
               assertion4(disp[i]<static_cast<int>(sizeof(GridVertexPacked)), i, disp[i], Attributes, sizeof(GridVertexPacked));
            }
            #ifndef MPI2
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), 		&disp[5] );
            disp[5] -= base;
            disp[5] += disp[0];
            #endif
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Aint lowerBound, typeExtent; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
            MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertexPacked::FullDatatype );
            MPI_Type_commit( &GridVertexPacked::FullDatatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::FullDatatype);
            MPI_Type_commit( &GridVertexPacked::FullDatatype );
            #endif
            
         }
         
      }
      
      
      void peano4::GridVertexPacked::shutdownDatatype() {
         MPI_Type_free( &GridVertexPacked::Datatype );
         MPI_Type_free( &GridVertexPacked::FullDatatype );
         
      }
      
      void peano4::GridVertexPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
         // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::GridVertexPacked " 
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
         msg << "was not able to send message peano4::GridVertexPacked "  
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
           msg << "testing for finished send task for peano4::GridVertexPacked " 
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
             "peano4::GridVertexPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::GridVertexPacked", 
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
      
      
      
      void peano4::GridVertexPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::GridVertexPacked from node " 
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
        msg << "failed to start to receive peano4::GridVertexPacked from node " 
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
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertexPacked failed: " 
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
        msg << "testing for finished receive task for peano4::GridVertexPacked failed: " 
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
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertexPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::GridVertexPacked from node " 
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
      
      
      
      bool peano4::GridVertexPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
      
      int peano4::GridVertexPacked::getSenderRank() const {
         assertion( _senderDestinationRank!=-1 );
         return _senderDestinationRank;
         
      }
   #endif
   
   
   #elif !defined(PEANO_DEBUG)
      peano4::GridVertex::PersistentRecords::PersistentRecords() {
         
      }
      
      
      peano4::GridVertex::PersistentRecords::PersistentRecords(const State& state, const int& adjacentRanks):
      _state(state),
      _adjacentRanks(adjacentRanks) {
         
      }
      
      
       peano4::GridVertex::State peano4::GridVertex::PersistentRecords::getState() const  {
         return _state;
      }
      
      
      
       void peano4::GridVertex::PersistentRecords::setState(const State& state)  {
         _state = state;
      }
      
      
      
       int peano4::GridVertex::PersistentRecords::getAdjacentRanks() const  {
         return _adjacentRanks;
      }
      
      
      
       void peano4::GridVertex::PersistentRecords::setAdjacentRanks(const int& adjacentRanks)  {
         _adjacentRanks = adjacentRanks;
      }
      
      
      peano4::GridVertex::GridVertex() {
         
      }
      
      
      peano4::GridVertex::GridVertex(const PersistentRecords& persistentRecords):
      _persistentRecords(persistentRecords._state, persistentRecords._adjacentRanks) {
         
      }
      
      
      peano4::GridVertex::GridVertex(const State& state, const int& adjacentRanks):
      _persistentRecords(state, adjacentRanks) {
         
      }
      
      
      peano4::GridVertex::GridVertex(const State& state, const int& adjacentRanks, const int& adjacentRanksOfPreviousIteration):
      _persistentRecords(state, adjacentRanks),_adjacentRanksOfPreviousIteration(adjacentRanksOfPreviousIteration) {
         
      }
      
      peano4::GridVertex::~GridVertex() { }
      
      
       peano4::GridVertex::State peano4::GridVertex::getState() const  {
         return _persistentRecords._state;
      }
      
      
      
       void peano4::GridVertex::setState(const State& state)  {
         _persistentRecords._state = state;
      }
      
      
      
       int peano4::GridVertex::getAdjacentRanks() const  {
         return _persistentRecords._adjacentRanks;
      }
      
      
      
       void peano4::GridVertex::setAdjacentRanks(const int& adjacentRanks)  {
         _persistentRecords._adjacentRanks = adjacentRanks;
      }
      
      
      
       int peano4::GridVertex::getAdjacentRanksOfPreviousIteration() const  {
         return _adjacentRanksOfPreviousIteration;
      }
      
      
      
       void peano4::GridVertex::setAdjacentRanksOfPreviousIteration(const int& adjacentRanksOfPreviousIteration)  {
         _adjacentRanksOfPreviousIteration = adjacentRanksOfPreviousIteration;
      }
      
      
      std::string peano4::GridVertex::toString(const State& param) {
         switch (param) {
            case HangingVertex: return "HangingVertex";
            case Unrefined: return "Unrefined";
            case Refined: return "Refined";
            case RefinementTriggered: return "RefinementTriggered";
            case Refining: return "Refining";
            case EraseTriggered: return "EraseTriggered";
            case Erasing: return "Erasing";
         }
         return "undefined";
      }
      
      std::string peano4::GridVertex::getStateMapping() {
         return "State(HangingVertex=0,Unrefined=1,Refined=2,RefinementTriggered=3,Refining=4,EraseTriggered=5,Erasing=6)";
      }
      
      
      std::string peano4::GridVertex::toString() const {
         std::ostringstream stringstr;
         toString(stringstr);
         return stringstr.str();
      }
      
      void peano4::GridVertex::toString (std::ostream& out) const {
         out << "("; 
         out << "state:" << toString(getState());
         out << ",";
         out << "adjacentRanks:" << getAdjacentRanks();
         out << ",";
         out << "adjacentRanksOfPreviousIteration:" << getAdjacentRanksOfPreviousIteration();
         out <<  ")";
      }
      
      
      peano4::GridVertex::PersistentRecords peano4::GridVertex::getPersistentRecords() const {
         return _persistentRecords;
      }
      
      peano4::GridVertexPacked peano4::GridVertex::convert() const{
         return GridVertexPacked(
            getState(),
            getAdjacentRanks(),
            getAdjacentRanksOfPreviousIteration()
         );
      }
      
      #ifdef Parallel
         tarch::logging::Log peano4::GridVertex::_log( "peano4::GridVertex" );
         
         MPI_Datatype peano4::GridVertex::Datatype = 0;
         MPI_Datatype peano4::GridVertex::FullDatatype = 0;
         
         
         void peano4::GridVertex::initDatatype() {
            {
               GridVertex dummyGridVertex[2];
               
               #ifdef MPI2
               const int Attributes = 1;
               #else
               const int Attributes = 2;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //state
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //state
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridVertex)), i, disp[i], Attributes, sizeof(GridVertex));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), 		&disp[1] );
               disp[1] -= base;
               disp[1] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertex::Datatype );
               MPI_Type_commit( &GridVertex::Datatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::Datatype);
               MPI_Type_commit( &GridVertex::Datatype );
               #endif
               
            }
            {
               GridVertex dummyGridVertex[2];
               
               #ifdef MPI2
               const int Attributes = 3;
               #else
               const int Attributes = 4;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //state
                  , MPI_INT		 //adjacentRanks
                  , MPI_INT		 //adjacentRanksOfPreviousIteration
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //state
                  , 1		 //adjacentRanks
                  , 1		 //adjacentRanksOfPreviousIteration
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[0] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks))), 		&disp[1] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks))), 		&disp[1] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._adjacentRanksOfPreviousIteration))), 		&disp[2] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._adjacentRanksOfPreviousIteration))), 		&disp[2] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridVertex)), i, disp[i], Attributes, sizeof(GridVertex));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), 		&disp[3] );
               disp[3] -= base;
               disp[3] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertex::FullDatatype );
               MPI_Type_commit( &GridVertex::FullDatatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::FullDatatype);
               MPI_Type_commit( &GridVertex::FullDatatype );
               #endif
               
            }
            
         }
         
         
         void peano4::GridVertex::shutdownDatatype() {
            MPI_Type_free( &GridVertex::Datatype );
            MPI_Type_free( &GridVertex::FullDatatype );
            
         }
         
         void peano4::GridVertex::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
            // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::GridVertex " 
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
         msg << "was not able to send message peano4::GridVertex "  
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
           msg << "testing for finished send task for peano4::GridVertex " 
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
             "peano4::GridVertex", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::GridVertex", 
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
         
         
         
         void peano4::GridVertex::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::GridVertex from node " 
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
        msg << "failed to start to receive peano4::GridVertex from node " 
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
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertex failed: " 
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
        msg << "testing for finished receive task for peano4::GridVertex failed: " 
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
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertex failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::GridVertex from node " 
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
         
         
         
         bool peano4::GridVertex::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
         
         int peano4::GridVertex::getSenderRank() const {
            assertion( _senderDestinationRank!=-1 );
            return _senderDestinationRank;
            
         }
      #endif
      
      
      peano4::GridVertexPacked::PersistentRecords::PersistentRecords() {
         if ((3 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((3 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::GridVertexPacked::PersistentRecords::PersistentRecords(const State& state, const int& adjacentRanks):
      _adjacentRanks(adjacentRanks) {
         setState(state);
         if ((3 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((3 < (8 * sizeof(short int))));
         
      }
      
      
       peano4::GridVertex::State peano4::GridVertexPacked::PersistentRecords::getState() const  {
         short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 6));
   return (State) tmp;
      }
      
      
      
       void peano4::GridVertexPacked::PersistentRecords::setState(const State& state)  {
         assertion((state >= 0 && state <= 6));
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   _packedRecords0 = static_cast<short int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<short int>(_packedRecords0 | static_cast<short int>(state) << (0));
      }
      
      
      
       int peano4::GridVertexPacked::PersistentRecords::getAdjacentRanks() const  {
         return _adjacentRanks;
      }
      
      
      
       void peano4::GridVertexPacked::PersistentRecords::setAdjacentRanks(const int& adjacentRanks)  {
         _adjacentRanks = adjacentRanks;
      }
      
      
      peano4::GridVertexPacked::GridVertexPacked() {
         if ((3 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((3 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::GridVertexPacked::GridVertexPacked(const PersistentRecords& persistentRecords):
      _persistentRecords(persistentRecords.getState(), persistentRecords._adjacentRanks) {
         if ((3 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((3 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::GridVertexPacked::GridVertexPacked(const State& state, const int& adjacentRanks):
      _persistentRecords(state, adjacentRanks) {
         if ((3 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((3 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::GridVertexPacked::GridVertexPacked(const State& state, const int& adjacentRanks, const int& adjacentRanksOfPreviousIteration):
      _persistentRecords(state, adjacentRanks),_adjacentRanksOfPreviousIteration(adjacentRanksOfPreviousIteration) {
         if ((3 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((3 < (8 * sizeof(short int))));
         
      }
      
      peano4::GridVertexPacked::~GridVertexPacked() { }
      
      
       peano4::GridVertex::State peano4::GridVertexPacked::getState() const  {
         short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 6));
   return (State) tmp;
      }
      
      
      
       void peano4::GridVertexPacked::setState(const State& state)  {
         assertion((state >= 0 && state <= 6));
   short int mask =  (1 << (3)) - 1;
   mask = static_cast<short int>(mask << (0));
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 | static_cast<short int>(state) << (0));
      }
      
      
      
       int peano4::GridVertexPacked::getAdjacentRanks() const  {
         return _persistentRecords._adjacentRanks;
      }
      
      
      
       void peano4::GridVertexPacked::setAdjacentRanks(const int& adjacentRanks)  {
         _persistentRecords._adjacentRanks = adjacentRanks;
      }
      
      
      
       int peano4::GridVertexPacked::getAdjacentRanksOfPreviousIteration() const  {
         return _adjacentRanksOfPreviousIteration;
      }
      
      
      
       void peano4::GridVertexPacked::setAdjacentRanksOfPreviousIteration(const int& adjacentRanksOfPreviousIteration)  {
         _adjacentRanksOfPreviousIteration = adjacentRanksOfPreviousIteration;
      }
      
      
      std::string peano4::GridVertexPacked::toString(const State& param) {
         return peano4::GridVertex::toString(param);
      }
      
      std::string peano4::GridVertexPacked::getStateMapping() {
         return peano4::GridVertex::getStateMapping();
      }
      
      
      
      std::string peano4::GridVertexPacked::toString() const {
         std::ostringstream stringstr;
         toString(stringstr);
         return stringstr.str();
      }
      
      void peano4::GridVertexPacked::toString (std::ostream& out) const {
         out << "("; 
         out << "state:" << toString(getState());
         out << ",";
         out << "adjacentRanks:" << getAdjacentRanks();
         out << ",";
         out << "adjacentRanksOfPreviousIteration:" << getAdjacentRanksOfPreviousIteration();
         out <<  ")";
      }
      
      
      peano4::GridVertexPacked::PersistentRecords peano4::GridVertexPacked::getPersistentRecords() const {
         return _persistentRecords;
      }
      
      peano4::GridVertex peano4::GridVertexPacked::convert() const{
         return GridVertex(
            getState(),
            getAdjacentRanks(),
            getAdjacentRanksOfPreviousIteration()
         );
      }
      
      #ifdef Parallel
         tarch::logging::Log peano4::GridVertexPacked::_log( "peano4::GridVertexPacked" );
         
         MPI_Datatype peano4::GridVertexPacked::Datatype = 0;
         MPI_Datatype peano4::GridVertexPacked::FullDatatype = 0;
         
         
         void peano4::GridVertexPacked::initDatatype() {
            {
               GridVertexPacked dummyGridVertexPacked[2];
               
               #ifdef MPI2
               const int Attributes = 1;
               #else
               const int Attributes = 2;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_SHORT		 //_packedRecords0
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //_packedRecords0
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[0] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridVertexPacked)), i, disp[i], Attributes, sizeof(GridVertexPacked));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), 		&disp[1] );
               disp[1] -= base;
               disp[1] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertexPacked::Datatype );
               MPI_Type_commit( &GridVertexPacked::Datatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::Datatype);
               MPI_Type_commit( &GridVertexPacked::Datatype );
               #endif
               
            }
            {
               GridVertexPacked dummyGridVertexPacked[2];
               
               #ifdef MPI2
               const int Attributes = 3;
               #else
               const int Attributes = 4;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                    MPI_INT		 //adjacentRanks
                  , MPI_SHORT		 //_packedRecords0
                  , MPI_INT		 //adjacentRanksOfPreviousIteration
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                    1		 //adjacentRanks
                  , 1		 //_packedRecords0
                  , 1		 //adjacentRanksOfPreviousIteration
                  #ifndef MPI2
                  , 1
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               MPI_Aint  base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked))), &base);
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks))), 		&disp[0] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks))), 		&disp[0] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[1] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[1] );
               #endif
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._adjacentRanksOfPreviousIteration))), 		&disp[2] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._adjacentRanksOfPreviousIteration))), 		&disp[2] );
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
                  assertion4(disp[i]<static_cast<int>(sizeof(GridVertexPacked)), i, disp[i], Attributes, sizeof(GridVertexPacked));
               }
               #ifndef MPI2
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), 		&disp[3] );
               disp[3] -= base;
               disp[3] += disp[0];
               #endif
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Aint lowerBound, typeExtent; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
               MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridVertexPacked::FullDatatype );
               MPI_Type_commit( &GridVertexPacked::FullDatatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::FullDatatype);
               MPI_Type_commit( &GridVertexPacked::FullDatatype );
               #endif
               
            }
            
         }
         
         
         void peano4::GridVertexPacked::shutdownDatatype() {
            MPI_Type_free( &GridVertexPacked::Datatype );
            MPI_Type_free( &GridVertexPacked::FullDatatype );
            
         }
         
         void peano4::GridVertexPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
            // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::GridVertexPacked " 
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
         msg << "was not able to send message peano4::GridVertexPacked "  
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
           msg << "testing for finished send task for peano4::GridVertexPacked " 
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
             "peano4::GridVertexPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::GridVertexPacked", 
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
         
         
         
         void peano4::GridVertexPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::GridVertexPacked from node " 
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
        msg << "failed to start to receive peano4::GridVertexPacked from node " 
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
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertexPacked failed: " 
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
        msg << "testing for finished receive task for peano4::GridVertexPacked failed: " 
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
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::GridVertexPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::GridVertexPacked from node " 
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
         
         
         
         bool peano4::GridVertexPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
         
         int peano4::GridVertexPacked::getSenderRank() const {
            assertion( _senderDestinationRank!=-1 );
            return _senderDestinationRank;
            
         }
      #endif
      
      
      
   
#endif


