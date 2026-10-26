// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "Simulation.h"

#include "peano4/parallel/Node.h"
#include "peano4/peano.h"
#include "RefinementControlService.h"
#include "tarch/accelerator/Device.h"
#include "tarch/Assertions.h"
#include "tarch/la/ScalarOperations.h"
#include "tarch/logging/Log.h"
#include "tarch/logging/Statistics.h"
#include "tarch/mpi/Rank.h"
#include "tarch/multicore/Core.h"
#include "tarch/multicore/multicore.h"
#include "tarch/NonCriticalAssertions.h"
#include "toolbox/loadbalancing/AbstractLoadBalancing.h"
#include "toolbox/loadbalancing/LoadBalancingService.h"


namespace {
  tarch::logging::Log _log("exahype2");

  /**
   * Run the runner which belongs to Node's current program step and report it
   */
  void runCurrentStep(
    peano4::RunMultipleSteps&         setupSimulation,
    peano4::RunMultipleSteps&         plot,
    peano4::RunMultipleSteps&         timeStep,
    peano4::RunMultipleSteps&         meshAdaptation,
    peano4::RunMultipleSteps::Report& report
  ) {
    const int step = peano4::parallel::Node::getInstance().getCurrentProgramStep(
    );

    if (step == plot.getFirstSubstep()) {
      plot.run();
      report.reportStep(plot);
    } else if (step == timeStep.getFirstSubstep()) {
      timeStep.run();
      report.reportStep(timeStep);
    } else if (step == setupSimulation.getFirstSubstep()) {
      setupSimulation.run();
      report.reportStep(setupSimulation);
    } else if (step == meshAdaptation.getFirstSubstep() and step != peano4::parallel::Node::UndefProgramStep) {
      meshAdaptation.run();
      report.reportStep(meshAdaptation);
    } else {
      assertionMsg(false, "Unknown step " << step << " in main().");
    }
  }

  /**
   * Make all ranks run the runner and run it locally
   */
  void runStep(
    peano4::RunMultipleSteps&         runner,
    peano4::RunMultipleSteps&         setupSimulation,
    peano4::RunMultipleSteps&         plot,
    peano4::RunMultipleSteps&         timeStep,
    peano4::RunMultipleSteps&         meshAdaptation,
    peano4::RunMultipleSteps::Report& report
  ) {
    peano4::parallel::Node::getInstance().setNextProgramStep(
      runner.getFirstSubstep()
    );
    peano4::parallel::Node::getInstance().continueToRun();
    runCurrentStep(setupSimulation, plot, timeStep, meshAdaptation, report);
  }
} // namespace


bool exahype2::configureRuntime(
  int                                        timeout,
  int                                        numberOfThreads,
  const std::set<int>&                       gpus,
  tarch::multicore::orchestration::Strategy* orchestration
) {
  if (timeout >= 0) {
    tarch::mpi::Rank::getInstance().setDeadlockTimeOut(timeout);
  }
  tarch::multicore::Core::getInstance().configure(numberOfThreads);
  tarch::accelerator::Device::getInstance().configure(gpus);
  tarch::multicore::setOrchestration(orchestration);

  if (tarch::mpi::Rank::getInstance().getNumberOfRanks() > 1 and tarch::multicore::Core::getInstance().getNumberOfThreads() <= 1) {
    logError(
      "configureRuntime(...)",
      "MPI runs without multithreading are not supported currently."
    );
    return false;
  }
  return true;
}


