# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os
import subprocess
import argparse
import jinja2

import peano4.api
import exahype2
import exahype2.dslhype as DSL

from CCZ4Solver import CCZ4Solver_FV_GlobalAdaptiveTimeStepWithEnclaveTasking

parser = argparse.ArgumentParser(
    description="ExaHyPE 2 - CCZ4 Finite Volume Rusanov Kernel Benchmarking Script"
)
parser.add_argument(
    "-m",
    "--build-mode",
    choices=peano4.api.output.CompileModes,
    default=peano4.api.output.CompileModes[0], # Release
    help="|".join(peano4.api.output.CompileModes),
)
parser.add_argument(
    "-t",
    "--num-threads",
    default=1,
    type=int,
    help="Number of launching threads",
)
parser.add_argument(
    "-ps",
    "--patch-size",
    default=16,
    type=int,
    help="Number of finite volumes per axis (dimension) per patch.",
)
parser.add_argument(
    "-p",
    "--num-patches",
    nargs="+",
    default=[32, 64, 128, 256, 512, 1024, 2048],
    type=int,
    help="Number of patches to study",
)
parser.add_argument(
    "-samples",
    "--samples",
    default=10,
    type=int,
    help="Number of samples per measurement",
)
parser.add_argument(
    "-a",
    "--accuracy",
    default=0.0,
    type=float,
    help="Floating point accuracy to which the different kernel variants have to match (absolute). Pass in 0 to disable correctness check. Pass in values < 0 to use machine epsilon (default).",
)
parser.add_argument(
    "-e",
    "--eval-eig",
    action="store_false",
    help="Evaluate max. eigenvalue",
)
parser.add_argument(
    "-cpu",
    "--cpu",
    action="store_true",
    help="Assess host kernels",
)
parser.add_argument(
    "-gpu",
    "--gpu",
    action="store_true",
    help="Assess device kernels",
)
parser.add_argument(
    "-serial",
    "--serial",
    action="store_true",
    help="Assess serial kernels",
)
parser.add_argument(
    "-parallel",
    "--parallel",
    action="store_true",
    help="Assess parallel kernels",
)
parser.add_argument(
    "-fpe",
    "--fpe",
    action="store_true",
    help="Enable a floating-point exception handler.",
)
parser.add_argument(
    "-d",
    "--dimensions",
    default=3,
    choices = [3],
    type=int,
    help="Number of dimensions to use: only 3 makes sense for this application",
)
parser.add_argument(
    "--enable-mlir",
    dest="enable_mlir",
    action="store_true",
    help="Enable MLIR kernel compilation support",
)
parser.add_argument(
    "--enable-cpp",
    dest="enable_cpp",
    action="store_true",
    help="Enable CPP kernels",
)
parser.add_argument(
    "--enable-memref-flattening",
    dest="enable_memref_flattening",
    action="store_true",
    help="Enable CPP kernels",
)
parser.add_argument(
    "--enable-function-inlining",
    dest="enable_function_inlining",
    action="store_true",
    help="Enable inlining of Polygeist functions",
)
parser.add_argument(
    "--enable-memory-reuse",
    dest="enable_memory_reuse",
    action="store_true",
    help="Enable reuse of scratchpad memory allocations",
)
parser.add_argument(
    "--enable-memory-fusion",
    dest="enable_memory_fusion",
    action="store_true",
    help="Enable fusion of allocations into one",
)
parser.add_argument(
    "--enable-loop-fusion",
    dest="enable_loop_fusion",
    action="store_true",
    help="Enable fusion of loops with the same bounds",
)
parser.add_argument(
    "--enable-imperfect-nested-loops-to-perfect",
    dest="enable_imperfect_nested_loops_to_perfect",
    action="store_true",
    help="Enable conversion of imperfectly nested parallels into a perfectly nested parallels",
)
parser.add_argument(
    "--enable-nested-parallel-ops-to-multidimensional",
    dest="enable_nested_parallel_ops_to_multidimensional",
    action="store_true",
    help="Enable conversion of nested parallels into a single multidimensional operation",
)
parser.add_argument(
    "--enable-omp-tasks",
    dest="enable_omp_tasks",
    action="store_true",
    help="Enable use of OpenMP tasks as opposed to loop parallelism",
)
parser.add_argument(
    "--omp-task-version",
    choices=["task", "taskloop"],
    default="taskloop"
)
parser.add_argument(
    "--omp-single-region",
    action="store_true",
)
parser.add_argument(
    "--nested-parallel-count",
    default=-1,
    type=int,
    help="Number of dimensions to use: 2 or 3. Conversion is enabled if the count is positive.",
)
parser.add_argument(
    "-j",
    default=-1,
    type=int,
    help="Number of parallel builds to use",
)
parser.add_argument(
    "-o",
    default="KernelBenchmarksFVRusanovDSL",
    type=str,
    help="Name of final application",
)
args = parser.parse_args()

