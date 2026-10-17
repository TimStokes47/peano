// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/la/Vector.h"
#include "tarch/multicore/BooleanSemaphore.h"


namespace exahype2 {
  class Solver;
}



class exahype2::Solver {
  public:
    /**
     * This is a "fake" enum, i.e. we do not use it to distinguish different
     * variants. Instead, we use it as a fix that allows us to "overload"
     * operations:
     *
     * In C++ you cannot overload w.r.t. static. We however need functions which
     * exist twice in ExaHyPE: Once as standard (virtual) member functions and
     * once at static version which an be offloaded to a GPU as it does not
     * have a state. Both function variants, in theory, have the same signature
     * but if they had, a compiler could not distinguish them. So I use this
     * enum for the GPU version.
     *
     * If you create a solver without GPU support, this enum will not be used.
     * It is however always created. Once you write a GPU version and then compile
     * without GPU support, you will thus still be able to have all your GPU
     * function variants, and you don't have to work with ifdefs.
     */
    enum class Offloadable {
      Yes
    };

    /**
     * The min and max time stamp and the time step size all start at 0, so
     * the very first time step does not advance the solution. It only
     * yields the first admissible time step size.
     */
    Solver();

    virtual double getMinTimeStamp() const;
    virtual double getMaxTimeStamp() const;
    virtual double getMinTimeStepSize() const;
    virtual double getMaxTimeStepSize() const;

    /**
     * Default implementation: rolls the previous sweep's accumulated
     * _minTimeStampUpdatedThisTimeStep/_maxTimeStampUpdatedThisTimeStep,
     * _minTimeStepSizeUpdatedThisTimeStep/_maxTimeStepSizeUpdatedThisTimeStep and
     * _minVolumeHUpdatedThisTimeStep/_maxVolumeHUpdatedThisTimeStep over into the
     * corresponding global attributes, and resets the UpdatedThisTimeStep
     * accumulators (and _cellUpdates) so the new sweep about to start can
     * accumulate its own contributions via updateCell(). globalMin/Max...
     * are not used by the default implementation - they are there for
     * solvers that need to react to the other solvers' bounds in a
     * multi-solver project.
     */
    virtual void startTimeStep(
      double globalMinTimeStamp,
      double globalMaxTimeStamp,
      double globalMinTimeStepSize,
      double globalMaxTimeStepSize
    );

    /**
     * Default implementation: MPI-all-reduces the UpdatedThisTimeStep
     * accumulators (min via MPI_MIN, max via MPI_MAX, _cellUpdates via
     * MPI_SUM) across ranks, so every rank's startTimeStep() rolls over
     * the same, globally synchronised values. A no-op in a build without
     * MPI support.
     */
    virtual void finishTimeStep();

    /**
     * Not pure virtual, unlike the other lifecycle hooks above: overriding
     * it is optional. The legacy python/exahype2 solvers override it to
     * drive their own _solverState transitions into
     * SolverState::Plotting/PlottingAfterGridInitialisation; newer solvers
     * that have no use for it can simply not override it.
     */
    virtual void startPlottingStep(
      double globalMinTimeStamp,
      double globalMaxTimeStamp,
      double globalMinTimeStepSize,
      double globalMaxTimeStepSize
    ) {}

    /**
     * Not pure virtual - @see startPlottingStep().
     */
    virtual void finishPlottingStep() {}

    virtual double getMaxMeshSize() const;
    virtual double getMinMeshSize() const;

    /**
     * Default implementation: always returns true, i.e. every cell uses
     * the stateless (GPU-offloadable) variants of the PDE terms wherever
     * a solver offers both a stateless and a stateful variant. A solver
     * overrides this to opt individual cells out of the stateless
     * variants and into the ones with state.
     */
    virtual bool cellCanUseStatelessPDETerms(
      const tarch::la::Vector<Dimensions, double>& cellCentre,
      const tarch::la::Vector<Dimensions, double>& cellH,
      double                                       t,
      double                                       dt
    ) const;

    /**
     * Thread-safely fold one cell's contribution into this time step's
     * running time stamp and time step size bounds (protected by
     * _semaphore). Does not touch _minVolumeH/_maxVolumeH - those describe
     * the mesh, not the time stepping, and are set elsewhere.
     */
    void updateCell(double timeStepSize, double timeStamp);

    /**
     * Thread-safely overwrite the current minimum and maximum time step size
     * (protected by _semaphore), i.e. what getMinTimeStepSize() and
     * getMaxTimeStepSize() return from now on. Optimistic time stepping uses
     * this setter to switch to the time step size it has estimated
     * beforehand (@see
     * exahype2.api.fv.actionsets.ReadjustTimeStepSizeForOptimisticGlobalTimestepping).
     * The accumulators of the time step that is currently running remain
     * unchanged.
     */
    void enforceTimeStepSize(double timeStepSize);

  protected:
    double _minTimeStamp;
    double _maxTimeStamp;

    double _minTimeStampUpdatedThisTimeStep;
    double _maxTimeStampUpdatedThisTimeStep;

    double _minVolumeH;
    double _maxVolumeH;

    double _minVolumeHUpdatedThisTimeStep;
    double _maxVolumeHUpdatedThisTimeStep;

    double _minTimeStepSize;
    double _maxTimeStepSize;

    double _minTimeStepSizeUpdatedThisTimeStep;
    double _maxTimeStepSizeUpdatedThisTimeStep;

    int _cellUpdates;

    tarch::multicore::BooleanSemaphore _semaphore;
};
