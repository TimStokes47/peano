#include "tarch/logging/Log.h"
#include "tarch/tests/TestCaseRegistry.h"
#include "tarch/logging/CommandLineLogger.h"
#include "tarch/parallel/Node.h"

#include "peano/peano.h"

#include "multigrid/runners/Runner.h"
#include "multigrid/mappings/CreateGrid.h"


tarch::logging::Log _log("");


int main(int argc, char** argv) {
  peano::fillLookupTables();

  int parallelSetup = peano::initParallelEnvironment(&argc,&argv);
  if ( parallelSetup!=0 ) {
    #ifdef Parallel
    // Please do not use the logging if MPI doesn't work properly.
    std::cerr << "mpi initialisation wasn't successful. Application shut down" << std::endl;
    #else
    _log.error("main()", "mpi initialisation wasn't successful. Application shut down");
    #endif
    return parallelSetup;
  }

  int sharedMemorySetup = peano::initSharedMemoryEnvironment();
  if (sharedMemorySetup!=0) {
    logError("main()", "shared memory initialisation wasn't successful. Application shut down");
    return sharedMemorySetup;
  }

  int programExitCode = 0;

  if (argc!=2) {
    std::cout << "Usage: ./executable scenario" << std::endl
              << std::endl
              << "Valid scenarios:" << std::endl
              <<  "Poisson" << std::endl
              <<  "AnisotropicPoisson" << std::endl
              <<  "ShiftedMinimalCheckerboard" << std::endl
              <<  "LayerProblem" << std::endl
              <<  "DiagonalFlow" << std::endl
              <<  "RecirculatingFlow" << std::endl
              << std::endl;
    programExitCode = 1;
  }
  else {
    if (std::string(argv[1])=="Poisson") {
      multigrid::mappings::CreateGrid::_scenario = multigrid::mappings::CreateGrid::Poisson;
    }
    else if (std::string(argv[1])=="AnisotropicPoisson") {
      multigrid::mappings::CreateGrid::_scenario = multigrid::mappings::CreateGrid::AnisotropicPoisson;
    }
    else if (std::string(argv[1])=="ShiftedMinimalCheckerboard") {
      multigrid::mappings::CreateGrid::_scenario = multigrid::mappings::CreateGrid::ShiftedMinimalCheckerboard;
    }
    else if (std::string(argv[1])=="LayerProblem") {
      multigrid::mappings::CreateGrid::_scenario = multigrid::mappings::CreateGrid::LayerProblem;
    }
    else if (std::string(argv[1])=="DiagonalFlow") {
      multigrid::mappings::CreateGrid::_scenario = multigrid::mappings::CreateGrid::DiagonalFlow;
    }
    else if (std::string(argv[1])=="RecirculatingFlow") {
      multigrid::mappings::CreateGrid::_scenario = multigrid::mappings::CreateGrid::RecirculatingFlow;
    }
    else {
      std::cerr << "invalid scenario. Please run without arguments to see list of supported scenarios" << std::endl;
      programExitCode = 2;
    }
  }

  // Configure the output
  tarch::logging::CommandLineLogger::getInstance().clearFilterList();
  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( ::tarch::logging::CommandLineLogger::FilterListEntry( "info", false ) );
  tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( ::tarch::logging::CommandLineLogger::FilterListEntry( "debug", true ) );
//  tarch::logging::CommandLineLogger::getInstance().setLogFormat( ... please consult source code documentation );

  // Runs the unit tests
  if (programExitCode==0) {
    tarch::tests::TestCaseRegistry::getInstance().getTestCaseCollection().run();
    programExitCode = tarch::tests::TestCaseRegistry::getInstance().getTestCaseCollection().getNumberOfErrors();
  }

  // dummy call to runner
  if (programExitCode==0) {
    tarch::logging::CommandLineLogger::getInstance().addFilterListEntry( ::tarch::logging::CommandLineLogger::FilterListEntry( "debug", -1, "multigrid", false ) );
    multigrid::runners::Runner runner;
    programExitCode = runner.run();
  }
  
  if (programExitCode==0) {
    #ifdef Parallel
    if (tarch::parallel::Node::getInstance().isGlobalMaster()) {
      logInfo( "main()", "Peano terminates successfully" );
    }
    #else
    logInfo( "main()", "Peano terminates successfully" );
    #endif
  }
  else {
    logInfo( "main()", "quit with error code " << programExitCode );
  }

  peano::shutdownParallelEnvironment();
  peano::shutdownSharedMemoryEnvironment();
  peano::releaseCachedData();

  return programExitCode;
}
