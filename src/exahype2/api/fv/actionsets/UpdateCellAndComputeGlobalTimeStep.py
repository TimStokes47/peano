# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class UpdateCellAndComputeGlobalTimeStep(AbstractFVActionSet):
    """!

    Update a Finite Volume patch (solver._patch) from its halo-augmented
    reconstruction (solver._patch_with_halo, @see
    toolbox.blockstructured.api.ReconstructPatchWithHalo) by applying the
    solver's numerical scheme (Rusanov flux, explicit Euler time stepping)
    once per volume, and derive the next global time step size from the
    CFL condition.

    Global time stepping: all cells share one time stamp and time step
    size, which are taken from the solver (getMinTimeStamp() and
    getMinTimeStepSize()) rather than from per-cell bookkeeping.

    The kernel updates all dimensions in one explicit Euler step, so the
    CFL condition reads dt * sum_d lambda_d/h_d <= 1, i.e. the admissible
    time step size is cfl_constant * h / (Dimensions * lambda_max), with h
    the minimal volume size and lambda_max the maximum eigenvalue.

    The compute kernel returns the maximum eigenvalue over the patch it has
    just updated. touchCellFirstTime() folds it into the static
    _maximumEigenvalue, and unprepareTraversal() turns the global maximum
    into the next time step size and feeds it into ::exahype2::Solver's own
    time step bookkeeping (updateCell()/finishTimeStep()/startTimeStep()).
    There is deliberately no separate, solver-level time stamp/time step
    size state on top of ::exahype2::Solver.

    As this is global time stepping, getMinTimeStamp()==getMaxTimeStamp()
    and getMinTimeStepSize()==getMaxTimeStepSize() always hold. Hence a
    single updateCell() call per time step, not one per cell, is enough to
    keep both the min and the max in sync.

    """

    def __init__(self, solver, cfl_constant, patch_size, guard):
        """!

        solver: exahype2.api.Solver
          Has to have solver._patch and solver._patch_with_halo set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        cfl_constant: float
          Safety factor the admissible time step size is scaled down by,
          relative to the theoretical CFL=1 stability limit (which already
          accounts for the number of dimensions, @see class docstring) - @see
          exahype2.api.fv.RusanovWithExplicitEuler's own cfl_constant
          parameter, which this is meant to be passed straight through
          from.

        patch_size: int
          Number of Finite Volumes per patch and dimension. The CFL
          condition is evaluated over the volume size, i.e. the cell size
          divided by patch_size.

        guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch is updated and contributes to
          the CFL condition. Typically the output of the solver's
          grid_selector (@see peano4.api.toolbox.GridSelector).

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard
        self.cfl_constant = cfl_constant
        self.patch_size = patch_size

    def get_body_of_prepareTraversal(self):
        """!

        prepareTraversal() is static - called once per rank, before any
        instance of this action set is created - so it can only ever touch
        static members, never a per-instance one. _maximumEigenvalue is
        static for exactly that reason: it is reset here, once, before
        every grid sweep, and then accumulated into by potentially many
        instances of this action set (one per tree, possibly running on
        different threads concurrently) as touchCellFirstTime() updates
        the cells - @see _semaphore, which guards that concurrent access.

        """
        return "  _maximumEigenvalue = 0.0;\n"

    def get_body_of_unprepareTraversal(self):
        """!

        Called once per rank, after every instance of this action set for
        the current grid sweep has already been destroyed (@see
        get_body_of_prepareTraversal()'s note on the static/per-instance
        split) - the one safe place both to read _maximumEigenvalue
        (nothing can still be writing it) and to issue an MPI reduction:
        tarch::mpi::Rank::allReduce()'s own documentation requires "only
        one thread per rank" to ever call it, which a once-per-rank
        static method already guarantees.

        This combines two different kinds of "global", reduced two
        different ways:

        - The mesh size comes out of peano4::grid::GridStatistics, which -
          @see page_grid_statistics - Peano itself never reduces across
          ranks, only ever accumulates locally per rank.
          peano4::grid::GridStatistics has no MPI_Op of its own (there is
          no meaningful "MPI_MIN of a GridStatistics"), so
          tarch::mpi::Rank::allReduce() cannot reduce it directly - this
          uses peano4::grid::reduceGridStatistics() instead, which every
          rank calls with its own local GridStatistics and gets back the
          same, fully combined one (@see its own documentation for how -
          a point-to-point gather-to-master via operator+(), then one
          MPI_Bcast - and page_grid_statistics for why this is a
          different, coarser-grained pattern than the scalar one below).
          GridStatistics holds the cell size; the CFL condition needs the
          volume size, i.e. the cell size divided by the patch size.
        - _maximumEigenvalue is a plain double, already only a local,
          per-rank accumulation (@see _semaphore) - that one *is* reduced
          directly via tarch::mpi::Rank::allReduce(..., MPI_MAX, ...),
          exactly the way page_grid_statistics's "Reducing a value across
          ranks yourself" section documents for a single scalar.

        Once newTimeStepSize is known, it folds it into the solver's own
        time step bookkeeping (@see this class' docstring for why a single
        updateCell() call, not one per cell, is correct for this solver).
        newTimeStepSize is capped by MaxTerminalTime minus the new time
        stamp, so the last time step ends exactly at the end time.
        The time stamp is advanced by the time step size the update in
        this very sweep used (getMaxTimeStepSize() before updateCell()),
        not by newTimeStepSize, which only the next update uses. The very
        first time step therefore runs with time step size 0 and leaves
        the time stamp at 0: it only yields the first admissible time
        step size.

        - updateCell(newTimeStepSize, newTimeStamp) folds the new values
          into exahype2::Solver's ...UpdatedThisTimeStep accumulators
          (thread-safely, though there is only one caller here).
        - finishTimeStep() then MPI-all-reduces that accumulator across
          ranks - a no-op given newTimeStepSize/newTimeStamp are already
          identical on every rank (they were computed from already-
          globally-reduced quantities above), but calling it anyway keeps
          the full ::exahype2::Solver lifecycle honest for any future
          override that relies on it actually running.
        - startTimeStep(...) immediately promotes that same accumulator
          into the stable _minTimeStamp/_maxTimeStamp/_minTimeStepSize/
          _maxTimeStepSize fields getMinTimeStamp()/.../getMaxTimeStepSize()
          expose - called here, right after finishTimeStep(), rather than
          at the *next* TimeStep phase's prepareTraversal() (the pattern
          exahype2's legacy main uses, @see
          python/exahype2/ExaHyPEMain.template.cpp), specifically so
          there is no one-phase lag: the very first TimeStep phase must
          already leave a genuinely positive getMaxTimeStepSize() behind
          for the generated main loop's
          tarch::la::greater(maxTimeStepSize,0.0) guard to see, or the
          simulation could never advance past its first iteration.

        """
        return f"""
  const peano4::grid::GridStatistics localStatistics  = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics();
  const peano4::grid::GridStatistics globalStatistics = peano4::grid::reduceGridStatistics(localStatistics);
  const double                       globalMinVolumeH = tarch::la::max( globalStatistics.getMinH() ) / {self.patch_size};

  // No semaphore required here, as it is only called once per rank
  double localMaximumEigenvalue = _maximumEigenvalue;
  double globalMaximumEigenvalue = localMaximumEigenvalue;
  #ifdef Parallel
  tarch::mpi::Rank::getInstance().allReduce(
    &localMaximumEigenvalue, &globalMaximumEigenvalue, 1, MPI_DOUBLE, MPI_MAX,
    [&]() -> void {{ tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }}
  );
  #endif

  // The time stamp advances by the time step size this sweep's update used,
  // newTimeStepSize is the one the next update will use
  const double usedTimeStepSize = repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStepSize();
  const double newTimeStamp     = repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStamp() + usedTimeStepSize;
  // The last time step is shortened such that the simulation ends exactly at MaxTerminalTime
  const double newTimeStepSize  = std::min(
    {self.cfl_constant} * globalMinVolumeH / globalMaximumEigenvalue / Dimensions,
    std::max(0.0, MaxTerminalTime - newTimeStamp)
  );

  repositories::{self._solver.get_name_of_global_instance()}.updateCell( newTimeStepSize, newTimeStamp );
  repositories::{self._solver.get_name_of_global_instance()}.finishTimeStep();
  repositories::{self._solver.get_name_of_global_instance()}.startTimeStep(
    repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStamp(),
    repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStamp(),
    repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize(),
    repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStepSize()
  );
  logInfo( "unprepareTraversal()", "t=" << repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStamp() << ", new dt=" << newTimeStepSize );
