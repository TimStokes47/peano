# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import jinja2

import peano4.api
from peano4.api.solversteps.ActionSet import ActionSet

from ..RefineErasePointwisely import RefineErasePointwisely


class DynamicMeshRefinement(ActionSet):
    """!

    Evaluate the point-wise refinement criterion at the end of a time step
    and collect the outcome as refine and erase requests

    The numerical schemes add this action set to the very last task of their
    time step. If the solver has an exahype2.api.RefineErasePointwisely PDE
    term, the action set loops over all volumes of each patch and invokes the
    term per volume centre. It initialises the term's output with Keep. From
    the outcomes of all the volumes, it derives one command for the whole
    cell:

    - If any volume says Refine, the cell asks for a refinement.
    - If all volumes say Erase, the cell asks to be erased, i.e. coarsened.
    - Otherwise, the cell is kept and does not issue any command.

    The solver's mesh adaptation (@ref exahype2.api.Solver.Solver.MeshAdaptation)
    can bound the mesh size. Refine commands are dropped for cells whose
    volumes are already smaller than its min_volume_size. Erase commands are
    dropped if the volumes of the coarsened cell would not be smaller than
    its max_volume_size. Without a mesh adaptation, or if a bound is None,
    the mesh size is not constrained.

    The action set collects the commands of one tree within a local
    ::exahype2::RefinementControl, merges it into the rank's
    ::exahype2::RefinementControlService at the end of the traversal, and
    commits the new events once per rank in unprepareTraversal(). It does not
    hand any event over to Peano, i.e. the time step itself never changes the
    mesh. The mesh adaptation phase consumes the requests (@ref
    exahype2::isMeshAdaptationRequired()).

    If the solver has no RefineErasePointwisely term, the action set does
    nothing.

    The action set works for all block-structured schemes which hold their
    unknowns as patch_size^d volumes (or grid cells) per mesh cell, i.e. for
    exahype2.api.fv and exahype2.api.rkfd4.

    """

    def __init__(self, solver, patch_size: int, guard: str, event_lifetime: int = 2):
        """!

        @param solver: exahype2.api.Solver
          The solver whose criterion is evaluated. Its mesh_adaptation, if
          there is one, provides the bounds on the mesh size.

        @param patch_size: int
          Number of volumes per axis within one patch.

        @param guard: str
          C++ boolean expression, evaluated per cell, guarding whether the
          criterion is evaluated for that cell.

        @param event_lifetime: int
          Number of mesh sweeps a refinement or erase event remains alive
          within the refinement service (@ref exahype2::RefinementControl::addCommand()).

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self._solver = solver
        self.patch_size = patch_size
        self.guard = guard
        self.event_lifetime = event_lifetime

        mesh_adaptation = solver.mesh_adaptation
        self.min_volume_size = (
            None if mesh_adaptation is None else mesh_adaptation.min_volume_size
        )
        self.max_volume_size = (
            None if mesh_adaptation is None else mesh_adaptation.max_volume_size
        )

    def has_criterion(self) -> bool:
        """!

        Does the solver have a RefineErasePointwisely term, i.e. is there
        anything to evaluate?

        """
        return any(
            isinstance(term, RefineErasePointwisely) for term in self._solver.pde_terms
        )

    TemplateEvaluateCriterion = """
  logTraceIn( "touchCellFirstTime(...)---DynamicMeshRefinement" );
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

  if ( anyVolumeWantsToRefine {{REFINE_BOUND}}) {
    _localRefinementControl.addCommand( marker.x(), marker.h(), ::exahype2::RefinementCommand::Refine, {{EVENT_LIFETIME}} );
  }
  else if ( not anyVolumeWantsToRefine and allVolumesWantToErase {{ERASE_BOUND}}) {
    _localRefinementControl.addCommand( marker.x(), marker.h(), ::exahype2::RefinementCommand::Erase, {{EVENT_LIFETIME}} );
  }
  logTraceOut( "touchCellFirstTime(...)---DynamicMeshRefinement" );
"""

    def get_body_of_operation(self, operation_name):
        if not self.has_criterion():
            return ""
        if operation_name == ActionSet.OPERATION_BEGIN_TRAVERSAL:
            return "  _localRefinementControl.clear();\n"
        if operation_name == ActionSet.OPERATION_END_TRAVERSAL:
            return "  ::exahype2::RefinementControlService::getInstance().merge( _localRefinementControl );\n"
        if operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            refine_bound = ""
            if self.min_volume_size is not None:
                min_cell_size = float(self.min_volume_size) * self.patch_size
                refine_bound = f"and tarch::la::max( marker.h() ) >= {min_cell_size!r} "
            erase_bound = ""
            if self.max_volume_size is not None:
                max_cell_size = float(self.max_volume_size) * self.patch_size
                erase_bound = (
                    f"and 3.0 * tarch::la::max( marker.h() ) < {max_cell_size!r} "
                )
            d = {
                "NUMBER_OF_VOLUMES_PER_AXIS": self.patch_size,
                "NUMBER_OF_UNKNOWNS": self._solver.unknowns,
                "NUMBER_OF_AUXILIARY_VARIABLES": self._solver.auxiliary_variables,
                "UNKNOWN_IDENTIFIER": self._solver._unknown_identifier(),
                "SOLVER_INSTANCE": self._solver.get_name_of_global_instance(),
                "REFINE_BOUND": refine_bound,
                "ERASE_BOUND": erase_bound,
                "EVENT_LIFETIME": self.event_lifetime,
            }
            return jinja2.Template(self.TemplateEvaluateCriterion).render(**d)
        return ""

    def get_guard_of_operation(self, operation_name):
        if operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.guard
        return "true"

    def get_body_of_unprepareTraversal(self):
        """!

        Commit the new events once per rank and mesh sweep, after all trees
        have merged their local events into the service

        """
        if not self.has_criterion():
            return "\n"
        return "  ::exahype2::RefinementControlService::getInstance().finishStep();\n"

    def get_attributes(self):
        if not self.has_criterion():
            return ""
        return """
    ::exahype2::RefinementControl         _localRefinementControl;
"""

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_includes(self):
        return (
            """
#include "exahype2/RefinementControl.h"
#include "exahype2/RefinementControlService.h"
#include "exahype2/fv/PatchUtils.h"
"""
            + self._solver._get_default_includes()
            + self._solver.user_solver_includes
        )
