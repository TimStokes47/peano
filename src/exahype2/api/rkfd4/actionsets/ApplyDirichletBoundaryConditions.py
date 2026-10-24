# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet

import peano4.api
import peano4.api.taskgraph
import jinja2

from ...DirichletBoundaryConditions import DirichletBoundaryConditions
from ...BoundaryConditions import get_boundary_conditions


class ApplyDirichletBoundaryConditions(ActionSet):
    """!

    Set the ghost grid cells along the domain boundary through the solver's
    exahype2.api.DirichletBoundaryConditions term

    The action set plugs into touchFaceFirstTime() and delegates to
    exahype2::blockstructured::applyDirichletBoundaryConditions(), which
    holds the logic described below. Each face carries an
    overlap of 2*overlap grid cells along its normal
    (solver._patch_overlap_current_time_step), i.e. 2*HALO_SIZE for the
    fourth-order Finite Differences of @ref exahype2.api.rkfd4 :
    the left half belongs to the cell left of the face, the right half to
    the cell right of it. On the domain boundary, one of these two cells
    lies outside of the domain. Nobody ever writes its half of the face
    data. This action set fills exactly this half: For each of the
    outside's grid cells, it invokes

    @code
    repositories::<solver>.dirichletBoundaryConditions( x, t, Q );
    @endcode

    with x being the centre of the (ghost) grid cell and t the time stamp
    of the Runge-Kutta stage, i.e. the solver's current time stamp
    getMinTimeStamp() plus relative_time_step_size times its time step size
    getMinTimeStepSize(). Faces along periodic axes are
    skipped (PeriodicBC from the project's Constants.h). Whether a face is
    on the domain boundary is derived from its position, i.e. compared
    against DomainOffset and DomainOffset+DomainSize.

    The subsequent reconstruction
    (toolbox.blockstructured.api.ReconstructPatchWithHalo) reads the face
    data in touchCellFirstTime(). As Peano touches all faces of a cell for
    the first time before it enters the cell, the boundary task and the
    reconstruction can share one mesh traversal (@ref
    peano4.api.taskgraph.compiler.FuseSteps.translate()).

    Use add_task_to_time_step() to add the corresponding task to a time
    step's task graph.

    ## Why Dirichlet conditions are not part of the roll over

    exahype2.api.SommerfeldBoundaryConditions do without a boundary task:
    They advance the ghost cells within the roll over of the face data
    after the projection. Dirichlet conditions could be realised the same
    way, as the roll over could evaluate the boundary values at the time
    stamp of the next update. We deliberately keep them as a separate task
    which runs directly before the reconstruction, as this will not work
    anymore once we support local time stepping: With local time stepping,
    each cell advances with its own time step size, and the time stamp at
    which the ghost cells have to hold the boundary values is only known
    once the adjacent cell starts its next update, but not when the face
    data are rolled over after its previous update.

    The class mirrors
    exahype2.api.fv.actionsets.ApplyDirichletBoundaryConditions, as rkfd4
    does not depend on the Finite Volume package. Both share the C++
    routine. Different to the Finite Volume version, it evaluates the
    boundary values at the time stamp of the Runge-Kutta stage. With the time stamp of the time step instead, all stages would
    see the same boundary values, and the scheme would drop to first order
    along the boundary.

    """

    TemplateTouchFaceFirstTime = jinja2.Template(
        """
  ::exahype2::blockstructured::applyDirichletBoundaryConditions(
    [](const tarch::la::Vector<Dimensions,double>& x, double t, double* Q) -> void {
      repositories::{{SOLVER_INSTANCE}}.dirichletBoundaryConditions(x, t, Q);
    },
    marker,
    DomainOffset,
    DomainSize,
    PeriodicBC,
    {{NUMBER_OF_GRID_CELLS_PER_AXIS}},
    {{OVERLAP}},
    {{NUMBER_OF_UNKNOWNS}} + {{NUMBER_OF_AUXILIARY_VARIABLES}},
    repositories::{{SOLVER_INSTANCE}}.getMinTimeStamp() + {{RELATIVE_TIME_STEP_SIZE}} * repositories::{{SOLVER_INSTANCE}}.getMinTimeStepSize(),
    {{FACE_ACCESSOR}}.value
  );
"""
    )

    def __init__(self, solver, patch_overlap, guard, relative_time_step_size=0.0):
        """!

        @param solver: exahype2.api.Solver
          The solver whose dirichletBoundaryConditions() is invoked.

        @param patch_overlap: peano4.api.datamodel.Patch
          The face data to set, i.e. solver._patch_overlap_current_time_step.
          Its dim[0] is twice the overlap, its dim[1] the number of grid
          cells per axis of the cell patch.

        @param guard: str
          C++ boolean expression over a FaceMarker called marker. Typically
          solver.grid_selector.createFaceGuard().

        @param relative_time_step_size: float
          The @f$ c_k @f$ of the Runge-Kutta stage, i.e. the stage's time
          stamp relative to the time step size, @ref
          exahype2.api.rungekutta.ButcherTableau.ButcherTableau.relative_time_step_sizes().

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self._solver = solver
        self.guard = guard
        self.d = {
            "RELATIVE_TIME_STEP_SIZE": repr(float(relative_time_step_size)),
            "SOLVER_INSTANCE": solver.get_name_of_global_instance(),
            "NUMBER_OF_GRID_CELLS_PER_AXIS": str(patch_overlap.dim[1]),
            "OVERLAP": str(int(patch_overlap.dim[0] / 2)),
            "NUMBER_OF_UNKNOWNS": str(solver.unknowns),
            "NUMBER_OF_AUXILIARY_VARIABLES": str(solver.auxiliary_variables),
            "FACE_ACCESSOR": "fineGridFace" + patch_overlap.name,
        }

    def get_body_of_operation(self, operation_name):
        if (
            operation_name
            == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME
        ):
            return self.TemplateTouchFaceFirstTime.render(**self.d)
        return ""

    def get_guard_of_operation(self, operation_name):
        if (
            operation_name
            == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME
        ):
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_includes(self):
        return (
            """
