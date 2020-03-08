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


tarch::logging::Log _log("examples::unittests");


std::bitset<Dimensions> periodicBC = 3;


void runTests() {
  tarch::tests::TestCaseRegistry::getInstance().getTestCaseCollection().run();
  int unitTestsErrors = tarch::tests::TestCaseRegistry::getInstance()
                       .getTestCaseCollection()
                       .getNumberOfErrors();

  if (unitTestsErrors != 0) {
    logError("main()", "unit tests failed. Quit.");
    exit(-2);
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
