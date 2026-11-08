""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import exahype2.api
import peano4.api
import peano4.api.datamodel
import peano4.api.taskgraph
import toolbox.blockstructured.api


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
    creates it and registers it with the project right after the CCZ4 solver.
    The reconstruction has to be realised after the CCZ4 solver, as it looks
    up the tasks and the data of the latter.

    ## Task graph

    For each stage of the Runge-Kutta scheme of the CCZ4 solver, the solver
    inserts one task. The task depends on whatever the CCZ4 solver's
    reconstruction of the stage depends on, and the latter depends on the new
    task. Within the task, we visit each cell once, build the stage's input
    with its halo, reconstruct the derivatives of the cell and project them
    onto the faces. In terms of @ref page_exahype_programmers_guide_coupling,
    this is a multiplicative coupling, with the coupling repeated per stage.

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

        It inserts the reconstruction tasks into the time stepping task graph.
        Patch size and halo are the ones of the CCZ4 solver's scheme.

        """

        def __init__(self, solver: "FOSOReconstructionSolver"):
            super().__init__(solver)

        @property
        def patch_size(self) -> int:
            return self._solver.ccz4.numerical_scheme.patch_size

        @property
        def halo_size(self) -> int:
            return self._solver.ccz4.numerical_scheme.halo_size

        def construct_output(self, exahype2_project, peano_project, time_step):
            """!

            Insert the reconstruction in front of each stage of the CCZ4 solver

            @param exahype2_project: exahype2.api.Project
              The project that hosts the solvers.

            @param peano_project: peano4.api.Project
              The Peano project that the exahype2 project assembles.

            @param time_step: peano4.api.taskgraph.Graph
              Task graph of the time step. The CCZ4 solver has added its tasks.

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

            # The input of a stage with halo. Nobody besides the reconstruction
            # task reads it, i.e. it is a scratch array.
            halo = peano4.api.datamodel.Patch(
                (n + 2 * h, n + 2 * h, n + 2 * h),
                ccz4.unknowns,
                name + "PrimaryVariablesWithHalo",
            )
            halo.grid_selector = ccz4.grid_selector
            halo.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
                halo, "double"
            )
            peano_project.datamodel.add_cell(halo)

            functor = (
                f"::exagrype::ccz4::{reconstruction_function}("
                f"oldQWithHalo, marker.h(), {n}, {h}, "
                f"fineGridCell{derivatives.patch.name}.value);"
            )

            ## Stages is the Runge-Kutta stages or 1 for FV
            for stage in range(stages):
                stage_input = ccz4._patch if stage == 0 else ccz4._stage_input
                suffix = f" stage {stage}" if per_stage else ""
                ccz4_reconstruction = [
                    task
                    for task in time_step.tasks
                    if task.name == f"{ccz4.name}_Reconstruct{suffix}"
                ]
                if len(ccz4_reconstruction) != 1:
                    raise ValueError(
                        f"Reconstruction {name}: cannot find the reconstruction{suffix} of solver {ccz4.name} in the time stepping task graph"
                    )
                ccz4_reconstruction = ccz4_reconstruction[0]

                task = peano4.api.taskgraph.Task(
                    name=f"{name}_Reconstruct derivatives"
                    + (f" for stage {stage}" if per_stage else ""),
                    entity=Entity.CELL,
                )
                # The CCZ4 solver's reconstruction reads the same data, so it has to survive.
                task.use_cell(stage_input, Access.READ_AND_WRITE)
                task.use_face(
                    ccz4._patch_overlap_current_time_step, Access.READ_AND_WRITE
                )
                task.use_cell(halo, Access.TEMPORARY)
                task.use_cell(derivatives.patch, Access.READ_AND_WRITE)
                task.use_face(derivatives.patch_overlap_update, Access.TEMPORARY)
                # Loaded and stored, so Peano's stacks hold exactly one entry per face
                # when the next task (the CCZ4 solver's reconstruction) loads it again
                task.use_face(
                    derivatives.patch_overlap_current_time_step,
                    Access.READ_AND_WRITE,
                )
                task.add_action_set(
                    toolbox.blockstructured.api.ReconstructPatchAndApplyFunctor(
                        patch=stage_input,
                        patch_overlap=ccz4._patch_overlap_current_time_step,
                        reconstructed_patch=halo,
                        functor_implementation=functor,
                        guard=cell_guard,
                        add_assertions_to_halo_exchange=False,
                        additional_includes=includes,
                    )
                )
                task.add_action_set(
                    actionsets.ProjectPatchOntoFaces(
                        derivatives.patch,
                        derivatives.patch_overlap_update,
                        guard=cell_guard,
                        includes=includes,
                    )
                )
                task.add_action_set(
                    actionsets.RollOverUpdatedFace(
                        derivatives.patch_overlap_current_time_step,
                        derivatives.patch_overlap_update,
                        guard=face_guard,
                        includes=includes,
                    )
                )

                # Take over what the CCZ4 solver waits for, then let it wait for us
                for dependency in ccz4_reconstruction.dependencies:
                    task.add_dependency(dependency)
                time_step.add_task(task)
                ccz4_reconstruction.add_dependency(task)

            title = (
                f"Time stepping task graph for {ccz4.name} with reconstruction {name}"
            )
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
