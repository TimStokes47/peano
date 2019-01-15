#include "peano.h"

#include "peano4/utils/Loop.h"

#include "tarch/multicore/MulticoreDefinitions.h"


#ifdef SharedMemoryParallelisation
#include "tarch/multicore/Core.h"
#endif


#include "tarch/mpi/Rank.h"


void peano4::fillLookupTables() {
/*
  Had in in Globals.cpp

  setupLookupTableForTwoPowI();
  setupLookupTableForThreePowI();
  setupLookupTableForFourPowI();
  setupLookupTableForDPowI();
*/

  peano4::utils::setupLookupTableForDLinearised();
  peano4::utils::setupLookupTableForDDelinearised();
}


int peano4::initParallelEnvironment(int* argc, char*** argv) {
  #ifdef Parallel
  if ( tarch::mpi::Rank::getInstance().init(argc,argv) ) {
    return 0;
  }
  else {
    return -2;
  }
  #else
  return 0;
  #endif
}


void peano4::shutdownParallelEnvironment() {
  tarch::mpi::Rank::getInstance().shutdown();
}


int peano4::initSharedMemoryEnvironment() {
  #ifdef SharedMemoryParallelisation
  if ( tarch::multicore::Core::getInstance().isInitialised() ) {
    return 0;
  }
  else {
    return -3;
  }
  #else
  return 0;
  #endif
}


void peano4::shutdownSharedMemoryEnvironment() {
  #ifdef SharedMemoryParallelisation
  tarch::multicore::Core::getInstance().shutDown();
  #endif
}

