#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/multicore/Core.h"


#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/grid/TraversalVTKPlotter.h"
#include "peano4/grid/EmptyTraversalObserver.h"


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
    0,
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

  // parallel version
  peano4::parallel::SpacetreeSet spacetreeSet;
  spacetreeSet.addSpacetree( spacetree );
//  peano4::grid::TraversalVTKPlotter observer( "grid" );
  peano4::grid::EmptyTraversalObserver observer;
  for (int i=0; i<4; i++) {
    spacetreeSet.traverse( observer );

    std::cout << "refined vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefinedVertices() << std::endl;
    std::cout << "unrefined vertices = " << spacetreeSet.getGridStatistics().getNumberOfUnrefinedVertices() << std::endl;
    std::cout << "refining vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefiningVertices() << std::endl;
    std::cout << "erasing vertices = " << spacetreeSet.getGridStatistics().getNumberOfErasingVertices() << std::endl;
  }

  std::cout << "== start split ==" << std::endl;

  spacetreeSet.addSpacetree( spacetree.split(spacetreeSet.getGridStatistics().getNumberOfRefinedVertices()/2) );
  for (int i=0; i<4; i++) {
    spacetreeSet.traverse( observer );

    std::cout << "refined vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefinedVertices() << std::endl;
    std::cout << "unrefined vertices = " << spacetreeSet.getGridStatistics().getNumberOfUnrefinedVertices() << std::endl;
    std::cout << "refining vertices = " << spacetreeSet.getGridStatistics().getNumberOfRefiningVertices() << std::endl;
    std::cout << "erasing vertices = " << spacetreeSet.getGridStatistics().getNumberOfErasingVertices() << std::endl;
  }

  return 0;
}
