#include "MyObserver.h"

#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/multicore/Core.h"
#include "tarch/mpi/Rank.h"

#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/parallel/SpacetreeSet.h"
#include "peano4/parallel/Node.h"


tarch::logging::Log _log("grid");




void runTests() {
  #if PeanoDebug>=1
  tarch::tests::TestCaseRegistry::getInstance().getTestCaseCollection().run();
  int unitTestsErrors = tarch::tests::TestCaseRegistry::getInstance()
                       .getTestCaseCollection()
                       .getNumberOfErrors();

  if (unitTestsErrors != 0) {
    logError("main()", "unit tests failed. Quit.");
    exit(-2);
  }
  #endif
}


void runSerial() {
  examples::grid::MyObserver emptyObserver;
  peano4::grid::Spacetree         spacetree(
#if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0}
#else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0}
#endif
  );

  for (int i=0; i<30; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();

    #if PeanoDebug>0
	emptyObserver.startNewSnapshot(false);
    #endif
    spacetree.traverse( emptyObserver );

    logInfo( "runSerial(...)", "refined vertices = " << spacetree.getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "runSerial(...)", "unrefined vertices = " << spacetree.getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "runSerial(...)", "refining vertices = " << spacetree.getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "runSerial(...)", "erasing vertices = " << spacetree.getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "runSerial(...)", "local unrefined cells = " << spacetree.getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "runSerial(...)", "local refined cell = " << spacetree.getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "runSerial(...)", "remote unrefined cells = " << spacetree.getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "runSerial(...)", "remote refined cells = " << spacetree.getGridStatistics().getNumberOfRemoteRefinedCells() );
  }


  peano4::grid::TraversalVTKPlotter plotterObserver( "grid-serial" );
  spacetree.traverse( plotterObserver );
}


void runMultithreaded() {
  peano4::parallel::SpacetreeSet::getInstance().init(
#if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0}
#else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0}
#endif
  );

  examples::grid::MyObserver emptyObserver;

  for (int i=0; i<3; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

    logInfo( "runMultithreaded(...)", "refined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "runMultithreaded(...)", "unrefined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "runMultithreaded(...)", "refining vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "runMultithreaded(...)", "erasing vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "runMultithreaded(...)", "local unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "runMultithreaded(...)", "local refined cell= " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "runMultithreaded(...)", "remote unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "runMultithreaded(...)", "remote refined cells= " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteRefinedCells() );
  }

  peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0);
  peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0);

  #if PeanoDebug>0
  emptyObserver.startNewSnapshot(true);
  #endif
  peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(1,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2,0)
  ) {
    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while ( not peano4::parallel::SpacetreeSet::getInstance().move(2, 0) ) {
    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
  ) {
    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
  ) {
    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(1,10,0)
  ) {
    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  for (int i=0; i<30; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();

    #if PeanoDebug>0
    emptyObserver.startNewSnapshot(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

    logInfo( "runMultithreaded(...)", "refined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "runMultithreaded(...)", "unrefined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "runMultithreaded(...)", "refining vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "runMultithreaded(...)", "erasing vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "runMultithreaded(...)", "local unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "runMultithreaded(...)", "local refined cell = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "runMultithreaded(...)", "remote unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "runMultithreaded(...)", "remote refined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteRefinedCells() );
  }


  peano4::grid::TraversalVTKPlotter plotterObserver( "grid-multithreaded" );
  peano4::parallel::SpacetreeSet::getInstance().traverse( plotterObserver );
}


void runParallel() {
  peano4::parallel::SpacetreeSet::getInstance().init(
	  #if Dimensions==2
	    {0.0, 0.0},
	    {1.0, 1.0}
	  #else
	    {0.0, 0.0, 0.0},
	    {1.0, 1.0, 1.0}
    #endif
  );

  examples::grid::MyObserver emptyObserver;

/*
  #if PeanoDebug>0
  emptyObserver.startNewSnapshot(false);
  #endif
  peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

  #if PeanoDebug>0
  emptyObserver.startNewSnapshot(false);
  #endif
  peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

  peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,1);
*/
  /*
  peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,std::min(2,maxRanks));

  peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(1,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2,0)
  ) {
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while ( not peano4::parallel::SpacetreeSet::getInstance().move(2, 0) ) {
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
  ) {
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
  ) {
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }

  while (
    not peano4::parallel::SpacetreeSet::getInstance().split(1,10,0)
  ) {
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
  }
*/


  if (tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(14);

    for (int i=0; i<30; i++) {
      tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();

      if (i==2) {
        peano4::parallel::SpacetreeSet::getInstance().split(
          0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2,
		  std::min(1,tarch::mpi::Rank::getInstance().getNumberOfRanks()-1)
		);
      }

      #if PeanoDebug>0
      emptyObserver.startNewSnapshot(true);
      #endif
      peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );

      logInfo( "runParallel(...)", "refined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefinedVertices() );
      logInfo( "runParallel(...)", "unrefined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfUnrefinedVertices() );
      logInfo( "runParallel(...)", "refining vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefiningVertices() );
      logInfo( "runParallel(...)", "erasing vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfErasingVertices() );
      logInfo( "runParallel(...)", "local unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells());
      logInfo( "runParallel(...)", "local refined cell = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalRefinedCells() );
      logInfo( "runParallel(...)", "remote unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteUnrefinedCells() );
      logInfo( "runParallel(...)", "remote refined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteRefinedCells() );
    }

    peano4::parallel::Node::getInstance().shutdown();
  }
  else {
	while (peano4::parallel::Node::getInstance().continueToRun()) {
      assertionEquals( peano4::parallel::Node::getInstance().getCurrentProgramStep(), 14 );

      tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
      peano4::parallel::SpacetreeSet::getInstance().traverse(emptyObserver);
	}
  }
}


int main(int argc, char** argv) {
  const int ExitCodeSuccess         = 0;
  const int ExitCodeUnitTestsFailed = 1;

  peano4::initParallelEnvironment(&argc,&argv);
  peano4::initSharedMemoryEnvironment();
  peano4::fillLookupTables();

  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( tarch::logging::CommandLineLogger::FilterListEntry(
    "debug", tarch::logging::CommandLineLogger::FilterListEntry::AnyRank, "tarch::multicore", true
  ));
  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( tarch::logging::CommandLineLogger::FilterListEntry(
    "info", tarch::logging::CommandLineLogger::FilterListEntry::AnyRank, "peano4::grid::EmptyTraversalObserver", true
  ));
  // @todo Wieder raus
  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( tarch::logging::CommandLineLogger::FilterListEntry(
    "debug", tarch::logging::CommandLineLogger::FilterListEntry::AnyRank, "peano4", false
  ));
  tarch::logging::CommandLineLogger::getInstance().setOutputFile( "trace.txt" );

  tarch::multicore::Core::getInstance().configure(4,2,1);

  runTests();
  #if defined(Parallel)
  runParallel();
  #elif defined(SharedMemoryParallelisation)
  runMultithreaded();
  #else
  runSerial();
  #endif

  peano4::shutdownSharedMemoryEnvironment();
  peano4::shutdownParallelEnvironment();

  return 0;
}
