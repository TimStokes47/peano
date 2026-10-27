# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import jinja2
import peano4.api

from .RollOverUpdatedFace import RollOverUpdatedFace
from ...blockstructured.BoundaryConditions import get_boundary_conditions
from ...blockstructured.SommerfeldBoundaryConditions import SommerfeldBoundaryConditions


class RollOverUpdatedFaceWithSommerfeldBoundaryConditions(RollOverUpdatedFace):
    """!

    Roll over the face data, and set the ghost volumes along the domain
    boundary through exahype2.api.SommerfeldBoundaryConditions first

    The action set replaces exahype2.api.fv.actionsets.RollOverUpdatedFace
    if the solver uses Sommerfeld boundary conditions. Such a solver has no
    separate boundary task (exahype2.api.fv.actionsets.ApplyDirichletBoundaryConditions).
    In touchFaceLastTime(), the action set first writes the ghost volumes
    into the ghost half of patch_overlap_update, and then rolls over both
    halves into patch_overlap_current_time_step like RollOverUpdatedFace.
    Faces within the domain and faces along periodic axes are rolled over
    unchanged. There are two variants:

    - With a time_step_size, the action set advances the ghost volumes
      through @ref exahype2::blockstructured::applySommerfeldBoundaryConditions().
      patch_overlap_current_time_step still holds the old solution, i.e.
      both its halves are valid, and patch_overlap_update holds the new
      interior solution. The time step uses this variant.
    - Without a time_step_size, the action set initialises the ghost volumes
      with the initial condition, i.e. it evaluates the solver's
      initPointWisely() at their centres through
      @ref exahype2::blockstructured::applyDirichletBoundaryConditions().
      The grid construction uses this variant, so the first time step finds
      valid ghost volumes.

    The C++ header documents the conditions, their discretisation and why
    they replace the roll over.

    """

    TemplateSommerfeldBoundaryConditions = jinja2.Template(
        """
    ::exahype2::blockstructured::applySommerfeldBoundaryConditions(
      [](
        const double*                                Q,
        const tarch::la::Vector<Dimensions, double>& x,
        int                                          normal
      ) -> double {
        double lambdaMax;
        repositories::{{SOLVER_INSTANCE}}.maxEigenvalue(Q, x, normal, &lambdaMax);
        return lambdaMax;
      },
      [](const tarch::la::Vector<Dimensions, double>& x, double* Q) -> void {
        repositories::{{SOLVER_INSTANCE}}.sommerfeldFarFieldSolution(x, Q);
      },
      marker,
      DomainOffset,
      DomainSize,
      PeriodicBC,
      {{DOFS_PER_AXIS}},
      {{OVERLAP}},
      {{NUMBER_OF_UNKNOWNS}},
      {{NUMBER_OF_AUXILIARY_VARIABLES}},
      tarch::la::Vector<Dimensions, double>({{ORIGIN}}),
      {{TIME_STEP_SIZE}},
      {{CURRENT_TIME_STEP_ACCESSOR}}.value,
      {{UPDATE_ACCESSOR}}.value
    );
"""
    )

    TemplateInitialGhostCells = jinja2.Template(
        """
    ::exahype2::blockstructured::applyDirichletBoundaryConditions(
      [](const tarch::la::Vector<Dimensions, double>& x, double t, double* Q) -> void {
        repositories::{{SOLVER_INSTANCE}}.initPointWisely(x, Q);
      },
      marker,
      DomainOffset,
      DomainSize,
      PeriodicBC,
      {{DOFS_PER_AXIS}},
      {{OVERLAP}},
      {{NUMBER_OF_UNKNOWNS}} + {{NUMBER_OF_AUXILIARY_VARIABLES}},
      repositories::{{SOLVER_INSTANCE}}.getMinTimeStamp(),
      {{UPDATE_ACCESSOR}}.value
    );
"""
    )

    def __init__(
        self,
        solver,
        patch_overlap_current_time_step,
        patch_overlap_update,
        guard,
        includes,
        origin,
        time_step_size=None,
    ):
        """!

        @param solver: exahype2.api.Solver
          The solver whose maxEigenvalue(), sommerfeldFarFieldSolution() and
          initPointWisely() are invoked.

        @param patch_overlap_current_time_step: peano4.api.datamodel.Patch
          @see RollOverUpdatedFace.

        @param patch_overlap_update: peano4.api.datamodel.Patch
          @see RollOverUpdatedFace.

        @param guard: str
          @see RollOverUpdatedFace.

        @param includes: str
          @see RollOverUpdatedFace.

        @param origin: List[float]
          Centre of the system, one entry per dimension, @ref
          exahype2.api.blockstructured.SommerfeldBoundaryConditions.SommerfeldBoundaryConditions.get_origin().

        @param time_step_size: str
          C++ expression for the time step size by which the ghost volumes
          are advanced, or None to initialise the ghost volumes with the
          initial condition instead.

        """
        super().__init__(
            patch_overlap_current_time_step, patch_overlap_update, guard, includes
        )
        self._time_step_size = time_step_size
        self.d["SOLVER_INSTANCE"] = solver.get_name_of_global_instance()
        self.d["NUMBER_OF_UNKNOWNS"] = str(solver.unknowns)
        self.d["NUMBER_OF_AUXILIARY_VARIABLES"] = str(solver.auxiliary_variables)
        self.d["ORIGIN"] = "{" + ", ".join(repr(float(x)) for x in origin) + "}"
        self.d["TIME_STEP_SIZE"] = time_step_size

    def get_body_of_operation(self, operation_name):
        result = super().get_body_of_operation(operation_name)
        if (
            operation_name
            == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_LAST_TIME
        ):
            template = (
                self.TemplateInitialGhostCells
                if self._time_step_size is None
                else self.TemplateSommerfeldBoundaryConditions
            )
            result = template.render(**self.d) + result
        return result

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def get_includes(self):
        return (
            """
#include "exahype2/blockstructured/DirichletBoundaryConditions.h"
#include "exahype2/blockstructured/SommerfeldBoundaryConditions.h"
"""
            + super().get_includes()
        )

    @staticmethod
    def create_roll_over(
        solver,
        exahype2_project,
        guard,
        includes,
        time_step_size=None,
    ):
        """!

        Create the roll over of the solver's face data

        @return RollOverUpdatedFace
          RollOverUpdatedFaceWithSommerfeldBoundaryConditions if the solver
          uses exahype2.api.SommerfeldBoundaryConditions, and a plain
          RollOverUpdatedFace otherwise, i.e. for Dirichlet boundary
          conditions or a domain without boundary.

        @param solver: exahype2.api.Solver
          The solver whose solver._patch_overlap_update is rolled over into
          solver._patch_overlap_current_time_step. Its boundary condition
          terms have to be consistent with the domain, @ref
          exahype2.api.blockstructured.BoundaryConditions.get_boundary_conditions().

        @param exahype2_project: exahype2.api.Project
          Provides the periodic boundary conditions.

        @param guard: str
          @see RollOverUpdatedFace.

        @param includes: str
          @see RollOverUpdatedFace.

        @param time_step_size: str
          @see the constructor. Ignored for a plain RollOverUpdatedFace.

        """
        term = get_boundary_conditions(solver, exahype2_project)
        if isinstance(term, SommerfeldBoundaryConditions):
            return RollOverUpdatedFaceWithSommerfeldBoundaryConditions(
                solver,
                solver._patch_overlap_current_time_step,
                solver._patch_overlap_update,
                guard=guard,
                includes=includes,
                origin=term.get_origin(len(exahype2_project.periodic_BC)),
                time_step_size=time_step_size,
            )
        return RollOverUpdatedFace(
            solver._patch_overlap_current_time_step,
            solver._patch_overlap_update,
            guard=guard,
            includes=includes,
        )
