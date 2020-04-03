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


tarch::logging::Log _log("::");



/**
 * @return continues to run
 */
bool selectNextAlgorithmicStep() {
  static int counter           = 0;
  bool       continueToSolve   = true;
  const int  NumberOfTimeSteps = 20;

  if (
    counter==0
	and
	peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getStationarySweeps()<5
  ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(
      examples::finitevolumes::observers::StepRepository::toProgramStep(
        examples::finitevolumes::observers::StepRepository::Steps::CreateGrid
	  )
    );
    continueToSolve = true;
  }
  else if (
    ( counter==0 and peano4::parallel::SpacetreeSet::getInstance().getGridStatistics().getStationarySweeps()>=5 )
	or
	counter >= NumberOfTimeSteps + 1
  ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(
      examples::finitevolumes::observers::StepRepository::toProgramStep(
        examples::finitevolumes::observers::StepRepository::Steps::PlotSolution
	  )
    );
    counter++;
    continueToSolve = counter <= NumberOfTimeSteps + 2;
  }
  else if ( counter>=1 and counter < NumberOfTimeSteps+1
  ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(
      examples::finitevolumes::observers::StepRepository::toProgramStep(
        examples::finitevolumes::observers::StepRepository::Steps::TimeStep
	  )
    );
    counter++;
    continueToSolve = true;
  }
  else {
    counter++;
    continueToSolve = false;
  }

  return continueToSolve;
}


void step() {
  int  stepIdentifier = peano4::parallel::Node::getInstance().getCurrentProgramStep();
  auto stepName       = examples::finitevolumes::observers::StepRepository::toStepEnum(stepIdentifier);

  static tarch::logging::Log _log("");
  logInfo( "step()", "run step " << examples::finitevolumes::observers::StepRepository::toString(stepName) );

  switch ( stepName ) {
    case examples::finitevolumes::observers::StepRepository::Steps::CreateGrid:
      {
        examples::finitevolumes::observers::CreateGrid  observer;
	    peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
	  }
	  break;
    case examples::finitevolumes::observers::StepRepository::Steps::PlotSolution:
      {
        examples::finitevolumes::observers::PlotSolution  observer;
	    peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
	  }
	  break;
    case examples::finitevolumes::observers::StepRepository::Steps::TimeStep:
      {
        examples::finitevolumes::observers::TimeStep  observer;
	    peano4::parallel::SpacetreeSet::getInstance().traverse(observer);
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
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
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
    "examples",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetTrace,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples",
    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
  ));
  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
    tarch::logging::LogFilter::FilterListEntry::TargetInfo,
    tarch::logging::LogFilter::FilterListEntry::AnyRank,
    "examples",
    tarch::logging::LogFilter::FilterListEntry::WhiteListEntry
  ));
 
  // @todo Add your namespace here
//  tarch::logging::LogFilter::getInstance().addFilterListEntry( tarch::logging::LogFilter::FilterListEntry(
//    tarch::logging::LogFilter::FilterListEntry::Target..., 
//    tarch::logging::LogFilter::FilterListEntry::AnyRank, 
//    "mynamespace", 
//    tarch::logging::LogFilter::FilterListEntry::BlackListEntry
//  ));

  peano4::initParallelEnvironment(&argc,&argv);
  peano4::fillLookupTables();
  
  examples::finitevolumes::observers::DataRepository::initDatatypes();


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

  examples::finitevolumes::observers::DataRepository::shutdownDatatypes();

  peano4::shutdownParallelEnvironment();

  return ExitCodeSuccess;
}
