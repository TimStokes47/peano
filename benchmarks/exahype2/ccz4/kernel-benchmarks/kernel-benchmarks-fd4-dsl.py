# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os
import subprocess
import argparse

import peano4.api
import exahype2

from CCZ4Solver import CCZ4Solver_FD4_GlobalAdaptiveTimeStepWithEnclaveTasking

parser = argparse.ArgumentParser(
    description="ExaHyPE 2 - CCZ4 FD4 Kernel Benchmarking Script"
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
    choices = [2, 3],
    type=int,
    help="Number of dimensions to use: 2 or 3",
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
    "-j",
    default=-1,
    type=int,
    help="Number of parallel builds to use",
)

args = parser.parse_args()

fd4_solver = CCZ4Solver_FD4_GlobalAdaptiveTimeStepWithEnclaveTasking(
    name="FD4",
    patch_size=args.patch_size,
    min_meshcell_h=0.001,  # max_cell_size -> arbitrary value
    max_meshcell_h=0.001,  # min_cell_size -> arbitrary value
    pde_terms_without_state=True,
    rk_order=1
)

project = exahype2.Project(
    namespace=["benchmarks", "exahype2", "kernelbenchmarks"],
    project_name="KernelBenchmarksFD4DSL",
    directory=".",
    executable=f"KernelBenchmarksFD4DSL.{args.build_mode}",
)
fd4_solver.add_all_solver_constants()
project.add_solver(fd4_solver)
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
project.constants.define_value("KOSigma", str(8))

# MLIR Integration Support
if args.enable_mlir:
    print(f"Enabling MLIR kernel compilation support for {args.dimensions}D...")

    # Set environment variable for dimension detection
    import os
    os.environ['MLIR_DIMENSIONS'] = str(args.dimensions)

    # Add MLIR-specific files and flags
    project.output.makefile.add_cpp_file("MLIRbridge.cpp")
    project.output.makefile.add_CXX_flag("-DUSE_MLIR")
    
    # NOTE: As we are using LLVM toolings for MLIR, we link against LLVM's OpenMP library  
    project.output.makefile.add_linker_flag("-lomp")   
    project.output.makefile.add_linker_flag("-lomptarget") 
    if args.gpu:
      print("Enabling MLIR GPU kernel compilation support")
      project.output.makefile.d["MLIR_GPU"] = True
      project.output.makefile.add_CXX_flag("-DMLIR_GPU")
      project.output.makefile.add_linker_flag("-ldl")
      project.output.makefile.add_linker_flag("-pthread")

      # NOTE: The current MLIR GPU linearization and lowering pipeline uses the base MLIR
      #       but may use the OMP version in future. Update here, as appropriate
      project.output.makefile.d["MLIR_SOURCES_GPU"] = f"fd4_{args.dimensions}d.mlir"
      project.output.makefile.d["MLIR_LL_FILE_GPU"] = f"fd4_{args.dimensions}d_gpu.ll" 
      project.output.makefile.d["MLIR_OBJECT_GPU"] = f"fd4_{args.dimensions}d_mlir_gpu.o"
      
      if project.output.makefile.d["GPU_VENDOR"] == "NVIDIA":
        project.output.makefile.d["MLIR_GPU_NVIDIA"] = True
        project.output.makefile.add_CXX_flag("-DMLIR_GPU_NVIDIA")
        CUDA_HOME = os.path.dirname(subprocess.check_output("which nvcc", shell=True).decode().strip())
        project.output.makefile.add_cpp_file("MLIRbridgeCUDAfunctions.cpp")
        project.output.makefile.add_linker_flag("-lcuda")
        project.output.makefile.add_linker_flag("-lcudart")
        project.output.makefile.add_linker_flag("-lmlir_cuda_runtime")
        project.output.makefile.add_linker_flag(f"-L{CUDA_HOME}/../targets/sbsa-linux/lib")
        cuda_bridge_header_file = "../../../../python/exahype2/dslhype/MLIRbridgeCUDAfunctions.h"
        cuda_bridge_file = "../../../../python/exahype2/dslhype/MLIRbridgeCUDAfunctions.cpp"
        os.system("cp {} MLIRbridgeCUDAfunctions.h".format(cuda_bridge_header_file))
        os.system("cp {} MLIRbridgeCUDAfunctions.cpp".format(cuda_bridge_file))
      elif project.output.makefile.d["GPU_VENDOR"] == "AMD":
        project.output.makefile.d["MLIR_GPU_AMD"] = True
        project.output.makefile.add_CXX_flag("-DMLIR_GPU_AMD")
        ROCM_HOME = os.path.dirname(subprocess.check_output("which hipcc", shell=True).decode().strip()) + "/.."
        os.environ["ROCM_HOME"] = ROCM_HOME
        project.output.makefile.add_cpp_file("MLIRbridgeHIPfunctions.cpp")
        project.output.makefile.add_CXX_flag("-D__HIP_PLATFORM_AMD__")
        project.output.makefile.add_linker_flag("-lamdhip64")
        project.output.makefile.add_CXX_flag(f"-I{ROCM_HOME}/include")
        project.output.makefile.add_linker_flag("-lmlir_rocm_runtime")
        hip_bridge_header_file = "../../../../python/exahype2/dslhype/MLIRbridgeHIPfunctions.h"
        hip_bridge_file = "../../../../python/exahype2/dslhype/MLIRbridgeHIPfunctions.cpp"
        os.system("cp {} MLIRbridgeHIPfunctions.h".format(hip_bridge_header_file))
        os.system("cp {} MLIRbridgeHIPfunctions.cpp".format(hip_bridge_file))
      else:
        raise RuntimeError("Unknown GPU vendor, please set it when configuring Peano with --with-gpu-vendor")
        
    project.output.makefile.d["MLIR_ENABLED"] = True
    project.output.makefile.d["MLIR_PHYSICS_FILE"] = f"ccz4_physics.mlir"
    project.output.makefile.d["MLIR_SOURCES"] = f"fd4_{args.dimensions}d.mlir"
    project.output.makefile.d["MLIR_LL_FILE"] = f"fd4_{args.dimensions}d.ll" 
    project.output.makefile.d["MLIR_OBJECT"] = f"fd4_{args.dimensions}d_mlir.o"
    project.output.makefile.d["MLIR_SOURCES_OMP"] = f"fd4_{args.dimensions}d_omp.mlir"
    project.output.makefile.d["MLIR_LL_FILE_OMP"] = f"fd4_{args.dimensions}d_omp.ll" 
    project.output.makefile.d["MLIR_OBJECT_OMP"] = f"fd4_{args.dimensions}d_mlir_omp.o"

    project.output.makefile.add_CXX_flag("-DASSESS_MLIR_KERNELS")

# CPP is enabled by default if MLIR is not enabled
if args.enable_cpp or not args.enable_mlir:
    project.output.makefile.add_CXX_flag("-DASSESS_CPP_KERNELS")

benchmark_main_file = "../../kernel-benchmarks/KernelBenchmarksFD4DSL-main.cpp"
os.system("cp {} KernelBenchmarksFD4DSL-main.cpp".format(benchmark_main_file))

project.output.makefile.add_CXX_flag("-DGRAV=9.81")

fd4_solver.add_makefile_parameters(
    project, "../../../../applications/exahype2/ccz4"
)
project.generate(throw_away_data_after_generation=False)
project.build(make=True, make_clean_first=True, throw_away_data_after_build=True, number_of_parallel_builds=args.j)

print(project)
print(fd4_solver)