#include "exahype2/blockstructured/DirichletBoundaryConditions.h"
"""
            + self._solver._get_default_includes()
            + self._solver.user_solver_includes
        )

    @staticmethod
    def add_task_to_time_step(
        solver,
        exahype2_project,
        time_step,
        reconstruction_task,
        name_suffix="",
        relative_time_step_size=0.0,
    ):
        """!

        Add the boundary task in front of the reconstruction of the halo

        If all axes of the domain are periodic, or if the solver uses
        exahype2.api.SommerfeldBoundaryConditions (which replace the roll
        over instead), the routine does nothing. Otherwise, it adds a FACE
        task which sets the ghost grid
        cells of solver._patch_overlap_current_time_step, and makes
        reconstruction_task depend on it.

        @param solver: exahype2.api.Solver
          The solver. Its boundary condition terms have to be consistent
          with the domain, @ref
          exahype2.api.BoundaryConditions.get_boundary_conditions().

        @param exahype2_project: exahype2.api.Project
          Provides the periodic boundary conditions.

        @param time_step: peano4.api.taskgraph.Graph
          The time step's task graph.

        @param reconstruction_task: peano4.api.taskgraph.Task
          The task reading solver._patch_overlap_current_time_step to
          reconstruct the patches' halos.

        @param name_suffix: str
          Appended to the task's name. A time step with several Runge-Kutta
          stages adds one boundary task per stage, and the names of the
          tasks have to differ.

        @param relative_time_step_size: float
          The @f$ c_k @f$ of the Runge-Kutta stage, see the constructor.

        @return peano4.api.taskgraph.Task or None
          The boundary task, or None if no Dirichlet task is required.

        """
        if not isinstance(
            get_boundary_conditions(solver, exahype2_project),
            DirichletBoundaryConditions,
        ):
            return None

        boundary_task = peano4.api.taskgraph.Task(
            name=f"{solver.name}_Apply Dirichlet BCs{name_suffix}",
            entity=peano4.api.taskgraph.Task.GridEntity.FACE,
        )
        boundary_task.use_face(
            solver._patch_overlap_current_time_step,
            peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE,
        )
        boundary_task.add_action_set(
            ApplyDirichletBoundaryConditions(
                solver,
                solver._patch_overlap_current_time_step,
                guard=solver.grid_selector.createFaceGuard(),
                relative_time_step_size=relative_time_step_size,
            )
        )
        time_step.add_task(boundary_task)
        reconstruction_task.add_dependency(boundary_task)
        return boundary_task
