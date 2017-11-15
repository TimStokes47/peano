#if defined(SharedTBBInvade)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/invasivetbb/Core.h"

#include "tarch/Assertions.h"

#include "tarch/parallel/Node.h"


tarch::logging::Log  tarch::multicore::Core::_log( "tarch::multicore::Core" );


tarch::multicore::Core::Core():
  _invadeRoot(),
  _basicInvasion( nullptr ) {
  logInfo( "Core()", "start cleanup of shared memory region ..." );
  SHMController::cleanup();
  logInfo( "Core()", "cleanup has been successful. Invade " << MinThreads << " threads as default" );
  _basicInvasion = new SHMInvade(MinThreads);
  logInfo( "Core()", "invasion successful" );
}


tarch::multicore::Core::~Core() {
  assertion(_basicInvasion != nullptr);
  delete _basicInvasion;
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static tarch::multicore::Core singleton;
  return singleton;
}


void tarch::multicore::Core::shutDown() {
}


void tarch::multicore::Core::configure( int numberOfThreads ) {
  assertion(_basicInvasion != nullptr);
  assertion(numberOfThreads>=0 || numberOfThreads==UseDefaultNumberOfThreads);
//  assertion2( numberOfThreads <= _invadeRoot.get_max_available_cores(), numberOfThreads, _invadeRoot.get_max_available_cores() );

  if (numberOfThreads < MinThreads ) {
    logWarning( "configure(int)", "requested " << numberOfThreads << " which is fewer than " << MinThreads << " threads. Increase manually to minimum thread count" );
    numberOfThreads = MinThreads;
  }
  if (numberOfThreads > _invadeRoot.get_max_available_cores() ) {
    logWarning( "configure(int)", "requested " << numberOfThreads << " threads on only " << _invadeRoot.get_max_available_cores() << " cores" );
  }

  delete _basicInvasion;
  _basicInvasion = new SHMInvade(numberOfThreads);
}


int tarch::multicore::Core::getNumberOfThreads() const {
  assertion( _invadeRoot.get_num_active_threads() <= _invadeRoot.get_max_available_cores() );
  return _invadeRoot.get_num_active_threads();
}


bool tarch::multicore::Core::isInitialised() const {
  #ifdef Parallel
  logInfo( "isInitialised()", "start to collect information from individual ranks" );

  const int tag = 0;

  if ( tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    int registeredRanks = 1;
    std::clock_t warningTimeStamp = tarch::parallel::Node::getInstance().getDeadlockWarningTimeStamp();
    std::clock_t timeoutTimeStamp = tarch::parallel::Node::getInstance().getDeadlockTimeOutTimeStamp();

    while ( registeredRanks < tarch::parallel::Node::getInstance().getNumberOfNodes() ) {
      if (clock()>warningTimeStamp) {
        tarch::parallel::Node::getInstance().writeTimeOutWarning(
          "tarch::multicore::Core",
          "isInitialised()",
          -1, tag, 1
        );
      }
      if (clock()>timeoutTimeStamp) {
        tarch::parallel::Node::getInstance().writeTimeOutWarning(
          "tarch::multicore::Core",
          "isInitialised()",
          -1, tag, 1
        );
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
  #endif

  return true;
}

#endif
