#if defined(SharedTBBInvade)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/invasivetbb/Core.h"


#include "tarch/Assertions.h"

#include "tarch/parallel/Node.h"


tarch::logging::Log  tarch::multicore::Core::_log( "tarch::multicore::Core" );


tarch::multicore::Core::Core():
  _invadeRoot(nullptr),
  _basicInvasion( nullptr ) {

  #ifdef Parallel
  SHMController::cleanup();
  MPI_Barrier( tarch::parallel::Node::getInstance().getCommunicator() );
  #endif

  _invadeRoot = new SHMInvadeRoot();

  #ifdef Parallel
  if ( tarch::parallel::Node::getInstance().isGlobalMaster() ) {
    logInfo( "Core()", _invadeRoot->get_max_available_cores() << " cores available in total" );
  }
  #endif

  _basicInvasion = new SHMInvade(MinThreads-1);
}


tarch::multicore::Core::~Core() {
  assertion(_basicInvasion != nullptr);
  delete _basicInvasion;

  assertion(_invadeRoot != nullptr);
  delete _invadeRoot;
}


tarch::multicore::Core& tarch::multicore::Core::getInstance() {
  static tarch::multicore::Core singleton;
  return singleton;
}


void tarch::multicore::Core::shutDown() {
}


void tarch::multicore::Core::configure( int numberOfThreads, bool enableInvasion ) {
  assertion(_basicInvasion != nullptr);
  assertion(numberOfThreads>=0 || numberOfThreads==UseDefaultNumberOfThreads);
  assertion2( numberOfThreads <= _invadeRoot->get_max_available_cores(), numberOfThreads, _invadeRoot->get_max_available_cores() );

  if (numberOfThreads < MinThreads ) {
    logWarning( "configure(int)", "requested " << numberOfThreads << " which is fewer than " << MinThreads << " threads. Increase manually to minimum thread count" );
    numberOfThreads = MinThreads;
  }
  if (numberOfThreads > _invadeRoot->get_max_available_cores() ) {
    logWarning( "configure(int)", "requested " << numberOfThreads << " threads on only " << _invadeRoot->get_max_available_cores() << " cores" );
  }

  const int oldActiveCores = getNumberOfThreads();

  delete _basicInvasion;
  _basicInvasion = new SHMInvade(numberOfThreads-1);

  logInfo( "configure(int)",
    "rank had " << oldActiveCores << " threads, tried to change to " << numberOfThreads <<
    " threads and got " << getNumberOfThreads() << " (" << _invadeRoot->get_free_cores() << " thread(s) remain available)"
  );

  if (enableInvasion) {
    _invadeRoot->switchOn();
  }
  else {
    _invadeRoot->switchOff();
  }
}


int tarch::multicore::Core::getNumberOfThreads() const {
  assertion( _invadeRoot->get_num_active_threads() <= _invadeRoot->get_max_available_cores() );
  return _invadeRoot->get_num_active_threads();
}


bool tarch::multicore::Core::isInitialised() const {
  #ifdef Parallel
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
  #endif

  return true;
}

#endif
