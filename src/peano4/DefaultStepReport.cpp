#include "peano4/DefaultStepReport.h"
#include "peano4/parallel/SpacetreeSet.h"


tarch::logging::Log peano4::DefaultStepReport::_log("peano4::DefaultStepReport");


void peano4::DefaultStepReport::reportStep(const RunMultipleSteps& step) {
  const auto stats = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics();

  logInfo( "reportStep(RunMultipleSteps)", "step " << step.getName() << ": " << step.getRuntimeMeasurements().getValue() << "s" );

  logInfo(
    "reportStep(RunMultipleSteps)",
    "min mesh size="    << tarch::la::max(stats.getMinH()) <<
    ", #local cells=("  << stats.getNumberOfLocalUnrefinedCells()  << "/" << stats.getNumberOfLocalRefinedCells()  << ")" <<
    ", #remote cells=(" << stats.getNumberOfRemoteUnrefinedCells() << "/" << stats.getNumberOfRemoteRefinedCells() << ")"
  );
}


void peano4::DefaultStepReport::reportFinalResults(const std::vector<std::reference_wrapper<RunMultipleSteps>>& steps) {
  for (auto p : steps) {
    auto& step = p.get();
    logInfo(
      "reportFinalResults()",
      step.getName() << ": " << step.getRuntimeMeasurements().getValue() <<
      "s\t" << step.getRuntimeMeasurements().toString()
    );
  }
}