fv_rusanov_solver = CCZ4Solver_FV_GlobalAdaptiveTimeStepWithEnclaveTasking(
    name="FVRusanov",
    patch_size=args.patch_size,
    min_volume_h=0.001,  # max_cell_size -> arbitrary value
    max_volume_h=0.001,  # min_cell_size -> arbitrary value
    pde_terms_without_state=True
)

project = exahype2.Project(
    namespace=["benchmarks", "exahype2", "kernelbenchmarks"],
    project_name="KernelBenchmarksFVRusanovDSL",
    directory=".",
    executable=f"{args.o}.{args.build_mode}",
)
fv_rusanov_solver.add_all_solver_constants()
project.add_solver(fv_rusanov_solver)

project.set_global_simulation_parameters(
    dimensions=args.dimensions,
    size=[1.0] * args.dimensions,
    offset=[0.0] * args.dimensions,
    min_end_time=0.1,
    max_end_time=0.1,
    first_plot_time_stamp=0.0,
    time_in_between_plots=0.0,
    periodic_BC=[False] * args.dimensions
)

project.set_Peano4_installation("../../../../", mode=peano4.api.output.string_to_mode(args.build_mode))
project = project.generate_Peano4_project(verbose=True)

accuracy = args.accuracy
if accuracy < 0:
    import numpy
    accuracy = default = numpy.finfo(float).eps
project.constants.export_constexpr_with_type("Accuracy", str(accuracy), "double")

project.constants.export_constexpr_with_type("NumberOfSamples", str(args.samples), "int")

formatted_num_patches = "{{{}}}".format(", ".join(str(val) for val in args.num_patches))
project.constants.export_const_with_type("NumberOfPatchesToStudy", str(formatted_num_patches), "tarch::la::Vector<%s, int>" % len(args.num_patches))

project.constants.export_constexpr_with_type("NumberOfLaunchingThreads", str(args.num_threads), "int")

if args.fpe:
    project.constants.export_boolean("EnableFPE", True)
else:
    project.constants.export_boolean("EnableFPE", False)

# CPU is enabled by default if neither CPU nor GPU are specified
if args.cpu == False and args.gpu == False:
    project.output.makefile.add_CXX_flag("-DASSESS_HOST_KERNELS")
else:
    if args.cpu: project.output.makefile.add_CXX_flag("-DASSESS_HOST_KERNELS")
    if args.gpu: project.output.makefile.add_CXX_flag("-DASSESS_DEVICE_KERNELS")

if args.serial == False and args.parallel == False:
    project.output.makefile.add_CXX_flag("-DASSESS_SERIAL_KERNELS")
    project.output.makefile.add_CXX_flag("-DASSESS_PARALLEL_KERNELS")
else:
    if args.serial: project.output.makefile.add_CXX_flag("-DASSESS_SERIAL_KERNELS")
    if args.parallel: project.output.makefile.add_CXX_flag("-DASSESS_PARALLEL_KERNELS")

project.constants.export_boolean("EvaluateFlux", False)
project.constants.export_boolean("EvaluateNonconservativeProduct", True)
project.constants.export_boolean("EvaluateSource", True)
project.constants.export_boolean("EvaluateMaximumEigenvalueAfterTimeStep", True if args.eval_eig else False)  # Reduction

project.constants.define_value("GAMMA", str(1.0))

# MLIR Integration Support
if args.enable_mlir:
    if args.enable_memory_reuse:
        max_patch_size = (args.patch_size + 2) * (args.patch_size + 2) * (args.patch_size + 2) * 59
    else:
        max_patch_size = -1
    DSL.set_mlir_environment(project, "rusanov", f"ccz4_physics.mlir", args.dimensions, args.gpu, 
                                args.enable_memref_flattening, 
                                args.enable_function_inlining, 
                                args.nested_parallel_count, 
                                args.enable_imperfect_nested_loops_to_perfect, 
                                args.enable_nested_parallel_ops_to_multidimensional,
                                args.enable_omp_tasks,
                                args.omp_task_version,
                                args.omp_single_region,
                                max_patch_size,
                                args.enable_memory_fusion,
                                args.enable_loop_fusion
    )
    project.output.makefile.add_CXX_flag("-DASSESS_MLIR_KERNELS")

# CPP is enabled by default if MLIR is not enabled
if args.enable_cpp or not args.enable_mlir:
    project.output.makefile.add_CXX_flag("-DASSESS_CPP_KERNELS")


benchmark_main_file = "../../kernel-benchmarks/KernelBenchmarksFVRusanovDSL-main.cpp"
os.system("cp {} KernelBenchmarksFVRusanovDSL-main.cpp".format(benchmark_main_file))

project.output.makefile.add_CXX_flag("-DGRAV=9.81")

fv_rusanov_solver.add_makefile_parameters(
    project, "../../../../applications/exahype2/ccz4"
)
project.generate(throw_away_data_after_generation=False)
print("Building...", flush=True)
project.build(make=True, make_clean_first=True, throw_away_data_after_build=True, number_of_parallel_builds=args.j)

print(project)
print(fv_rusanov_solver, flush=True)

