#include "GridVertex.h"

#if !defined(PeanoDebug)
   peano4::grid::GridVertex::PersistentRecords::PersistentRecords() {
      
   }
   
   
   peano4::grid::GridVertex::PersistentRecords::PersistentRecords(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep):
   _state(state),
   _adjacentRanks(adjacentRanks),
   _hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep),
   _isAntecessorOfRefinedVertexInCurrentTreeSweep(isAntecessorOfRefinedVertexInCurrentTreeSweep) {
      
   }
   
   
    peano4::grid::GridVertex::State peano4::grid::GridVertex::PersistentRecords::getState() const  {
      return _state;
   }
   
   
   
    void peano4::grid::GridVertex::PersistentRecords::setState(const State& state)  {
      _state = state;
   }
   
   
   
    tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertex::PersistentRecords::getAdjacentRanks() const  {
      return _adjacentRanks;
   }
   
   
   
    void peano4::grid::GridVertex::PersistentRecords::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
      _adjacentRanks = (adjacentRanks);
   }
   
   
   
    bool peano4::grid::GridVertex::PersistentRecords::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
      return _hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
   }
   
   
   
    void peano4::grid::GridVertex::PersistentRecords::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
      _hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep = hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
   }
   
   
   
    bool peano4::grid::GridVertex::PersistentRecords::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
      return _isAntecessorOfRefinedVertexInCurrentTreeSweep;
   }
   
   
   
    void peano4::grid::GridVertex::PersistentRecords::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
      _isAntecessorOfRefinedVertexInCurrentTreeSweep = isAntecessorOfRefinedVertexInCurrentTreeSweep;
   }
   
   
   peano4::grid::GridVertex::GridVertex() {
      
   }
   
   
   peano4::grid::GridVertex::GridVertex(const PersistentRecords& persistentRecords):
   _persistentRecords(persistentRecords._state, persistentRecords._adjacentRanks, persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep) {
      
   }
   
   
   peano4::grid::GridVertex::GridVertex(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep):
   _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep) {
      
   }
   
   
   peano4::grid::GridVertex::GridVertex(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const int& numberOfAdjacentRefinedLocalCells):
   _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep),_numberOfAdjacentRefinedLocalCells(numberOfAdjacentRefinedLocalCells) {
      
   }
   
   peano4::grid::GridVertex::~GridVertex() { }
   
   
    peano4::grid::GridVertex::State peano4::grid::GridVertex::getState() const  {
      return _persistentRecords._state;
   }
   
   
   
    void peano4::grid::GridVertex::setState(const State& state)  {
      _persistentRecords._state = state;
   }
   
   
   
    tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertex::getAdjacentRanks() const  {
      return _persistentRecords._adjacentRanks;
   }
   
   
   
    void peano4::grid::GridVertex::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
      _persistentRecords._adjacentRanks = (adjacentRanks);
   }
   
   
   
    int peano4::grid::GridVertex::getAdjacentRanks(int elementIndex) const  {
      assertion(elementIndex>=0);
      assertion(elementIndex<TwoPowerD);
      return _persistentRecords._adjacentRanks[elementIndex];
      
   }
   
   
   
    void peano4::grid::GridVertex::setAdjacentRanks(int elementIndex, const int& adjacentRanks)  {
      assertion(elementIndex>=0);
      assertion(elementIndex<TwoPowerD);
      _persistentRecords._adjacentRanks[elementIndex]= adjacentRanks;
      
   }
   
   
   
    bool peano4::grid::GridVertex::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
      return _persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
   }
   
   
   
    void peano4::grid::GridVertex::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
      _persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep = hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
   }
   
   
   
    bool peano4::grid::GridVertex::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
      return _persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep;
   }
   
   
   
    void peano4::grid::GridVertex::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
      _persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep = isAntecessorOfRefinedVertexInCurrentTreeSweep;
   }
   
   
   
    int peano4::grid::GridVertex::getNumberOfAdjacentRefinedLocalCells() const  {
      return _numberOfAdjacentRefinedLocalCells;
   }
   
   
   
    void peano4::grid::GridVertex::setNumberOfAdjacentRefinedLocalCells(const int& numberOfAdjacentRefinedLocalCells)  {
      _numberOfAdjacentRefinedLocalCells = numberOfAdjacentRefinedLocalCells;
   }
   
   
   std::string peano4::grid::GridVertex::toString(const State& param) {
      switch (param) {
         case HangingVertex: return "HangingVertex";
         case New: return "New";
         case Unrefined: return "Unrefined";
         case Refined: return "Refined";
         case RefinementTriggered: return "RefinementTriggered";
         case Refining: return "Refining";
         case EraseTriggered: return "EraseTriggered";
         case Erasing: return "Erasing";
         case Delete: return "Delete";
      }
      return "undefined";
   }
   
   std::string peano4::grid::GridVertex::getStateMapping() {
      return "State(HangingVertex=0,New=1,Unrefined=2,Refined=3,RefinementTriggered=4,Refining=5,EraseTriggered=6,Erasing=7,Delete=8)";
   }
   
   
   std::string peano4::grid::GridVertex::toString() const {
      std::ostringstream stringstr;
      toString(stringstr);
      return stringstr.str();
   }
   
   void peano4::grid::GridVertex::toString (std::ostream& out) const {
      out << "("; 
      out << "state:" << toString(getState());
      out << ",";
      out << "adjacentRanks:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getAdjacentRanks(i) << ",";
   }
   out << getAdjacentRanks(TwoPowerD-1) << "]";
      out << ",";
      out << "hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep:" << getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep();
      out << ",";
      out << "isAntecessorOfRefinedVertexInCurrentTreeSweep:" << getIsAntecessorOfRefinedVertexInCurrentTreeSweep();
      out << ",";
      out << "numberOfAdjacentRefinedLocalCells:" << getNumberOfAdjacentRefinedLocalCells();
      out <<  ")";
   }
   
   
   peano4::grid::GridVertex::PersistentRecords peano4::grid::GridVertex::getPersistentRecords() const {
      return _persistentRecords;
   }
   
   peano4::grid::GridVertexPacked peano4::grid::GridVertex::convert() const{
      return GridVertexPacked(
         getState(),
         getAdjacentRanks(),
         getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(),
         getIsAntecessorOfRefinedVertexInCurrentTreeSweep(),
         getNumberOfAdjacentRefinedLocalCells()
      );
   }
   
   #ifdef Parallel
      tarch::logging::Log peano4::grid::GridVertex::_log( "peano4::grid::GridVertex" );
      
      MPI_Datatype peano4::grid::GridVertex::Datatype = 0;
      MPI_Datatype peano4::grid::GridVertex::FullDatatype = 0;
      
      
      void peano4::grid::GridVertex::initDatatype() {
         {
            GridVertex dummyGridVertex[16];
            
            #ifdef MPI2
            const int Attributes = 4;
            #else
            const int Attributes = 4+2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
               #ifndef MPI2
                 MPI_LB,
               #endif
                 MPI_INT		 //state
               , MPI_INT		 //adjacentRanks
               , MPI_CXX_BOOL		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
               , MPI_INT		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
               #ifndef MPI2
               1, // lower bound
               #endif
                 1		 //state
               , TwoPowerD		 //adjacentRanks
               , 1		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
               , 1		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , 1 // upper bound
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            int       currentAddress = -1;
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &disp[currentAddress]);
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #endif
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &disp[currentAddress]);
            #endif
            for (int i=1; i<Attributes; i++) {
            
               assertion1( disp[i] > disp[i-1], i );
            }
            MPI_Aint base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
            #endif
            for (int i=0; i<Attributes; i++) {
            
               disp[i] = disp[i] - base;
               
            }
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Aint typeExtent; 
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &typeExtent);
            typeExtent = MPI_Aint_diff(typeExtent, base);
            MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertex::Datatype );
            MPI_Type_commit( &GridVertex::Datatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::Datatype);
            MPI_Type_commit( &GridVertex::Datatype );
            #endif
            
         }
         {
            GridVertex dummyGridVertex[16];
            
            #ifdef MPI2
            const int Attributes = 5;
            #else
            const int Attributes = 5+2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
               #ifndef MPI2
                 MPI_LB,
               #endif
                 MPI_INT		 //state
               , MPI_INT		 //adjacentRanks
               , MPI_CXX_BOOL		 //hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep
               , MPI_CXX_BOOL		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
               , MPI_INT		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
               #ifndef MPI2
               1, // lower bound
               #endif
                 1		 //state
               , TwoPowerD		 //adjacentRanks
               , 1		 //hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep
               , 1		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
               , 1		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , 1 // upper bound
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            int       currentAddress = -1;
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &disp[currentAddress]);
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #endif
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &disp[currentAddress]);
            #endif
            for (int i=1; i<Attributes; i++) {
            
               assertion1( disp[i] > disp[i-1], i );
            }
            MPI_Aint base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
            #endif
            for (int i=0; i<Attributes; i++) {
            
               disp[i] = disp[i] - base;
               
            }
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Aint typeExtent; 
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &typeExtent);
            typeExtent = MPI_Aint_diff(typeExtent, base);
            MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertex::FullDatatype );
            MPI_Type_commit( &GridVertex::FullDatatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::FullDatatype);
            MPI_Type_commit( &GridVertex::FullDatatype );
            #endif
            
         }
         
      }
      
      
      void peano4::grid::GridVertex::shutdownDatatype() {
         MPI_Type_free( &GridVertex::Datatype );
         MPI_Type_free( &GridVertex::FullDatatype );
         
      }
      
      void peano4::grid::GridVertex::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
         // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridVertex " 
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
         msg << "was not able to send message peano4::grid::GridVertex "  
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
           msg << "testing for finished send task for peano4::grid::GridVertex " 
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
             "peano4::grid::GridVertex", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridVertex", 
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
      
      
      
      void peano4::grid::GridVertex::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridVertex from rank " 
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
        msg << "failed to start to receive peano4::grid::GridVertex from rank " 
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
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertex failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridVertex failed: " 
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
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertex failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridVertex from rank " 
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
      
      
      
      bool peano4::grid::GridVertex::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
      
      int peano4::grid::GridVertex::getSenderRank() const {
         assertion( _senderDestinationRank!=-1 );
         return _senderDestinationRank;
         
      }
   #endif
   
   
   peano4::grid::GridVertexPacked::PersistentRecords::PersistentRecords() {
      if ((6 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((6 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::grid::GridVertexPacked::PersistentRecords::PersistentRecords(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep):
   _adjacentRanks(adjacentRanks) {
      setState(state);
      setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep);
      setIsAntecessorOfRefinedVertexInCurrentTreeSweep(isAntecessorOfRefinedVertexInCurrentTreeSweep);
      if ((6 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((6 < (8 * sizeof(short int))));
      
   }
   
   
    peano4::grid::GridVertex::State peano4::grid::GridVertexPacked::PersistentRecords::getState() const  {
      short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 8));
   return (State) tmp;
   }
   
   
   
    void peano4::grid::GridVertexPacked::PersistentRecords::setState(const State& state)  {
      assertion((state >= 0 && state <= 8));
   short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   _packedRecords0 = static_cast<short int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<short int>(_packedRecords0 | static_cast<short int>(state) << (0));
   }
   
   
   
    tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertexPacked::PersistentRecords::getAdjacentRanks() const  {
      return _adjacentRanks;
   }
   
   
   
    void peano4::grid::GridVertexPacked::PersistentRecords::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
      _adjacentRanks = (adjacentRanks);
   }
   
   
   
    bool peano4::grid::GridVertexPacked::PersistentRecords::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
      short int mask = 1 << (4);
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   return (tmp != 0);
   }
   
   
   
    void peano4::grid::GridVertexPacked::PersistentRecords::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
      short int mask = 1 << (4);
   _packedRecords0 = static_cast<short int>( hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
   }
   
   
   
    bool peano4::grid::GridVertexPacked::PersistentRecords::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
      short int mask = 1 << (5);
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   return (tmp != 0);
   }
   
   
   
    void peano4::grid::GridVertexPacked::PersistentRecords::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
      short int mask = 1 << (5);
   _packedRecords0 = static_cast<short int>( isAntecessorOfRefinedVertexInCurrentTreeSweep ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
   }
   
   
   peano4::grid::GridVertexPacked::GridVertexPacked() {
      if ((6 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((6 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::grid::GridVertexPacked::GridVertexPacked(const PersistentRecords& persistentRecords):
   _persistentRecords(persistentRecords.getState(), persistentRecords._adjacentRanks, persistentRecords.getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(), persistentRecords.getIsAntecessorOfRefinedVertexInCurrentTreeSweep()) {
      if ((6 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((6 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::grid::GridVertexPacked::GridVertexPacked(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep):
   _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep) {
      if ((6 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((6 < (8 * sizeof(short int))));
      
   }
   
   
   peano4::grid::GridVertexPacked::GridVertexPacked(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const int& numberOfAdjacentRefinedLocalCells):
   _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep) {
      setNumberOfAdjacentRefinedLocalCells(numberOfAdjacentRefinedLocalCells);
      if ((6 >= (8 * sizeof(short int)))) {
         std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
         std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
         std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
      }
      assertion((6 < (8 * sizeof(short int))));
      
   }
   
   peano4::grid::GridVertexPacked::~GridVertexPacked() { }
   
   
    peano4::grid::GridVertex::State peano4::grid::GridVertexPacked::getState() const  {
      short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 8));
   return (State) tmp;
   }
   
   
   
    void peano4::grid::GridVertexPacked::setState(const State& state)  {
      assertion((state >= 0 && state <= 8));
   short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 | static_cast<short int>(state) << (0));
   }
   
   
   
    tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertexPacked::getAdjacentRanks() const  {
      return _persistentRecords._adjacentRanks;
   }
   
   
   
    void peano4::grid::GridVertexPacked::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
      _persistentRecords._adjacentRanks = (adjacentRanks);
   }
   
   
   
    int peano4::grid::GridVertexPacked::getAdjacentRanks(int elementIndex) const  {
      assertion(elementIndex>=0);
      assertion(elementIndex<TwoPowerD);
      return _persistentRecords._adjacentRanks[elementIndex];
      
   }
   
   
   
    void peano4::grid::GridVertexPacked::setAdjacentRanks(int elementIndex, const int& adjacentRanks)  {
      assertion(elementIndex>=0);
      assertion(elementIndex<TwoPowerD);
      _persistentRecords._adjacentRanks[elementIndex]= adjacentRanks;
      
   }
   
   
   
    bool peano4::grid::GridVertexPacked::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
      short int mask = 1 << (4);
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
   }
   
   
   
    void peano4::grid::GridVertexPacked::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
      short int mask = 1 << (4);
   _persistentRecords._packedRecords0 = static_cast<short int>( hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
   }
   
   
   
    bool peano4::grid::GridVertexPacked::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
      short int mask = 1 << (5);
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
   }
   
   
   
    void peano4::grid::GridVertexPacked::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
      short int mask = 1 << (5);
   _persistentRecords._packedRecords0 = static_cast<short int>( isAntecessorOfRefinedVertexInCurrentTreeSweep ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
   }
   
   
   
    int peano4::grid::GridVertexPacked::getNumberOfAdjacentRefinedLocalCells() const  {
      return _numberOfAdjacentRefinedLocalCells;
   }
   
   
   
    void peano4::grid::GridVertexPacked::setNumberOfAdjacentRefinedLocalCells(const int& numberOfAdjacentRefinedLocalCells)  {
      _numberOfAdjacentRefinedLocalCells = numberOfAdjacentRefinedLocalCells;
   }
   
   
   std::string peano4::grid::GridVertexPacked::toString(const State& param) {
      return peano4::grid::GridVertex::toString(param);
   }
   
   std::string peano4::grid::GridVertexPacked::getStateMapping() {
      return peano4::grid::GridVertex::getStateMapping();
   }
   
   
   
   std::string peano4::grid::GridVertexPacked::toString() const {
      std::ostringstream stringstr;
      toString(stringstr);
      return stringstr.str();
   }
   
   void peano4::grid::GridVertexPacked::toString (std::ostream& out) const {
      out << "("; 
      out << "state:" << toString(getState());
      out << ",";
      out << "adjacentRanks:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getAdjacentRanks(i) << ",";
   }
   out << getAdjacentRanks(TwoPowerD-1) << "]";
      out << ",";
      out << "hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep:" << getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep();
      out << ",";
      out << "isAntecessorOfRefinedVertexInCurrentTreeSweep:" << getIsAntecessorOfRefinedVertexInCurrentTreeSweep();
      out << ",";
      out << "numberOfAdjacentRefinedLocalCells:" << getNumberOfAdjacentRefinedLocalCells();
      out <<  ")";
   }
   
   
   peano4::grid::GridVertexPacked::PersistentRecords peano4::grid::GridVertexPacked::getPersistentRecords() const {
      return _persistentRecords;
   }
   
   peano4::grid::GridVertex peano4::grid::GridVertexPacked::convert() const{
      return GridVertex(
         getState(),
         getAdjacentRanks(),
         getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(),
         getIsAntecessorOfRefinedVertexInCurrentTreeSweep(),
         getNumberOfAdjacentRefinedLocalCells()
      );
   }
   
   #ifdef Parallel
      tarch::logging::Log peano4::grid::GridVertexPacked::_log( "peano4::grid::GridVertexPacked" );
      
      MPI_Datatype peano4::grid::GridVertexPacked::Datatype = 0;
      MPI_Datatype peano4::grid::GridVertexPacked::FullDatatype = 0;
      
      
      void peano4::grid::GridVertexPacked::initDatatype() {
         {
            GridVertexPacked dummyGridVertexPacked[16];
            
            #ifdef MPI2
            const int Attributes = 3;
            #else
            const int Attributes = 3+2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
               #ifndef MPI2
                 MPI_LB,
               #endif
                 MPI_INT		 //adjacentRanks
               , MPI_SHORT		 //_packedRecords0
               , MPI_INT		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
               #ifndef MPI2
               1, // lower bound
               #endif
                 TwoPowerD		 //adjacentRanks
               , 1		 //_packedRecords0
               , 1		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , 1 // upper bound
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            int       currentAddress = -1;
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &disp[currentAddress]);
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #endif
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &disp[currentAddress]);
            #endif
            for (int i=1; i<Attributes; i++) {
            
               assertion1( disp[i] > disp[i-1], i );
            }
            MPI_Aint base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
            #endif
            for (int i=0; i<Attributes; i++) {
            
               disp[i] = disp[i] - base;
               
            }
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Aint typeExtent; 
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &typeExtent);
            typeExtent = MPI_Aint_diff(typeExtent, base);
            MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertexPacked::Datatype );
            MPI_Type_commit( &GridVertexPacked::Datatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::Datatype);
            MPI_Type_commit( &GridVertexPacked::Datatype );
            #endif
            
         }
         {
            GridVertexPacked dummyGridVertexPacked[16];
            
            #ifdef MPI2
            const int Attributes = 3;
            #else
            const int Attributes = 3+2;
            #endif
            MPI_Datatype subtypes[Attributes] = {
               #ifndef MPI2
                 MPI_LB,
               #endif
                 MPI_INT		 //adjacentRanks
               , MPI_SHORT		 //_packedRecords0
               , MPI_INT		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , MPI_UB
               #endif
               
            };
            
            int blocklen[Attributes] = {
               #ifndef MPI2
               1, // lower bound
               #endif
                 TwoPowerD		 //adjacentRanks
               , 1		 //_packedRecords0
               , 1		 //numberOfAdjacentRefinedLocalCells
               #ifndef MPI2
               , 1 // upper bound
               #endif
               
            };
            
            MPI_Aint  disp[Attributes];
            int       currentAddress = -1;
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &disp[currentAddress]);
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
            #endif
            currentAddress++;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
            #endif
            #ifndef MPI2
            currentAddress++;
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &disp[currentAddress]);
            #endif
            for (int i=1; i<Attributes; i++) {
            
               assertion1( disp[i] > disp[i-1], i );
            }
            MPI_Aint base;
            #ifdef MPI2
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
            #else
            MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
            #endif
            for (int i=0; i<Attributes; i++) {
            
               disp[i] = disp[i] - base;
               
            }
            #ifdef MPI2
            MPI_Datatype tmpType; 
            MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
            MPI_Aint typeExtent; 
            MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &typeExtent);
            typeExtent = MPI_Aint_diff(typeExtent, base);
            MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertexPacked::FullDatatype );
            MPI_Type_commit( &GridVertexPacked::FullDatatype );
            #else
            MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::FullDatatype);
            MPI_Type_commit( &GridVertexPacked::FullDatatype );
            #endif
            
         }
         
      }
      
      
      void peano4::grid::GridVertexPacked::shutdownDatatype() {
         MPI_Type_free( &GridVertexPacked::Datatype );
         MPI_Type_free( &GridVertexPacked::FullDatatype );
         
      }
      
      void peano4::grid::GridVertexPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
         // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridVertexPacked " 
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
         msg << "was not able to send message peano4::grid::GridVertexPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridVertexPacked " 
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
             "peano4::grid::GridVertexPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridVertexPacked", 
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
      
      
      
      void peano4::grid::GridVertexPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridVertexPacked from rank " 
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
        msg << "failed to start to receive peano4::grid::GridVertexPacked from rank " 
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
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertexPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridVertexPacked failed: " 
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
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertexPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridVertexPacked from rank " 
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
      
      
      
      bool peano4::grid::GridVertexPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
      
      int peano4::grid::GridVertexPacked::getSenderRank() const {
         assertion( _senderDestinationRank!=-1 );
         return _senderDestinationRank;
         
      }
   #endif
   
   
   #elif defined(PeanoDebug)
      peano4::grid::GridVertex::PersistentRecords::PersistentRecords() {
         
      }
      
      
      peano4::grid::GridVertex::PersistentRecords::PersistentRecords(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const tarch::la::Vector<Dimensions,double>& x, const int& level):
      _state(state),
      _adjacentRanks(adjacentRanks),
      _hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep),
      _isAntecessorOfRefinedVertexInCurrentTreeSweep(isAntecessorOfRefinedVertexInCurrentTreeSweep),
      _x(x),
      _level(level) {
         
      }
      
      
       peano4::grid::GridVertex::State peano4::grid::GridVertex::PersistentRecords::getState() const  {
         return _state;
      }
      
      
      
       void peano4::grid::GridVertex::PersistentRecords::setState(const State& state)  {
         _state = state;
      }
      
      
      
       tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertex::PersistentRecords::getAdjacentRanks() const  {
         return _adjacentRanks;
      }
      
      
      
       void peano4::grid::GridVertex::PersistentRecords::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
         _adjacentRanks = (adjacentRanks);
      }
      
      
      
       bool peano4::grid::GridVertex::PersistentRecords::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
         return _hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
      }
      
      
      
       void peano4::grid::GridVertex::PersistentRecords::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
         _hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep = hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
      }
      
      
      
       bool peano4::grid::GridVertex::PersistentRecords::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
         return _isAntecessorOfRefinedVertexInCurrentTreeSweep;
      }
      
      
      
       void peano4::grid::GridVertex::PersistentRecords::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
         _isAntecessorOfRefinedVertexInCurrentTreeSweep = isAntecessorOfRefinedVertexInCurrentTreeSweep;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridVertex::PersistentRecords::getX() const  {
         return _x;
      }
      
      
      
       void peano4::grid::GridVertex::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _x = (x);
      }
      
      
      
       int peano4::grid::GridVertex::PersistentRecords::getLevel() const  {
         return _level;
      }
      
      
      
       void peano4::grid::GridVertex::PersistentRecords::setLevel(const int& level)  {
         _level = level;
      }
      
      
      peano4::grid::GridVertex::GridVertex() {
         
      }
      
      
      peano4::grid::GridVertex::GridVertex(const PersistentRecords& persistentRecords):
      _persistentRecords(persistentRecords._state, persistentRecords._adjacentRanks, persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep, persistentRecords._x, persistentRecords._level) {
         
      }
      
      
      peano4::grid::GridVertex::GridVertex(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const tarch::la::Vector<Dimensions,double>& x, const int& level):
      _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep, x, level) {
         
      }
      
      
      peano4::grid::GridVertex::GridVertex(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const int& numberOfAdjacentRefinedLocalCells, const tarch::la::Vector<Dimensions,double>& x, const int& level):
      _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep, x, level),_numberOfAdjacentRefinedLocalCells(numberOfAdjacentRefinedLocalCells) {
         
      }
      
      peano4::grid::GridVertex::~GridVertex() { }
      
      
       peano4::grid::GridVertex::State peano4::grid::GridVertex::getState() const  {
         return _persistentRecords._state;
      }
      
      
      
       void peano4::grid::GridVertex::setState(const State& state)  {
         _persistentRecords._state = state;
      }
      
      
      
       tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertex::getAdjacentRanks() const  {
         return _persistentRecords._adjacentRanks;
      }
      
      
      
       void peano4::grid::GridVertex::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
         _persistentRecords._adjacentRanks = (adjacentRanks);
      }
      
      
      
       int peano4::grid::GridVertex::getAdjacentRanks(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<TwoPowerD);
         return _persistentRecords._adjacentRanks[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridVertex::setAdjacentRanks(int elementIndex, const int& adjacentRanks)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<TwoPowerD);
         _persistentRecords._adjacentRanks[elementIndex]= adjacentRanks;
         
      }
      
      
      
       bool peano4::grid::GridVertex::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
         return _persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
      }
      
      
      
       void peano4::grid::GridVertex::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
         _persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep = hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep;
      }
      
      
      
       bool peano4::grid::GridVertex::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
         return _persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep;
      }
      
      
      
       void peano4::grid::GridVertex::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
         _persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep = isAntecessorOfRefinedVertexInCurrentTreeSweep;
      }
      
      
      
       int peano4::grid::GridVertex::getNumberOfAdjacentRefinedLocalCells() const  {
         return _numberOfAdjacentRefinedLocalCells;
      }
      
      
      
       void peano4::grid::GridVertex::setNumberOfAdjacentRefinedLocalCells(const int& numberOfAdjacentRefinedLocalCells)  {
         _numberOfAdjacentRefinedLocalCells = numberOfAdjacentRefinedLocalCells;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridVertex::getX() const  {
         return _persistentRecords._x;
      }
      
      
      
       void peano4::grid::GridVertex::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _persistentRecords._x = (x);
      }
      
      
      
       double peano4::grid::GridVertex::getX(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         return _persistentRecords._x[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridVertex::setX(int elementIndex, const double& x)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         _persistentRecords._x[elementIndex]= x;
         
      }
      
      
      
       int peano4::grid::GridVertex::getLevel() const  {
         return _persistentRecords._level;
      }
      
      
      
       void peano4::grid::GridVertex::setLevel(const int& level)  {
         _persistentRecords._level = level;
      }
      
      
      std::string peano4::grid::GridVertex::toString(const State& param) {
         switch (param) {
            case HangingVertex: return "HangingVertex";
            case New: return "New";
            case Unrefined: return "Unrefined";
            case Refined: return "Refined";
            case RefinementTriggered: return "RefinementTriggered";
            case Refining: return "Refining";
            case EraseTriggered: return "EraseTriggered";
            case Erasing: return "Erasing";
            case Delete: return "Delete";
         }
         return "undefined";
      }
      
      std::string peano4::grid::GridVertex::getStateMapping() {
         return "State(HangingVertex=0,New=1,Unrefined=2,Refined=3,RefinementTriggered=4,Refining=5,EraseTriggered=6,Erasing=7,Delete=8)";
      }
      
      
      std::string peano4::grid::GridVertex::toString() const {
         std::ostringstream stringstr;
         toString(stringstr);
         return stringstr.str();
      }
      
      void peano4::grid::GridVertex::toString (std::ostream& out) const {
         out << "("; 
         out << "state:" << toString(getState());
         out << ",";
         out << "adjacentRanks:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getAdjacentRanks(i) << ",";
   }
   out << getAdjacentRanks(TwoPowerD-1) << "]";
         out << ",";
         out << "hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep:" << getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep();
         out << ",";
         out << "isAntecessorOfRefinedVertexInCurrentTreeSweep:" << getIsAntecessorOfRefinedVertexInCurrentTreeSweep();
         out << ",";
         out << "numberOfAdjacentRefinedLocalCells:" << getNumberOfAdjacentRefinedLocalCells();
         out << ",";
         out << "x:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(Dimensions-1) << "]";
         out << ",";
         out << "level:" << getLevel();
         out <<  ")";
      }
      
      
      peano4::grid::GridVertex::PersistentRecords peano4::grid::GridVertex::getPersistentRecords() const {
         return _persistentRecords;
      }
      
      peano4::grid::GridVertexPacked peano4::grid::GridVertex::convert() const{
         return GridVertexPacked(
            getState(),
            getAdjacentRanks(),
            getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(),
            getIsAntecessorOfRefinedVertexInCurrentTreeSweep(),
            getNumberOfAdjacentRefinedLocalCells(),
            getX(),
            getLevel()
         );
      }
      
      #ifdef Parallel
         tarch::logging::Log peano4::grid::GridVertex::_log( "peano4::grid::GridVertex" );
         
         MPI_Datatype peano4::grid::GridVertex::Datatype = 0;
         MPI_Datatype peano4::grid::GridVertex::FullDatatype = 0;
         
         
         void peano4::grid::GridVertex::initDatatype() {
            {
               GridVertex dummyGridVertex[16];
               
               #ifdef MPI2
               const int Attributes = 6;
               #else
               const int Attributes = 6+2;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                  #ifndef MPI2
                    MPI_LB,
                  #endif
                    MPI_INT		 //state
                  , MPI_INT		 //adjacentRanks
                  , MPI_CXX_BOOL		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
                  , MPI_DOUBLE		 //x
                  , MPI_INT		 //level
                  , MPI_INT		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                  #ifndef MPI2
                  1, // lower bound
                  #endif
                    1		 //state
                  , TwoPowerD		 //adjacentRanks
                  , 1		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
                  , Dimensions		 //x
                  , 1		 //level
                  , 1		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , 1 // upper bound
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               int       currentAddress = -1;
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &disp[currentAddress]);
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #endif
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &disp[currentAddress]);
               #endif
               for (int i=1; i<Attributes; i++) {
               
                  assertion1( disp[i] > disp[i-1], i );
               }
               MPI_Aint base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
               #endif
               for (int i=0; i<Attributes; i++) {
               
                  disp[i] = disp[i] - base;
                  
               }
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Aint typeExtent; 
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &typeExtent);
               typeExtent = MPI_Aint_diff(typeExtent, base);
               MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertex::Datatype );
               MPI_Type_commit( &GridVertex::Datatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::Datatype);
               MPI_Type_commit( &GridVertex::Datatype );
               #endif
               
            }
            {
               GridVertex dummyGridVertex[16];
               
               #ifdef MPI2
               const int Attributes = 7;
               #else
               const int Attributes = 7+2;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                  #ifndef MPI2
                    MPI_LB,
                  #endif
                    MPI_INT		 //state
                  , MPI_INT		 //adjacentRanks
                  , MPI_CXX_BOOL		 //hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep
                  , MPI_CXX_BOOL		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
                  , MPI_DOUBLE		 //x
                  , MPI_INT		 //level
                  , MPI_INT		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                  #ifndef MPI2
                  1, // lower bound
                  #endif
                    1		 //state
                  , TwoPowerD		 //adjacentRanks
                  , 1		 //hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep
                  , 1		 //isAntecessorOfRefinedVertexInCurrentTreeSweep
                  , Dimensions		 //x
                  , 1		 //level
                  , 1		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , 1 // upper bound
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               int       currentAddress = -1;
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &disp[currentAddress]);
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._state))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._isAntecessorOfRefinedVertexInCurrentTreeSweep))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #endif
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &disp[currentAddress]);
               #endif
               for (int i=1; i<Attributes; i++) {
               
                  assertion1( disp[i] > disp[i-1], i );
               }
               MPI_Aint base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[0]))), &base);
               #endif
               for (int i=0; i<Attributes; i++) {
               
                  disp[i] = disp[i] - base;
                  
               }
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Aint typeExtent; 
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertex[1]))), &typeExtent);
               typeExtent = MPI_Aint_diff(typeExtent, base);
               MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertex::FullDatatype );
               MPI_Type_commit( &GridVertex::FullDatatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertex::FullDatatype);
               MPI_Type_commit( &GridVertex::FullDatatype );
               #endif
               
            }
            
         }
         
         
         void peano4::grid::GridVertex::shutdownDatatype() {
            MPI_Type_free( &GridVertex::Datatype );
            MPI_Type_free( &GridVertex::FullDatatype );
            
         }
         
         void peano4::grid::GridVertex::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
            // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridVertex " 
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
         msg << "was not able to send message peano4::grid::GridVertex "  
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
           msg << "testing for finished send task for peano4::grid::GridVertex " 
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
             "peano4::grid::GridVertex", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridVertex", 
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
         
         
         
         void peano4::grid::GridVertex::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridVertex from rank " 
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
        msg << "failed to start to receive peano4::grid::GridVertex from rank " 
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
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertex failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridVertex failed: " 
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
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertex", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertex failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridVertex from rank " 
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
         
         
         
         bool peano4::grid::GridVertex::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
         
         int peano4::grid::GridVertex::getSenderRank() const {
            assertion( _senderDestinationRank!=-1 );
            return _senderDestinationRank;
            
         }
      #endif
      
      
      peano4::grid::GridVertexPacked::PersistentRecords::PersistentRecords() {
         if ((6 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((6 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::grid::GridVertexPacked::PersistentRecords::PersistentRecords(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const tarch::la::Vector<Dimensions,double>& x, const int& level):
      _adjacentRanks(adjacentRanks),
      _x(x),
      _level(level) {
         setState(state);
         setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep);
         setIsAntecessorOfRefinedVertexInCurrentTreeSweep(isAntecessorOfRefinedVertexInCurrentTreeSweep);
         if ((6 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((6 < (8 * sizeof(short int))));
         
      }
      
      
       peano4::grid::GridVertex::State peano4::grid::GridVertexPacked::PersistentRecords::getState() const  {
         short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 8));
   return (State) tmp;
      }
      
      
      
       void peano4::grid::GridVertexPacked::PersistentRecords::setState(const State& state)  {
         assertion((state >= 0 && state <= 8));
   short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   _packedRecords0 = static_cast<short int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<short int>(_packedRecords0 | static_cast<short int>(state) << (0));
      }
      
      
      
       tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertexPacked::PersistentRecords::getAdjacentRanks() const  {
         return _adjacentRanks;
      }
      
      
      
       void peano4::grid::GridVertexPacked::PersistentRecords::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
         _adjacentRanks = (adjacentRanks);
      }
      
      
      
       bool peano4::grid::GridVertexPacked::PersistentRecords::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
         short int mask = 1 << (4);
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   return (tmp != 0);
      }
      
      
      
       void peano4::grid::GridVertexPacked::PersistentRecords::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
         short int mask = 1 << (4);
   _packedRecords0 = static_cast<short int>( hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
      }
      
      
      
       bool peano4::grid::GridVertexPacked::PersistentRecords::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
         short int mask = 1 << (5);
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   return (tmp != 0);
      }
      
      
      
       void peano4::grid::GridVertexPacked::PersistentRecords::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
         short int mask = 1 << (5);
   _packedRecords0 = static_cast<short int>( isAntecessorOfRefinedVertexInCurrentTreeSweep ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridVertexPacked::PersistentRecords::getX() const  {
         return _x;
      }
      
      
      
       void peano4::grid::GridVertexPacked::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _x = (x);
      }
      
      
      
       int peano4::grid::GridVertexPacked::PersistentRecords::getLevel() const  {
         return _level;
      }
      
      
      
       void peano4::grid::GridVertexPacked::PersistentRecords::setLevel(const int& level)  {
         _level = level;
      }
      
      
      peano4::grid::GridVertexPacked::GridVertexPacked() {
         if ((6 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((6 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::grid::GridVertexPacked::GridVertexPacked(const PersistentRecords& persistentRecords):
      _persistentRecords(persistentRecords.getState(), persistentRecords._adjacentRanks, persistentRecords.getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(), persistentRecords.getIsAntecessorOfRefinedVertexInCurrentTreeSweep(), persistentRecords._x, persistentRecords._level) {
         if ((6 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((6 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::grid::GridVertexPacked::GridVertexPacked(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const tarch::la::Vector<Dimensions,double>& x, const int& level):
      _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep, x, level) {
         if ((6 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((6 < (8 * sizeof(short int))));
         
      }
      
      
      peano4::grid::GridVertexPacked::GridVertexPacked(const State& state, const tarch::la::Vector<TwoPowerD,int>& adjacentRanks, const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep, const int& numberOfAdjacentRefinedLocalCells, const tarch::la::Vector<Dimensions,double>& x, const int& level):
      _persistentRecords(state, adjacentRanks, hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep, isAntecessorOfRefinedVertexInCurrentTreeSweep, x, level) {
         setNumberOfAdjacentRefinedLocalCells(numberOfAdjacentRefinedLocalCells);
         if ((6 >= (8 * sizeof(short int)))) {
            std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
            std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
            std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
         }
         assertion((6 < (8 * sizeof(short int))));
         
      }
      
      peano4::grid::GridVertexPacked::~GridVertexPacked() { }
      
      
       peano4::grid::GridVertex::State peano4::grid::GridVertexPacked::getState() const  {
         short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<short int>(tmp >> (0));
   assertion(( tmp >= 0 &&  tmp <= 8));
   return (State) tmp;
      }
      
      
      
       void peano4::grid::GridVertexPacked::setState(const State& state)  {
         assertion((state >= 0 && state <= 8));
   short int mask =  (1 << (4)) - 1;
   mask = static_cast<short int>(mask << (0));
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<short int>(_persistentRecords._packedRecords0 | static_cast<short int>(state) << (0));
      }
      
      
      
       tarch::la::Vector<TwoPowerD,int> peano4::grid::GridVertexPacked::getAdjacentRanks() const  {
         return _persistentRecords._adjacentRanks;
      }
      
      
      
       void peano4::grid::GridVertexPacked::setAdjacentRanks(const tarch::la::Vector<TwoPowerD,int>& adjacentRanks)  {
         _persistentRecords._adjacentRanks = (adjacentRanks);
      }
      
      
      
       int peano4::grid::GridVertexPacked::getAdjacentRanks(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<TwoPowerD);
         return _persistentRecords._adjacentRanks[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridVertexPacked::setAdjacentRanks(int elementIndex, const int& adjacentRanks)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<TwoPowerD);
         _persistentRecords._adjacentRanks[elementIndex]= adjacentRanks;
         
      }
      
      
      
       bool peano4::grid::GridVertexPacked::getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep() const  {
         short int mask = 1 << (4);
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
      }
      
      
      
       void peano4::grid::GridVertexPacked::setHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(const bool& hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep)  {
         short int mask = 1 << (4);
   _persistentRecords._packedRecords0 = static_cast<short int>( hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
      }
      
      
      
       bool peano4::grid::GridVertexPacked::getIsAntecessorOfRefinedVertexInCurrentTreeSweep() const  {
         short int mask = 1 << (5);
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
      }
      
      
      
       void peano4::grid::GridVertexPacked::setIsAntecessorOfRefinedVertexInCurrentTreeSweep(const bool& isAntecessorOfRefinedVertexInCurrentTreeSweep)  {
         short int mask = 1 << (5);
   _persistentRecords._packedRecords0 = static_cast<short int>( isAntecessorOfRefinedVertexInCurrentTreeSweep ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
      }
      
      
      
       int peano4::grid::GridVertexPacked::getNumberOfAdjacentRefinedLocalCells() const  {
         return _numberOfAdjacentRefinedLocalCells;
      }
      
      
      
       void peano4::grid::GridVertexPacked::setNumberOfAdjacentRefinedLocalCells(const int& numberOfAdjacentRefinedLocalCells)  {
         _numberOfAdjacentRefinedLocalCells = numberOfAdjacentRefinedLocalCells;
      }
      
      
      
       tarch::la::Vector<Dimensions,double> peano4::grid::GridVertexPacked::getX() const  {
         return _persistentRecords._x;
      }
      
      
      
       void peano4::grid::GridVertexPacked::setX(const tarch::la::Vector<Dimensions,double>& x)  {
         _persistentRecords._x = (x);
      }
      
      
      
       double peano4::grid::GridVertexPacked::getX(int elementIndex) const  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         return _persistentRecords._x[elementIndex];
         
      }
      
      
      
       void peano4::grid::GridVertexPacked::setX(int elementIndex, const double& x)  {
         assertion(elementIndex>=0);
         assertion(elementIndex<Dimensions);
         _persistentRecords._x[elementIndex]= x;
         
      }
      
      
      
       int peano4::grid::GridVertexPacked::getLevel() const  {
         return _persistentRecords._level;
      }
      
      
      
       void peano4::grid::GridVertexPacked::setLevel(const int& level)  {
         _persistentRecords._level = level;
      }
      
      
      std::string peano4::grid::GridVertexPacked::toString(const State& param) {
         return peano4::grid::GridVertex::toString(param);
      }
      
      std::string peano4::grid::GridVertexPacked::getStateMapping() {
         return peano4::grid::GridVertex::getStateMapping();
      }
      
      
      
      std::string peano4::grid::GridVertexPacked::toString() const {
         std::ostringstream stringstr;
         toString(stringstr);
         return stringstr.str();
      }
      
      void peano4::grid::GridVertexPacked::toString (std::ostream& out) const {
         out << "("; 
         out << "state:" << toString(getState());
         out << ",";
         out << "adjacentRanks:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getAdjacentRanks(i) << ",";
   }
   out << getAdjacentRanks(TwoPowerD-1) << "]";
         out << ",";
         out << "hasBeenAntecessorOfRefinedVertexInPreviousTreeSweep:" << getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep();
         out << ",";
         out << "isAntecessorOfRefinedVertexInCurrentTreeSweep:" << getIsAntecessorOfRefinedVertexInCurrentTreeSweep();
         out << ",";
         out << "numberOfAdjacentRefinedLocalCells:" << getNumberOfAdjacentRefinedLocalCells();
         out << ",";
         out << "x:[";
   for (int i = 0; i < Dimensions-1; i++) {
      out << getX(i) << ",";
   }
   out << getX(Dimensions-1) << "]";
         out << ",";
         out << "level:" << getLevel();
         out <<  ")";
      }
      
      
      peano4::grid::GridVertexPacked::PersistentRecords peano4::grid::GridVertexPacked::getPersistentRecords() const {
         return _persistentRecords;
      }
      
      peano4::grid::GridVertex peano4::grid::GridVertexPacked::convert() const{
         return GridVertex(
            getState(),
            getAdjacentRanks(),
            getHasBeenAntecessorOfRefinedVertexInPreviousTreeSweep(),
            getIsAntecessorOfRefinedVertexInCurrentTreeSweep(),
            getNumberOfAdjacentRefinedLocalCells(),
            getX(),
            getLevel()
         );
      }
      
      #ifdef Parallel
         tarch::logging::Log peano4::grid::GridVertexPacked::_log( "peano4::grid::GridVertexPacked" );
         
         MPI_Datatype peano4::grid::GridVertexPacked::Datatype = 0;
         MPI_Datatype peano4::grid::GridVertexPacked::FullDatatype = 0;
         
         
         void peano4::grid::GridVertexPacked::initDatatype() {
            {
               GridVertexPacked dummyGridVertexPacked[16];
               
               #ifdef MPI2
               const int Attributes = 5;
               #else
               const int Attributes = 5+2;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                  #ifndef MPI2
                    MPI_LB,
                  #endif
                    MPI_INT		 //adjacentRanks
                  , MPI_DOUBLE		 //x
                  , MPI_INT		 //level
                  , MPI_SHORT		 //_packedRecords0
                  , MPI_INT		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                  #ifndef MPI2
                  1, // lower bound
                  #endif
                    TwoPowerD		 //adjacentRanks
                  , Dimensions		 //x
                  , 1		 //level
                  , 1		 //_packedRecords0
                  , 1		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , 1 // upper bound
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               int       currentAddress = -1;
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &disp[currentAddress]);
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #endif
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &disp[currentAddress]);
               #endif
               for (int i=1; i<Attributes; i++) {
               
                  assertion1( disp[i] > disp[i-1], i );
               }
               MPI_Aint base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
               #endif
               for (int i=0; i<Attributes; i++) {
               
                  disp[i] = disp[i] - base;
                  
               }
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Aint typeExtent; 
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &typeExtent);
               typeExtent = MPI_Aint_diff(typeExtent, base);
               MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertexPacked::Datatype );
               MPI_Type_commit( &GridVertexPacked::Datatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::Datatype);
               MPI_Type_commit( &GridVertexPacked::Datatype );
               #endif
               
            }
            {
               GridVertexPacked dummyGridVertexPacked[16];
               
               #ifdef MPI2
               const int Attributes = 5;
               #else
               const int Attributes = 5+2;
               #endif
               MPI_Datatype subtypes[Attributes] = {
                  #ifndef MPI2
                    MPI_LB,
                  #endif
                    MPI_INT		 //adjacentRanks
                  , MPI_DOUBLE		 //x
                  , MPI_INT		 //level
                  , MPI_SHORT		 //_packedRecords0
                  , MPI_INT		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , MPI_UB
                  #endif
                  
               };
               
               int blocklen[Attributes] = {
                  #ifndef MPI2
                  1, // lower bound
                  #endif
                    TwoPowerD		 //adjacentRanks
                  , Dimensions		 //x
                  , 1		 //level
                  , 1		 //_packedRecords0
                  , 1		 //numberOfAdjacentRefinedLocalCells
                  #ifndef MPI2
                  , 1 // upper bound
                  #endif
                  
               };
               
               MPI_Aint  disp[Attributes];
               int       currentAddress = -1;
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &disp[currentAddress]);
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._adjacentRanks[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._level))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
               #endif
               currentAddress++;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]._numberOfAdjacentRefinedLocalCells))), 		&disp[currentAddress] );
               #endif
               #ifndef MPI2
               currentAddress++;
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &disp[currentAddress]);
               #endif
               for (int i=1; i<Attributes; i++) {
               
                  assertion1( disp[i] > disp[i-1], i );
               }
               MPI_Aint base;
               #ifdef MPI2
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
               #else
               MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[0]))), &base);
               #endif
               for (int i=0; i<Attributes; i++) {
               
                  disp[i] = disp[i] - base;
                  
               }
               #ifdef MPI2
               MPI_Datatype tmpType; 
               MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
               MPI_Aint typeExtent; 
               MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridVertexPacked[1]))), &typeExtent);
               typeExtent = MPI_Aint_diff(typeExtent, base);
               MPI_Type_create_resized( tmpType, 0, typeExtent, &GridVertexPacked::FullDatatype );
               MPI_Type_commit( &GridVertexPacked::FullDatatype );
               #else
               MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridVertexPacked::FullDatatype);
               MPI_Type_commit( &GridVertexPacked::FullDatatype );
               #endif
               
            }
            
         }
         
         
         void peano4::grid::GridVertexPacked::shutdownDatatype() {
            MPI_Type_free( &GridVertexPacked::Datatype );
            MPI_Type_free( &GridVertexPacked::FullDatatype );
            
         }
         
         void peano4::grid::GridVertexPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
            // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridVertexPacked " 
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
         msg << "was not able to send message peano4::grid::GridVertexPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridVertexPacked " 
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
             "peano4::grid::GridVertexPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridVertexPacked", 
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
         
         
         
         void peano4::grid::GridVertexPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridVertexPacked from rank " 
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
        msg << "failed to start to receive peano4::grid::GridVertexPacked from rank " 
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
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertexPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridVertexPacked failed: " 
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
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridVertexPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridVertexPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridVertexPacked from rank " 
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
         
         
         
         bool peano4::grid::GridVertexPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
         
         int peano4::grid::GridVertexPacked::getSenderRank() const {
            assertion( _senderDestinationRank!=-1 );
            return _senderDestinationRank;
            
         }
      #endif
      
      
      
   
#endif


