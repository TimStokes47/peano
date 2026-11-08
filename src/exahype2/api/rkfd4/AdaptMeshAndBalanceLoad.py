# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ..blockstructured.AdaptMeshAndBalanceLoad import (
    AdaptMeshAndBalanceLoad as BlockStructuredAdaptMeshAndBalanceLoad,
)
from .actionsets import ProjectPatchOntoFaces
from .actionsets import RollOverUpdatedFaceWithSommerfeldBoundaryConditions


class AdaptMeshAndBalanceLoad(BlockStructuredAdaptMeshAndBalanceLoad):
    """!

    Default mesh adaptation of the fourth-order finite differences

    exahype2.api.rkfd4.CentralDifferencesWithRungeKutta uses this mesh
    adaptation unless the user assigns another one to solver.mesh_adaptation.
    @see exahype2.api.blockstructured.AdaptMeshAndBalanceLoad for the tasks.
    The finite differences only plug in their own projection onto the faces,
    including the restriction of the hanging faces. The interpolation and
    restriction of the patches treat the grid cells of a patch like Finite
    Volumes, i.e. the scheme drops to low order wherever the mesh changes.

    """

    def _add_further_adaptation_action_sets(self, adapt_task):
        """!

        Keep the right-hand side estimates in sync with the mesh

        solver._rhs_estimates lives on the cell stack between the sweeps of
        the time step. A sweep only creates and destroys the entries of the
        cell data that it uses. The mesh adaptation creates and destroys
        cells, so it has to use the estimates, too. Otherwise, the cells it
        creates have no entry on the stack, and the next time step pops from
        a stack which is too short.

        @param adapt_task: peano4.api.taskgraph.Task
          The task which adapts the mesh.

        """
        adapt_task.use_cell(
            self._solver._rhs_estimates,
            peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE,
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
