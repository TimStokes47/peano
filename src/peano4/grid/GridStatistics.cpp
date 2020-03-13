#include "GridStatistics.h"

peano4::grid::GridStatistics::PersistentRecords::PersistentRecords() {
   
}


peano4::grid::GridStatistics::PersistentRecords::PersistentRecords(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells, const int& stationarySweeps, const bool& coarseningHasBeenVetoed):
_numberOfRefinedVertices(numberOfRefinedVertices),
_numberOfUnrefinedVertices(numberOfUnrefinedVertices),
_numberOfErasingVertices(numberOfErasingVertices),
_numberOfRefiningVertices(numberOfRefiningVertices),
_numberOfLocalUnrefinedCells(numberOfLocalUnrefinedCells),
_numberOfRemoteUnrefinedCells(numberOfRemoteUnrefinedCells),
_numberOfLocalRefinedCells(numberOfLocalRefinedCells),
_numberOfRemoteRefinedCells(numberOfRemoteRefinedCells),
_stationarySweeps(stationarySweeps),
_coarseningHasBeenVetoed(coarseningHasBeenVetoed) {
   
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



 int peano4::grid::GridStatistics::PersistentRecords::getStationarySweeps() const  {
   return _stationarySweeps;
}



 void peano4::grid::GridStatistics::PersistentRecords::setStationarySweeps(const int& stationarySweeps)  {
   _stationarySweeps = stationarySweeps;
}



 bool peano4::grid::GridStatistics::PersistentRecords::getCoarseningHasBeenVetoed() const  {
   return _coarseningHasBeenVetoed;
}



 void peano4::grid::GridStatistics::PersistentRecords::setCoarseningHasBeenVetoed(const bool& coarseningHasBeenVetoed)  {
   _coarseningHasBeenVetoed = coarseningHasBeenVetoed;
}


peano4::grid::GridStatistics::GridStatistics() {
   
}


peano4::grid::GridStatistics::GridStatistics(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._numberOfRefinedVertices, persistentRecords._numberOfUnrefinedVertices, persistentRecords._numberOfErasingVertices, persistentRecords._numberOfRefiningVertices, persistentRecords._numberOfLocalUnrefinedCells, persistentRecords._numberOfRemoteUnrefinedCells, persistentRecords._numberOfLocalRefinedCells, persistentRecords._numberOfRemoteRefinedCells, persistentRecords._stationarySweeps, persistentRecords._coarseningHasBeenVetoed) {
   
}


peano4::grid::GridStatistics::GridStatistics(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells, const int& stationarySweeps, const bool& coarseningHasBeenVetoed):
_persistentRecords(numberOfRefinedVertices, numberOfUnrefinedVertices, numberOfErasingVertices, numberOfRefiningVertices, numberOfLocalUnrefinedCells, numberOfRemoteUnrefinedCells, numberOfLocalRefinedCells, numberOfRemoteRefinedCells, stationarySweeps, coarseningHasBeenVetoed) {
   
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



 int peano4::grid::GridStatistics::getStationarySweeps() const  {
   return _persistentRecords._stationarySweeps;
}



 void peano4::grid::GridStatistics::setStationarySweeps(const int& stationarySweeps)  {
   _persistentRecords._stationarySweeps = stationarySweeps;
}



 bool peano4::grid::GridStatistics::getCoarseningHasBeenVetoed() const  {
   return _persistentRecords._coarseningHasBeenVetoed;
}



 void peano4::grid::GridStatistics::setCoarseningHasBeenVetoed(const bool& coarseningHasBeenVetoed)  {
   _persistentRecords._coarseningHasBeenVetoed = coarseningHasBeenVetoed;
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
   out << ",";
   out << "stationarySweeps:" << getStationarySweeps();
   out << ",";
   out << "coarseningHasBeenVetoed:" << getCoarseningHasBeenVetoed();
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
      getNumberOfRemoteRefinedCells(),
      getStationarySweeps(),
      getCoarseningHasBeenVetoed()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridStatistics::_log( "peano4::grid::GridStatistics" );
   
   MPI_Datatype peano4::grid::GridStatistics::Datatype = 0;
   MPI_Datatype peano4::grid::GridStatistics::FullDatatype = 0;
   
   
   void peano4::grid::GridStatistics::initDatatype() {
      {
         GridStatistics dummyGridStatistics[16];
         
         #ifdef MPI2
         const int Attributes = 9;
         #else
         const int Attributes = 9+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            , MPI_BYTE		 //coarseningHasBeenVetoed
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            , 1		 //coarseningHasBeenVetoed
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._coarseningHasBeenVetoed))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._coarseningHasBeenVetoed))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &GridStatistics::Datatype );
         errorCode += MPI_Type_commit( &GridStatistics::Datatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatistics::Datatype);
         errorCode += MPI_Type_commit( &GridStatistics::Datatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      {
         GridStatistics dummyGridStatistics[16];
         
         #ifdef MPI2
         const int Attributes = 10;
         #else
         const int Attributes = 10+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            , MPI_INT		 //stationarySweeps
            , MPI_BYTE		 //coarseningHasBeenVetoed
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            , 1		 //stationarySweeps
            , 1		 //coarseningHasBeenVetoed
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._stationarySweeps))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._stationarySweeps))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._coarseningHasBeenVetoed))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]._persistentRecords._coarseningHasBeenVetoed))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatistics[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &GridStatistics::FullDatatype );
         errorCode += MPI_Type_commit( &GridStatistics::FullDatatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatistics::FullDatatype);
         errorCode += MPI_Type_commit( &GridStatistics::FullDatatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
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
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridStatistics " 
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
         msg << "was not able to send message peano4::grid::GridStatistics "  
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
           msg << "testing for finished send task for peano4::grid::GridStatistics " 
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
             "peano4::grid::GridStatistics", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridStatistics", 
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
   
   
   
   void peano4::grid::GridStatistics::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridStatistics from rank " 
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
        msg << "failed to start to receive peano4::grid::GridStatistics from rank " 
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
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatistics failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridStatistics failed: " 
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
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatistics", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatistics failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridStatistics from rank " 
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
   
   
   
   bool peano4::grid::GridStatistics::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridStatistics::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif


peano4::grid::GridStatisticsPacked::PersistentRecords::PersistentRecords() {
   if ((1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((1 < (8 * sizeof(short int))));
   
}


peano4::grid::GridStatisticsPacked::PersistentRecords::PersistentRecords(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells, const int& stationarySweeps, const bool& coarseningHasBeenVetoed):
_numberOfRefinedVertices(numberOfRefinedVertices),
_numberOfUnrefinedVertices(numberOfUnrefinedVertices),
_numberOfErasingVertices(numberOfErasingVertices),
_numberOfRefiningVertices(numberOfRefiningVertices),
_numberOfLocalUnrefinedCells(numberOfLocalUnrefinedCells),
_numberOfRemoteUnrefinedCells(numberOfRemoteUnrefinedCells),
_numberOfLocalRefinedCells(numberOfLocalRefinedCells),
_numberOfRemoteRefinedCells(numberOfRemoteRefinedCells),
_stationarySweeps(stationarySweeps) {
   setCoarseningHasBeenVetoed(coarseningHasBeenVetoed);
   if ((1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((1 < (8 * sizeof(short int))));
   
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



 int peano4::grid::GridStatisticsPacked::PersistentRecords::getStationarySweeps() const  {
   return _stationarySweeps;
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setStationarySweeps(const int& stationarySweeps)  {
   _stationarySweeps = stationarySweeps;
}



 bool peano4::grid::GridStatisticsPacked::PersistentRecords::getCoarseningHasBeenVetoed() const  {
   short int mask = 1 << (0);
   short int tmp = static_cast<short int>(_packedRecords0 & mask);
   return (tmp != 0);
}



 void peano4::grid::GridStatisticsPacked::PersistentRecords::setCoarseningHasBeenVetoed(const bool& coarseningHasBeenVetoed)  {
   short int mask = 1 << (0);
   _packedRecords0 = static_cast<short int>( coarseningHasBeenVetoed ? (_packedRecords0 | mask) : (_packedRecords0 & ~mask));
}


peano4::grid::GridStatisticsPacked::GridStatisticsPacked() {
   if ((1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((1 < (8 * sizeof(short int))));
   
}


peano4::grid::GridStatisticsPacked::GridStatisticsPacked(const PersistentRecords& persistentRecords):
_persistentRecords(persistentRecords._numberOfRefinedVertices, persistentRecords._numberOfUnrefinedVertices, persistentRecords._numberOfErasingVertices, persistentRecords._numberOfRefiningVertices, persistentRecords._numberOfLocalUnrefinedCells, persistentRecords._numberOfRemoteUnrefinedCells, persistentRecords._numberOfLocalRefinedCells, persistentRecords._numberOfRemoteRefinedCells, persistentRecords._stationarySweeps, persistentRecords.getCoarseningHasBeenVetoed()) {
   if ((1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((1 < (8 * sizeof(short int))));
   
}


peano4::grid::GridStatisticsPacked::GridStatisticsPacked(const int& numberOfRefinedVertices, const int& numberOfUnrefinedVertices, const int& numberOfErasingVertices, const int& numberOfRefiningVertices, const int& numberOfLocalUnrefinedCells, const int& numberOfRemoteUnrefinedCells, const int& numberOfLocalRefinedCells, const int& numberOfRemoteRefinedCells, const int& stationarySweeps, const bool& coarseningHasBeenVetoed):
_persistentRecords(numberOfRefinedVertices, numberOfUnrefinedVertices, numberOfErasingVertices, numberOfRefiningVertices, numberOfLocalUnrefinedCells, numberOfRemoteUnrefinedCells, numberOfLocalRefinedCells, numberOfRemoteRefinedCells, stationarySweeps, coarseningHasBeenVetoed) {
   if ((1 >= (8 * sizeof(short int)))) {
      std::cerr << "Packed-Type in " << __FILE__ << " too small. Either use bigger data type or append " << std::endl << std::endl;
      std::cerr << "  Packed-Type: short int hint-size no-of-bits;  " << std::endl << std::endl;
      std::cerr << "to your data type spec to guide DaStGen how many bits (no-of-bits) a data type has on your machine. DaStGen then can split up the bitfields into several attributes. " << std::endl; 
   }
   assertion((1 < (8 * sizeof(short int))));
   
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



 int peano4::grid::GridStatisticsPacked::getStationarySweeps() const  {
   return _persistentRecords._stationarySweeps;
}



 void peano4::grid::GridStatisticsPacked::setStationarySweeps(const int& stationarySweeps)  {
   _persistentRecords._stationarySweeps = stationarySweeps;
}



 bool peano4::grid::GridStatisticsPacked::getCoarseningHasBeenVetoed() const  {
   short int mask = 1 << (0);
   short int tmp = static_cast<short int>(_persistentRecords._packedRecords0 & mask);
   return (tmp != 0);
}



 void peano4::grid::GridStatisticsPacked::setCoarseningHasBeenVetoed(const bool& coarseningHasBeenVetoed)  {
   short int mask = 1 << (0);
   _persistentRecords._packedRecords0 = static_cast<short int>( coarseningHasBeenVetoed ? (_persistentRecords._packedRecords0 | mask) : (_persistentRecords._packedRecords0 & ~mask));
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
   out << ",";
   out << "stationarySweeps:" << getStationarySweeps();
   out << ",";
   out << "coarseningHasBeenVetoed:" << getCoarseningHasBeenVetoed();
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
      getNumberOfRemoteRefinedCells(),
      getStationarySweeps(),
      getCoarseningHasBeenVetoed()
   );
}

#ifdef Parallel
   tarch::logging::Log peano4::grid::GridStatisticsPacked::_log( "peano4::grid::GridStatisticsPacked" );
   
   MPI_Datatype peano4::grid::GridStatisticsPacked::Datatype = 0;
   MPI_Datatype peano4::grid::GridStatisticsPacked::FullDatatype = 0;
   
   
   void peano4::grid::GridStatisticsPacked::initDatatype() {
      {
         GridStatisticsPacked dummyGridStatisticsPacked[16];
         
         #ifdef MPI2
         const int Attributes = 9;
         #else
         const int Attributes = 9+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            , MPI_SHORT		 //_packedRecords0
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            , 1		 //_packedRecords0
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &GridStatisticsPacked::Datatype );
         errorCode += MPI_Type_commit( &GridStatisticsPacked::Datatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatisticsPacked::Datatype);
         errorCode += MPI_Type_commit( &GridStatisticsPacked::Datatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
      }
      {
         GridStatisticsPacked dummyGridStatisticsPacked[16];
         
         #ifdef MPI2
         const int Attributes = 10;
         #else
         const int Attributes = 10+2;
         #endif
         MPI_Datatype subtypes[Attributes] = {
            #ifndef MPI2
              MPI_LB,
            #endif
              MPI_INT		 //numberOfRefinedVertices
            , MPI_INT		 //numberOfUnrefinedVertices
            , MPI_INT		 //numberOfErasingVertices
            , MPI_INT		 //numberOfRefiningVertices
            , MPI_INT		 //numberOfLocalUnrefinedCells
            , MPI_INT		 //numberOfRemoteUnrefinedCells
            , MPI_INT		 //numberOfLocalRefinedCells
            , MPI_INT		 //numberOfRemoteRefinedCells
            , MPI_INT		 //stationarySweeps
            , MPI_SHORT		 //_packedRecords0
            #ifndef MPI2
            , MPI_UB
            #endif
            
         };
         
         int blocklen[Attributes] = {
            #ifndef MPI2
            1, // lower bound
            #endif
              1		 //numberOfRefinedVertices
            , 1		 //numberOfUnrefinedVertices
            , 1		 //numberOfErasingVertices
            , 1		 //numberOfRefiningVertices
            , 1		 //numberOfLocalUnrefinedCells
            , 1		 //numberOfRemoteUnrefinedCells
            , 1		 //numberOfLocalRefinedCells
            , 1		 //numberOfRemoteRefinedCells
            , 1		 //stationarySweeps
            , 1		 //_packedRecords0
            #ifndef MPI2
            , 1 // upper bound
            #endif
            
         };
         
         MPI_Aint  disp[Attributes];
         int       currentAddress = -1;
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]))), &disp[currentAddress]);
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfUnrefinedVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfErasingVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRefiningVertices))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteUnrefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfLocalRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._numberOfRemoteRefinedCells))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._stationarySweeps))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._stationarySweeps))), 		&disp[currentAddress] );
         #endif
         currentAddress++;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]._persistentRecords._packedRecords0))), 		&disp[currentAddress] );
         #endif
         #ifndef MPI2
         currentAddress++;
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[1]))), &disp[currentAddress]);
         #endif
         for (int i=1; i<Attributes; i++) {
         
            assertion1( disp[i] > disp[i-1], i );
         }
         MPI_Aint base;
         #ifdef MPI2
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]))), &base);
         #else
         MPI_Address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[0]))), &base);
         #endif
         for (int i=0; i<Attributes; i++) {
         
            disp[i] = disp[i] - base;
            
         }
         int errorCode = 0;
         #ifdef MPI2
         MPI_Datatype tmpType; 
         errorCode += MPI_Type_create_struct( Attributes, blocklen, disp, subtypes, &tmpType );
         MPI_Aint typeExtent; 
         MPI_Get_address( const_cast<void*>(static_cast<const void*>(&(dummyGridStatisticsPacked[1]))), &typeExtent);
         typeExtent = MPI_Aint_diff(typeExtent, base);
         errorCode += MPI_Type_create_resized( tmpType, 0, typeExtent, &GridStatisticsPacked::FullDatatype );
         errorCode += MPI_Type_commit( &GridStatisticsPacked::FullDatatype );
         #else
         errorCode += MPI_Type_struct( Attributes, blocklen, disp, subtypes, &GridStatisticsPacked::FullDatatype);
         errorCode += MPI_Type_commit( &GridStatisticsPacked::FullDatatype );
         #endif
         if (errorCode) logError( "initDatatype()", "error committing datatype: " << errorCode );
         
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
      const int result = MPI_Send(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, destination, tag, tarch::mpi::Rank::getInstance().getCommunicator()); 
       if  (result!=MPI_SUCCESS) { 
         std::ostringstream msg; 
         msg << "was not able to send message peano4::grid::GridStatisticsPacked " 
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
         msg << "was not able to send message peano4::grid::GridStatisticsPacked "  
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
           msg << "testing for finished send task for peano4::grid::GridStatisticsPacked " 
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
             "peano4::grid::GridStatisticsPacked", 
             "send(int)", destination,tag,1 
           ); 
           triggeredTimeoutWarning = true; 
         } 
         if ( 
           tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
           (clock()>timeOutShutdown) 
         ) { 
           tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
             "peano4::grid::GridStatisticsPacked", 
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
   
   
   
   void peano4::grid::GridStatisticsPacked::receive(int source, int tag, bool exchangeOnlyAttributesMarkedWithParallelise, ExchangeMode mode) {
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
        msg << "failed to start to receive peano4::grid::GridStatisticsPacked from rank " 
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
        msg << "failed to start to receive peano4::grid::GridStatisticsPacked from rank " 
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
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Test( sendRequestHandle, &flag, source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
        if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatisticsPacked failed: " 
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
        msg << "testing for finished receive task for peano4::grid::GridStatisticsPacked failed: " 
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
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
          triggeredTimeoutWarning = true; 
        } 
        if ( 
          tarch::mpi::Rank::getInstance().isTimeOutDeadlockEnabled() && 
          (clock()>timeOutShutdown) 
        ) { 
          tarch::mpi::Rank::getInstance().triggerDeadlockTimeOut( 
            "peano4::grid::GridStatisticsPacked", 
            "receive(int)", source,tag,1 
          ); 
        } 
        tarch::mpi::Rank::getInstance().receiveDanglingMessages(); 
        result = MPI_Iprobe(source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), &flag, MPI_STATUS_IGNORE ); 
         if (result!=MPI_SUCCESS) { 
          std::ostringstream msg; 
          msg << "testing for finished receive task for peano4::grid::GridStatisticsPacked failed: " 
              << tarch::mpi::MPIReturnValueToString(result); 
          _log.error("receive(int)", msg.str() ); 
        } 
      } 
      result = MPI_Recv(this, 1, exchangeOnlyAttributesMarkedWithParallelise ? Datatype : FullDatatype, source, tag, tarch::mpi::Rank::getInstance().getCommunicator(), source==MPI_ANY_SOURCE ? &status : MPI_STATUS_IGNORE ); 
      if ( result != MPI_SUCCESS ) { 
        std::ostringstream msg; 
        msg << "failed to start to receive peano4::grid::GridStatisticsPacked from rank " 
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
   
   
   
   bool peano4::grid::GridStatisticsPacked::isMessageInQueue(int tag, bool exchangeOnlyAttributesMarkedWithParallelise) {
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
   
   int peano4::grid::GridStatisticsPacked::getSenderRank() const {
      assertion( _senderDestinationRank!=-1 );
      return _senderDestinationRank;
      
   }
#endif



