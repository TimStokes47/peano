# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..blockstructured.AdaptMeshAndBalanceLoad import (
    AdaptMeshAndBalanceLoad as BlockStructuredAdaptMeshAndBalanceLoad,
)
from .actionsets import DiscardPatchInterior
from .actionsets import ProjectPatchOntoFaces
from .actionsets import RollOverUpdatedFaceWithSommerfeldBoundaryConditions


class AdaptMeshAndBalanceLoad(BlockStructuredAdaptMeshAndBalanceLoad):
    """!

    Default mesh adaptation of the Finite Volume schemes

    exahype2.api.fv.RusanovWithExplicitEuler and
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler use this mesh
    adaptation unless the user assigns another one to solver.mesh_adaptation.
    @see exahype2.api.blockstructured.AdaptMeshAndBalanceLoad for the tasks.
    On top of these, the Finite Volumes

    - wait for and drop the optimistic patch interior updates which the last
      time step has spawned
      (exahype2.api.fv.actionsets.DiscardPatchInterior), as they belong to
      the old mesh, and
    - use their own projection onto the faces.

    """

    def _add_further_adaptation_action_sets(self, adapt_task):
        ## Only the optimistic time stepping spawns tasks which update the
        ## patch interior, and only if it holds their task numbers per cell
        task_number = getattr(self._solver, "_optimistic_cell_update_task_number", None)
        if task_number is not None:
            adapt_task.use_cell(
                task_number, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE
            )
            adapt_task.add_action_set(
                DiscardPatchInterior(
                    self._solver,
                    guard=self._solver.grid_selector.createCellGuard(),
                    task_number=task_number,
                )
            )

    def _add_projection_action_sets(self, project_on_faces_task, exahype2_project):
        includes = (
            self._solver._get_default_includes() + self._solver.user_solver_includes
        )
        project_on_faces_task.add_action_set(
            ProjectPatchOntoFaces(
                self._solver._patch,
                self._solver._patch_overlap_update,
                guard=self._solver.grid_selector.createCellGuard(),
                includes=includes,
            )
        )
        ## Only numerical schemes which support adaptive meshes offer the
        ## restriction of the hanging faces
        if hasattr(
            self._solver.numerical_scheme, "create_restriction_of_hanging_faces"
        ):
            project_on_faces_task.add_action_set(
                self._solver.numerical_scheme.create_restriction_of_hanging_faces()
            )
        project_on_faces_task.add_action_set(
            RollOverUpdatedFaceWithSommerfeldBoundaryConditions.create_roll_over(
                self._solver,
                exahype2_project,
                guard=self._solver.grid_selector.createFaceGuard(),
                includes=includes,
                time_step_size="0.0",
            )
        )
