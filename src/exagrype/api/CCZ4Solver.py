""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import enum
from typing import Dict, List, Union

import exahype2.api
import peano4.api
import peano4.api.taskgraph
import toolbox.blockstructured.api

from . import initialvalues


class CCZ4Solver(exahype2.api.Solver):
    """!

    Solver for the Einstein equations in the first-order CCZ4 formulation

    The solver is a plain exahype2.api.Solver with the 59 CCZ4 unknowns and
    the PDE terms of CCZ4 pre-registered. All terms delegate to the
    ExaGRyPE library (configure Peano with --enable-exagrype):

    - the non-conservative product to exagrype::ccz4::ncp(),
    - the volumetric source to exagrype::ccz4::source(),
    - the maximum eigenvalue to exagrype::ccz4::maxEigenvalue(),
    - the initial condition to one of the routines from
      exagrype::initialvalues, @see exagrype.api.initialvalues.

    CCZ4 has no conservative flux. You still have to assign a numerical
    scheme, a grid construction and (optionally) a plotter, exactly as for
    any other exahype2.api.Solver, and to add the solver to an
    exahype2.api.Project. The project has to be three-dimensional: the
    generated solver class refuses to compile otherwise.

    ## Shortcomings

    - The fourth-order Finite Differences of the production runs are
      available through exahype2.api.rkfd4, i.e. pass a
      CentralDifferencesWithRungeKutta as numerical scheme. They have no
      artificial dissipation. exagrype.api.FD4WithKODissipation is the same
      scheme with a Kreiss-Oliger dissipation on top. Unlike the legacy
      ExaGRyPE, neither supports adaptive mesh refinement yet.
    - The solution is pushed back onto the CCZ4 constraints
      (exagrype::ccz4::enforceCCZ4constraints()) only if you switch it on
      via enforce_ccz4_constraint(). Without it, long runs drift away from
      the constraint surface.
    - exagrype::ccz4::ncp() omits the advection terms
      @f$ \\beta^k \\partial _k Q @f$ of all unknowns besides the metric,
      the lapse, the shift and phi (whose advection is part of the
      algebraic source) and K0. It expects the numerical scheme to add them
      with lopsided stencils. The fourth-order Finite Differences do so if
      you select the kernel CentralDifferencesWithLopsidedAdvection with
      ADVECTION_VELOCITY and lopsided_advection_quantities(). The Finite
      Volumes of exahype2.api.fv do not add them, i.e. they evolve CCZ4
      without these advection terms.
    - The second-order formulation, black holes (TwoPunctures), tracers and
      the ADM/Psi4 postprocessing are not available yet.

    """

    class EnforceCCZ4Constraint(enum.Enum):
        """!

        When to push the solution back onto the algebraic CCZ4 constraints

        - No: never. This is the default.
        - OnlyFinalSolution: after each task which writes a new solution
          (exahype2.api.Solver.SolutionUpdateTaskTag), i.e. once per time
          step.
        - IntermediateSteps: in addition after each task which writes an
          intermediate solution (exahype2.api.Solver.IntermediateSolutionUpdateTag),
          i.e. after each Runge-Kutta stage. For single-step schemes such as
          the Finite Volumes, this is the same as OnlyFinalSolution.

        @ref exagrype.api.CCZ4Solver.CCZ4Solver.enforce_ccz4_constraint()

        """

        No = 0
        OnlyFinalSolution = 1
        IntermediateSteps = 2

    Unknowns = {
        "G": 6,
        "K": 6,
        "theta": 1,
        "Z": 3,
        "lapse": 1,
        "shift": 3,
        "b": 3,
        "dLapse": 3,
        "dxShift": 3,
        "dyShift": 3,
        "dzShift": 3,
        "dxG": 6,
        "dyG": 6,
        "dzG": 6,
        "traceK": 1,
        "phi": 1,
        "P": 3,
        "K0": 1,
    }
    """!

    The unknowns of the first-order CCZ4 formulation in the order the
    kernels in exagrype::ccz4 expect them. The names match the legacy
    ExaGRyPE solvers.

    """

    DefaultParameters = {
        "CCZ4LapseType": 0,
        "CCZ4SO": 0,
        "CCZ4ds": 1.0,
        "CCZ4c": 1.0,
        "CCZ4e": 1.0,
        "CCZ4f": 0.75,
        "CCZ4bs": 0.0,
        "CCZ4sk": 0.0,
        "CCZ4xi": 1.0,
        "CCZ4itau": 1.0,
        "CCZ4eta": 1.0,
        "CCZ4k1": 0.1,
        "CCZ4k2": 0.0,
        "CCZ4k3": 0.5,
        "CCZ4GLMc": 1.2,
        "CCZ4GLMd": 2.0,
        "CCZ4mu": 0.2,
    }
    """!

    Default values of the CCZ4 parameters, taken from the legacy ExaGRyPE
    solvers. Integer values become int constants of the generated solver
    class, all others double constants.

    """

    ADVECTION_VELOCITY = "shift"
    """!

    The variable which advects the unknowns, i.e. the velocity to pass to
    lopsided advection kernels (@see lopsided_advection_quantities()).

    """

    def __init__(
        self,
        name: str,
        parameters: Dict[str, Union[int, float]] = None,
        initial_condition: exahype2.api.InitPointWisely = None,
    ):
        """!

        Create a CCZ4 solver

        @param name: str
          Name of the solver, @see exahype2.api.Solver.

        @param parameters: Dict[str, Union[int, float]]
          CCZ4 parameters that differ from DefaultParameters, e.g.
          {"CCZ4k1": 0.0}. Keys which are not in DefaultParameters are
          rejected, as they most likely are typos.

        @param initial_condition: exahype2.api.InitPointWisely
          Initial condition. Defaults to exagrype.api.initialvalues.gauge_wave().
          Pass any other routine from exagrype.api.initialvalues, or your
          own exahype2.api.InitPointWisely.

        """
        super().__init__(name, unknowns=self.Unknowns, auxiliary_variables=0)

        self._enforce_ccz4_constraint = CCZ4Solver.EnforceCCZ4Constraint.No

        self._parameters = dict(self.DefaultParameters)
        for key, value in (parameters or {}).items():
            if key not in self.DefaultParameters:
                raise KeyError(
                    f"Solver {name}: unknown CCZ4 parameter {key!r}. "
                    f"Available parameters are {list(self.DefaultParameters)}."
                )
            self._parameters[key] = value

        self.add_abstract_solver_attribute(
            'static_assert(Dimensions == 3, "CCZ4 (ExaGRyPE) is three-dimensional only");'
        )
        for key, value in self._parameters.items():
            cpp_type = (
                "int" if isinstance(self.DefaultParameters[key], int) else "double"
            )
            self.add_abstract_solver_attribute(
                f"static constexpr {cpp_type} {key} = {value};"
            )

        self.add_user_solver_includes(
            """
#include "exagrype/ccz4/Kernels.h"
#include "exagrype/initialvalues/InitialValues.h"
#include "exagrype/initialvalues/ApplyTwoPunctures.h"
"""
        )

        self.add_pde_term(
            exahype2.api.NonconservativeProduct(
                implementation=self._ncp_implementation()
            )
        )
        self.add_pde_term(
            exahype2.api.VolumetricSource(implementation=self._source_implementation())
        )
        self.add_pde_term(
            exahype2.api.MaxEigenvalue(
                implementation=self._max_eigenvalue_implementation()
            )
        )
        self.add_pde_term(
            initial_condition
            if initial_condition is not None
            else initialvalues.gauge_wave()
        )

    def lopsided_advection_quantities(self) -> List[str]:
        """!

        The unknowns whose advection terms exagrype::ccz4::ncp() omits

        exagrype::ccz4::ncp() is invoked with its advection switched off.
        The unknowns returned are those whose advection terms
        @f$ \\beta^k \\partial _k Q @f$ hence have to be added by the
        numerical scheme, e.g. via

        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
          solver, patch_size, rk_order=4,
          right_hand_side_kernel = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta.RightHandSideKernel.CentralDifferencesWithLopsidedAdvection,
          advection_velocity     = exagrype.api.CCZ4Solver.ADVECTION_VELOCITY,
          advected_quantities    = solver.lopsided_advection_quantities(),
        )
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        These are all unknowns besides the conformal metric G, the lapse,
        the shift, phi and K0: The advection terms of the metric, the lapse,
        the shift and phi are part of the algebraic source
        (exagrype::ccz4::source()), which expresses them through the
        auxiliary unknowns, i.e. the first derivatives. K0 does not evolve.

        The advection of b and of the derivatives of the shift (dxShift,
        dyShift, dzShift) is scaled with CCZ4bs and CCZ4sk. If either of
        them is zero, the routine leaves these unknowns out.

        The equation of b couples the advection of b and of Ghat (the
        unknown Z): Its
        advection terms are CCZ4sk * (CCZ4xi * beta^k d_k Ghat + CCZ4bs *
        (beta^k d_k b - beta^k d_k Ghat)). A kernel which advects each
        quantity on its own, i.e. adds beta^k d_k b to b, reproduces this
        only for CCZ4xi=1, CCZ4bs=1 and CCZ4sk=1. This is the case for the
        default CCZ4xi and the moving puncture setups with CCZ4bs=CCZ4sk=1.

        @return List[str]
          Names of unknowns, @see Unknowns.

        """
        excluded = {"G", "lapse", "shift", "phi", "K0"}
        if self._parameters["CCZ4bs"] == 0 or self._parameters["CCZ4sk"] == 0:
            excluded |= {"b", "dxShift", "dyShift", "dzShift"}
        return [name for name in self.Unknowns if name not in excluded]

    def enforce_ccz4_constraint(self, value: "CCZ4Solver.EnforceCCZ4Constraint"):
        """!

        Set when the solver pushes the solution back onto the CCZ4 constraints

        By default, the solver does not enforce the constraints. If you
        switch the enforcement on, generate_solver_realisation() inserts a
        task which applies exagrype::ccz4::enforceCCZ4constraints() to each
        volume or grid point after each tagged solution update of the
        numerical scheme.

        @param value: CCZ4Solver.EnforceCCZ4Constraint
          No, OnlyFinalSolution or IntermediateSteps.

        """
        if not isinstance(value, CCZ4Solver.EnforceCCZ4Constraint):
            raise TypeError(
                f"Solver {self.name}: enforce_ccz4_constraint() expects a "
                f"CCZ4Solver.EnforceCCZ4Constraint, but got {value!r}"
            )
        self._enforce_ccz4_constraint = value

    def generate_solver_realisation(
        self,
        exahype2_project,
        peano_project,
        setup_simulation,
        time_step,
        plot,
        mesh_adaptation,
    ):
        """!

        Create the solver's task graphs and insert the constraint enforcement

        The routine first calls the superclass, which lets the grid
        construction, the numerical scheme and the plotter add their tasks.
        If the constraint enforcement is switched on
        (enforce_ccz4_constraint()), it afterwards inserts the enforcement
        after each tagged solution update of the numerical scheme. Which
        patch a tagged task writes and which guard it uses depends on the
        scheme, so the routine distinguishes the schemes explicitly
        (@ref page_exahype2_solver_manipulate_solver_steps). It raises an
        exception for any other scheme.

        All insertions work on the patch which the tagged tasks write and
        use the cell guard:

        - exahype2.api.fv.RusanovWithExplicitEuler: one task after Update
          cell.
        - exahype2.api.fv.RusanovWithOptimisticExplicitEuler: one task after
          both Update cell and Insert patch interior. Each cell runs exactly
          one of the two, so the task constrains every cell once, after its
          update.
        - exahype2.api.rkfd4.CentralDifferencesWithRungeKutta and its
          subclass exagrype.api.FD4WithKODissipation: one task after
          Complete time step and, for IntermediateSteps, one task after each
          Prepare stage k on the stage input.

        The scheme's log of the time stepping graph does not show the
        enforcement. With TEXT or VISUALISATION logging, the routine hence
        logs the graph once more.

        """
        super().generate_solver_realisation(
            exahype2_project,
            peano_project,
            setup_simulation,
            time_step,
            plot,
            mesh_adaptation,
        )
        Enforce = CCZ4Solver.EnforceCCZ4Constraint
        if self._enforce_ccz4_constraint == Enforce.No:
            return

        ## The patches which the tagged tasks write: The solution and, for
        ## Runge-Kutta, the stage input
        scheme = self.numerical_scheme
        match scheme:
            case (
                exahype2.api.fv.RusanovWithExplicitEuler()
                | exahype2.api.fv.RusanovWithOptimisticExplicitEuler()
            ):
                stage_input = None
            case exahype2.api.rkfd4.CentralDifferencesWithRungeKutta():
                stage_input = self._stage_input
            case _:
                raise TypeError(
                    f"Solver {self.name}: cannot enforce the CCZ4 constraints for "
                    f"the numerical scheme {type(scheme).__name__}. Supported are "
                    "exahype2.api.fv.RusanovWithExplicitEuler, "
                    "exahype2.api.fv.RusanovWithOptimisticExplicitEuler and "
                    "exahype2.api.rkfd4.CentralDifferencesWithRungeKutta."
                )

        ## One task after all writers of the new solution: The optimistic
        ## Finite Volumes write it in one of two tasks, depending on the cell
        self._insert_constraint_enforcement(
            time_step,
            self._tasks_of_this_solver(
                time_step, exahype2.api.Solver.SolutionUpdateTaskTag
            ),
            self._patch,
        )
        ## One task after each stage, as the stages run one after the other
        if (
            self._enforce_ccz4_constraint == Enforce.IntermediateSteps
            and stage_input is not None
        ):
            for anchor in self._tasks_of_this_solver(
                time_step, exahype2.api.Solver.IntermediateSolutionUpdateTag
            ):
                self._insert_constraint_enforcement(time_step, [anchor], stage_input)

        title = (
            f"Time stepping task graph for {self.name} with CCZ4 constraint enforcement"
        )
        if peano4.api.Logger().is_visualisation:
            time_step.visualise(title=title)
        if peano4.api.Logger().is_text:
            time_step.log_as_text(title=title)

    def _tasks_of_this_solver(self, time_step, tag):
        """!
        Tasks of time_step which carry tag and belong to this solver, as the
        graph holds the tasks of all solvers
        """
        return [
            task
            for task in time_step.find_tasks(tag)
            if task.name.startswith(self.name + "_")
        ]

    def _insert_constraint_enforcement(self, time_step, anchors, patch):
        """!

        Insert the enforcement of the CCZ4 constraints after anchors

        The routine creates a task which applies
        exagrype::ccz4::enforceCCZ4constraints() to each volume or grid point
        of patch, and inserts it directly after all anchors via
        peano4.api.taskgraph.Graph.insert_after(). The anchors' successors,
        e.g. the projection onto the faces, hence see the constrained data
        only. The task uses the solver's cell guard: If several anchors
        write patch, each cell is updated by one of them, and the task runs
        once per cell after all of them.

        @param time_step: peano4.api.taskgraph.Graph
          The time stepping task graph which holds the anchors.

        @param anchors: List[peano4.api.taskgraph.Task]
          The tasks which write patch. If the list is empty, the routine
          does nothing.

        @param patch: peano4.api.datamodel.Patch
          The cell data which the anchors write, i.e. the solution or a
          stage input.

        """
        if not anchors:
            return
        anchor_names = " and ".join(
            anchor.name[len(self.name) + 1 :] for anchor in anchors
        )
        task = peano4.api.taskgraph.Task(
            name=f"{self.name}_Enforce CCZ4 constraints after {anchor_names}",
            entity=peano4.api.taskgraph.Task.GridEntity.CELL,
        )
        ## READ_AND_WRITE keeps the data alive for later tasks
        task.use_cell(patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        task.add_action_set(
            toolbox.blockstructured.api.ApplyPointWiselyToPatch(
                patch=patch,
                implementation="::exagrype::ccz4::enforceCCZ4constraints(Q);",
                guard=self.grid_selector.createCellGuard(),
                additional_includes=self._get_default_includes()
                + self.user_solver_includes,
            )
        )
        time_step.insert_after(anchors, task)

    @property
    def parameters(self) -> Dict[str, Union[int, float]]:
        """!
        The CCZ4 parameters the solver is generated with (read-only copy).
        """
        return dict(self._parameters)

    @staticmethod
    def _ncp_implementation() -> str:
        """!

        The Finite Volume kernels hand over the jump deltaQ along one normal,
        the fourth-order Finite Differences of exahype2.api.rkfd4 the
        derivative along one normal times the mesh width,
        whereas exagrype::ccz4::ncp() expects the gradient along all three
        directions. We hence embed the jump into an otherwise empty gradient.

        """
        return """
  double deltaQSerialised[NumberOfUnknowns * 3];
  for (int i = 0; i < NumberOfUnknowns; i++) {
    deltaQSerialised[i + 0 * NumberOfUnknowns] = 0.0;
    deltaQSerialised[i + 1 * NumberOfUnknowns] = 0.0;
    deltaQSerialised[i + 2 * NumberOfUnknowns] = 0.0;

    deltaQSerialised[i + normal * NumberOfUnknowns] = deltaQ[i];
  }
  ::exagrype::ccz4::ncp(BTimesDeltaQ, Q, deltaQSerialised, normal, CCZ4LapseType, CCZ4ds, CCZ4c, CCZ4e, CCZ4f, CCZ4bs, CCZ4sk, CCZ4xi, CCZ4mu, CCZ4SO);
"""

    @staticmethod
    def _source_implementation() -> str:
        return """
  tarch::memset(S, 0, NumberOfUnknowns * sizeof(double));
  ::exagrype::ccz4::source(S, Q, CCZ4LapseType, CCZ4ds, CCZ4c, CCZ4e, CCZ4f, CCZ4bs, CCZ4sk, CCZ4xi, CCZ4itau, CCZ4eta, CCZ4k1, CCZ4k2, CCZ4k3, CCZ4SO);
"""

    @staticmethod
    def _max_eigenvalue_implementation() -> str:
        return """
  ::exagrype::ccz4::maxEigenvalue(Q, normal, CCZ4e, CCZ4ds, CCZ4GLMc, CCZ4GLMd, lambdaMax);
"""
