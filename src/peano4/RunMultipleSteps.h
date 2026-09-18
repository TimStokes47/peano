// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include <functional>
#include <string>
#include <vector>

#include "tarch/timing/Watch.h"
#include "tarch/timing/Measurement.h"


namespace peano4 {
  class RunMultipleSteps;
}


/**
 * Represents a series of steps which can be called by a runner
 *
 * Abstract class which makes limited sense in isolation, but has to be seen in
 * the context of codes alike ExaHyPE. They break down the calculation into a
 * sequence of phases (e.g. grid construction, time steps, plots) and each of
 * these can consist of multiple individual substeps. Think about a Runge-Kutta
 * scheme.
 *
 * Such steps should be collocated in an implementation of RunMultipleSteps and
 * their sequence triggered by run(). Once we combine things in a subclass of
 * this one, we can hand the collection of steps around for postprocessing for
 * example.
 */
class peano4::RunMultipleSteps {
  public:
    /**
     * Interface for anything that wants to report on a RunMultipleSteps
     * instance (or a whole collection thereof) once it has run.
     *
     * Purely abstract - it holds no state and has no implementation of its
     * own. @see peano4::DefaultStepReport for a concrete (stub) realisation.
     */
    class Report {
      public:
        virtual ~Report() = default;

        /**
         * Report on one particular step sequence right after it has run.
         */
        virtual void reportStep(const RunMultipleSteps& step) = 0;

        /**
         * Report on an arbitrary, unspecified number of step sequences,
         * e.g. once a whole simulation has terminated.
         */
        virtual void reportFinalResults(const std::vector<std::reference_wrapper<RunMultipleSteps>>& steps) = 0;
    };

    RunMultipleSteps(const std::string& name, int firstSubstep);
    virtual ~RunMultipleSteps() = default;

    /**
     * Run the mesh sweeps (Observers) realising this algorithmic phase.
     */
    virtual void run() = 0;

    tarch::timing::Measurement getRuntimeMeasurements() const;

    /**
     * Get the first substep which identifies this step sequence.
     */
    int getFirstSubstep() const;

    /**
     * Name of this step sequence, i.e. the program phase it represents.
     * Set once by the subclass through the constructor and never changed
     * afterwards. In ExaHyPE, this is e.g. "Plot" or "TimeStep".
     */
    std::string getName() const;

  protected:

    const std::string          _name;

    const int                  _firstSubstep;

    tarch::timing::Watch       _watch;

    /**
     * Measure time spent in this particular sequence of steps. Is an average.
     */
    tarch::timing::Measurement _runtimeMeasurement;
};
