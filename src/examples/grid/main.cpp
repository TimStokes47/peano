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


tarch::logging::Log _log("examples::grid");


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
	  emptyObserver.beginTraversalOnRank(false);
    #endif
    spacetree.traverse( emptyObserver );
    #if PeanoDebug>0
    emptyObserver.endTraversalOnRank(false);
    #endif

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


void updateDomainDecomposition() {
//  static int phase = 0;
  static int phase = -1;

  if (phase==0) {
    if (
      peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD
      and
      peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
      and
      peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,0)
    ) {
      phase++;
    }
  }
  else if (
    phase==1
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*2
  ) {
    if ( not peano4::parallel::SpacetreeSet::getInstance().split(1,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3,0) ) {
      phase++;
    }
  }
  else if (
    phase==2
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*4
  ) {
    if ( not peano4::parallel::SpacetreeSet::getInstance().split(1,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3/3,0) ) {
      phase++;
    }
  }
  else if (
    phase==3
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*6
  ) {
    if ( not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3/3/3,0) ) {
      phase++;
    }
  }
  else if (
    phase==4
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*8
  ) {
    if ( not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3/3/3/3,0) ) {
      phase++;
    }
  }
  else if (
    phase==5
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*10
  ) {
    if ( not peano4::parallel::SpacetreeSet::getInstance().split(1,10,0) ) {
      phase++;
    }
  }
}


void runParallel() {
  std::bitset<Dimensions> periodicBC = 3;

  peano4::parallel::SpacetreeSet::getInstance().init(
    #if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0},
    #else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0},
    #endif
    periodicBC
  );

  examples::grid::MyObserver emptyObserver;

  for (int i=0; i<100; i++) {
	tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();
    #if PeanoDebug>0
    emptyObserver.beginTraversalOnRank(true);
    #endif
    peano4::parallel::SpacetreeSet::getInstance().traverse( emptyObserver );
    #if PeanoDebug>0
    emptyObserver.endTraversalOnRank(true);
    #endif

    logInfo( "runParallel(...)", "refined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefinedVertices() );
    logInfo( "runParallel(...)", "unrefined vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfUnrefinedVertices() );
    logInfo( "runParallel(...)", "refining vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRefiningVertices() );
    logInfo( "runParallel(...)", "erasing vertices = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfErasingVertices() );
    logInfo( "runParallel(...)", "local unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells());
    logInfo( "runParallel(...)", "local refined cell= " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalRefinedCells() );
    logInfo( "runParallel(...)", "remote unrefined cells = " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteUnrefinedCells() );
    logInfo( "runParallel(...)", "remote refined cells= " << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfRemoteRefinedCells() );

    updateDomainDecomposition();
  }

  peano4::grid::TraversalVTKPlotter plotterObserver( "grid-parallel" );
  peano4::parallel::SpacetreeSet::getInstance().traverse( plotterObserver );
}


/*
  if (tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(14);

  }
  else {
    while (peano4::parallel::Node::getInstance().continueToRun()) {
      assertionEquals( peano4::parallel::Node::getInstance().getCurrentProgramStep(), 14 );

      tarch::logging::CommandLineLogger::getInstance().closeOutputStreamAndReopenNewOne();

      #if PeanoDebug>0
      emptyObserver.beginTraversalOnRank(true);
      #endif
      peano4::parallel::SpacetreeSet::getInstance().traverse(emptyObserver);
      #if PeanoDebug>0
      emptyObserver.endTraversalOnRank(true);
      #endif
    }
  }
}

*/


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
  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( tarch::logging::CommandLineLogger::FilterListEntry(
    "debug", tarch::logging::CommandLineLogger::FilterListEntry::AnyRank, "peano4", false
  ));
  tarch::logging::CommandLineLogger::getInstance().setOutputFile( "trace.txt" );

  tarch::multicore::Core::getInstance().configure(4,2,1);

  runTests();
  #if defined(Parallel) or defined(SharedMemoryParallelisation)
  runParallel();
  #else
  runSerial();
  #endif

  peano4::shutdownSharedMemoryEnvironment();
  peano4::shutdownParallelEnvironment();

  return 0;
}
