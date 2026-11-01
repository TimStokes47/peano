# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class ComputeGlobalCFLCondition(AbstractFVActionSet):
    """!

    Reduce the cells' maximum eigenvalues and derive the next global time
    step size from the CFL condition.

    exahype2.api.fv.actionsets.UpdateCell stores the maximum eigenvalue of
    every patch it has updated in the cell data solver._max_eigenvalue. This
    action set runs in a subsequent mesh traversal: touchCellFirstTime()
    folds each cell's value into the static _maxEigenvalue, and
    unprepareTraversal() turns the global maximum into the next time step
    size and feeds it into ::exahype2::Solver's own time step bookkeeping
    (updateCell()/finishTimeStep()/startTimeStep()). There is deliberately no
    separate, solver-level time stamp/time step size state on top of
    ::exahype2::Solver.

    Global time stepping: all cells share one time stamp and time step size,
    which are taken from the solver (getMinTimeStamp() and
    getMinTimeStepSize()) rather than from per-cell bookkeeping.

    The kernel updates all dimensions in one explicit Euler step, so the CFL
    condition reads dt * sum_d lambda_d/h_d <= 1, i.e. the admissible time
    step size is cfl_constant * h / (Dimensions * lambda_max), with h the
    minimal volume size and lambda_max the maximum eigenvalue.

    As this is global time stepping, getMinTimeStamp()==getMaxTimeStamp()
    and getMinTimeStepSize()==getMaxTimeStepSize() always hold. Hence a
    single updateCell() call per time step, not one per cell, is enough to
    keep both the min and the max in sync.

    """

    def __init__(self, solver, cfl_constant, patch_size, guard, max_eigenvalue=None):
        """!

        @param solver: exahype2.api.Solver
          Has to have solver._max_eigenvalue set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        @param cfl_constant: float
          Safety factor the admissible time step size is scaled down by,
          relative to the theoretical CFL=1 stability limit (which already
          accounts for the number of dimensions, see class docstring) - @see
          exahype2.api.fv.RusanovWithExplicitEuler's own cfl_constant
          parameter, which this is meant to be passed straight through
          from.

        @param patch_size: int
          Number of Finite Volumes per patch and dimension. The CFL
          condition is evaluated over the volume size, i.e. the cell size
          divided by patch_size.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell contributes to the CFL condition. Has
          to match the guard of exahype2.api.fv.actionsets.UpdateCell, as
          only the cells updated there hold a valid maximum eigenvalue.

        @param max_eigenvalue: peano4.api.datamodel.DaStGen2
          The cell data holding the maximum eigenvalue (first entry).
          Defaults to solver._max_eigenvalue.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard
        self._max_eigenvalue = max_eigenvalue if max_eigenvalue is not None else solver._max_eigenvalue
        self.cfl_constant = cfl_constant
        self.patch_size = patch_size

    def get_body_of_prepareTraversal(self):
        """!

        prepareTraversal() is static - called once per rank, before any
        instance of this action set is created - so it can only ever touch
        static members, never a per-instance one. _maxEigenvalue is static
        for exactly that reason: it is reset here, once, before every grid
        sweep, and then accumulated into by potentially many instances of
        this action set (one per tree, possibly running on different threads
        concurrently) as touchCellFirstTime() visits the cells - see
        _semaphore, which guards that concurrent access.

        """
        return "  _maxEigenvalue = 0.0;\n"

    def get_body_of_unprepareTraversal(self):
        """!

        Called once per rank, after every instance of this action set for
        the current grid sweep has already been destroyed (see
        get_body_of_prepareTraversal()'s note on the static/per-instance
        split) - the one safe place both to read _maxEigenvalue (nothing can
        still be writing it) and to issue an MPI reduction:
        tarch::mpi::Rank::allReduce()'s own documentation requires "only one
        thread per rank" to ever call it, which a once-per-rank static method
        already guarantees.

        This combines two different kinds of "global", reduced two different
        ways:

        - The mesh size comes out of peano4::grid::GridStatistics, which
          peano4::parallel::SpacetreeSet reduces over all ranks at the end
          of every traversal (@ref page_grid_statistics). Every rank hence
          reads the same, global value. GridStatistics holds the cell size;
          the CFL condition needs the volume size, i.e. the cell size
          divided by the patch size.
        - _maxEigenvalue is a plain double, already only a local, per-rank
          accumulation (see _semaphore) - that one *is* reduced directly via
          tarch::mpi::Rank::allReduce(..., MPI_MAX, ...).

        Once newTimeStepSize is known, it folds it into the solver's own
        time step bookkeeping (see this class' docstring for why a single
        updateCell() call, not one per cell, is correct for this solver).
        newTimeStepSize is not truncated by MaxTerminalTime: the last time
        step may overshoot the end time, and the simulation stops afterwards,
        i.e. MaxTerminalTime is a lower bound on the final time. The time
        stamp is advanced by the time step size the update used
        (getMaxTimeStepSize() before updateCell()), not by newTimeStepSize,
        which only the next update uses. The very first time step therefore
        runs with time step size 0 and leaves the time stamp at 0: it only
        yields the first admissible time step size.

        - updateCell(newTimeStepSize, newTimeStamp) folds the new values
          into exahype2::Solver's ...UpdatedThisTimeStep accumulators.
        - finishTimeStep() then MPI-all-reduces that accumulator across
          ranks - a no-op given newTimeStepSize/newTimeStamp are already
          identical on every rank, but calling it anyway keeps the full
          ::exahype2::Solver lifecycle honest for any future override that
          relies on it actually running.
        - startTimeStep(...) immediately promotes that same accumulator into
          the stable _minTimeStamp/_maxTimeStamp/_minTimeStepSize/
          _maxTimeStepSize fields getMinTimeStamp()/.../getMaxTimeStepSize()
          expose, so there is no one-phase lag: the very first TimeStep
          phase must already leave a genuinely positive getMaxTimeStepSize()
          behind for the generated main loop's
          tarch::la::greater(maxTimeStepSize,0.0) guard to see, or the
          simulation could never advance past its first iteration.

        """
        return f"""
  const peano4::grid::GridStatistics globalStatistics = peano4::parallel::SpacetreeSet::getInstance().getGridStatistics();
  const double                       globalMinVolumeH = tarch::la::max( globalStatistics.getMinH() ) / {self.patch_size};

  // No semaphore required here, as it is only called once per rank
  double localMaxEigenvalue  = _maxEigenvalue;
  double globalMaxEigenvalue = localMaxEigenvalue;
  #ifdef Parallel
  tarch::mpi::Rank::getInstance().allReduce(
    &localMaxEigenvalue, &globalMaxEigenvalue, 1, MPI_DOUBLE, MPI_MAX,
    [&]() -> void {{ tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }}
  );
  #endif

  // The time stamp advances by the time step size the update used,
  // newTimeStepSize is the one the next update will use
  const double usedTimeStepSize = repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStepSize();
  const double newTimeStamp     = repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStamp() + usedTimeStepSize;
  // The last time step is not truncated, i.e. it may overshoot MaxTerminalTime
  const double newTimeStepSize  = {self.cfl_constant} * globalMinVolumeH / globalMaxEigenvalue / Dimensions;

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
  static double                              _maxEigenvalue;
  static tarch::multicore::BooleanSemaphore  _semaphore;
"""

    def get_static_initialisations(self, full_qualified_classname):
        return (
            "double "
            + full_qualified_classname
            + "::_maxEigenvalue = 0.0;\n"
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
  tarch::multicore::Lock lock(_semaphore);
  _maxEigenvalue = std::max( fineGridCell{self._max_eigenvalue.name}.getMaxEigenvalue(0), _maxEigenvalue );
"""
        return result

    def get_guard_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
