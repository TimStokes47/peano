#if defined(SharedTBBInvade)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/invasivetbb/Core.h"

#include "shminvade/SHMController.h"
#include "shminvade/SHMStrategy.h"
#include "shminvade/SHMMultipleRanksPerNodeStrategy.h"

#include "tarch/Assertions.h"

#include "tarch/parallel/Node.h"


tarch::logging::Log  tarch::multicore::Core::_log( "tarch::multicore::Core" );


tarch::multicore::Core::Core():
  _isInitialised(false) {

  #ifdef Parallel
  shminvade::SHMStrategy::getInstance().setStrategy( new shminvade::SHMMultipleRanksPerNodeStrategy() );


  shminvade::SHMStrategy::getInstance().cleanUp();
  MPI_Barrier( tarch::parallel::Node::getInstance().getCommunicator() );
  #endif
}


tarch::multicore::Core::~Core() {
  shminvade::SHMController::getInstance().shutdown();
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static tarch::multicore::Core singleton;
  return singleton;
}


void tarch::multicore::Core::shutDown() {
}


void tarch::multicore::Core::configure( int numberOfThreads, bool enableInvasion ) {
  assertion(numberOfThreads>=0 || numberOfThreads==UseDefaultNumberOfThreads);

  if (numberOfThreads < MinThreads ) {
    logWarning( "configure(int)", "requested " << numberOfThreads << " which is fewer than " << MinThreads << " threads. Increase manually to minimum thread count" );
    numberOfThreads = MinThreads;
  }
  if (numberOfThreads > shminvade::SHMController::getInstance().getMaxAvailableCores() ) {
    logWarning( "configure(int)", "requested " << numberOfThreads << " threads on only " << shminvade::SHMController::getInstance().getMaxAvailableCores() << " cores" );
  }

  // Switch on. Might be temporary
  shminvade::SHMController::getInstance().switchOn();

  const int oldActiveCores = getNumberOfThreads();

  logInfo( "configure(int)",
    "rank had " << oldActiveCores << " threads, tried to change to " << numberOfThreads <<
    " threads and got " << getNumberOfThreads() << " (" << shminvade::SHMController::getInstance().getFreeCores() << " thread(s) remain available)"
  );

  if (enableInvasion) {
    shminvade::SHMController::getInstance().switchOn();
  }
  else {
    shminvade::SHMController::getInstance().switchOff();
  }
}


int tarch::multicore::Core::getNumberOfThreads() const {
  return shminvade::SHMController::getInstance().getBookedCores();
}


bool tarch::multicore::Core::isInitialised() {
  #ifdef Parallel
  if (!_isInitialised) {
    logInfo( "isInitialised()", "start to collect information from individual ranks" );

    const int tag = 0;

    if ( tarch::parallel::Node::getInstance().isGlobalMaster() ) {
      int registeredRanks = 1;

      logInfo( "isInitialised()", "global master got " << getNumberOfThreads() << " thread(s)");

      const int waitForTimeout = 60;
      std::clock_t timeoutTimeStamp = clock() + waitForTimeout * CLOCKS_PER_SEC;

      while ( registeredRanks < tarch::parallel::Node::getInstance().getNumberOfNodes() ) {
        if (clock()>timeoutTimeStamp) {
          logError("isInitialised()", "global master waited for more than " << waitForTimeout <<
          "s on information from other ranks" );
          return false;
        }

        MPI_Status status;
        int threadsBooked = 0.0;
        MPI_Recv(&threadsBooked, 1, MPI_INT, MPI_ANY_SOURCE, tag, tarch::parallel::Node::getInstance().getCommunicator(), &status );
        registeredRanks++;

        logInfo( "isInitialised()", "received notification that rank " << status.MPI_SOURCE << " has successfully booked " << threadsBooked << " thread(s)");
      }
    }
    else {
      int threadsBooked = getNumberOfThreads();
      MPI_Send(&threadsBooked, 1, MPI_INT, tarch::parallel::Node::getGlobalMasterRank(), tag, tarch::parallel::Node::getInstance().getCommunicator() );
    }

    MPI_Barrier( tarch::parallel::Node::getInstance().getCommunicator() );

    _isInitialised = true;
  }
  #endif

  return true;
}

#endif
