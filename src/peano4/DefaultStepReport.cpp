// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#include "peano4/DefaultStepReport.h"


tarch::logging::Log peano4::DefaultStepReport::_log("peano4::DefaultStepReport");


void peano4::DefaultStepReport::reportStep(RunMultipleSteps& step) {
  logInfo( "reportStep()", "step " << step.getName() << ": " << step.getRuntimeMeasurements().getValue() << "s" );
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
