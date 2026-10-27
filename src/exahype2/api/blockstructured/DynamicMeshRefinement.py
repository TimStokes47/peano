# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import jinja2

import peano4.api
from peano4.api.solversteps.ActionSet import ActionSet

class DynamicMeshRefinement(ActionSet):
    """!

    Evaluate the point-wise refinement criterion and collect the outcome as
    refine and erase requests

    The numerical schemes add this action set to the very last task of their
    time step if the solver has an exahype2.api.RefineErasePointwisely PDE
    term (@ref exahype2.api.blockstructured.create_dynamic_mesh_refinement_action_set.create_dynamic_mesh_refinement_action_set()). The action set
    loops over all volumes of each patch and invokes the term per volume
    centre. It initialises the term's output with Keep. From the outcomes of
    all the volumes, it derives one command for the whole cell:

    - If any volume says Refine, the cell asks for a refinement.
    - If all volumes say Erase, the cell asks to be erased, i.e. coarsened.
    - Otherwise, the cell is kept and does not issue any command.

    The action set bounds the mesh size. Refine commands are dropped for
    cells whose volumes are already smaller than min_volume_size. Erase
    commands are dropped if the volumes of the coarsened cell would not be
    smaller than max_volume_size. If a bound is None, the mesh size is not
    constrained. As the action set drops these commands, it does not issue
    requests which the mesh cannot realise anyway, and the mesh adaptation
    phase is only triggered if the mesh actually can change. The numerical
    schemes take the two bounds as constructor arguments and pass them
    through.

    Peano erases the children of a cell only if they all ask for it. Erase
    commands therefore carry the cell's position within its parent, and the
    refinement service keeps them only if all 3^d children of a parent have
    asked to be erased within the same mesh sweep
    (@ref exahype2::RefinementControlService::keepOnlyCompleteSiblingEraseEvents()).
    Incomplete sets of erase requests are dropped right away, so they never
    trigger the mesh adaptation phase.

    The action set collects the commands of one tree within a local
    ::exahype2::RefinementControl, merges it into the rank's
    ::exahype2::RefinementControlService at the end of the traversal, and
    commits the new events once per rank in unprepareTraversal(). By
    default, it does not hand any event over to Peano, i.e. the time step
    itself never changes the mesh. The mesh adaptation phase consumes the
    requests (@ref exahype2::isMeshAdaptationRequired()). The setup's mesh
    adaptation (@ref exahype2.api.fv.SetupSimulationWithUserCodedRefinementCriterion)
    instead lets the action set hand the events over to Peano right away
    (hand_events_to_peano).

    Subclasses which evaluate a different adaptivity criterion only replace
    the per-volume evaluation (@ref exahype2.api.blockstructured.DynamicMeshRefinement.DynamicMeshRefinement.get_volume_evaluation()), e.g.
    exahype2.api.blockstructured.DynamicMeshRefinementWithGradients.

    The action set works for all block-structured schemes which hold their
    unknowns as patch_size^d volumes (or grid cells) per mesh cell, i.e. for
    exahype2.api.fv and exahype2.api.rkfd4.

    """

    def __init__(
        self,
        solver,
        patch_size: int,
        guard: str,
        min_volume_size: float = None,
        max_volume_size: float = None,
        event_lifetime: int = 2,
        hand_events_to_peano: bool = False,
    ):
        """!

        @param solver: exahype2.api.Solver
          The solver whose criterion is evaluated.

        @param patch_size: int
          Number of volumes per axis within one patch.

        @param guard: str
          C++ boolean expression, evaluated per cell, guarding whether the
          criterion is evaluated for that cell.

        @param min_volume_size: float
          A cell asks for a refinement only if its volumes are at least of
          this size. None does not bound the refinement.

        @param max_volume_size: float
          A cell asks to be erased only if the volumes of the coarser cell
          are strictly smaller than this size. None does not bound the
          coarsening.

        @param event_lifetime: int
          Number of mesh sweeps a refinement or erase event remains alive
          within the refinement service (@ref exahype2::RefinementControl::addCommand()).

        @param hand_events_to_peano: bool
          Hand the refinement service's committed events over to Peano, so
          Peano realises them in the subsequent mesh sweeps. The time step
          leaves this off, the setup's mesh adaptation switches it on.

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self._solver = solver
        self.patch_size = patch_size
        self.guard = guard
        self.event_lifetime = event_lifetime
        self.min_volume_size = min_volume_size
        self.max_volume_size = max_volume_size
        self.hand_events_to_peano = hand_events_to_peano

    ## Per-volume evaluation of the point-wise criterion
    TemplateEvaluateVolume = """
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
"""

    def get_volume_evaluation(self) -> str:
        """!

        C++ Jinja2 template evaluating the criterion within one volume

        The template runs within a loop over the volumes of the patch, i.e.
        it has access to the volume's index volume. It has to update the
        two flags anyVolumeWantsToRefine and allVolumesWantToErase.

        """
        return self.TemplateEvaluateVolume

    TemplateEvaluateCriterion = """
  logTraceIn( "touchCellFirstTime(...)---DynamicMeshRefinement" );
  bool anyVolumeWantsToRefine = false;
  bool allVolumesWantToErase  = true;
  dfor( volume, {{NUMBER_OF_VOLUMES_PER_AXIS}} ) {
{{EVALUATE_VOLUME}}
  }

  if ( anyVolumeWantsToRefine {{REFINE_BOUND}}) {
    _localRefinementControl.addCommand( marker.x(), marker.h(), ::exahype2::RefinementCommand::Refine, {{EVENT_LIFETIME}} );
  }
  else if ( not anyVolumeWantsToRefine and allVolumesWantToErase {{ERASE_BOUND}}) {
    _localRefinementControl.addCommand( marker.x(), marker.h(), ::exahype2::RefinementCommand::Erase, {{EVENT_LIFETIME}}, marker.getRelativePositionWithinFatherCell() );
  }
  logTraceOut( "touchCellFirstTime(...)---DynamicMeshRefinement" );
"""

    def get_body_of_operation(self, operation_name):
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
            template = self.TemplateEvaluateCriterion.replace(
                "{{EVALUATE_VOLUME}}", self.get_volume_evaluation()
            )
            return jinja2.Template(template).render(**d)
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
        return "  ::exahype2::RefinementControlService::getInstance().finishStep();\n"

    def get_body_of_createGridControlEventsForNextTraversal(self):
        if not self.hand_events_to_peano:
            return super().get_body_of_createGridControlEventsForNextTraversal()
        return "  return ::exahype2::RefinementControlService::getInstance().createGridControlEventsForNextTraversal();\n"

    def get_attributes(self):
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
