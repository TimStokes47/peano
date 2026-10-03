# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class ComputeCFLConditionAndGlobalTimeStepSize(AbstractFVActionSet):
    """!

    Evaluate the CFL condition over solver._patch's volumes and feed the
    resulting admissible time step size into ::exahype2::Solver's own
    global time step bookkeeping - repositories::<solver instance>.
    updateCell(newTimeStepSize, newTimeStamp), immediately finalised via
    finishTimeStep()+startTimeStep() (@see get_body_of_unprepareTraversal()
    for why both run back-to-back there, with no lag across TimeStep
    phases). This is the base class every generated Abstract<SolverName>
    already inherits (@see exahype2::Solver, src/exahype2/Solver.h) -
    there is deliberately no separate, solver-level time stamp/time step
    size state introduced on top of it (RusanovWithExplicitEuler does not
    call exahype2.api.Solver.add_abstract_solver_attribute()/
    add_abstract_solver_method() for this).

    RusanovWithExplicitEuler only ever advances the whole domain by one,
    globally shared time step size (no local time stepping), so
    getMinTimeStamp()==getMaxTimeStamp() and
    getMinTimeStepSize()==getMaxTimeStepSize() always hold here - hence a
    single updateCell() call per time step (not one per cell/patch) is
    enough to keep both the min and the max in sync; there is no genuine
    spread across cells for the base class's min/max tracking to capture
    for this particular numerical scheme.

    @todo Not implemented yet: the actual per-volume eigenvalue evaluation
    in touchCellFirstTime() is still a placeholder
    (_maximumEigenvalue = std::max(1.0, _maximumEigenvalue)) - depends on
    exahype2.api.fv.actionsets.UpdateCellForGlobalTimeStepping's own @todo
    (the eigenvalues this class would evaluate come out of the kernel call
    there, which does not hand them on yet). The reduction machinery around that
    gap (the static _maximumEigenvalue/_semaphore accumulation, and
    unprepareTraversal()'s MPI reduction into a genuinely global
    newTimeStepSize, then advancing the solver's time stamp/time step size
    with it via updateCell()/startTimeStep()/finishTimeStep()) is already
    in place and correct.

    """

    def __init__(self, solver, cfl_constant, guard="true"):
        """!

        solver: exahype2.api.Solver
          Has to have solver._patch set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        cfl_constant: float
          Safety factor the admissible time step size is scaled down by,
          relative to the theoretical CFL=1 stability limit - @see
          exahype2.api.fv.RusanovWithExplicitEuler's own cfl_constant
          parameter, which this is meant to be passed straight through
          from.

        guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch contributes to the CFL
          condition/global time step size.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard
        self.cfl_constant = cfl_constant

    def get_body_of_prepareTraversal(self):
        """!

        prepareTraversal() is static - called once per rank, before any
        instance of this action set is created - so it can only ever touch
        static members, never a per-instance one. _maximumEigenvalue is
        static for exactly that reason: it is reset here, once, before
        every grid sweep, and then accumulated into by potentially many
        instances of this action set (one per tree, possibly running on
        different threads concurrently) as touchCellFirstTime() evaluates
        the CFL condition per cell - @see _semaphore, which guards that
        concurrent access.

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
        - _maximumEigenvalue is a plain double, already only a local,
          per-rank accumulation (@see _semaphore) - that one *is* reduced
          directly via tarch::mpi::Rank::allReduce(..., MPI_MAX, ...),
          exactly the way page_grid_statistics's "Reducing a value across
          ranks yourself" section documents for a single scalar.

        Once newTimeStepSize is known, it folds it into the solver's own
        time step bookkeeping (@see this class' docstring for why a single
        updateCell() call, not one per cell, is correct for this solver):

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
  const double                       globalMinH       = tarch::la::max( globalStatistics.getMinH() );

  // No semaphore required here, as it is only called once per rank
  double localMaximumEigenvalue = _maximumEigenvalue;
  double globalMaximumEigenvalue = localMaximumEigenvalue;
  #ifdef Parallel
  tarch::mpi::Rank::getInstance().allReduce(
    &localMaximumEigenvalue, &globalMaximumEigenvalue, 1, MPI_DOUBLE, MPI_MAX,
    [&]() -> void {{ tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }}
  );
  #endif

  const double newTimeStepSize = {self.cfl_constant} * globalMinH / globalMaximumEigenvalue;
  const double newTimeStamp    = repositories::{self._solver.get_name_of_global_instance()}.getMaxTimeStamp() + newTimeStepSize;

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
//  if ({self.guard}) {{
  if (marker.hasBeenRefined()) {{
    // @todo Evaluate the CFL condition over fineGridCell{self._solver._patch.name}
    //       and feed the resulting eigenvalue into _maximumEigenvalue below -
    //       not implemented yet, @see this class' docstring.
    
    // @todo Tim This is you
    tarch::multicore::Lock lock(_semaphore);
    _maximumEigenvalue = std::max(1.0, _maximumEigenvalue);
  }}
"""
        return result

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