void exahype2::runSimulation(
  peano4::RunMultipleSteps&         setupSimulation,
  peano4::RunMultipleSteps&         plot,
  peano4::RunMultipleSteps&         timeStep,
  peano4::RunMultipleSteps&         meshAdaptation,
  peano4::RunMultipleSteps::Report& report,
  std::function<bool()>             wantsToPlot,
  std::function<double()>           getMinTimeStamp,
  std::function<double()>           getMaxTimeStamp,
  std::function<double()>           getMaxTimeStepSize,
  double                            minTerminalTime,
  double                            maxTerminalTime
) {
#if defined(SharedOMP)
#pragma omp parallel
  {
#pragma omp master
    {
#endif

#if defined(UseSmartMPI)
      const bool isGlobalMaster = tarch::mpi::Rank::getInstance().isGlobalMaster(
      );
      const bool isPeanoWorker = not tarch::mpi::Rank::getInstance()
                                       .isGlobalMaster()
                                 and smartmpi::isComputeRank();
#else
  const bool isGlobalMaster = tarch::mpi::Rank::getInstance().isGlobalMaster();
  const bool isPeanoWorker  = not isGlobalMaster;
#endif

      if (isGlobalMaster) {
        bool haveJustWrittenSnapshot = false;

        runStep(
          setupSimulation,
          setupSimulation,
          plot,
          timeStep,
          meshAdaptation,
          report
        );

        double maxTimeStepSize             = 1.0;
        int    numberOfStagnatingTimeSteps = 0;
        bool   timeStampsHaveStagnated     = false;
        while (
          getMinTimeStamp() < minTerminalTime
          and getMaxTimeStamp() < maxTerminalTime
          and tarch::la::greater(maxTimeStepSize, 0.0)
          and not timeStampsHaveStagnated
        ) {
          // Each solver's plotter decides itself whether it plots in the plot
          // phase. A plot does not advance the simulation, so it never
          // decides whether the loop goes on, and the next iteration runs a
          // time step.
          if (wantsToPlot() and not haveJustWrittenSnapshot) {
            haveJustWrittenSnapshot = true;
            runStep(
              plot,
              setupSimulation,
              plot,
              timeStep,
              meshAdaptation,
              report
            );
          } else {
            haveJustWrittenSnapshot      = false;
            const double oldMinTimeStamp = getMinTimeStamp();
            const double oldMaxTimeStamp = getMaxTimeStamp();
            runStep(
              timeStep,
              setupSimulation,
              plot,
              timeStep,
              meshAdaptation,
              report
            );
            maxTimeStepSize = getMaxTimeStepSize();

            if (meshAdaptation.getFirstSubstep() != peano4::parallel::Node::UndefProgramStep and isMeshAdaptationRequired()) {
              runStep(
                meshAdaptation,
                setupSimulation,
                plot,
                timeStep,
                meshAdaptation,
                report
              );
            }

            if (getMinTimeStamp() == oldMinTimeStamp and getMaxTimeStamp() == oldMaxTimeStamp) {
              numberOfStagnatingTimeSteps++;
            } else {
              numberOfStagnatingTimeSteps = 0;
            }

            if (numberOfStagnatingTimeSteps == 1) {
              logWarning(
                "runSimulation(...)",
                "min and max time stamp have not changed in time step: min time stamp="
                  << getMinTimeStamp()
                  << ", max time stamp=" << getMaxTimeStamp()
                  << ". Simulation terminates if they do not change in the next time step either"
              );
            } else if (numberOfStagnatingTimeSteps >= 2) {
              logError(
                "runSimulation(...)",
                "min and max time stamp have not changed in two consecutive time steps: min time stamp="
                  << getMinTimeStamp() << ", max time stamp="
                  << getMaxTimeStamp() << ". Terminate simulation"
              );
              timeStampsHaveStagnated = true;
            }
          }
        }

        if (wantsToPlot()) {
          runStep(plot, setupSimulation, plot, timeStep, meshAdaptation, report);
        }
      } else if (isPeanoWorker) {
        while (peano4::parallel::Node::getInstance().continueToRun()) {
          runCurrentStep(
            setupSimulation,
            plot,
            timeStep,
            meshAdaptation,
            report
          );
        }
      } else {
        assertionMsg(
          false,
          "This rank is neither global master nor Peano worker. This should not happen yet, as SmartMPI is not active."
        );
      }

#if defined(SharedOMP)
    }
  }
#endif

  report.reportFinalResults({setupSimulation, timeStep, plot, meshAdaptation});
}


bool exahype2::isMeshAdaptationRequired() {
  const bool hasPendingRefinementEvents
    = not ::exahype2::RefinementControlService::getInstance()
            .createGridControlEventsForNextTraversal()
            .empty();
  const bool hasSplitRecently
    = ::toolbox::loadbalancing::LoadBalancingService::getInstance()
        .hasLoadBalancing()
      and ::toolbox::loadbalancing::LoadBalancingService::getInstance()
            .getLoadBalancing()
            ->hasSplitRecently();
  return hasPendingRefinementEvents or hasSplitRecently;
}


void exahype2::shutdownRuntime(std::function<void()> shutdownDatatypes) {
  tarch::logging::Statistics::getInstance().writeToCSV();

  toolbox::loadbalancing::LoadBalancingService::getInstance().shutdown();
  peano4::shutdownSingletons();
  shutdownDatatypes();
  tarch::multicore::shutdownSmartMPI();
  tarch::shutdownNonCriticalAssertionEnvironment();
  peano4::shutdownParallelEnvironment();
}
