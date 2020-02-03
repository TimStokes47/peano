#include "main.h"
#include "Constants.h"

#include "tarch/logging/Log.h"
#include "tarch/logging/ChromeTraceFileLogger.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/LogFilter.h"

#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "observers/DataRepository.h"
#include "observers/PlotSolution.h"
#include "observers/PlotMaterialParameter.h"
#include "observers/CreateGrid.h"
#include "observers/SetupScenario.h"
#include "observers/ComputeResidualWithGeometricOperators.h"
#include "observers/ComputeGlobalResidualAndError.h"
#include "observers/JacobiUpdate.h"


tarch::logging::Log _log("::");


int main(int argc, char** argv) {
  const int ExitCodeSuccess         = 0;
  const int ExitCodeUnitTestsFailed = 1;

  peano4::initParallelEnvironment(&argc,&argv);
  peano4::initSharedMemoryEnvironment();
  peano4::fillLookupTables();

  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "peano4",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "peano4",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "peano4",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "tarch",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "tarch",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "tarch",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));

  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples::algebraicmg",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples::algebraicmg",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples::algebraicmg",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));

  tarch::logging::ChromeTraceFileLogger::getInstance().setOutputFile( "trace" );

  tarch::logging::LogFilter::getInstance().printFilterListToCout();

  #if PeanoDebug>=2
  tarch::tests::TestCaseRegistry::getInstance().getTestCaseCollection().run();
  int unitTestsErrors = tarch::tests::TestCaseRegistry::getInstance()
                       .getTestCaseCollection()
                       .getNumberOfErrors();

  if (unitTestsErrors != 0) {
    logError("main()", "unit tests failed. Quit.");
    exit(ExitCodeUnitTestsFailed);
  }
  #endif

  peano4::parallel::SpacetreeSet::getInstance().init(
    #if Dimensions==2
    { 0.0, 0.0},
    { 1.0, 1.0},
/*
    {-1.0, -1.0},
    { 2.0,  2.0},
*/
    #else
    {-1.0, -1.0, -1.0},
    { 2.0,  2.0,  2.0},
    #endif
    0
  );
  if (tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    do {
      examples::algebraicmg::observers::CreateGrid  observer;
      peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
    }
    while (peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getStationarySweeps()<2);

    {
      examples::algebraicmg::observers::SetupScenario  setupScenario;
      peano4::parallel::SpacetreeSet::getInstance().traverse(setupScenario);

      examples::algebraicmg::observers::PlotMaterialParameter  plotMaterialParameter;
      peano4::parallel::SpacetreeSet::getInstance().traverse(plotMaterialParameter);
    }

    for (int i=0; i<20; i++)
    {
      examples::algebraicmg::observers::ComputeResidualWithGeometricOperators  computeResidual;
      peano4::parallel::SpacetreeSet::getInstance().traverse(computeResidual);
      examples::algebraicmg::observers::JacobiUpdate  jacobiUpdate;
      peano4::parallel::SpacetreeSet::getInstance().traverse(jacobiUpdate);
      examples::algebraicmg::observers::ComputeGlobalResidualAndError computeGlobalResidualAndError;
      peano4::parallel::SpacetreeSet::getInstance().traverse(computeGlobalResidualAndError);
    }

    {
      examples::algebraicmg::observers::PlotSolution  observer;
      peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
    }
  }
  else {
    while (peano4::parallel::Node::getInstance().continueToRun()) {
      logDebug( "runParallel(...)", "trigger a new sweep with step " << peano4::parallel::Node::getInstance().getCurrentProgramStep() );
    }
  }

  peano4::shutdownSharedMemoryEnvironment();
  peano4::shutdownParallelEnvironment();

  return ExitCodeSuccess;
}
