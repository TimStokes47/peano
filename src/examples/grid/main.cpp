#include "../../examples/grid/MyObserver.h"
#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/multicore/Core.h"


#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"


#include "peano4/parallel/SpacetreeSet.h"


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
  applications4::grid::MyObserver emptyObserver;
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

    logInfo( "main(...)", "refined vertices = " << spacetree.getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "main(...)", "unrefined vertices = " << spacetree.getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "main(...)", "refining vertices = " << spacetree.getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "main(...)", "erasing vertices = " << spacetree.getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "main(...)", "local unrefined cells = " << spacetree.getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "main(...)", "local refined cell = " << spacetree.getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "main(...)", "remote unrefined cells = " << spacetree.getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "main(...)", "remote refined cells = " << spacetree.getGridStatistics().getNumberOfRemoteRefinedCells() );
  }


  peano4::grid::TraversalVTKPlotter plotterObserver( "grid-serial" );
  spacetree.traverse( plotterObserver );
}


void runMultithreaded() {
  peano4::parallel::SpacetreeSet spacetreeSet(
#if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0}
#else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0}
#endif
  );

  applications4::grid::MyObserver emptyObserver;

  for (int i=0; i<3; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
    spacetreeSet.traverse( emptyObserver );

    logInfo( "main(...)", "refined vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "main(...)", "unrefined vertices = " << spacetreeSet.getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "main(...)", "refining vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "main(...)", "erasing vertices = " << spacetreeSet.getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "main(...)", "local unrefined cells = " << spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "main(...)", "local refined cell= " << spacetreeSet.getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "main(...)", "remote unrefined cells = " << spacetreeSet.getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "main(...)", "remote refined cells= " << spacetreeSet.getGridStatistics().getNumberOfRemoteRefinedCells() );
  }

  spacetreeSet.split(0,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0);
  spacetreeSet.split(0,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0);

  spacetreeSet.traverse( emptyObserver );

  while (
    not spacetreeSet.split(1,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2,0)
  ) {
    spacetreeSet.traverse( emptyObserver );
  }

  while ( not spacetreeSet.move(2, 0) ) {
    spacetreeSet.traverse( emptyObserver );
  }

  while (
    not spacetreeSet.split(2,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
  ) {
    spacetreeSet.traverse( emptyObserver );
  }

  while (
    not spacetreeSet.split(2,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
  ) {
    spacetreeSet.traverse( emptyObserver );
  }

  while (
    not spacetreeSet.split(1,10,0)
  ) {
    spacetreeSet.traverse( emptyObserver );
  }

  for (int i=0; i<30; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();

    spacetreeSet.traverse( emptyObserver );

    logInfo( "main(...)", "refined vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "main(...)", "unrefined vertices = " << spacetreeSet.getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "main(...)", "refining vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "main(...)", "erasing vertices = " << spacetreeSet.getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "main(...)", "local unrefined cells = " << spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "main(...)", "local refined cell = " << spacetreeSet.getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "main(...)", "remote unrefined cells = " << spacetreeSet.getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "main(...)", "remote refined cells = " << spacetreeSet.getGridStatistics().getNumberOfRemoteRefinedCells() );
  }


  peano4::grid::TraversalVTKPlotter plotterObserver( "grid-multithreaded" );
  spacetreeSet.traverse( plotterObserver );
}


void runParallel() {
  peano4::parallel::SpacetreeSet spacetreeSet(
#if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0}
#else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0}
#endif
  );



  peano4::grid::TraversalVTKPlotter plotterObserver( "grid-parallel" );
  spacetreeSet.traverse( plotterObserver );
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
  tarch::logging::CommandLineLogger::getInstance().setOutputFile( "trace.txt" );

  tarch::multicore::Core::getInstance().configure(4,2,1);

  runTests();
  // @todo move down
  //runParallel();
  //runSerial();
  runMultithreaded();

  peano4::shutdownSharedMemoryEnvironment();
  peano4::shutdownParallelEnvironment();

  return 0;
}
