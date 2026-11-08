""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import exahype2.api
import peano4.api
import peano4.api.datamodel
import peano4.api.taskgraph
import toolbox.blockstructured.api

from .actionsets import ApplyReconstructionFunctor


class FOSOReconstructionSolver(exahype2.api.Solver):
    """!

    Reconstructs the first derivatives which exagrype.api.FOSOCCZ4Solver needs

    The solver is the second partner in the ping-pong of the first-/second-order
    reconstructed CCZ4 formulation (@ref page_exagrype_fo_so_ccz4). Where the
    CCZ4 solver sees 25 unknowns and 34 auxiliary variables (the derivatives),
    the reconstruction solver sees the same 59 entries the other way round: its
    34 unknowns are the derivatives, and its 25 auxiliary variables are the
    unknowns of the CCZ4 solver. Nothing is copied: the derivatives are the
    very patch that the CCZ4 solver holds as auxiliary variables "Derivatives",
    and the reconstruction reads the CCZ4 solver's patch.

    It is a real exahype2.api.Solver (it has a solver class and a global
    instance in the solver repository), yet it is much simpler than a normal
    solver:

    - It has no PDE terms and no compute kernel. Its only compute step
      evaluates the stencil on the unknowns of the partner.
    - It computes no time step size. Its C++ class forwards the time stamps
      and time step sizes of the CCZ4 solver, so the global minimum over all
      solvers is the one of the CCZ4 solver.
    - It does neither add lopsided advection nor enforce constraints.
    - It has no grid construction of its own, no plotter and no mesh
      adaptation. The CCZ4 solver builds the mesh and initialises the
      derivatives.

    Users do not create the solver themselves: exagrype.api.FOSOSolver
    creates it and realises it right after the CCZ4 solver. The reconstruction
    has to be realised after the CCZ4 solver, as it uses its data.

    ## Task graph

    For each stage of the Runge-Kutta scheme of the CCZ4 solver (once for FV),
    the solver creates a chain of tasks in a task graph of its own:

    1. Reconstruct the primary variables and the derivative variables
       (halo from the cell and the faces). From the point of view of this
       solver, the primary variables are auxiliary variables, and the
       derivative variables are the unknowns. The two tasks run in parallel.
    2. Interpolate the coarse face data of both onto hanging faces (again two
       parallel tasks).
    3. Apply the stencil, i.e. recompute the derivatives from the primary variables.
    4. Project the derivatives onto the faces.
    5. Restrict them on hanging faces, but only if the derivatives' can_change flag
       (@ref exahype2.api.AuxiliaryVariables) is set, which it is.
    6. Roll the face data over.

    The graph has no dependencies between stages. The host concatenates the graph with the
    one of the CCZ4 solver and makes the stage's reconstruction task of the
    CCZ4 solver wait for the last task, while the first tasks wait for what the
    latter waited for (exagrype.api.FOSOSolver.couple()). In terms of
    @ref page_exahype_programmers_guide_coupling, this is a multiplicative
    coupling, with the coupling repeated per stage.

    ## Shortcomings

    - Domain boundaries other than periodic ones and Sommerfeld boundary
      conditions are not supported, as the reconstruction reads the halo of the
      CCZ4 solver's unknowns before the Dirichlet boundary conditions
      are applied. The reconstruction takes the same dependencies as the
      CCZ4 solver's reconstruction, which makes it work in principle, but this has
      not been tested.
    - Supported are exahype2.api.rkfd4.CentralDifferencesWithRungeKutta
      (fourth-order stencil, halo of at least three grid cells, one task per
      Runge-Kutta stage) and exahype2.api.fv.RusanovWithExplicitEuler
      (second-order central differences, overlap of one is enough, a single
      task as there is a single stage). The optimistic Finite Volume scheme is
      not supported.

    """

    class Scheme(exahype2.api.Solver.NumericalScheme):
        """!

        The numerical scheme of the reconstruction solver

        It builds the reconstruction tasks, one per stage of the CCZ4 solver's
        scheme, in a task graph of its own. The tasks have no dependencies.
        Where they belong in the graph of the CCZ4 solver is the business of
        the host (exagrype.api.FOSOSolver): after construct_output(),
        stage_first_tasks and stage_last_tasks hold the tasks that start and end
        the chain of each stage, and
        names_of_partner_reconstruction_tasks holds the name of the task of
        the CCZ4 solver which has to wait for it. Patch size and halo are the
        ones of the CCZ4 solver's scheme.

        """

        def __init__(self, solver: "FOSOReconstructionSolver"):
            super().__init__(solver)
            self.stage_first_tasks = []
            """! The tasks which start the reconstruction of each stage (a list per stage), filled by construct_output() """
            self.stage_last_tasks = []
            """! The task which ends the reconstruction of each stage, filled by construct_output() """
            self.names_of_partner_reconstruction_tasks = []
            """! Name of the reconstruction task of the CCZ4 solver per stage, filled by construct_output() """

        @property
        def patch_size(self) -> int:
            return self._solver.ccz4.numerical_scheme.patch_size

        @property
        def halo_size(self) -> int:
            return self._solver.ccz4.numerical_scheme.halo_size

        def construct_output(
            self, exahype2_project, peano_project, time_step, add_global_tasks=True
        ):
            """!

            Create the reconstruction tasks for each stage of the CCZ4 solver

            The CCZ4 solver has to be realised before, as the routine
            needs its data. The solver has no global tasks, i.e. it ignores
            add_global_tasks: the derivatives follow the mesh that the CCZ4
            solver refines.

            @param exahype2_project: exahype2.api.Project
              The project that hosts the solvers.

            @param peano_project: peano4.api.Project
              The Peano project that the exahype2 project assembles.

            @param time_step: peano4.api.taskgraph.Graph
              Task graph of the reconstruction solver. It receives the tasks.

            @param add_global_tasks: bool
              Ignored.

            """
            reconstruction = self._solver
            ccz4 = reconstruction.ccz4
            name = reconstruction.name
            scheme = ccz4.numerical_scheme
            match scheme:
                case exahype2.api.rkfd4.CentralDifferencesWithRungeKutta():
                    if scheme.HALO_SIZE < 3:
                        raise ValueError(
                            f"Reconstruction {name}: the halo of the scheme of {ccz4.name} has to be "
                            f"at least three, but is {scheme.HALO_SIZE}. The solver evaluates fluxes and "
                            "non-conservative products on the faces of the cell, i.e. it needs the "
                            "derivatives one layer outside of the cell, and the fourth-order stencil reaches "
                            "two further layers. Use a scheme with lopsided advection or "
                            "exagrype.api.FD4WithKODissipation."
                        )
                    actionsets = exahype2.api.rkfd4.actionsets
                    reconstruction_function = "reconstructFirstDerivativesFD4"
                    stages = scheme.rk_order
                    per_stage = True
                case exahype2.api.fv.RusanovWithExplicitEuler():
                    # Second-order central differences reach one layer out, and the
                    # derivatives of the halo come from the neighbour via the faces
                    actionsets = exahype2.api.fv.actionsets
                    reconstruction_function = (
                        "reconstructFirstDerivativesCentralDifferences"
                    )
                    stages = 1
                    per_stage = False
                case _:
                    raise TypeError(
                        f"Reconstruction {name}: solver {ccz4.name} has to use "
                        "exahype2.api.rkfd4.CentralDifferencesWithRungeKutta (or a subclass) or "
                        "exahype2.api.fv.RusanovWithExplicitEuler as numerical scheme, "
                        f"but uses {type(scheme).__name__}"
                    )
            derivatives = ccz4.derivatives
            if derivatives.patch is None:
                raise ValueError(
                    f"Reconstruction {name}: solver {ccz4.name} has not been realised yet. "
                    "The CCZ4 solver has to be realised before its reconstruction."
                )

            Access = peano4.api.taskgraph.Task.DataAccess
            Entity = peano4.api.taskgraph.Task.GridEntity
            n = scheme.patch_size
            h = scheme.HALO_SIZE if per_stage else scheme.overlap
            cell_guard = ccz4.grid_selector.createCellGuard()
            face_guard = ccz4.grid_selector.createFaceGuard()
            includes = (
                ccz4._get_default_includes()
                + ccz4.user_solver_includes
                + '\n#include "exagrype/ccz4/SecondOrderAuxiliaryVariablesReconstruction.h"\n'
            )

            # The reconstructed primary variables and derivative variables.
            # Nobody besides the tasks of the stage reads them, i.e. they are
            # scratch arrays.
            def create_scratch_halo(unknowns, halo_name):
                halo = peano4.api.datamodel.Patch(
                    (n + 2 * h, n + 2 * h, n + 2 * h), unknowns, halo_name
                )
                halo.grid_selector = ccz4.grid_selector
                halo.generator = (
                    peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
                        halo, "double"
                    )
                )
                peano_project.datamodel.add_cell(halo)
                return halo

            primary_halo = create_scratch_halo(
                ccz4.unknowns, name + "PrimaryVariablesWithHalo"
            )
            derivative_halo = create_scratch_halo(
                derivatives.size, name + derivatives.data_name + "WithHalo"
            )

            functor = (
                f"::exagrype::ccz4::{reconstruction_function}("
                f"oldQWithHalo, marker.h(), {n}, {h}, newQ);"
            )

            self.stage_first_tasks = []
            self.stage_last_tasks = []
            self.names_of_partner_reconstruction_tasks = []
            ## Stages is the Runge-Kutta stages or 1 for FV
            for stage in range(stages):
                stage_input = ccz4._patch if stage == 0 else ccz4._stage_input
                suffix = f" stage {stage}" if per_stage else ""
                self.names_of_partner_reconstruction_tasks.append(
                    f"{ccz4.name}_Reconstruct{suffix}"
                )

                def create_task(task_name):
                    task = peano4.api.taskgraph.Task(
                        name=f"{name}_{task_name}{suffix}", entity=Entity.CELL
                    )
                    time_step.add_task(task)
                    return task

                ## Reconstruct, both the primary variables (the auxiliary
                ## variables of this solver) and the derivative variables (its
                ## unknowns). The two are independent. The CCZ4 solver's
                ## reconstruction reads the same data, so it has to survive.
                reconstruct_primary = create_task("Reconstruct primary variables")
                reconstruct_primary.use_cell(stage_input, Access.READ_AND_WRITE)
                reconstruct_primary.use_cell(primary_halo, Access.WRITE)
                reconstruct_primary.use_face(
                    ccz4._patch_overlap_current_time_step, Access.READ_AND_WRITE
                )
                reconstruct_primary.add_action_set(
                    toolbox.blockstructured.api.ReconstructPatchWithHalo(
                        patch=stage_input,
                        patch_overlap=ccz4._patch_overlap_current_time_step,
                        reconstructed_patch=primary_halo,
                        guard=cell_guard,
                        add_assertions_to_halo_exchange=False,
                        additional_includes=includes,
                    )
                )
                reconstruct_derivatives = create_task(
                    "Reconstruct derivative variables"
                )
                reconstruct_derivatives.use_cell(
                    derivatives.patch, Access.READ_AND_WRITE
                )
                reconstruct_derivatives.use_cell(derivative_halo, Access.WRITE)
                reconstruct_derivatives.use_face(
                    derivatives.patch_overlap_current_time_step, Access.READ_AND_WRITE
                )
                reconstruct_derivatives.add_action_set(
                    toolbox.blockstructured.api.ReconstructPatchWithHalo(
                        patch=derivatives.patch,
                        patch_overlap=derivatives.patch_overlap_current_time_step,
                        reconstructed_patch=derivative_halo,
                        guard=cell_guard,
                        add_assertions_to_halo_exchange=False,
                        additional_includes=includes,
                    )
                )

                ## Interpolate the coarse face data onto hanging faces. As in the
                ## CCZ4 solver, this takes effect before the reconstruction reads
                ## the faces (Peano touches all faces before it enters the cell).
                interpolate_primary = create_task(
                    "Interpolate primary variables on hanging faces"
                )
                interpolate_primary.use_face(
                    ccz4._patch_overlap_current_time_step, Access.READ_AND_WRITE
                )
                interpolate_primary.add_action_set(
                    toolbox.blockstructured.api.InterpolateHangingFaces(
                        ccz4._patch_overlap_current_time_step,
                        scheme=scheme.interpolation,
                        additional_includes=includes,
                    )
                )
                interpolate_primary.add_dependency(reconstruct_primary)
                interpolate_derivatives = create_task(
                    "Interpolate derivative variables on hanging faces"
                )
                interpolate_derivatives.use_face(
                    derivatives.patch_overlap_current_time_step, Access.READ_AND_WRITE
                )
                interpolate_derivatives.add_action_set(
                    toolbox.blockstructured.api.InterpolateHangingFaces(
                        derivatives.patch_overlap_current_time_step,
                        scheme=scheme.interpolation,
                        additional_includes=includes,
                    )
                )
                interpolate_derivatives.add_dependency(reconstruct_derivatives)

                ## The stencil
                apply_stencil = create_task("Reconstruct derivatives")
                apply_stencil.use_cell(primary_halo, Access.READ_AND_DISCARD)
                apply_stencil.use_cell(derivative_halo, Access.READ_AND_DISCARD)
                apply_stencil.use_cell(derivatives.patch, Access.READ_AND_WRITE)
                apply_stencil.add_action_set(
                    ApplyReconstructionFunctor(
                        patch=derivatives.patch,
                        primary_variables_with_halo=primary_halo,
                        derivative_variables_with_halo=derivative_halo,
                        functor_implementation=functor,
                        guard=cell_guard,
                        additional_includes=includes,
                    )
                )
                apply_stencil.add_dependency(interpolate_primary)
                apply_stencil.add_dependency(interpolate_derivatives)

                ## Project the derivatives onto the faces
                project = create_task("Project derivative variables onto faces")
                project.use_cell(derivatives.patch, Access.READ_AND_WRITE)
                project.use_face(derivatives.patch_overlap_update, Access.TEMPORARY)
                ## Declared to make the compiler start a new mesh traversal here (it
                ## does not fuse tasks with conflicting face data). The projection, the
                ## restriction and the roll over share the temporary update patch,
                ## i.e. they have to run in one traversal.
                project.use_face(
                    derivatives.patch_overlap_current_time_step, Access.WRITE
                )
                project.add_action_set(
                    actionsets.ProjectPatchOntoFaces(
                        derivatives.patch,
                        derivatives.patch_overlap_update,
                        guard=cell_guard,
                        includes=includes,
                    )
                )
                project.add_dependency(apply_stencil)
                last_task = project

                ## Restrict from the hanging faces onto the coarse faces, but only
                ## if the derivatives are allowed to change, i.e. in the
                ## sense of the auxiliary variables' can_change flag.
                if derivatives.can_change:
                    restrict = create_task(
                        "Restrict derivative variables on hanging faces"
                    )
                    restrict.use_face(
                        derivatives.patch_overlap_update, Access.TEMPORARY
                    )
                    restrict.add_action_set(
                        toolbox.blockstructured.api.RestrictHangingFaces(
                            derivatives.patch_overlap_update,
                            scheme=scheme.restriction,
                            clear_guard=face_guard,
                            additional_includes=includes,
                        )
                    )
                    restrict.add_dependency(last_task)
                    last_task = restrict

                ## Roll over
                roll_over = create_task("Roll over derivative variable faces")
                roll_over.use_face(derivatives.patch_overlap_update, Access.TEMPORARY)
                roll_over.use_face(
                    derivatives.patch_overlap_current_time_step, Access.TEMPORARY
                )
                roll_over.add_action_set(
                    actionsets.RollOverUpdatedFace(
                        derivatives.patch_overlap_current_time_step,
                        derivatives.patch_overlap_update,
                        guard=face_guard,
                        includes=includes,
                    )
                )
                roll_over.add_dependency(last_task)

                self.stage_first_tasks.append(
                    [reconstruct_primary, reconstruct_derivatives]
                )
                self.stage_last_tasks.append(roll_over)

            title = f"Time stepping task graph for {name}{reconstruction.task_graph_title_suffix}"
            if peano4.api.Logger().is_visualisation:
                time_step.visualise(title=title)
            if peano4.api.Logger().is_text:
                time_step.log_as_text(title=title)

    class NoGridConstruction(exahype2.api.Solver.GridConstruction):
        """!

        The reconstruction solver builds no mesh

        The CCZ4 solver constructs the mesh, and initialises the derivatives
        which are the unknowns of the reconstruction solver.

        """

        def construct_output(self, exahype2_project, peano_project, setup_simulation):
            pass

    def __init__(self, name: str, ccz4: "FOSOCCZ4Solver"):
        """!

        @param name: str
          Name of the solver, @see exahype2.api.Solver.

        @param ccz4: exagrype.api.FOSOCCZ4Solver
          The CCZ4 solver whose derivatives this solver reconstructs.

        """
        if not hasattr(ccz4, "derivatives"):
            raise TypeError(
                f"The reconstruction serves a FOSOCCZ4Solver, but got {type(ccz4).__name__}"
            )
        super().__init__(name, dict(ccz4.DerivativeUnknowns))
        self.ccz4 = ccz4
        """! The partner whose derivatives we reconstruct """
        self.grid_selector = ccz4.grid_selector
        self.numerical_scheme = FOSOReconstructionSolver.Scheme(self)
        self.grid_construction = FOSOReconstructionSolver.NoGridConstruction(self)
        self.add_user_solver_includes('\n#include "repositories/SolverRepository.h"\n')
        self._forward_time_stamps_and_time_step_sizes()

    @property
    def task_graph_title_suffix(self) -> str:
        return f" (reconstruction part of {self.ccz4.name})"

    def _forward_time_stamps_and_time_step_sizes(self):
        """!

        The solver does not time step. It therefore hands out what the CCZ4
        solver hands out. Otherwise, the global minimum and maximum which the
        solver repository evaluates over all solvers would be spoilt.

        """
        for getter in [
            "getMinTimeStamp",
            "getMaxTimeStamp",
            "getMinTimeStepSize",
            "getMaxTimeStepSize",
            "getAdmissibleTimeStepSize",
        ]:
            self.add_abstract_solver_method(
                f"double {getter}() const override;",
                lambda qualified_class_name, getter=getter: (
                    f"double {qualified_class_name}::{getter}() const {{\n"
                    f"  return repositories::{self.ccz4.get_name_of_global_instance()}.{getter}();\n"
                    "}\n"
                ),
            )

    def create_readme_descriptor(self, domain_offset, domain_size) -> str:
        return (
            f"Reconstruction {self.name}: reconstructs the {self.unknowns} first "
            f"derivatives of solver {self.ccz4.name} from its {self.ccz4.unknowns} unknowns"
        )
