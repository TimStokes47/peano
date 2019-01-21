#include "peano4/grid/GridTraversalEvent.h"

peano4::grid::GridTraversalEvent::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridTraversalEvent::PersistentRecords::PersistentRecords(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexData, const tarch::la::Vector<TwoTimesD,int>& faceData, const int& cellData):
_x(x),
_h(h),
_isRefined(isRefined),
_vertexData(vertexData),
_faceData(faceData),
_cellData(cellData) {
   
}


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::PersistentRecords::getX() const  {
   return _x;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _x = (x);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::PersistentRecords::getH() const  {
   return _h;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _h = (h);
}



 bool peano4::grid::GridTraversalEvent::PersistentRecords::getIsRefined() const  {
   return _isRefined;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setIsRefined(const bool& isRefined)  {
   _isRefined = isRefined;
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getVertexData() const  {
   return _vertexData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setVertexData(const tarch::la::Vector<TwoPowerD,int>& vertexData)  {
   _vertexData = (vertexData);
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::PersistentRecords::getFaceData() const  {
   return _faceData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setFaceData(const tarch::la::Vector<TwoTimesD,int>& faceData)  {
   _faceData = (faceData);
}



 int peano4::grid::GridTraversalEvent::PersistentRecords::getCellData() const  {
   return _cellData;
}



 void peano4::grid::GridTraversalEvent::PersistentRecords::setCellData(const int& cellData)  {
   _cellData = cellData;
}


peano4::grid::GridTraversalEvent::GridTraversalEvent() {
   
}


peano4::grid::GridTraversalEvent::GridTraversalEvent(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._x, persistentRecords._h, persistentRecords._isRefined, persistentRecords._vertexData, persistentRecords._faceData, persistentRecords._cellData) {
   
}


peano4::grid::GridTraversalEvent::GridTraversalEvent(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexData, const tarch::la::Vector<TwoTimesD,int>& faceData, const int& cellData):
_persistentRecords(x, h, isRefined, vertexData, faceData, cellData) {
   
}


peano4::grid::GridTraversalEvent::~GridTraversalEvent() { }


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::getX() const  {
   return _persistentRecords._x;
}



 void peano4::grid::GridTraversalEvent::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _persistentRecords._x = (x);
}



 double peano4::grid::GridTraversalEvent::getX(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._x[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setX(int elementIndex, const double& x)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._x[elementIndex]= x;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEvent::getH() const  {
   return _persistentRecords._h;
}



 void peano4::grid::GridTraversalEvent::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _persistentRecords._h = (h);
}



 double peano4::grid::GridTraversalEvent::getH(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._h[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setH(int elementIndex, const double& h)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._h[elementIndex]= h;
   
}



 bool peano4::grid::GridTraversalEvent::getIsRefined() const  {
   return _persistentRecords._isRefined;
}



 void peano4::grid::GridTraversalEvent::setIsRefined(const bool& isRefined)  {
   _persistentRecords._isRefined = isRefined;
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEvent::getVertexData() const  {
   return _persistentRecords._vertexData;
}



 void peano4::grid::GridTraversalEvent::setVertexData(const tarch::la::Vector<TwoPowerD,int>& vertexData)  {
   _persistentRecords._vertexData = (vertexData);
}



 int peano4::grid::GridTraversalEvent::getVertexData(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   return _persistentRecords._vertexData[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setVertexData(int elementIndex, const int& vertexData)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   _persistentRecords._vertexData[elementIndex]= vertexData;
   
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEvent::getFaceData() const  {
   return _persistentRecords._faceData;
}



 void peano4::grid::GridTraversalEvent::setFaceData(const tarch::la::Vector<TwoTimesD,int>& faceData)  {
   _persistentRecords._faceData = (faceData);
}



 int peano4::grid::GridTraversalEvent::getFaceData(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   return _persistentRecords._faceData[elementIndex];
   
}



 void peano4::grid::GridTraversalEvent::setFaceData(int elementIndex, const int& faceData)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   _persistentRecords._faceData[elementIndex]= faceData;
   
}



 int peano4::grid::GridTraversalEvent::getCellData() const  {
   return _persistentRecords._cellData;
}



 void peano4::grid::GridTraversalEvent::setCellData(const int& cellData)  {
   _persistentRecords._cellData = cellData;
}


std::string peano4::grid::GridTraversalEvent::toString(const RefinementControl& param) {
   switch (param) {
      case Enter: return "Enter";
      case Leave: return "Leave";
   }
   return "undefined";
}

std::string peano4::grid::GridTraversalEvent::getRefinementControlMapping() {
   return "RefinementControl(Enter=0,Leave=1)";
}


std::string peano4::grid::GridTraversalEvent::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::GridTraversalEvent::toString (std::ostream& out) const {
   out << "("; 
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
   out << "isRefined:" << getIsRefined();
   out << ",";
   out << "vertexData:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getVertexData(i) << ",";
   }
   out << getVertexData(TwoPowerD-1) << "]";
   out << ",";
   out << "faceData:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getFaceData(i) << ",";
   }
   out << getFaceData(TwoTimesD-1) << "]";
   out << ",";
   out << "cellData:" << getCellData();
   out <<  ")";
}


peano4::grid::GridTraversalEvent::PersistentRecords peano4::grid::GridTraversalEvent::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::GridTraversalEventPacked peano4::grid::GridTraversalEvent::convert() const{
   return GridTraversalEventPacked(
      getX(),
      getH(),
      getIsRefined(),
      getVertexData(),
      getFaceData(),
      getCellData()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridTraversalEvent::_log( "peano4::grid::GridTraversalEvent" );
   
   MPI_Datatype peano4::grid::GridTraversalEvent::Datatype = 0;
   MPI_Datatype peano4::grid::GridTraversalEvent::FullDatatype = 0;
   
   
   void peano4::grid::GridTraversalEvent::initDatatype() {
      {
         GridTraversalEvent dummyGridTraversalEvent[16];
         
         #ifdef MPI2
         const int Attributes = 6;
         #else
         const int Attributes = 6+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_CXX_BOOL		 //isRefined
            , MPI_INT		 //vertexData
            , MPI_INT		 //faceData
            , MPI_INT		 //cellData
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , 1		 //isRefined
            , TwoPowerD		 //vertexData
            , TwoTimesD		 //faceData
            , 1		 //cellData
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEvent::Datatype );
         MPI_Type_commit( &GridTraversalEvent::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEvent::Datatype);
         MPI_Type_commit( &GridTraversalEvent::Datatype );
         #endif
         
      }
      {
         GridTraversalEvent dummyGridTraversalEvent[16];
         
         #ifdef MPI2
         const int Attributes = 6;
         #else
         const int Attributes = 6+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_CXX_BOOL		 //isRefined
            , MPI_INT		 //vertexData
            , MPI_INT		 //faceData
            , MPI_INT		 //cellData
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , 1		 //isRefined
            , TwoPowerD		 //vertexData
            , TwoTimesD		 //faceData
            , 1		 //cellData
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._isRefined))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._vertexData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceData[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._faceData[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]._persistentRecords._cellData))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEvent[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEvent::FullDatatype );
         MPI_Type_commit( &GridTraversalEvent::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEvent::FullDatatype);
         MPI_Type_commit( &GridTraversalEvent::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::grid::GridTraversalEvent::shutdownDatatype() {
      MPI_Type_free( &GridTraversalEvent::Datatype );
      MPI_Type_free( &GridTraversalEvent::FullDatatype );
      
   }
   
   void peano4::grid::GridTraversalEvent::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridTraversalEvent " 
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
         msg << "was not able to send message peano4::grid::GridTraversalEvent "  
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
           msg << "testing for finished send task for peano4::grid::GridTraversalEvent " 
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
             "peano4::grid::GridTraversalEvent", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridTraversalEvent", 
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
   
   
   
   void peano4::grid::GridTraversalEvent::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridTraversalEvent from node " 
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
        msg << "failed to start to receive peano4::grid::GridTraversalEvent from node " 
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
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEvent failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridTraversalEvent failed: " 
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
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEvent", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEvent failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEvent from node " 
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
   
   
   
   bool peano4::grid::GridTraversalEvent::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridTraversalEvent::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::grid::GridTraversalEventPacked::PersistentRecords::PersistentRecords() {
   if ((20 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((20 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::PersistentRecords::PersistentRecords(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexData, const tarch::la::Vector<TwoTimesD,int>& faceData, const int& cellData):
_x(x),
_h(h) {
   setIsRefined(isRefined);
   setVertexData(vertexData);
   setFaceData(faceData);
   setCellData(cellData);
   if ((20 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((20 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   
}


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::PersistentRecords::getX() const  {
   return _x;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _x = (x);
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::PersistentRecords::getH() const  {
   return _h;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _h = (h);
}



 bool peano4::grid::GridTraversalEventPacked::PersistentRecords::getIsRefined() const  {
   long int mask = 1 << (0);
   long int tmp = static_cast<long int>(_packedRecords0 & mask);
   return (tmp != 0);
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setIsRefined(const bool& isRefined)  {
   long int mask = 1 << (0);
   _packedRecords0 = static_cast<long int>( isRefined ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getVertexData() const  {
   long int mask = 15;
   mask = mask << (0);
   tarch::la::Vector<TwoPowerD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _packedRecords1 & mask;
      tmp = tmp >> (0 + i * 4);
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= 0 && result[i] <= 9));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setVertexData(const tarch::la::Vector<TwoPowerD,int>& vertexData)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((vertexData[i] >= 0 && vertexData[i] <= 9));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 15;
      mask = mask << (0 + i * 4 );
      _packedRecords1 = _packedRecords1 & ~mask;
      long int tmp = static_cast<long int>(vertexData[i]);
      _packedRecords1 = _packedRecords1 | ( tmp << (0 + i * 4));
   }
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::PersistentRecords::getFaceData() const  {
   long int mask = 7;
   mask = mask << (1);
   tarch::la::Vector<TwoTimesD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _packedRecords0 & mask;
      tmp = tmp >> (1 + i * 3);
      result[i] = (int) tmp;
      mask = mask << 3;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= 0 && result[i] <= 7));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setFaceData(const tarch::la::Vector<TwoTimesD,int>& faceData)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((faceData[i] >= 0 && faceData[i] <= 7));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 7;
      mask = mask << (1 + i * 3 );
      _packedRecords0 = _packedRecords0 & ~mask;
      long int tmp = static_cast<long int>(faceData[i]);
      _packedRecords0 = _packedRecords0 | ( tmp << (1 + i * 3));
   }
}



 int peano4::grid::GridTraversalEventPacked::PersistentRecords::getCellData() const  {
   long int mask =  (1 << (1)) - 1;
   mask = static_cast<long int>(mask << (19));
   long int tmp = static_cast<long int>(_packedRecords0 & mask);
   tmp = static_cast<long int>(tmp >> (19));
   assertion(( tmp >= 0 &&  tmp <= 1));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::PersistentRecords::setCellData(const int& cellData)  {
   assertion((cellData >= 0 && cellData <= 1));
   long int mask =  (1 << (1)) - 1;
   mask = static_cast<long int>(mask << (19));
   _packedRecords0 = static_cast<long int>(_packedRecords0 & ~mask);
   _packedRecords0 = static_cast<long int>(_packedRecords0 | static_cast<long int>(cellData) << (19));
}


peano4::grid::GridTraversalEventPacked::GridTraversalEventPacked() {
   if ((20 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((20 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::GridTraversalEventPacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._x, persistentRecords._h, persistentRecords.getIsRefined(), persistentRecords.getVertexData(), persistentRecords.getFaceData(), persistentRecords.getCellData()) {
   if ((20 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((20 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::GridTraversalEventPacked(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>& h, const bool& isRefined, const tarch::la::Vector<TwoPowerD,int>& vertexData, const tarch::la::Vector<TwoTimesD,int>& faceData, const int& cellData):
_persistentRecords(x, h, isRefined, vertexData, faceData, cellData) {
   if ((20 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((20 < (8 * sizeof(long int))));
   if ((64 >= (8 * sizeof(long int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: long int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((64 < (8 * sizeof(long int))));
   
}


peano4::grid::GridTraversalEventPacked::~GridTraversalEventPacked() { }


 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::getX() const  {
   return _persistentRecords._x;
}



 void peano4::grid::GridTraversalEventPacked::setX(const tarch::la::Vector<Dimensions,double>& x)  {
   _persistentRecords._x = (x);
}



 double peano4::grid::GridTraversalEventPacked::getX(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._x[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setX(int elementIndex, const double& x)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._x[elementIndex]= x;
   
}



 tarch::la::Vector<Dimensions,double> peano4::grid::GridTraversalEventPacked::getH() const  {
   return _persistentRecords._h;
}



 void peano4::grid::GridTraversalEventPacked::setH(const tarch::la::Vector<Dimensions,double>& h)  {
   _persistentRecords._h = (h);
}



 double peano4::grid::GridTraversalEventPacked::getH(int elementIndex) const  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   return _persistentRecords._h[elementIndex];
   
}



 void peano4::grid::GridTraversalEventPacked::setH(int elementIndex, const double& h)  {
   assertion(elementIndex>=0);
   assertion(elementIndex<Dimensions);
   _persistentRecords._h[elementIndex]= h;
   
}



 bool peano4::grid::GridTraversalEventPacked::getIsRefined() const  {
   long int mask = 1 << (0);
   long int tmp = static_cast<long int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
}



 void peano4::grid::GridTraversalEventPacked::setIsRefined(const bool& isRefined)  {
   long int mask = 1 << (0);
   _persistentRecords._packedRecords0 = static_cast<long int>( isRefined ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
}



 tarch::la::Vector<TwoPowerD,int> peano4::grid::GridTraversalEventPacked::getVertexData() const  {
   long int mask = 15;
   mask = mask << (0);
   tarch::la::Vector<TwoPowerD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoPowerD; i++) {
      tmp = _persistentRecords._packedRecords1 & mask;
      tmp = tmp >> (0 + i * 4);
      result[i] = (int) tmp;
      mask = mask << 4;
   }
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((result[i] >= 0 && result[i] <= 9));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setVertexData(const tarch::la::Vector<TwoPowerD,int>& vertexData)  {
   
   for (int i = 0; i < TwoPowerD; i++) {
      assertion((vertexData[i] >= 0 && vertexData[i] <= 9));
   }
   for (int i = 0; i < TwoPowerD; i++) {
      long int mask = 15;
      mask = mask << (0 + i * 4 );
      _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 & ~mask;
      long int tmp = static_cast<long int>(vertexData[i]);
      _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 | ( tmp << (0 + i * 4));
   }
}



 int peano4::grid::GridTraversalEventPacked::getVertexData(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   long int mask = 15;
   mask = mask << (0 + elementIndex * 4);
   long int tmp = _persistentRecords._packedRecords1 & mask;
   tmp = tmp >> (0 + elementIndex * 4);
   assertion((tmp >= 0) && (tmp <= 9));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setVertexData(int elementIndex, const int& vertexData)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoPowerD);
   assertion((vertexData >= 0 && vertexData <= 9));
   long int mask = 15;
   mask = mask << (0 + elementIndex * 4);
   _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 & ~mask;
   long int tmp = static_cast<long int>(vertexData);
   _persistentRecords._packedRecords1 = _persistentRecords._packedRecords1 | ( tmp << (0 + elementIndex * 4));
}



 tarch::la::Vector<TwoTimesD,int> peano4::grid::GridTraversalEventPacked::getFaceData() const  {
   long int mask = 7;
   mask = mask << (1);
   tarch::la::Vector<TwoTimesD,int> result;
   long int tmp;
   
   for (int i = 0; i < TwoTimesD; i++) {
      tmp = _persistentRecords._packedRecords0 & mask;
      tmp = tmp >> (1 + i * 3);
      result[i] = (int) tmp;
      mask = mask << 3;
   }
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((result[i] >= 0 && result[i] <= 7));
   }
   return result;
}



 void peano4::grid::GridTraversalEventPacked::setFaceData(const tarch::la::Vector<TwoTimesD,int>& faceData)  {
   
   for (int i = 0; i < TwoTimesD; i++) {
      assertion((faceData[i] >= 0 && faceData[i] <= 7));
   }
   for (int i = 0; i < TwoTimesD; i++) {
      long int mask = 7;
      mask = mask << (1 + i * 3 );
      _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
      long int tmp = static_cast<long int>(faceData[i]);
      _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 | ( tmp << (1 + i * 3));
   }
}



 int peano4::grid::GridTraversalEventPacked::getFaceData(int elementIndex) const  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   long int mask = 7;
   mask = mask << (1 + elementIndex * 3);
   long int tmp = _persistentRecords._packedRecords0 & mask;
   tmp = tmp >> (1 + elementIndex * 3);
   assertion((tmp >= 0) && (tmp <= 7));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setFaceData(int elementIndex, const int& faceData)  {
   
   assertion(elementIndex>=0);
   assertion(elementIndex<TwoTimesD);
   assertion((faceData >= 0 && faceData <= 7));
   long int mask = 7;
   mask = mask << (1 + elementIndex * 3);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 & ~mask;
   long int tmp = static_cast<long int>(faceData);
   _persistentRecords._packedRecords0 = _persistentRecords._packedRecords0 | ( tmp << (1 + elementIndex * 3));
}



 int peano4::grid::GridTraversalEventPacked::getCellData() const  {
   long int mask =  (1 << (1)) - 1;
   mask = static_cast<long int>(mask << (19));
   long int tmp = static_cast<long int>(_persistentRecords._packedRecords0 & mask);
   tmp = static_cast<long int>(tmp >> (19));
   assertion(( tmp >= 0 &&  tmp <= 1));
   return (int) tmp;
}



 void peano4::grid::GridTraversalEventPacked::setCellData(const int& cellData)  {
   assertion((cellData >= 0 && cellData <= 1));
   long int mask =  (1 << (1)) - 1;
   mask = static_cast<long int>(mask << (19));
   _persistentRecords._packedRecords0 = static_cast<long int>(_persistentRecords._packedRecords0 & ~mask);
   _persistentRecords._packedRecords0 = static_cast<long int>(_persistentRecords._packedRecords0 | static_cast<long int>(cellData) << (19));
}


std::string peano4::grid::GridTraversalEventPacked::toString(const RefinementControl& param) {
   return peano4::grid::GridTraversalEvent::toString(param);
}

std::string peano4::grid::GridTraversalEventPacked::getRefinementControlMapping() {
   return peano4::grid::GridTraversalEvent::getRefinementControlMapping();
}



std::string peano4::grid::GridTraversalEventPacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::GridTraversalEventPacked::toString (std::ostream& out) const {
   out << "("; 
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
   out << "isRefined:" << getIsRefined();
   out << ",";
   out << "vertexData:[";
   for (int i = 0; i < TwoPowerD-1; i++) {
      out << getVertexData(i) << ",";
   }
   out << getVertexData(TwoPowerD-1) << "]";
   out << ",";
   out << "faceData:[";
   for (int i = 0; i < TwoTimesD-1; i++) {
      out << getFaceData(i) << ",";
   }
   out << getFaceData(TwoTimesD-1) << "]";
   out << ",";
   out << "cellData:" << getCellData();
   out <<  ")";
}


peano4::grid::GridTraversalEventPacked::PersistentRecords peano4::grid::GridTraversalEventPacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::GridTraversalEvent peano4::grid::GridTraversalEventPacked::convert() const{
   return GridTraversalEvent(
      getX(),
      getH(),
      getIsRefined(),
      getVertexData(),
      getFaceData(),
      getCellData()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridTraversalEventPacked::_log( "peano4::grid::GridTraversalEventPacked" );
   
   MPI_Datatype peano4::grid::GridTraversalEventPacked::Datatype = 0;
   MPI_Datatype peano4::grid::GridTraversalEventPacked::FullDatatype = 0;
   
   
   void peano4::grid::GridTraversalEventPacked::initDatatype() {
      {
         GridTraversalEventPacked dummyGridTraversalEventPacked[16];
         
         #ifdef MPI2
         const int Attributes = 4;
         #else
         const int Attributes = 4+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_LONG		 //_packedRecords0
            , MPI_LONG		 //_packedRecords1
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , 1		 //_packedRecords0
            , 1		 //_packedRecords1
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEventPacked::Datatype );
         MPI_Type_commit( &GridTraversalEventPacked::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEventPacked::Datatype);
         MPI_Type_commit( &GridTraversalEventPacked::Datatype );
         #endif
         
      }
      {
         GridTraversalEventPacked dummyGridTraversalEventPacked[16];
         
         #ifdef MPI2
         const int Attributes = 4;
         #else
         const int Attributes = 4+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_DOUBLE		 //x
            , MPI_DOUBLE		 //h
            , MPI_LONG		 //_packedRecords0
            , MPI_LONG		 //_packedRecords1
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              Dimensions		 //x
            , Dimensions		 //h
            , 1		 //_packedRecords0
            , 1		 //_packedRecords1
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._x[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._h[0]))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]._persistentRecords._packedRecords1))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridTraversalEventPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         MPI_Type_create_resized( tmpType, 0, typeExtent, &GridTraversalEventPacked::FullDatatype );
         MPI_Type_commit( &GridTraversalEventPacked::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridTraversalEventPacked::FullDatatype);
         MPI_Type_commit( &GridTraversalEventPacked::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::grid::GridTraversalEventPacked::shutdownDatatype() {
      MPI_Type_free( &GridTraversalEventPacked::Datatype );
      MPI_Type_free( &GridTraversalEventPacked::FullDatatype );
      
   }
   
   void peano4::grid::GridTraversalEventPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridTraversalEventPacked " 
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
         msg << "was not able to send message peano4::grid::GridTraversalEventPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridTraversalEventPacked " 
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
             "peano4::grid::GridTraversalEventPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridTraversalEventPacked", 
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
   
   
   
   void peano4::grid::GridTraversalEventPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridTraversalEventPacked from node " 
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
        msg << "failed to start to receive peano4::grid::GridTraversalEventPacked from node " 
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
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEventPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridTraversalEventPacked failed: " 
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
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridTraversalEventPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridTraversalEventPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridTraversalEventPacked from node " 
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
   
   
   
   bool peano4::grid::GridTraversalEventPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridTraversalEventPacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



