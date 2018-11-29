#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"


#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"


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

  peano4::grid::Spacetree spacetree = peano4::grid::Spacetree::createTrivialTree(
    {0.0, 0.0},
    {1.0, 1.0}
  );

  for (int i=0; i<3; i++) {
    spacetree.traverse();
  }

  return 0;
}
