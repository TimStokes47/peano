from collections import OrderedDict

class PipelineBuilder:
    xdsl_passes = [
        "reuse-memory-allocations",
        "fuse-memory-allocations",
        "imperfect-nested-loops-to-perfect",
        "nested-parallel-ops-to-multidimensional",
        "nested-parallel-ops-to-for-ops",
        "scf-parallel-loop-fusion",
        "convert-scf-to-omp-tasks"
    ]

    def __init__(self):
        self.use_gpu = False
        self.gpu_vendor  = None
        self.gpu_architecture = None

        self.use_omp = False
        self.use_omp_tasks = False
        self.omp_task_version="taskloop"
        self.omp_single_region=False

        self.flatten_memrefs = False

        self.nested_parallel_ops_to_for_ops = False
        self.max_nested_parallel_count = None

        self.loop_fusion = False

        self.imperfect_nested_loops_to_perfect = False
        self.nested_parallel_ops_to_multidimensional = False

        self.fuse_memory_allocations = False
        self.memory_reuse = False
        self.max_patch_size = None

        self.inline_polygeist_functions = False
        self.polygeist_function_source_file = None

    def generate_pass_string(self, passes, pass_name):
        if passes[pass_name] =={}:
            return pass_name

        arguments = []
        for argument in passes[pass_name].keys():
            arguments.append(f"{argument}={passes[pass_name][argument]}")
        return f"{pass_name}{{{' '.join(arguments)}}}"

    def generate_opt_commands(self):
        passes = OrderedDict()

        if self.memory_reuse:
            passes.update({"reuse-memory-allocations" : {"max-allocation-size": str(self.max_patch_size)}})

        if self.loop_fusion:
            passes.update({"cse": {}})
            passes.update({"scf-parallel-loop-fusion": {"cell_local_callees": "sourceTerm_bridge,flux_bridge,nonconservativeProduct_bridge,maxEigenvalue_bridge"}})

        if self.imperfect_nested_loops_to_perfect:
            passes.update({"control-flow-sink" : {}})
            passes.update({"imperfect-nested-loops-to-perfect" :{}})

        if self.nested_parallel_ops_to_for_ops and self.nested_parallel_ops_to_multidimensional:
            raise Exception("These passes are contradictory")
        if self.nested_parallel_ops_to_multidimensional:
            passes.update({"nested-parallel-ops-to-multidimensional" : {}})
        elif self.nested_parallel_ops_to_for_ops:
            passes.update({"nested-parallel-ops-to-for-ops" : {"number-of-parallel-ops": str(self.max_nested_parallel_count)}})

        if self.flatten_memrefs:
            passes.update({"dslhype-linearize-nested-memrefs" : {}})
            passes.update({"canonicalize" : {}})

            if self.fuse_memory_allocations:
                passes.update({"fuse-memory-allocations": {}})
        
        if self.use_gpu:
            passes.update({f"""canonicalize,cse,loop-invariant-code-motion,dslhype-host-to-device-memspace{{mem-space=global}},dslhype-include-physics{{physics-file={self.polygeist_function_source_file} mem-space=global}},dslhype-host-to-device-memspace{{mem-space=global}},inline,expand-strided-metadata,fold-memref-alias-ops,symbol-dce,canonicalize,func.func(gpu-map-parallel-loops),convert-parallel-loops-to-gpu,fold-memref-alias-ops,lower-affine,gpu-kernel-outlining,func.func(gpu-async-region),canonicalize,convert-arith-to-llvm{{index-bitwidth=64}},convert-scf-to-cf,convert-cf-to-llvm{{index-bitwidth=64}},nvvm-attach-target{{chip={self.gpu_architecture}}},gpu.module(inline,convert-gpu-to-nvvm,canonicalize,cse),gpu-to-llvm,gpu-module-to-binary,finalize-memref-to-llvm{{index-bitwidth=64}},reconcile-unrealized-casts""": {}})
        else:
            if self.use_omp:
                if self.use_omp_tasks:
                    task_args = {}
                    if self.omp_task_version == "taskloop":
                        task_args.update({"mode": "taskloop"})
                    else:
                        task_args.update({"mode": "task"})
                        task_args.update({"depend": "true"})
                        task_args.update({"chunk": "8"})
                    if self.omp_single_region:
                        task_args.update({"single_region": "true"})
                    passes.update({"convert-scf-to-omp-tasks" : task_args})
                else:
                    passes.update({"convert-scf-to-openmp" : {}})
            
            if self.inline_polygeist_functions:
                passes.update({"dslhype-include-physics" : {"physics-file": self.polygeist_function_source_file}})
                passes.update({"inline" : {}})
                passes.update({"canonicalize" : {}})
    
            
            passes.update({"cse" : {}})
            passes.update({"loop-invariant-code-motion" : {}})
            passes.update({"canonicalize" : {}})
            passes.update({"expand-strided-metadata" : {}})
            passes.update({"fold-memref-alias-ops" : {}})
            passes.update({"lower-affine" : {}})
            passes.update({"loop-invariant-code-motion" : {}})
            passes.update({"canonicalize" : {}})
            passes.update({"cse" : {}})
            passes.update({"convert-scf-to-cf" : {}})

            if self.use_omp:
                passes.update({"convert-openmp-to-llvm" : {}})
    
            passes.update({"convert-cf-to-llvm" : {}})
            passes.update({"convert-arith-to-llvm" : {}})
            passes.update({"convert-math-to-llvm" : {}})
            passes.update({"convert-func-to-llvm" : {}})
            passes.update({"finalize-memref-to-llvm" : {"index-bitwidth": "64"}})
            passes.update({"reconcile-unrealized-casts" : {}})
            passes.update({"canonicalize" : {}})
            passes.update({"cse" : {}})

        pass_names = list(passes.keys())
        initial_dslhype_opt_passes = []
        index = 0

        for pass_name in pass_names:
            if pass_name in PipelineBuilder.xdsl_passes:
                break
            initial_dslhype_opt_passes.append(self.generate_pass_string(passes, pass_name))
            index = index + 1

        xdsl_passes_processed = False
        pipeline_command = f"""dslhype-opt --mlir-print-op-generic --pass-pipeline="builtin.module({",".join(initial_dslhype_opt_passes)})" $<"""
        while index < len(pass_names):
            next_set_of_passes = []
            if xdsl_passes_processed:
                for pass_name in pass_names[index:]:
                    if pass_name in PipelineBuilder.xdsl_passes:
                        break
                    next_set_of_passes.append(self.generate_pass_string(passes, pass_name))
                    index = index + 1
                pipeline_command = f"""{pipeline_command} | dslhype-opt --mlir-print-op-generic --pass-pipeline="builtin.module({",".join(next_set_of_passes)})" """
                xdsl_passes_processed = False
            else:
                for pass_name in pass_names[index:]:
                    if pass_name not in PipelineBuilder.xdsl_passes:
                        break
                    next_set_of_passes.append('"' + self.generate_pass_string(passes, pass_name) + '"')
                    index = index + 1
                pipeline_command = f"""{pipeline_command} | xdsl-opt -p {",".join(next_set_of_passes)},strip-visibility-property"""
                xdsl_passes_processed = True

        return pipeline_command

    def enable_gpu(self, gpu_vendor, gpu_architecture, polygeist_function_source_file):
        self.use_gpu = True
        self.gpu_vendor = gpu_vendor
        self.gpu_architecture = gpu_architecture
        
        # GPU usage requires flattened memrefs and inlined functions
        self.enable_flatten_memrefs()
        self.enable_inline_polygeist_functions(polygeist_function_source_file)

    def enable_omp(self, use_tasks=False, omp_task_version="taskloop", omp_single_region=False):
        self.use_omp = True
        self.use_omp_tasks = use_tasks
        self.omp_task_version=omp_task_version
        self.omp_single_region=omp_single_region

    def enable_flatten_memrefs(self):
        self.flatten_memrefs = True

    def enable_memory_fusion(self):
        self.fuse_memory_allocations = True

    def enable_memory_reuse(self, max_patch_size):
        self.memory_reuse = True
        self.max_patch_size = max_patch_size

    def enable_inline_polygeist_functions(self, polygeist_function_source_file):
        self.inline_polygeist_functions = True
        self.polygeist_function_source_file = polygeist_function_source_file

    def enable_imperfect_nested_loops_to_perfect(self):
        self.imperfect_nested_loops_to_perfect = True

    def enable_loop_fusion(self):
        self.loop_fusion = True

    def enable_nested_parallel_ops_to_multidimensional(self):
        self.enable_imperfect_nested_loops_to_perfect()
        self.nested_parallel_ops_to_multidimensional = True
    
    def enable_nested_parallel_ops_to_for_ops(self, max_nested_parallel_count=-1):
        self.nested_parallel_ops_to_for_ops = True
        self.max_nested_parallel_count = max_nested_parallel_count