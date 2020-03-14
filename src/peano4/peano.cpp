#include "peano.h"

#include "peano4/utils/Loop.h"
#include "peano4/parallel/Node.h"

#include "tarch/multicore/multicore.h"
#include "tarch/multicore/Core.h"
#include "tarch/mpi/Rank.h"
#include "tarch/tarch.h"


void peano4::writeCopyrightMessage() {
  static bool wrote = false;
  if (
    not wrote and tarch::mpi::Rank::getInstance().getRank() == tarch::mpi::Rank::getGlobalMasterRank() ) {
    std::cout << "Peano 4 (C) www.peano-framework.org " << std::endl
		      << "Written by Tobias Weinzierl et al" << std::endl;
    std::string buildInfo = "build: ";

    buildInfo += std::to_string(Dimensions);
    buildInfo += "d";

    #ifdef Parallel
    buildInfo += ", with mpi";
    #else
    buildInfo += ", no mpi";
    #endif

    #if defined(SharedOMP)
    buildInfo += ", omp";
    #elif defined(SharedTBB)
    buildInfo += ", tbb";
    #elif defined(SharedCPP)
    buildInfo += ", C++ threading";
    #else
    buildInfo += ", no threading";
    #endif

    #if PeanoDebug>0
    buildInfo += ", debug level=" + std::to_string(PeanoDebug);
    #endif

    std::cout << buildInfo << std::endl;
  }
  wrote = true;
}


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
  int result = 0;
  #ifdef Parallel
  if ( tarch::mpi::Rank::getInstance().init(argc,argv) ) {
	peano4::parallel::Node::initMPIDatatypes();

	// @todo Dummy values
	clock_t timeout = 60;
	tarch::mpi::Rank::getInstance().setTimeOutWarning(timeout/4);
	tarch::mpi::Rank::getInstance().setDeadlockTimeOut(timeout);
  }
  else {
    result = -2;
  }
  #endif

  writeCopyrightMessage();

  return result;
}


void peano4::shutdownParallelEnvironment() {
  peano4::parallel::Node::getInstance().shutdown();
// @todo fuer globale Semaphoren. Has to come after shutdown the shutdown tells the other continues to stop.
  tarch::mpi::Rank::getInstance().barrier();

  peano4::parallel::Node::shutdownMPIDatatypes();
  tarch::mpi::Rank::getInstance().shutdown();
}


int peano4::initSharedMemoryEnvironment() {
  int result = 0;
  #ifdef SharedMemoryParallelisation
  if ( tarch::multicore::Core::getInstance().isInitialised() ) {
  }
  else {
    result = -3;
  }
  #endif
  writeCopyrightMessage();
  return result;
}


void peano4::shutdownSharedMemoryEnvironment() {
  #ifdef SharedMemoryParallelisation
  tarch::multicore::Core::getInstance().shutDown();
  #endif
}

