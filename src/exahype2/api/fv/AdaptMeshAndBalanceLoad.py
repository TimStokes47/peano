# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import toolbox.blockstructured.api
import toolbox.loadbalancing.api.actionsets

from ..Solver import Solver
from ..blockstructured.BoundaryConditions import get_access_to_persistent_face_data
from ..blockstructured.ConsumeRefinementRequests import ConsumeRefinementRequests
from .actionsets import DiscardPatchInterior
from .actionsets import ProjectPatchOntoFaces
from .actionsets import RollOverUpdatedFaceWithSommerfeldBoundaryConditions


class AdaptMeshAndBalanceLoad(Solver.MeshAdaptation):
    """!

    Default mesh adaptation of the Finite Volume schemes

    exahype2.api.fv.RusanovWithExplicitEuler and
    exahype2.api.fv.RusanovWithOptimisticExplicitEuler use this mesh
    adaptation unless the user assigns another one to solver.mesh_adaptation.
    The generated main runs the phase right after a time step if this time
    step has asked for a refinement or erase, or if the load balancing has
    split (@ref exahype2::isMeshAdaptationRequired()). The phase consists of
    two tasks:

    1. Adapt mesh and balance load repeats until the mesh is stationary, the
       refinement service holds no requests anymore, and the load balancing
       has not split recently. It hosts the following action sets:
       - For optimistic time stepping, exahype2.api.fv.actionsets.DiscardPatchInterior
         waits for the optimistic patch interior updates which the last time
         step has spawned and drops them, as they belong to the old mesh.
       - exahype2.api.blockstructured.ConsumeRefinementRequests hands the
         requests of the last time step over to Peano. It does not issue any
         new request. If the mesh changes, it resets the solver's time step
         size to zero, so the next time step derives a new admissible time
         step size.
       - toolbox.blockstructured.api.InterpolateAndRestrictCells
         interpolates the patches of new cells from their parent and
         restricts the patches of erased cells into their parent.
       - toolbox.loadbalancing.api.actionsets.LoadBalancing informs the load
         balancing after each mesh sweep. The last task of the time step has
         switched it on.
    2. Project on faces projects the patches onto the faces, as the new
       mesh has new faces. It is the same projection as at the end of a time
       step, including the restriction of the hanging faces if the scheme
       supports adaptive meshes. Sommerfeld boundary conditions advance the
       ghost volumes by a time step size of zero, i.e. they keep them. Ghost
       volumes of faces along the boundary which the refinement has created
       are hence not set properly.

    """

    ## Repeat condition of the task which adapts the mesh and balances the load
    RepeatCondition = (
        "not peano4::grid::isGridStationary( peano4::parallel::SpacetreeSet::getInstance().getGridStatistics() )"
        " or not ::exahype2::RefinementControlService::getInstance().createGridControlEventsForNextTraversal().empty()"
        " or ( ::toolbox::loadbalancing::LoadBalancingService::getInstance().hasLoadBalancing()"
        " and ::toolbox::loadbalancing::LoadBalancingService::getInstance().getLoadBalancing()->hasSplitRecently() )"
    )

    def __init__(
        self,
        solver: Solver,
        interpolation: "toolbox.blockstructured.api.InterpolateAndRestrictCells.Interpolation" = toolbox.blockstructured.api.InterpolateAndRestrictCells.Interpolation.Linear,
        restriction: "toolbox.blockstructured.api.InterpolateAndRestrictCells.Restriction" = toolbox.blockstructured.api.InterpolateAndRestrictCells.Restriction.Averaging,
    ):
        """!

        @param solver: exahype2.api.Solver
          Solver with a Finite Volume scheme.

        @param interpolation: toolbox.blockstructured.api.InterpolateAndRestrictCells.Interpolation
          How the patch of a new cell is initialised from its parent. The
          default is d-linear interpolation.

        @param restriction: toolbox.blockstructured.api.InterpolateAndRestrictCells.Restriction
          How the patches of erased cells are handed over to their parent.
          The default is averaging.

        """
        super().__init__(solver)
        self.interpolation = interpolation
        self.restriction = restriction

    def construct_output(self, exahype2_project, peano_project, mesh_adaptation):
        """!

        Add the two tasks of the mesh adaptation to mesh_adaptation

        """
        peano4.api.Logger().log(
            "Create mesh adaptation and load balancing task graph for Finite Volume scheme"
        )
        Access = peano4.api.taskgraph.Task.DataAccess
        includes = (
            self._solver._get_default_includes() + self._solver.user_solver_includes
        )
        cell_guard = self._solver.grid_selector.createCellGuard()

        adapt_task = peano4.api.taskgraph.Task(
            name=f"{self._solver.name}_Adapt mesh and balance load",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
            task_has_to_repeat=self.RepeatCondition,
            may_spawn_background_tasks=False,
        )
        adapt_task.use_cell(self._solver._patch, Access.READ_AND_WRITE)
        ## Only the optimistic time stepping spawns tasks which update the
        ## patch interior, and only if it holds their task numbers per cell
        task_number = getattr(self._solver, "_optimistic_cell_update_task_number", None)
        if task_number is not None:
            adapt_task.use_cell(task_number, Access.READ_AND_WRITE)
            adapt_task.add_action_set(
                DiscardPatchInterior(
                    self._solver, guard=cell_guard, task_number=task_number
                )
            )
        adapt_task.add_action_set(ConsumeRefinementRequests(self._solver))
        adapt_task.add_action_set(
            toolbox.blockstructured.api.InterpolateAndRestrictCells(
                self._solver._patch,
                interpolation=self.interpolation,
                restriction=self.restriction,
                additional_includes=includes,
            )
        )
        adapt_task.add_action_set(toolbox.loadbalancing.api.actionsets.LoadBalancing())

        project_on_faces_task = peano4.api.taskgraph.Task(
            name=f"{self._solver.name}_Project on faces",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
        )
        project_on_faces_task.add_dependency(adapt_task)
        project_on_faces_task.use_cell(self._solver._patch, Access.READ_AND_WRITE)
        project_on_faces_task.use_face(
            self._solver._patch_overlap_update, Access.TEMPORARY
        )
        project_on_faces_task.use_face(
            self._solver._patch_overlap_current_time_step,
            get_access_to_persistent_face_data(
                self._solver, exahype2_project, Access.WRITE
            ),
        )
        project_on_faces_task.add_action_set(
            ProjectPatchOntoFaces(
                self._solver._patch,
                self._solver._patch_overlap_update,
                guard=cell_guard,
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

        mesh_adaptation.add_task(adapt_task)
        mesh_adaptation.add_task(project_on_faces_task)

        if peano4.api.Logger().is_visualisation:
            mesh_adaptation.visualise(
                title=f"Mesh adaptation task graph for {self._solver.name}"
            )
        if peano4.api.Logger().is_text:
            mesh_adaptation.log_as_text(
                title=f"Mesh adaptation task graph for {self._solver.name}"
            )
