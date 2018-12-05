#include "peano4/grid/GridStatistics.h"

peano4::grid::GridStatistics::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridStatistics::PersistentRecords::PersistentRecords(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells):
_numberOfRefinedVertices(numberOfRefinedVertices),
_numberOfUnrefinedVertices(numberOfUnrefinedVertices),
_numberOfErasingVertices(numberOfErasingVertices),
_numberOfRefiningVertices(numberOfRefiningVertices),
_numberOfLocalUnrefinedCells(numberOfLocalUnrefinedCells),
_numberOfRemoteUnrefinedCells(numberOfRemoteUnrefinedCells),
_numberOfLocalRefinedCells(numberOfLocalRefinedCells),
_numberOfRemoteRefinedCells(numberOfRemoteRefinedCells) {
   
}


 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfRefinedVertices() const  {
   return _numberOfRefinedVertices;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfRefinedVertices(const int& numberOfRefinedVertices)  {
   _numberOfRefinedVertices = numberOfRefinedVertices;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfUnrefinedVertices() const  {
   return _numberOfUnrefinedVertices;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfUnrefinedVertices(const int& numberOfUnrefinedVertices)  {
   _numberOfUnrefinedVertices = numberOfUnrefinedVertices;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfErasingVertices() const  {
   return _numberOfErasingVertices;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfErasingVertices(const int& numberOfErasingVertices)  {
   _numberOfErasingVertices = numberOfErasingVertices;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfRefiningVertices() const  {
   return _numberOfRefiningVertices;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfRefiningVertices(const int& numberOfRefiningVertices)  {
   _numberOfRefiningVertices = numberOfRefiningVertices;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfLocalUnrefinedCells() const  {
   return _numberOfLocalUnrefinedCells;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfLocalUnrefinedCells(const int& numberOfLocalUnrefinedCells)  {
   _numberOfLocalUnrefinedCells = numberOfLocalUnrefinedCells;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfRemoteUnrefinedCells() const  {
   return _numberOfRemoteUnrefinedCells;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfRemoteUnrefinedCells(const int& numberOfRemoteUnrefinedCells)  {
   _numberOfRemoteUnrefinedCells = numberOfRemoteUnrefinedCells;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfLocalRefinedCells() const  {
   return _numberOfLocalRefinedCells;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfLocalRefinedCells(const int& numberOfLocalRefinedCells)  {
   _numberOfLocalRefinedCells = numberOfLocalRefinedCells;
}



 int peano4::grid::GridStatistics::PersistentRecords::getNumberOfRemoteRefinedCells() const  {
   return _numberOfRemoteRefinedCells;
}



 void peano4::grid::GridStatistics::PersistentRecords::setNumberOfRemoteRefinedCells(const int& numberOfRemoteRefinedCells)  {
   _numberOfRemoteRefinedCells = numberOfRemoteRefinedCells;
}


peano4::grid::GridStatistics::GridStatistics() {
   
}


peano4::grid::GridStatistics::GridStatistics(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._numberOfRefinedVertices, persistentRecords._numberOfUnrefinedVertices, persistentRecords._numberOfErasingVertices, persistentRecords._numberOfRefiningVertices, persistentRecords._numberOfLocalUnrefinedCells, persistentRecords._numberOfRemoteUnrefinedCells, persistentRecords._numberOfLocalRefinedCells, persistentRecords._numberOfRemoteRefinedCells) {
   
}


peano4::grid::GridStatistics::GridStatistics(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells):
_persistentRecords(numberOfRefinedVertices, numberOfUnrefinedVertices, numberOfErasingVertices, numberOfRefiningVertices, numberOfLocalUnrefinedCells, numberOfRemoteUnrefinedCells, numberOfLocalRefinedCells, numberOfRemoteRefinedCells) {
   
}


peano4::grid::GridStatistics::~GridStatistics() { }


 int peano4::grid::GridStatistics::getNumberOfRefinedVertices() const  {
   return _persistentRecords._numberOfRefinedVertices;
}



 void peano4::grid::GridStatistics::setNumberOfRefinedVertices(const int& numberOfRefinedVertices)  {
   _persistentRecords._numberOfRefinedVertices = numberOfRefinedVertices;
}



 int peano4::grid::GridStatistics::getNumberOfUnrefinedVertices() const  {
   return _persistentRecords._numberOfUnrefinedVertices;
}



 void peano4::grid::GridStatistics::setNumberOfUnrefinedVertices(const int& numberOfUnrefinedVertices)  {
   _persistentRecords._numberOfUnrefinedVertices = numberOfUnrefinedVertices;
}



 int peano4::grid::GridStatistics::getNumberOfErasingVertices() const  {
   return _persistentRecords._numberOfErasingVertices;
}



 void peano4::grid::GridStatistics::setNumberOfErasingVertices(const int& numberOfErasingVertices)  {
   _persistentRecords._numberOfErasingVertices = numberOfErasingVertices;
}



 int peano4::grid::GridStatistics::getNumberOfRefiningVertices() const  {
   return _persistentRecords._numberOfRefiningVertices;
}



 void peano4::grid::GridStatistics::setNumberOfRefiningVertices(const int& numberOfRefiningVertices)  {
   _persistentRecords._numberOfRefiningVertices = numberOfRefiningVertices;
}



 int peano4::grid::GridStatistics::getNumberOfLocalUnrefinedCells() const  {
   return _persistentRecords._numberOfLocalUnrefinedCells;
}



 void peano4::grid::GridStatistics::setNumberOfLocalUnrefinedCells(const int& numberOfLocalUnrefinedCells)  {
   _persistentRecords._numberOfLocalUnrefinedCells = numberOfLocalUnrefinedCells;
}



 int peano4::grid::GridStatistics::getNumberOfRemoteUnrefinedCells() const  {
   return _persistentRecords._numberOfRemoteUnrefinedCells;
}



 void peano4::grid::GridStatistics::setNumberOfRemoteUnrefinedCells(const int& numberOfRemoteUnrefinedCells)  {
   _persistentRecords._numberOfRemoteUnrefinedCells = numberOfRemoteUnrefinedCells;
}



 int peano4::grid::GridStatistics::getNumberOfLocalRefinedCells() const  {
   return _persistentRecords._numberOfLocalRefinedCells;
}



 void peano4::grid::GridStatistics::setNumberOfLocalRefinedCells(const int& numberOfLocalRefinedCells)  {
   _persistentRecords._numberOfLocalRefinedCells = numberOfLocalRefinedCells;
}



 int peano4::grid::GridStatistics::getNumberOfRemoteRefinedCells() const  {
   return _persistentRecords._numberOfRemoteRefinedCells;
}



 void peano4::grid::GridStatistics::setNumberOfRemoteRefinedCells(const int& numberOfRemoteRefinedCells)  {
   _persistentRecords._numberOfRemoteRefinedCells = numberOfRemoteRefinedCells;
}




std::string peano4::grid::GridStatistics::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::GridStatistics::toString (std::ostream& out) const {
   out << "("; 
   out << "numberOfRefinedVertices:" << getNumberOfRefinedVertices();
   out << ",";
   out << "numberOfUnrefinedVertices:" << getNumberOfUnrefinedVertices();
   out << ",";
   out << "numberOfErasingVertices:" << getNumberOfErasingVertices();
   out << ",";
   out << "numberOfRefiningVertices:" << getNumberOfRefiningVertices();
   out << ",";
   out << "numberOfLocalUnrefinedCells:" << getNumberOfLocalUnrefinedCells();
   out << ",";
   out << "numberOfRemoteUnrefinedCells:" << getNumberOfRemoteUnrefinedCells();
   out << ",";
   out << "numberOfLocalRefinedCells:" << getNumberOfLocalRefinedCells();
   out << ",";
   out << "numberOfRemoteRefinedCells:" << getNumberOfRemoteRefinedCells();
   out <<  ")";
}


peano4::grid::GridStatistics::PersistentRecords peano4::grid::GridStatistics::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::GridStatisticsPacked peano4::grid::GridStatistics::convert() const{
   return GridStatisticsPacked(
      getNumberOfRefinedVertices(),
      getNumberOfUnrefinedVertices(),
      getNumberOfErasingVertices(),
      getNumberOfRefiningVertices(),
      getNumberOfLocalUnrefinedCells(),
      getNumberOfRemoteUnrefinedCells(),
      getNumberOfLocalRefinedCells(),
      getNumberOfRemoteRefinedCells()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridStatistics::_log( "peano4::grid::GridStatistics" );
   
   MPI_Datatype peano4::grid::GridStatistics::Datatype = 0;
   MPI_Datatype peano4::grid::GridStatistics::FullDatatype = 0;
   
   
   void peano4::grid::GridStatistics::initDatatype() {
      {
         GridStatistics dummyGridStatistics[2];
         
         #ifdef MPI2
         const int Attributes = 8;
         #else
         const int Attributes = 9;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
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
            assertion4(disp[i]<static_cast<int>(sizeof(GridStatistics)), i, disp[i], Attributes, sizeof(GridStatistics));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[1]))), 		&disp[8] );
         disp[8] -= base;
         disp[8] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridStatistics::Datatype );
         MPI_Type_commit( &GridStatistics::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatistics::Datatype);
         MPI_Type_commit( &GridStatistics::Datatype );
         #endif
         
      }
      {
         GridStatistics dummyGridStatistics[2];
         
         #ifdef MPI2
         const int Attributes = 8;
         #else
         const int Attributes = 9;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
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
            assertion4(disp[i]<static_cast<int>(sizeof(GridStatistics)), i, disp[i], Attributes, sizeof(GridStatistics));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[1]))), 		&disp[8] );
         disp[8] -= base;
         disp[8] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridStatistics::FullDatatype );
         MPI_Type_commit( &GridStatistics::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatistics::FullDatatype);
         MPI_Type_commit( &GridStatistics::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::grid::GridStatistics::shutdownDatatype() {
      MPI_Type_free( &GridStatistics::Datatype );
      MPI_Type_free( &GridStatistics::FullDatatype );
      
   }
   
   void peano4::grid::GridStatistics::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridStatistics " 
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
         msg << "was not able to send message peano4::grid::GridStatistics "  
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
           msg << "testing for finished send task for peano4::grid::GridStatistics " 
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
             "peano4::grid::GridStatistics", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridStatistics", 
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
   
   
   
   void peano4::grid::GridStatistics::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridStatistics from node " 
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
        msg << "failed to start to receive peano4::grid::GridStatistics from node " 
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
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatistics failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridStatistics failed: " 
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
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatistics failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridStatistics from node " 
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
   
   
   
   bool peano4::grid::GridStatistics::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridStatistics::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::grid::GridStatisticsPacked::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridStatisticsPacked::PersistentRecords::PersistentRecords(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells):
_numberOfRefinedVertices(numberOfRefinedVertices),
_numberOfUnrefinedVertices(numberOfUnrefinedVertices),
_numberOfErasingVertices(numberOfErasingVertices),
_numberOfRefiningVertices(numberOfRefiningVertices),
_numberOfLocalUnrefinedCells(numberOfLocalUnrefinedCells),
_numberOfRemoteUnrefinedCells(numberOfRemoteUnrefinedCells),
_numberOfLocalRefinedCells(numberOfLocalRefinedCells),
_numberOfRemoteRefinedCells(numberOfRemoteRefinedCells) {
   
}


 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfRefinedVertices() const  {
   return _numberOfRefinedVertices;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfRefinedVertices(const int& numberOfRefinedVertices)  {
   _numberOfRefinedVertices = numberOfRefinedVertices;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfUnrefinedVertices() const  {
   return _numberOfUnrefinedVertices;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfUnrefinedVertices(const int& numberOfUnrefinedVertices)  {
   _numberOfUnrefinedVertices = numberOfUnrefinedVertices;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfErasingVertices() const  {
   return _numberOfErasingVertices;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfErasingVertices(const int& numberOfErasingVertices)  {
   _numberOfErasingVertices = numberOfErasingVertices;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfRefiningVertices() const  {
   return _numberOfRefiningVertices;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfRefiningVertices(const int& numberOfRefiningVertices)  {
   _numberOfRefiningVertices = numberOfRefiningVertices;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfLocalUnrefinedCells() const  {
   return _numberOfLocalUnrefinedCells;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfLocalUnrefinedCells(const int& numberOfLocalUnrefinedCells)  {
   _numberOfLocalUnrefinedCells = numberOfLocalUnrefinedCells;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfRemoteUnrefinedCells() const  {
   return _numberOfRemoteUnrefinedCells;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfRemoteUnrefinedCells(const int& numberOfRemoteUnrefinedCells)  {
   _numberOfRemoteUnrefinedCells = numberOfRemoteUnrefinedCells;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfLocalRefinedCells() const  {
   return _numberOfLocalRefinedCells;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfLocalRefinedCells(const int& numberOfLocalRefinedCells)  {
   _numberOfLocalRefinedCells = numberOfLocalRefinedCells;
}



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getNumberOfRemoteRefinedCells() const  {
   return _numberOfRemoteRefinedCells;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setNumberOfRemoteRefinedCells(const int& numberOfRemoteRefinedCells)  {
   _numberOfRemoteRefinedCells = numberOfRemoteRefinedCells;
}


peano4::grid::GridStatisticsPacked::GridStatisticsPacked() {
   
}


peano4::grid::GridStatisticsPacked::GridStatisticsPacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._numberOfRefinedVertices, persistentRecords._numberOfUnrefinedVertices, persistentRecords._numberOfErasingVertices, persistentRecords._numberOfRefiningVertices, persistentRecords._numberOfLocalUnrefinedCells, persistentRecords._numberOfRemoteUnrefinedCells, persistentRecords._numberOfLocalRefinedCells, persistentRecords._numberOfRemoteRefinedCells) {
   
}


peano4::grid::GridStatisticsPacked::GridStatisticsPacked(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells):
_persistentRecords(numberOfRefinedVertices, numberOfUnrefinedVertices, numberOfErasingVertices, numberOfRefiningVertices, numberOfLocalUnrefinedCells, numberOfRemoteUnrefinedCells, numberOfLocalRefinedCells, numberOfRemoteRefinedCells) {
   
}


peano4::grid::GridStatisticsPacked::~GridStatisticsPacked() { }


 int peano4::grid::GridStatisticsPacked::getNumberOfRefinedVertices() const  {
   return _persistentRecords._numberOfRefinedVertices;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfRefinedVertices(const int& numberOfRefinedVertices)  {
   _persistentRecords._numberOfRefinedVertices = numberOfRefinedVertices;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfUnrefinedVertices() const  {
   return _persistentRecords._numberOfUnrefinedVertices;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfUnrefinedVertices(const int& numberOfUnrefinedVertices)  {
   _persistentRecords._numberOfUnrefinedVertices = numberOfUnrefinedVertices;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfErasingVertices() const  {
   return _persistentRecords._numberOfErasingVertices;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfErasingVertices(const int& numberOfErasingVertices)  {
   _persistentRecords._numberOfErasingVertices = numberOfErasingVertices;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfRefiningVertices() const  {
   return _persistentRecords._numberOfRefiningVertices;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfRefiningVertices(const int& numberOfRefiningVertices)  {
   _persistentRecords._numberOfRefiningVertices = numberOfRefiningVertices;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfLocalUnrefinedCells() const  {
   return _persistentRecords._numberOfLocalUnrefinedCells;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfLocalUnrefinedCells(const int& numberOfLocalUnrefinedCells)  {
   _persistentRecords._numberOfLocalUnrefinedCells = numberOfLocalUnrefinedCells;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfRemoteUnrefinedCells() const  {
   return _persistentRecords._numberOfRemoteUnrefinedCells;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfRemoteUnrefinedCells(const int& numberOfRemoteUnrefinedCells)  {
   _persistentRecords._numberOfRemoteUnrefinedCells = numberOfRemoteUnrefinedCells;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfLocalRefinedCells() const  {
   return _persistentRecords._numberOfLocalRefinedCells;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfLocalRefinedCells(const int& numberOfLocalRefinedCells)  {
   _persistentRecords._numberOfLocalRefinedCells = numberOfLocalRefinedCells;
}



 int peano4::grid::GridStatisticsPacked::getNumberOfRemoteRefinedCells() const  {
   return _persistentRecords._numberOfRemoteRefinedCells;
}



 void peano4::grid::GridStatisticsPacked::setNumberOfRemoteRefinedCells(const int& numberOfRemoteRefinedCells)  {
   _persistentRecords._numberOfRemoteRefinedCells = numberOfRemoteRefinedCells;
}




std::string peano4::grid::GridStatisticsPacked::toString() const {
   std::ostringstream stringstr;
   toString(stringstr);
   return stringstr.str();
}

void peano4::grid::GridStatisticsPacked::toString (std::ostream& out) const {
   out << "("; 
   out << "numberOfRefinedVertices:" << getNumberOfRefinedVertices();
   out << ",";
   out << "numberOfUnrefinedVertices:" << getNumberOfUnrefinedVertices();
   out << ",";
   out << "numberOfErasingVertices:" << getNumberOfErasingVertices();
   out << ",";
   out << "numberOfRefiningVertices:" << getNumberOfRefiningVertices();
   out << ",";
   out << "numberOfLocalUnrefinedCells:" << getNumberOfLocalUnrefinedCells();
   out << ",";
   out << "numberOfRemoteUnrefinedCells:" << getNumberOfRemoteUnrefinedCells();
   out << ",";
   out << "numberOfLocalRefinedCells:" << getNumberOfLocalRefinedCells();
   out << ",";
   out << "numberOfRemoteRefinedCells:" << getNumberOfRemoteRefinedCells();
   out <<  ")";
}


peano4::grid::GridStatisticsPacked::PersistentRecords peano4::grid::GridStatisticsPacked::getPersistentRecords() const {
   return _persistentRecords;
}

peano4::grid::GridStatistics peano4::grid::GridStatisticsPacked::convert() const{
   return GridStatistics(
      getNumberOfRefinedVertices(),
      getNumberOfUnrefinedVertices(),
      getNumberOfErasingVertices(),
      getNumberOfRefiningVertices(),
      getNumberOfLocalUnrefinedCells(),
      getNumberOfRemoteUnrefinedCells(),
      getNumberOfLocalRefinedCells(),
      getNumberOfRemoteRefinedCells()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridStatisticsPacked::_log( "peano4::grid::GridStatisticsPacked" );
   
   MPI_Datatype peano4::grid::GridStatisticsPacked::Datatype = 0;
   MPI_Datatype peano4::grid::GridStatisticsPacked::FullDatatype = 0;
   
   
   void peano4::grid::GridStatisticsPacked::initDatatype() {
      {
         GridStatisticsPacked dummyGridStatisticsPacked[2];
         
         #ifdef MPI2
         const int Attributes = 8;
         #else
         const int Attributes = 9;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
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
            assertion4(disp[i]<static_cast<int>(sizeof(GridStatisticsPacked)), i, disp[i], Attributes, sizeof(GridStatisticsPacked));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[1]))), 		&disp[8] );
         disp[8] -= base;
         disp[8] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridStatisticsPacked::Datatype );
         MPI_Type_commit( &GridStatisticsPacked::Datatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatisticsPacked::Datatype);
         MPI_Type_commit( &GridStatisticsPacked::Datatype );
         #endif
         
      }
      {
         GridStatisticsPacked dummyGridStatisticsPacked[2];
         
         #ifdef MPI2
         const int Attributes = 8;
         #else
         const int Attributes = 9;
         #endif
         MPI_Datatype subtypes[Attributes] = {
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            #ifndef MPI2
            , 1
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         MPI_Aint  base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked))), &base);
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[0] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[1] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[2] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[3] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[4] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[5] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[6] );
         #endif
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[7] );
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
            assertion4(disp[i]<static_cast<int>(sizeof(GridStatisticsPacked)), i, disp[i], Attributes, sizeof(GridStatisticsPacked));
         }
         #ifndef MPI2
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[1]))), 		&disp[8] );
         disp[8] -= base;
         disp[8] += disp[0];
         #endif
         #ifdef MPI2
         MPI_Datatype tmpType; 
         MPI_Aint lowerBound, typeExtent; 
         MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Type_get_extent( tmpType, &lowerBound, &typeExtent );
         MPI_Type_create_resized( tmpType, lowerBound, typeExtent, &GridStatisticsPacked::FullDatatype );
         MPI_Type_commit( &GridStatisticsPacked::FullDatatype );
         #else
         MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatisticsPacked::FullDatatype);
         MPI_Type_commit( &GridStatisticsPacked::FullDatatype );
         #endif
         
      }
      
   }
   
   
   void peano4::grid::GridStatisticsPacked::shutdownDatatype() {
      MPI_Type_free( &GridStatisticsPacked::Datatype );
      MPI_Type_free( &GridStatisticsPacked::FullDatatype );
      
   }
   
   void peano4::grid::GridStatisticsPacked::send(int destination, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
      // ============================= 
// start injected snippet/aspect 
// ============================= 
switch (mode) { 
  case ExchangeMode::Blocking: 
    {
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::parallel::Node::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridStatisticsPacked " 
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
         msg << "was not able to send message peano4::grid::GridStatisticsPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridStatisticsPacked " 
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
             "peano4::grid::GridStatisticsPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridStatisticsPacked", 
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
   
   
   
   void peano4::grid::GridStatisticsPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridStatisticsPacked from node " 
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
        msg << "failed to start to receive peano4::grid::GridStatisticsPacked from node " 
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
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatisticsPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridStatisticsPacked failed: " 
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
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::parallel::Node::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::parallel::Node::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::parallel::Node::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::parallel::Node::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatisticsPacked failed: " 
              << tarch::parallel::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::parallel::Node::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridStatisticsPacked from node " 
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
   
   
   
   bool peano4::grid::GridStatisticsPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridStatisticsPacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



