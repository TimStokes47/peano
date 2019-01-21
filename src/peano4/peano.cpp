#include "peano.h"

#include "peano4/utils/Loop.h"
#include "peano4/parallel/Node.h"

#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/Core.h"
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
	peano4::parallel::Node::initMPIDatatypes();

	// @todo Dummy values
	clock_t timeout = 60;
	tarch::mpi::Rank::getInstance().setTimeOutWarning(timeout/4);
	tarch::mpi::Rank::getInstance().setDeadlockTimeOut(timeout);
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
  peano4::parallel::Node::shutdownMPIDatatypes();
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

