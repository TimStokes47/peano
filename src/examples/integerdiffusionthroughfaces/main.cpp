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


void runExample() {
  peano4::parallel::SpacetreeSet spacetreeSet(
#if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0}
#else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0}
#endif
  );

  examples::integerdiffusionthroughfaces::MyObserver emptyObserver;

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
  logInfo( "main(...)", "quit successfully" );
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
  runExample();

  peano4::shutdownSharedMemoryEnvironment();
  peano4::shutdownParallelEnvironment();

  return 0;
}
