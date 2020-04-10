#include "finitevolumes-main.h"
#include "Constants.h"

#include "tarch/logging/Log.h"
#include "tarch/logging/LogFilter.h"
#include "tarch/multicore/multicore.h"

#include "peano4/peano.h"
#include "peano4/grid/Spacetree.h"
#include "peano4/parallel/SpacetreeSet.h"

#include "observers/DataRepository.h"
#include "observers/StepRepository.h"

#include "observers/CreateGrid.h"
#include "observers/PlotSolution.h"
#include "observers/TimeStep.h"

#include "peano4/UnitTests.h"
#include "tarch/UnitTests.h"



using namespace examples::exahype2::finitevolumes;



tarch::logging::Log _log("::");


// @todo Das sollte das Python-Ding direkt generieren (aber net ueberschreiben)


/**
 * @return continues to run
 */
bool selectNextAlgorithmicStep() {
  static bool   gridConstructed   = false;
  static double nextPlotTimeStamp = FirstPlotTimeStamp;
  static bool   haveWrittenVeryLastSnapshot = false;
  bool          continueToSolve   = true;

  if (not gridConstructed) {
    peano4::parallel::Node::getInstance().setNextProgramStep(
      observers::StepRepository::toProgramStep( observers::StepRepository::Steps::CreateGrid )
    );
    if ( peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getStationarySweeps()>=5 ) {
      gridConstructed = true;
    }
    continueToSolve = true;
  }
  else {
    if ( observers::getMinTimeStamp()>=nextPlotTimeStamp  and TimeInBetweenPlots>0.0 ) {
      nextPlotTimeStamp += TimeInBetweenPlots;
      peano4::parallel::Node::getInstance().setNextProgramStep(
        observers::StepRepository::toProgramStep( observers::StepRepository::Steps::PlotSolution )
      );
      continueToSolve = true;
    }
    else if ( observers::getMinTimeStamp()<TerminalTime ) {
      peano4::parallel::Node::getInstance().setNextProgramStep(
        observers::StepRepository::toProgramStep( observers::StepRepository::Steps::TimeStep )
      );
      continueToSolve = true;
    }
    else {
      if (not haveWrittenVeryLastSnapshot and TimeInBetweenPlots>0.0) {
        haveWrittenVeryLastSnapshot = true;
        peano4::parallel::Node::getInstance().setNextProgramStep(
          observers::StepRepository::toProgramStep( observers::StepRepository::Steps::PlotSolution )
        );
        continueToSolve = true;
      }
      else {
        continueToSolve = false;
      }
    }
  }

  return continueToSolve;
}


void step() {
  int  stepIdentifier = peano4::parallel::Node::getInstance().getCurrentProgramStep();
  auto stepName       = observers::StepRepository::toStepEnum(stepIdentifier);

  static tarch::logging::Log _log("");
  logInfo( "step()", "run " << observers::StepRepository::toString(stepName) );

  switch ( stepName ) {
    case observers::StepRepository::Steps::CreateGrid:
      {
        observers::CreateGrid  observer;
        peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
      }
      break;
    case observers::StepRepository::Steps::PlotSolution:
      {
        observers::PlotSolution  observer;
        peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
      }
      break;
    case observers::StepRepository::Steps::TimeStep:
      {
        const double minTimeStamp    = observers::getMinTimeStamp();
        const double maxTimeStamp    = observers::getMaxTimeStamp();
        const double minTimeStepSize = observers::getMinTimeStepSize();
        const double maxTimeStepSize = observers::getMaxTimeStepSize();

        logInfo( "step()", "t_{min}  = " << minTimeStamp );
        logInfo( "step()", "t_{max}  = " << maxTimeStamp );
        logInfo( "step()", "dt_{min} = " << minTimeStepSize );
        logInfo( "step()", "dt_{max} = " << maxTimeStepSize );
        observers::startTimeStep( minTimeStamp, maxTimeStamp, minTimeStepSize, maxTimeStepSize );
        observers::TimeStep  observer;
        peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
        observers::finishTimeStep();
      }
      break;
  }
}



int main(int argc, char** argv) {
  const int ExitCodeSuccess         = 0;
  const int ExitCodeUnitTestsFailed = 1;

  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug, 
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
    tarch::logging::LogFilter::FilterListEntry::TargetInfo, 
    tarch::logging::LogFilter::FilterListEntry::AnyRank, 
    "peano4", 
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));
  
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug, 
    tarch::logging::LogFilter::FilterListEntry::AnyRank, 
    "tarch", 
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace, 
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
    tarch::logging::LogFilter::FilterListEntry::TargetDebug,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples::exahype2::finitevolumes",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples::exahype2::finitevolumes",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples::exahype2::finitevolumes",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));

  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetDebug, 
    tarch::logging::LogFilter::FilterListEntry::AnyRank, 
    "exahype2::fv",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace, 
    tarch::logging::LogFilter::FilterListEntry::AnyRank, 
    "exahype2::fv",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo, 
    tarch::logging::LogFilter::FilterListEntry::AnyRank, 
    "exahype2::fv",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));

  peano4::initParallelEnvironment(&argc,&argv);
  peano4::fillLookupTables();
  
  examples::exahype2::finitevolumes::observers::DataRepository::initDatatypes();


  #if PeanoDebug>=2
  tarch::tests::TestCase* peanoCoreTests = peano4::getUnitTests();
  peanoCoreTests->run();
  if (peanoCoreTests->getNumberOfErrors() != 0) {
    logError("main()", "Peano4 core unit tests failed. Quit.");
    exit(ExitCodeUnitTestsFailed);
  }
  delete peanoCoreTests;

  tarch::tests::TestCase* peanoTarchTests = tarch::getUnitTests();
  peanoTarchTests->run();
  if (peanoTarchTests->getNumberOfErrors() != 0) {
    logError("main()", "technical architecture (tarch) unit tests failed. Quit.");
    exit(ExitCodeUnitTestsFailed);
  }
  delete peanoTarchTests;
 
  #endif

  peano4::parallel::SpacetreeSet::getInstance().init(
    #if Dimensions==2
    {0.0, 0.0},
    {1.0, 1.0},
    #else
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0},
    #endif
    0
  );
  if (tarch::mpi::Rank::getInstance().isGlobalMaster() ) {
    while ( selectNextAlgorithmicStep() ) {
      step();
    }
  }
  else {
    while (peano4::parallel::Node::getInstance().continueToRun()) {
      step();
    }
  }

  examples::exahype2::finitevolumes::observers::DataRepository::shutdownDatatypes();

  peano4::shutdownParallelEnvironment();

  return ExitCodeSuccess;
}
