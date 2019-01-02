#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/multicore/Core.h"


#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"


#include "MyObserver.h"


#include "peano4/parallel/SpacetreeSet.h"


tarch::logging::Log _log("grid");


int main(int argc, char** argv) {
  const int ExitCodeSuccess         = 0;
  const int ExitCodeUnitTestsFailed = 1;

  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( tarch::logging::CommandLineLogger::FilterListEntry(
    "debug", tarch::logging::CommandLineLogger::FilterListEntry::AnyRank, "tarch::multicore", true
  ));

  #if PeanoDebug>=1
  tarch::tests::TestCaseRegistry::getInstance().getTestCaseCollection().run();
  int unitTestsErrors = tarch::tests::TestCaseRegistry::getInstance()
                         .getTestCaseCollection()
                         .getNumberOfErrors();

  if (unitTestsErrors != 0) {
    logError("main()", "unit tests failed. Quit.");
    return -2;
  }
  #endif


  peano4::fillLookupTables();

  tarch::multicore::Core::getInstance().configure(4,2,1);

  peano4::grid::Spacetree spacetree(
    {0.0, 0.0},
    {1.0, 1.0}
  );

  // serial version
/*
  peano4::grid::TraversalVTKPlotter observer( "grid" );
  for (int i=0; i<10; i++) {
    spacetree.traverse( observer );
  }
*/

  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( tarch::logging::CommandLineLogger::FilterListEntry(
    "info", tarch::logging::CommandLineLogger::FilterListEntry::AnyRank, "peano4::grid::EmptyTraversalObserver", true
  ));


  tarch::logging::CommandLineLogger::getInstance().setOutputFile( "trace.txt" );


  // parallel version
  peano4::parallel::SpacetreeSet spacetreeSet;
  spacetreeSet.addSpacetree( std::move(spacetree) );

  applications4::grid::MyObserver emptyObserver;

  for (int i=0; i<2; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
	emptyObserver.startNewSnapshot();

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

  spacetreeSet.split(0,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3);
  spacetreeSet.split(0,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3);

  spacetreeSet.traverse( emptyObserver );

  spacetreeSet.split(1,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2);
  spacetreeSet.split(2,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3);
  spacetreeSet.split(2,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3);

  spacetreeSet.traverse( emptyObserver );
  spacetreeSet.traverse( emptyObserver );

  spacetreeSet.split(1,1);

  for (int i=0; i<10; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
	emptyObserver.startNewSnapshot();

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


//  std::cout << "== Split again ==" << std::endl;
/*
  spacetreeSet.split(1,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2);
  spacetreeSet.split(2,spacetreeSet.getGridStatistics().getNumberOfLocalUnrefinedCells()/3/2);
*/


  for (int i=0; i<10; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
	emptyObserver.startNewSnapshot();

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

  peano4::grid::TraversalVTKPlotter plotterObserver( "grid" );
  plotterObserver.startNewSnapshot();
  spacetreeSet.traverse( plotterObserver );

  return 0;}
