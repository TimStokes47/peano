// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "peano4/RunMultipleSteps.h"


namespace peano4 {
  class DefaultStepReport;
}


/**
 * Default realisation of peano4::RunMultipleSteps::Report.
 *
 * This class is very close to the original ExaHyPE 2 output and provides
 * information per step, and finally summarises the outcomes of the individual
 * steps in a table.
 *
 * It does not provide any additional information on tracers, postprocessing,
 * and so forth. This would have to be implemented in a bespoke reporting
 * class.
 */
class peano4::DefaultStepReport: public peano4::RunMultipleSteps::Report {
  public:
    DefaultStepReport() = default;
    virtual ~DefaultStepReport() = default;

    virtual void reportStep(const RunMultipleSteps& step) override;

    virtual void reportFinalResults(const std::vector<std::reference_wrapper<RunMultipleSteps>>& steps) override;
  private:
    static tarch::logging::Log _log;
};
