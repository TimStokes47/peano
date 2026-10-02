# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.datamodel
import toolbox.blockstructured.api
import exahype2.api
import exahype2.api.dslhype as dslhype
 
from ..Solver import Solver
from .actionsets import ComputeCFLConditionAndGlobalTimeStepSize
from .actionsets import HandleBoundary
from .actionsets import ProjectPatchOntoFaces
from .actionsets import RollOverUpdatedFace
from .actionsets import UpdateCell


class RusanovWithExplicitEuler(Solver.NumericalScheme):
    """!

    Finite Volumes with a Rusanov Riemann solver and explicit Euler time
    stepping, realised over a block-structured mesh.

    """

    def __init__(self, solver: Solver, patch_size: int, overlap: int = 1, cfl_constant: float = 0.9):
        """!

        The constructor memorises the attributes. However, it does not do this
        within this class, but it adds the important properties straight to the
        solver, so anybody else working against this solver knows about the
        properties introduced by the numerical scheme.

        @see @ref __create_data_structures()

        patch_size: int
          Number of Finite Volumes per axis within one patch (i.e. within
          one mesh cell).

        overlap: int
          Number of Finite Volumes the halo layer reconstructed around a
          patch - from the neighbouring patches' faces - is wide.

        cfl_constant: float
          Safety factor the admissible time step size (@see
          exahype2.api.fv.actionsets.ComputeCFLConditionAndGlobalTimeStepSize,
          which this is passed straight through to) is scaled down by,
          relative to the theoretical CFL=1 stability limit. 0.9 is a
          conservative default for an explicit-Euler Finite Volume scheme;
          this codebase has no prior precedent value to match (grep for
          "cfl" outside this class finds only exaparty's unrelated SPH
          solvers, which default to 0.1).

        """
        super().__init__(solver)
        self.patch_size = patch_size
        self.overlap = overlap
        self.kernel_namespace = ["exahype2", "fv", "rusanov"]
        self.cfl_constant = cfl_constant
        self.__create_data_structures()

    def __create_time_step_functions(self):
        """!
        
        The functions and files created here are those related to the numerical scheme
        only, not the application. Creating functions to interact with PDE terms and 
        creating calls to this function is handled in the base solver, as it has knowledge 
        of these PDE terms.

        Creating a timestep function requires specifying the template parameters that
        it uses, the functor parameters that are used for non stateless kernels, and
        the python function that is parsed by dslhypeHyPE.

        TODO: Currently the MLIR types are passed manually here, this should ideally
        be inferred

        """

        template_parameters = [
            dslhype.SyntaxTree.Argument("NV", dslhype.SyntaxTree.TInteger()),
            dslhype.SyntaxTree.Argument("H", dslhype.SyntaxTree.TInteger()),
            dslhype.SyntaxTree.Argument("NumberOfUnknowns", dslhype.SyntaxTree.TInteger()),
            dslhype.SyntaxTree.Argument("NumberOfAuxiliaryVariables", dslhype.SyntaxTree.TInteger()),
            #dslhype.SyntaxTree.Argument("EvaluateFlux", dslhype.SyntaxTree.TBoolean()),
            #dslhype.SyntaxTree.Argument("EvaluateNonconservativeProduct", dslhype.SyntaxTree.TBoolean()),
            #dslhype.SyntaxTree.Argument("EvaluateSource", dslhype.SyntaxTree.TBoolean()),
            #dslhype.SyntaxTree.Argument("EvaluateMaximumEigenvalueAfterTimeStep", dslhype.SyntaxTree.TBoolean())
        ]
    
        functor_parameters = [
            dslhype.SyntaxTree.Argument("flux", dslhype.SyntaxTree.TCustom("const FluxFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()"),
            dslhype.SyntaxTree.Argument("nonconservativeProduct", dslhype.SyntaxTree.TCustom("const NonconservativeProductFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()"),
            dslhype.SyntaxTree.Argument("sourceTerm", dslhype.SyntaxTree.TCustom("const SourceFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, !llvm.ptr) -> ()"),
            dslhype.SyntaxTree.Argument("maxEigenvalue", dslhype.SyntaxTree.TCustom("const MaxEigenvalueFunctor&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()")
        ]

        time_step_dsl_function_2d = exahype2.api.fv.timeStepWithRusanov2D
        time_step_dsl_function_3d = exahype2.api.fv.timeStepWithRusanov3D
        dslhype.translate_dslhype_function(
            time_step_dsl_function_2d,
            self.kernel_namespace,
            template_parameters,
            functor_parameters,
            self._solver.pde_terms,
            "rusanov2d",
            dslhype.Target.Serial,
            stateless=False
        )
        dslhype.translate_dslhype_function(
            time_step_dsl_function_3d,
            self.kernel_namespace,
            template_parameters,
            functor_parameters,
            self._solver.pde_terms,
            "rusanov3d",
            dslhype.Target.Serial,
            stateless=False
        )
        file = open("kernels/rusanov.h", "w")
        file.write("""#if Dimensions == 2
    #include "rusanov2d.h"
#else
    #include "rusanov3d.h"
#endif""")
        file.close()


    def __create_data_structures(self):
        """!

        Create the mesh data structures (a Finite Volume patch per cell,
        plus the face-based overlap patches used to reconstruct its halo)
        this numerical scheme requires.

        Ported from the legacy exahype2.solvers.fv.FV.create_data_structures():
        one @ref peano4.api.datamodel.Patch of patch_size^d Finite Volumes
        per mesh cell (_patch), plus a halo-augmented copy of it
        (_patch_with_halo, (patch_size+2*overlap)^d, @see
        toolbox.blockstructured.api.ReconstructPatchWithHalo) and two overlap
        patches of width 2*overlap attached to the mesh faces:

        - _patch_overlap_current_time_step: the persistent halo data
          toolbox.blockstructured.api.ReconstructPatchWithHalo reads from to
          rebuild _patch_with_halo (@see construct_output(), which hands it
          _patch/_patch_overlap_current_time_step/_patch_with_halo directly
          rather than the whole solver).
        - _patch_overlap_update: the projection of the just-updated _patch
          accumulated by the current sweep (@see
          exahype2.api.fv.actionsets.ProjectPatchOntoFaces), rolled over
          into _patch_overlap_current_time_step by
          exahype2.api.fv.actionsets.RollOverUpdatedFace once the update is
          complete.

        All four are assigned onto self._solver (the shared
        exahype2.api.Solver instance both this numerical scheme and its
        surrounding solver's Solver.GridConstruction/Solver.Plotter
        counterparts hold a reference to as self._solver), not onto self,
        since the remaining FV action sets that need them - @see
        exahype2.api.fv.actionsets.HandleBoundary,
        exahype2.api.fv.actionsets.UpdateCell,
        exahype2.api.fv.actionsets.AdaptivityCriterion - are handed that
        shared solver and look the patches up as solver._patch /
        solver._patch_overlap_*.

        Storage is hardwired to smart pointers (peano4.api.datamodel's
        PatchToDoubleArrayWithSmartPointer), matching the legacy default
        (Storage.SmartPointers for both cell and face data) - the legacy
        Storage enum's other two backends (CallStack, Heap) are not
        exposed here.

        Two pieces of the legacy method are deliberately not ported yet:
        - The cell/face label creation
          (exahype2.grid.create_cell_label()/create_face_label()) - that
          module has not been ported to the new API.
        - The load/store/compute guards
          (FV._provide_cell_data_to_compute_kernels_default_guard() and
          friends) - they are keyed off the legacy SolverState state
          machine, which the task-graph architecture has no equivalent of:
          a task only ever runs within its own phase's task graph in the
          first place, so "are we still constructing the grid" does not
          need to be re-checked at runtime the same way.

        """
        total_unknowns = self._solver.unknowns + self._solver.auxiliary_variables
        identifier = self._solver._unknown_identifier()

        self._solver._patch = peano4.api.datamodel.Patch(
            (self.patch_size, self.patch_size, self.patch_size),
            total_unknowns,
            identifier,
        )
        self._solver._patch_with_halo = peano4.api.datamodel.Patch(
            (self.patch_size+2*self.overlap, self.patch_size+2*self.overlap, self.patch_size+2*self.overlap),
            total_unknowns,
            identifier + "WithHalo",
        )
        self._solver._patch_overlap_current_time_step = peano4.api.datamodel.Patch(
            (2 * self.overlap, self.patch_size, self.patch_size),
            total_unknowns,
            identifier + "CurrentTimeStep",
        )
        self._solver._patch_overlap_update = peano4.api.datamodel.Patch(
            (2 * self.overlap, self.patch_size, self.patch_size),
            total_unknowns,
            identifier + "Update",
        )


    def __add_data_to_repository(self, peano_project):
        """!
        
        Consult @ref page_exahype_programmers_guide_new_solver for some information
        how this routine fits to the typical workflow when we introduce a new
        solver.
        
        """
        peano4.api.Logger().log("""
Patch data
----------
{}
Patch overlap data
------------------
{}
{}""".format( str(self._solver._patch), str(self._solver._patch_overlap_current_time_step), str(self._solver._patch_overlap_update) ) )

        peano_project.datamodel.add_cell(self._solver._patch)
        peano_project.datamodel.add_cell(self._solver._patch_with_halo)
        peano_project.datamodel.add_face(self._solver._patch_overlap_current_time_step)
        peano_project.datamodel.add_face(self._solver._patch_overlap_update)

        self._solver._patch.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch, "double"
        )
        self._solver._patch_overlap_current_time_step.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_overlap_current_time_step, "double"
        )
        self._solver._patch_overlap_update.generator = peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer(
            self._solver._patch_overlap_update, "double"
        )
        
        self._solver._patch_overlap_current_time_step.generator.merge_method_definition = (
            toolbox.blockstructured.api.get_face_merge_implementation(
                self._solver._patch_overlap_current_time_step
            )
        )
        self._solver._patch_overlap_current_time_step.generator.includes += """
#include "peano4/utils/Loop.h"
"""

        self._solver._patch_overlap_current_time_step.generator.send_condition = "true"
        self._solver._patch_overlap_current_time_step.generator.receive_and_merge_condition = "true"

        self._solver._patch_overlap_update.generator.send_condition = "false"
        self._solver._patch_overlap_update.generator.receive_and_merge_condition = "false"

    def construct_kernel_call(self):
        pde_term_calls = []
        for application_term in self._solver.pde_terms:
            if application_term.NAME == "flux":
                pde_term_calls.append(f"""[&](
    [[maybe_unused]] const double* __restrict__                   Q,
    [[maybe_unused]] const tarch::la::Vector<Dimensions, double>& x,
    [[maybe_unused]] int                                          normal,
    [[maybe_unused]] double* __restrict__                         F
)->void {{
    repositories::instanceOf{self._solver.name}.flux(Q, x, normal, F);
}}""")
                
        for application_term in self._solver.pde_terms:
            if application_term.NAME == "maxEigenvalue":
                pde_term_calls.append(f"""[&](
    [[maybe_unused]] const double* __restrict__                   Q,
    [[maybe_unused]] const tarch::la::Vector<Dimensions, double>& x,
    [[maybe_unused]] int                                          normal,
    [[maybe_unused]] double* __restrict__                         lambdaMax
)->void {{
    repositories::instanceOf{self._solver.name}.flux(Q, x, normal, lambdaMax);
}}""")
        
        return f"""timeStepWithRusanov<
    {self.patch_size},
    {self.overlap},
    {self._solver.unknowns},
    {self._solver.auxiliary_variables}
>(
    patchData,
    {",\n".join(pde_term_calls)}
);"""

    def construct_output(self,
                         exahype2_project,
                         peano_project,
                         time_step):
        """!

        """
        peano4.api.Logger().log("Create time stepping task graph for Finite Volume scheme over block-structured meshes with Rusanov flux")

        self.__add_data_to_repository(peano_project)

        self.__create_time_step_functions()
        self._solver.compute_kernel_call = self.construct_kernel_call()
        
        reconstruction_task         = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Reconstruct and apply BCs", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        update_cell_task            = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Update cell", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        project_on_faces_task       = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Project on faces", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        compute_time_step_size_task = peano4.api.taskgraph.Task(name=f"{self._solver.name}_Compute time step size", entity=peano4.api.taskgraph.Task.GridEntity.CELL)
        
        time_step.add_task(reconstruction_task)
        time_step.add_task(update_cell_task)
        time_step.add_task(project_on_faces_task)
        time_step.add_task(compute_time_step_size_task)

        ## Build the reconstruction tasks befilling the patch_with_halo, which is logically temporary
        reconstruction_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        reconstruction_task.use_cell(self._solver._patch_with_halo, peano4.api.taskgraph.Task.DataAccess.WRITE)
        reconstruction_task.use_face(self._solver._patch_overlap_current_time_step, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        reconstruction_task.add_action_set(
            toolbox.blockstructured.api.ReconstructPatchWithHalo(
                patch=self._solver._patch,
                patch_overlap=self._solver._patch_overlap_current_time_step,
                reconstructed_patch=self._solver._patch_with_halo,
                guard="not marker.willBeRefined() and not marker.hasBeenRefined()",
                add_assertions_to_halo_exchange=False,
                additional_includes=self._solver._get_default_includes() + self._solver.user_solver_includes,
            )
        )
        reconstruction_task.add_action_set( HandleBoundary(self._solver) )

        ## Do the actual time step update
        update_cell_task.add_dependency(reconstruction_task)
        update_cell_task.use_cell(self._solver._patch_with_halo, peano4.api.taskgraph.Task.DataAccess.READ_AND_DISCARD)
        update_cell_task.use_cell(self._solver._patch,           peano4.api.taskgraph.Task.DataAccess.WRITE)
        update_cell_task.add_action_set( UpdateCell(self._solver) )
        
        ## Next project the new data onto the faces. Also roll over the solution
        project_on_faces_task.add_dependency(update_cell_task)
        project_on_faces_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)
        project_on_faces_task.use_face(self._solver._patch_overlap_update, peano4.api.taskgraph.Task.DataAccess.TEMPORARY)
        project_on_faces_task.use_face(self._solver._patch_overlap_current_time_step, peano4.api.taskgraph.Task.DataAccess.WRITE)
        project_on_faces_task.add_action_set(
            ProjectPatchOntoFaces(
                self._solver._patch,
                self._solver._patch_overlap_update,
                guard="true",
                includes=self._solver._get_default_includes() + self._solver.user_solver_includes,
            )
        )
        project_on_faces_task.add_action_set(
            RollOverUpdatedFace(
                self._solver._patch_overlap_current_time_step,
                self._solver._patch_overlap_update,
                guard="true",
                includes=self._solver._get_default_includes() + self._solver.user_solver_includes,
            )
        )
        
        ## Parallel to the projection, determine the CFL condition and compute time step size
        compute_time_step_size_task.add_dependency(update_cell_task)
        compute_time_step_size_task.use_cell(self._solver._patch, peano4.api.taskgraph.Task.DataAccess.READ_AND_WRITE)        
        compute_time_step_size_task.add_action_set(
            ComputeCFLConditionAndGlobalTimeStepSize(self._solver, self.cfl_constant, guard="true")
        )
        
        if peano4.api.Logger().is_visualisation:
            time_step.visualise(title="Time stepping task graph for {}".format(self._solver.name))
        if peano4.api.Logger().is_text:
            time_step.log_as_text(title="Time stepping task graph for {}".format(self._solver.name))
