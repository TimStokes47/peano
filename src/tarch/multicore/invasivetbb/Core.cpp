#if defined(SharedTBBInvade)
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/invasivetbb/Core.h"
#include "tarch/Assertions.h"


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

  MPI_Barrier();
  #endif

  return true;
}

#endif
