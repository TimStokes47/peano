#include "MyObserver.h"

#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/logging/ChromeTraceFileLogger.h"
#include "tarch/logging/LogFilter.h"
#include "tarch/multicore/Core.h"
#include "tarch/mpi/Rank.h"

#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/parallel/SpacetreeSet.h"
#include "peano4/parallel/Node.h"


tarch::logging::Log _log("examples::grid");


std::bitset<Dimensions> periodicBC = 3;


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
  static int phase = 0;

  int targetRank = (phase+1) % tarch::mpi::Rank::getInstance().getNumberOfRanks();

  if (phase==0) {
    const int spacetreeOfInterest = 0;

    if ( peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(spacetreeOfInterest) ) {
      if (
        peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD
        and
        peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,targetRank)
        and
        peano4::parallel::SpacetreeSet::getInstance().split(0,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3,targetRank)
      ) {
        phase++;
      }
    }
    else phase++;
  }
  else if (
    phase==1
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*2
  ) {
    const int spacetreeOfInterest = 1;

    if ( peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(spacetreeOfInterest) ) {
      if ( not peano4::parallel::SpacetreeSet::getInstance().split(1,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3,targetRank) ) {
        phase++;
      }
    }
    else phase++;
  }
  else if (
    phase==2
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*4
  ) {
    const int spacetreeOfInterest = periodicBC==0 ? 1 : 0;
    if ( peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(spacetreeOfInterest) ) {
      if ( not peano4::parallel::SpacetreeSet::getInstance().split(
        spacetreeOfInterest,
        peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3/3,targetRank)
      ) {
        phase++;
      }
    }
    else phase++;
  }
  else if (
    phase==3
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*6
  ) {
    const int spacetreeOfInterest = 2;
    if ( peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(spacetreeOfInterest) ) {
      if ( not peano4::parallel::SpacetreeSet::getInstance().split(2,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3/3/3,targetRank) ) {
        phase++;
      }
    }
    else phase++;
  }
  else if (
    phase==4
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*8
  ) {
    const int spacetreeOfInterest = 2;
    if ( peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(spacetreeOfInterest) ) {
      if ( not peano4::parallel::SpacetreeSet::getInstance().split(spacetreeOfInterest,peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()/3/3/3/3/3,targetRank) ) {
        phase++;
      }
    }
    else phase++;
  }
  else if (
    phase==5
    and
    peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getNumberOfLocalUnrefinedCells()>=ThreePowerD*10
  ) {
    const int spacetreeOfInterest = 1;
    if ( peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(spacetreeOfInterest) ) {
      if ( not peano4::parallel::SpacetreeSet::getInstance().split(1,10,targetRank) ) {
        phase++;
      }
    }
    else phase++;
  }
}


void runParallel() {
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

  if (tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(14);
    for (int i=0; i<50; i++) {
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

    logInfo( "runParallel(...)", "plot final grid" );
    peano4::grid::TraversalVTKPlotter plotterObserver( "grid-parallel" );
    peano4::parallel::SpacetreeSet::getInstance().traverse( plotterObserver );
    logInfo( "runParallel(...)", "terminated successfully" );
  }
  else { // not the global master
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


int main(int argc, char** argv) {
  const int ExitCodeSuccess         = 0;
  const int ExitCodeUnitTestsFailed = 1;

  peano4::initParallelEnvironment(&argc,&argv);
  peano4::initSharedMemoryEnvironment();
  peano4::fillLookupTables();

  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug, tarch::logging::LogFilter::FilterListEntry::AnyRank, "peano4", true
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo, tarch::logging::LogFilter::FilterListEntry::AnyRank, "peano4", true
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace, tarch::logging::LogFilter::FilterListEntry::AnyRank, "peano4", true
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug, tarch::logging::LogFilter::FilterListEntry::AnyRank, "tarch", true
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo, tarch::logging::LogFilter::FilterListEntry::AnyRank, "tarch", true
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace, tarch::logging::LogFilter::FilterListEntry::AnyRank, "tarch", true
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
