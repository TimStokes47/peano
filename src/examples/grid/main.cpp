#include "../../examples/grid/MyObserver.h"

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

  applications4::grid::MyObserver emptyObserver;

  if (tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(14);

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






    /*

      @todo Ich weiss nicht, ob man hier die Differenzen braucht!


             int count = 2;
             int array_of_blocklengths[] = { 1, 1 };
             MPI_Aint array_of_displacements[] = { offsetof( foo, value ),
                                                   offsetof( foo, rank ) };
             MPI_Datatype array_of_types[] = { MPI_FLOAT, MPI_CHAR };
             MPI_Datatype tmp_type, my_mpi_type;
             MPI_Aint lb, extent;

             MPI_Type_create_struct( count, array_of_blocklengths, array_of_displacements,
                                     array_of_types, &tmp_type );
             MPI_Type_get_extent( tmp_type, &lb, &extent );
             MPI_Type_create_resized( tmp_type, lb, extent, &my_mpi_type );
             MPI_Type_commit( &my_mpi_type );
    */



  }
  else {
	while (peano4::parallel::Node::getInstance().continueToRun()) {
      assertionEquals( peano4::parallel::Node::getInstance().getCurrentProgramStep(), 14 );
      spacetreeSet.traverse(emptyObserver);
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
  tarch::logging::CommandLineLogger::getInstance().setOutputFile( "trace.txt" );

  tarch::multicore::Core::getInstance().configure(4,2,1);

  runTests();
  #if Parallel
  runParallel();
  #else
  runSerial();
  runMultithreaded();
  #endif

  peano4::shutdownSharedMemoryEnvironment();
  peano4::shutdownParallelEnvironment();

  return 0;
}