"""

    def get_attributes(self):
        return """
  static double                              _maximumEigenvalue;
  static tarch::multicore::BooleanSemaphore  _semaphore;
"""

    def get_static_initialisations(self, full_qualified_classname):
        return (
            "double "
            + full_qualified_classname
            + "::_maximumEigenvalue = 0.0;\n"
            + "tarch::multicore::BooleanSemaphore "
            + full_qualified_classname
            + "::_semaphore;\n"
        )

    def get_includes(self):
        return (
            AbstractFVActionSet.get_includes(self)
            + """
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"
#include "tarch/mpi/Rank.h"
#include "tarch/services/ServiceRepository.h"
#include "tarch/la/VectorOperations.h"
#include "peano4/parallel/SpacetreeSet.h"
#include "peano4/grid/grid.h"
"""
        )

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            result = f"""
  if ({self.guard}) {{
    double timeStamp    = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStamp();
    double timeStepSize = repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize();
    ::exahype2::CellData patchData(fineGridCell{self._solver._patch_with_halo.name}.value, marker.x(), marker.h(), timeStamp, timeStepSize, fineGridCell{self._solver._patch.name}.value);
    {self._solver.compute_kernel_call}

    tarch::multicore::Lock lock(_semaphore);
    _maximumEigenvalue = std::max(patchData.maxEigenvalue[0], _maximumEigenvalue);
  }}
"""
        return result

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
