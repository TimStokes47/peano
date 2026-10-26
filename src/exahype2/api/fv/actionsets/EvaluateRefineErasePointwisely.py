# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api
import jinja2


class EvaluateRefineErasePointwisely(AbstractFVActionSet):
    """!

    Evaluate the point-wise refinement criterion and feed ExaHyPE's
    refinement service

    The action set loops over all volumes of a patch and invokes the
    solver's exahype2.api.RefineErasePointwisely term per volume centre. It
    initialises the term's output with Keep. From the outcomes of all the
    volumes, it derives one command for the whole cell:

    - If any volume says Refine, the cell asks for a refinement.
    - If all volumes say Erase, the cell asks to be erased, i.e. coarsened.
    - Otherwise, the cell is kept and does not issue any command.

    Refine commands are dropped for cells whose volumes are already smaller
    than the minimum volume size. Erase commands are dropped if the
    coarsened cell would be coarser than the cells of the initial regular
    mesh (@ref exahype2.api.fv.SetupSimulationWithUserCodedRefinementCriterion).
    Erase commands carry the cell's position within its parent, so the
    refinement service keeps them only if all 3^d children of the parent ask
    to be erased (@ref exahype2::RefinementControlService::keepOnlyCompleteSiblingEraseEvents()).

    The action set collects the commands of one tree within a local
    ::exahype2::RefinementControl and merges it into the rank's
    ::exahype2::RefinementControlService at the end of the traversal. After
    the traversal, unprepareTraversal() commits these new events
    (finishStep()). createGridControlEventsForNextTraversal() hands out the
    service's committed events, so Peano realises them in the subsequent
    mesh sweeps. Consult exahype2::RefinementControl for this multistep
    process.

    """

    def __init__(
        self,
        solver,
        guard: str,
        min_cell_size: float,
        max_cell_size: float,
        event_lifetime: int = 2,
    ):
        """!

        @param solver: exahype2.api.Solver
          Has to have an exahype2.api.RefineErasePointwisely PDE term.

        @param guard: str
          C++ boolean expression, evaluated per cell, guarding whether the
          criterion is evaluated for that cell.

        @param min_cell_size: float
          Cells with an edge length smaller than this value are not refined
          any further.

        @param max_cell_size: float
          Cells are erased only if the resulting coarser cell is smaller than
          this value.

        @param event_lifetime: int
          Number of mesh sweeps a refinement or erase event remains alive
          within the refinement service (@ref exahype2::RefinementControl::addCommand()).

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard
        self.min_cell_size = min_cell_size
        self.max_cell_size = max_cell_size
        self.event_lifetime = event_lifetime

    TemplateEvaluateCriterion = """
  logTraceIn( "touchCellFirstTime(...)---EvaluateRefineErasePointwisely" );
  bool anyVolumeWantsToRefine = false;
  bool allVolumesWantToErase  = true;
  dfor( volume, {{NUMBER_OF_VOLUMES_PER_AXIS}} ) {
    const int index = peano4::utils::dLinearised(volume,{{NUMBER_OF_VOLUMES_PER_AXIS}}) * ({{NUMBER_OF_UNKNOWNS}} + {{NUMBER_OF_AUXILIARY_VARIABLES}});
    ::exahype2::RefinementCommand command = ::exahype2::RefinementCommand::Keep;
    repositories::{{SOLVER_INSTANCE}}.refineErasePointwisely(
      fineGridCell{{UNKNOWN_IDENTIFIER}}.value + index,
      ::exahype2::fv::getVolumeCentre( marker.x(), marker.h(), {{NUMBER_OF_VOLUMES_PER_AXIS}}, volume),
      repositories::{{SOLVER_INSTANCE}}.getMinTimeStamp(),
      &command
    );
    anyVolumeWantsToRefine = anyVolumeWantsToRefine or command == ::exahype2::RefinementCommand::Refine;
    allVolumesWantToErase  = allVolumesWantToErase  and command == ::exahype2::RefinementCommand::Erase;
  }

  if ( anyVolumeWantsToRefine and tarch::la::max( marker.h() ) >= {{MIN_CELL_SIZE}} ) {
    _localRefinementControl.addCommand( marker.x(), marker.h(), ::exahype2::RefinementCommand::Refine, {{EVENT_LIFETIME}} );
  }
  else if ( not anyVolumeWantsToRefine and allVolumesWantToErase and 3.0 * tarch::la::max( marker.h() ) < {{MAX_CELL_SIZE}} ) {
    _localRefinementControl.addCommand( marker.x(), marker.h(), ::exahype2::RefinementCommand::Erase, {{EVENT_LIFETIME}}, marker.getRelativePositionWithinFatherCell() );
  }
  logTraceOut( "touchCellFirstTime(...)---EvaluateRefineErasePointwisely" );
"""

    def get_body_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_BEGIN_TRAVERSAL:
            return "  _localRefinementControl.clear();\n"
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_END_TRAVERSAL:
            return "  ::exahype2::RefinementControlService::getInstance().merge( _localRefinementControl );\n"
        if (
            operation_name
            == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
        ):
            d = {
                "NUMBER_OF_VOLUMES_PER_AXIS": self._solver.numerical_scheme.patch_size,
                "NUMBER_OF_UNKNOWNS": self._solver.unknowns,
                "NUMBER_OF_AUXILIARY_VARIABLES": self._solver.auxiliary_variables,
                "UNKNOWN_IDENTIFIER": self._solver._unknown_identifier(),
                "SOLVER_INSTANCE": self._solver.get_name_of_global_instance(),
                "MIN_CELL_SIZE": repr(float(self.min_cell_size)),
                "MAX_CELL_SIZE": repr(float(self.max_cell_size)),
                "EVENT_LIFETIME": self.event_lifetime,
            }
            return jinja2.Template(self.TemplateEvaluateCriterion).render(**d)
        return ""

    def get_guard_of_operation(self, operation_name):
        if (
            operation_name
            == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
        ):
            return self.guard
        return "true"

    def get_body_of_unprepareTraversal(self):
        """!

        Commit the new events once per rank and mesh sweep, after all trees
        have merged their local events into the service

        """
        return "  ::exahype2::RefinementControlService::getInstance().finishStep();\n"

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return "  return ::exahype2::RefinementControlService::getInstance().createGridControlEventsForNextTraversal();\n"

    def get_attributes(self):
        return """
    ::exahype2::RefinementControl         _localRefinementControl;
"""

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def get_includes(self):
        return (
            super().get_includes()
            + """
#include "exahype2/RefinementControl.h"
#include "exahype2/RefinementControlService.h"
#include "peano4/utils/Loop.h"
"""
        )
