// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include <string>

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
     * The time step size the solver would use if enforceTimeStepSize() had
     * not been called, i.e. the maximum time step size as startTimeStep()
     * has rolled it over. enforceTimeStepSize() does not alter it.
     */
    virtual double getAdmissibleTimeStepSize() const;

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
     * Solver state for the header of a checkpoint
     *
     * The checkpoint plotters (@ref exahype2.api.fv.WriteCheckpoint and
     * @ref exahype2.api.rkfd4.WriteCheckpoint) write the result into the
     * meta-data block of each Peano patch file. Every line holds one
     * attribute as key-value pair, with the attribute's name without its
     * leading underscore as key:
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~
     * minTimeStamp 1.00000000000000006e-01
     * maxTimeStamp 1.00000000000000006e-01
     * ~~~~~~~~~~~~~~~~~~~~~~~
     *
     * The lines are separated by line breaks. The values carry 17
     * significant digits, so a double survives the round trip through the
     * file unchanged.
     *
     * The routine writes the time stamps, the time step sizes, the
     * admissible time step size and the mesh sizes. It leaves out the
     * bookkeeping of the time step which is currently running (the
     * *UpdatedThisTimeStep attributes) and the cell update counter.
     * Generated solvers override it to append their own attributes via
     * toCheckpointMetaDataEntry().
     */
    virtual std::string toCheckpointMetaData() const;

    /**
     * One line of toCheckpointMetaData()
     *
     * @return key and value separated by a blank, with the value in
     *   scientific notation with 17 significant digits. No line break.
     */
    static std::string toCheckpointMetaDataEntry(const std::string& key, double value);

    /**
     * Restore the solver state from the header of a checkpoint
     *
     * Counterpart of toCheckpointMetaData(): The routine sets the time
     * stamps, the time step sizes, the admissible time step size and the
     * mesh sizes to the values in metaData. Attributes which metaData does
     * not hold keep their values. Afterwards, it resets the bookkeeping of
     * the time step as startTimeStep() does, so the next time step
     * continues from the restored state. Generated solvers override it to
     * restore their own attributes via getCheckpointMetaDataEntry().
     *
     * exahype2.api.blockstructured.LoadCheckpoint calls the routine once
     * the solution has been read from the checkpoint.
     */
    virtual void fromCheckpointMetaData(const std::string& metaData);

    /**
     * Find one entry of a checkpoint's meta data
     *
     * @param metaData Key-value pairs as written by toCheckpointMetaData(),
     *   one per line. Leading white spaces per line are ignored.
     * @param key Key of the entry, i.e. the attribute's name without
     *   leading underscore.
     * @param value Set to the entry's value if metaData holds the key.
     *   Otherwise, it is left unchanged.
     * @return Whether metaData holds the key.
     */
    static bool getCheckpointMetaDataEntry(const std::string& metaData, const std::string& key, double& value);

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
     * unchanged, and so does getAdmissibleTimeStepSize().
     */
    void enforceTimeStepSize(double timeStepSize);

    /**
     * Restart the time stepping with a time step size of zero
     *
     * Thread-safely sets the current minimum and maximum time step size and
     * the admissible time step size to zero (protected by _semaphore). The
     * next time step hence behaves like the very first one: It does not
     * advance the time stamp, but it derives an admissible time step size
     * from the current solution and mesh. The mesh adaptation uses this
     * routine whenever it has changed the mesh, as the time step size of the
     * old mesh might violate the CFL condition on the new one (@see
     * exahype2.api.blockstructured.ConsumeRefinementRequests). Optimistic
     * time stepping does not apply any estimate in this time step, as the
     * admissible time step size is zero.
     */
    void resetTimeStepSize();

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

    /**
     * @see getAdmissibleTimeStepSize()
     */
    double _admissibleTimeStepSize;

    double _minTimeStepSizeUpdatedThisTimeStep;
    double _maxTimeStepSizeUpdatedThisTimeStep;

    int _cellUpdates;

    tarch::multicore::BooleanSemaphore _semaphore;
};
