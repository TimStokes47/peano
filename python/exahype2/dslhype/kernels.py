import jinja2
import os
import subprocess
import exahype2.dslhype as DSL

def create_kernel_definitions(
    function, 
    template_parameters, 
    functor_arguments, 
    solver_filename, 
    solver_includes, 
    solver_namespace,
    solver_dictionary,
    enabled_pde_terms,
    use_mlir = True
):
  cpp_trees = []
  mlir_trees = []

  cpp_printer = DSL.printers.CPPPrinter(spaces_per_tab=4)
  omp_printer = DSL.printers.OMPPrinter(spaces_per_tab=4)
  sycl_printer = DSL.printers.SYCLPrinter(spaces_per_tab=4)
  if use_mlir:
    mlir_printer = DSL.printers.MLIRPrinter(spaces_per_tab=4, use_omp=False)
    mlir_omp_printer = DSL.printers.MLIRPrinter(spaces_per_tab=4, use_omp=True)

  cpp_trees.append(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms))
  cpp_trees.append(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms, stateless = True))
  cpp_trees.append(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms, stateless = True, use_accelerator=True, use_memory_manager=True))

  if not os.path.exists("kernels"):
      os.makedirs("kernels")
  
  declaration_file = open(f"kernels/{solver_filename}.h", "w")
  definition_file = open(f"kernels/{solver_filename}.cpph", "w")
  declaration_file.write("#pragma once\n")
  declaration_file.write(solver_includes)
  for i, tree in enumerate(cpp_trees):
    if not tree.use_accelerator:
      declaration_file.write(tree.print_declaration(cpp_printer))
      declaration_file.write(tree.print_declaration_with_timer(cpp_printer))
      definition_file.write(tree.print(cpp_printer))
      definition_file.write(tree.print_definition_with_timer(cpp_printer))

    if tree.use_accelerator:
      declaration_file.write("#if defined (GPUOffloadingOMP)\n")
      definition_file.write("#if defined (GPUOffloadingOMP)\n")
    else:
      declaration_file.write("#if defined (SharedOMP)\n")
      definition_file.write("#if defined (SharedOMP)\n")
    
    declaration_file.write(tree.print_declaration(omp_printer))
    declaration_file.write(tree.print_declaration_with_timer(omp_printer))
    definition_file.write(tree.print(omp_printer))
    definition_file.write(tree.print_definition_with_timer(omp_printer))
    declaration_file.write ("#endif\n")
    definition_file.write ("#endif\n")

    declaration_file.write("#if defined (GPUOffloadingSYCL)\n")
    definition_file.write("#if defined (GPUOffloadingSYCL)\n")
    declaration_file.write(tree.print_declaration(sycl_printer))
    declaration_file.write(tree.print_declaration_with_timer(sycl_printer))
    definition_file.write(tree.print(sycl_printer))
    definition_file.write(tree.print_definition_with_timer(sycl_printer))
    declaration_file.write ("#endif\n")
    definition_file.write ("#endif\n")

  declaration_file.write(f"""#ifdef USE_MLIR
#include "MLIRbridge.cpph"
#endif
#include "{solver_filename}.cpph"

""")
  declaration_file.close()
  definition_file.close()

  # These function definitions are used to generate declarations in the MLIR bridge files
  mlir_function_definition = DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms)
  mlir_stateless_function_definition = DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms, stateless=True)
  gpu_namespace = solver_namespace.copy()
  gpu_namespace = gpu_namespace + ["mlir", "gpu"]
  mlir_gpu_function_definition = DSL.Parser().parse(function, template_parameters, functor_arguments, gpu_namespace, enabled_pde_terms, stateless=True, use_accelerator=True)

  mlir_cpu_function_definitions = [
    mlir_function_definition,
    mlir_stateless_function_definition
  ]

  if use_mlir:
    file = open(f"kernels/{solver_filename}.mlir", "w")
    file.write(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms).print(mlir_printer))
    file.close()

    file = open(f"kernels/{solver_filename}_omp.mlir", "w")
    file.write(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, enabled_pde_terms).print(mlir_omp_printer))
    file.close()

    # Generate MLIR bridge files
    with open("MLIRbridge.cpph", 'w') as file:
      file.write(f"""
#include "MemRefDescriptor.cpph"
{solver_includes}
// MLIR-compatible structure using memref descriptors
struct MLIRCellData {{
    MemRefDescriptor2D QIn;             // Field 0
    MemRefDescriptor2D QOut;            // Field 1  
    MemRefDescriptorNested cellCentre;  // Field 2
    MemRefDescriptorNested cellSize;    // Field 3
    MemRefDescriptor1D t;               // Field 4
    MemRefDescriptor1D dt;              // Field 5
    MemRefDescriptor1D id;              // Field 6
    int64_t numberOfCells;              // Field 7
    int64_t memoryLocation;             // Field 8
    int64_t targetDevice;               // Field 9
    double** QOut_legacy;               // Field 10
    double* maxEigenvalue;              // Field 11
}};
void convertToMLIRStruct(exahype2::CellData<double, double>* patchData, MLIRCellData* mlir_data);
void convertToMLIRStructGPU(exahype2::CellData<double, double>* copyCellData, MLIRCellData* mlir_data);
void cleanupMLIRStruct(MLIRCellData* mlir_data);
void cleanupMLIRStructGPU(MLIRCellData* mlir_data);
void copybackMLIRresults(exahype2::CellData<double, double>* patchData, MLIRCellData* mlir_data);
""")
      file.write(f"""extern "C" void {solver_dictionary["KERNEL_NAME"]}_MLIR_wrapper(exahype2::CellData<double, double>* patchData, bool use_omp);\n""")
      for definition in mlir_cpu_function_definitions:
        definition._arguments[0]._type = DSL.SyntaxTree.TCustom("CellData<double, double>&")
        file.write(definition.print_declaration(mlir_printer))
        file.write(definition.print_declaration_with_timer(mlir_printer))
        file.write(definition.print_declaration(mlir_omp_printer))
        file.write(definition.print_declaration_with_timer(mlir_omp_printer))
        definition._arguments[0]._type = DSL.SyntaxTree.TCustom("MLIRCellData*")
        file.write(definition.print_declaration(mlir_printer))
        file.write(definition.print_declaration_with_timer(mlir_printer))
        file.write(definition.print_declaration(mlir_omp_printer))
        file.write(definition.print_declaration_with_timer(mlir_omp_printer))

      file.write("#ifdef MLIR_GPU\n")
      mlir_gpu_function_definition._arguments[1]._type = DSL.SyntaxTree.TCustom("MLIRCellData*")
      file.write(mlir_gpu_function_definition.print_declaration(cpp_printer))
      file.write(mlir_gpu_function_definition.print_declaration_with_timer(cpp_printer))
      file.write("#endif")
  
    argument_names = []
    memref_constructors = """"""
    for argument in functor_arguments:
      if argument._is_function:
        argument_names.append(argument.id + "_bridge")
      else:
        argument_names.append(argument.id + "_memref")
        memref_constructors += f"""
      auto {argument.id}_memref = MemRefDescriptor1D();
      {argument.id}_memref.allocated = const_cast<double*>({solver_dictionary["SOLVER_CLASS_NAMESPACE"]}::{solver_dictionary["SOLVER_CLASS"]}::{argument.id});
      {argument.id}_memref.aligned = const_cast<double*>({solver_dictionary["SOLVER_CLASS_NAMESPACE"]}::{solver_dictionary["SOLVER_CLASS"]}::{argument.id});
      {argument.id}_memref.offset = 0;
      {argument.id}_memref.sizes[0] = {solver_dictionary["SOLVER_CLASS_NAMESPACE"]}::{solver_dictionary["SOLVER_CLASS"]}::DGOrder + 1;
      {argument.id}_memref.strides[0] = 1;
"""
    solver_dictionary["FUNCTION_ARGUMENTS"] = ",".join(argument_names)
    solver_dictionary["MEMREF_CONSTRUCTORS"] = memref_constructors

    template_strings = [argument.print(cpp_printer) for argument in mlir_function_definition._template]
    solver_dictionary["TEMPLATE_DEFINITION"] = "template <" + ",".join(template_strings) + ">\n"
    filepath = os.path.realpath(__file__)
    filepath_root = filepath[0:filepath.find("dslhype/")]
    with open(filepath_root + "dslhype/MLIRbridge.template.cpp", 'r') as template_file:
      mlir_bridge_template = template_file.read()
    mlir_bridge_cpp = jinja2.Template(mlir_bridge_template).render(**solver_dictionary)
    with open("MLIRbridge.cpp", 'w') as file:
      file.write(mlir_bridge_cpp)

def set_mlir_environment(project, filename_root, physics_file, dimensions, use_gpu, flatten_memrefs=False, inline_functions=False, nested_parallel_ops_to_for_ops=-1, imperfect_nested_loops_to_perfect=False, nested_parallel_ops_to_multidimensional=False, use_omp_tasks=False, omp_task_version="taskloop", omp_single_region=False, max_patch_size=-1, use_memory_fusion=False, use_loop_fusion=False):
  print(f"Enabling MLIR kernel compilation support for {dimensions}D...")

  serial_pipeline_builder = DSL.PipelineBuilder()
  if flatten_memrefs:
    serial_pipeline_builder.enable_flatten_memrefs()
  if inline_functions:
    serial_pipeline_builder.enable_inline_polygeist_functions(physics_file)
  if imperfect_nested_loops_to_perfect:
    serial_pipeline_builder.enable_imperfect_nested_loops_to_perfect()
  if nested_parallel_ops_to_multidimensional:
    serial_pipeline_builder.enable_nested_parallel_ops_to_multidimensional()
  if nested_parallel_ops_to_for_ops > -1:
    serial_pipeline_builder.enable_nested_parallel_ops_to_for_ops(nested_parallel_ops_to_for_ops)
  if max_patch_size > -1:
    serial_pipeline_builder.enable_memory_reuse(max_patch_size)
  if use_memory_fusion:
    serial_pipeline_builder.enable_memory_fusion()
  if use_loop_fusion:
    serial_pipeline_builder.enable_loop_fusion()
  
  serial_pipeline = serial_pipeline_builder.generate_opt_commands()
  project.output.makefile.d["SERIAL_PIPELINE"] = serial_pipeline

  omp_pipeline_builder = DSL.PipelineBuilder()
  omp_pipeline_builder.enable_omp(use_omp_tasks, omp_task_version, omp_single_region)
  if flatten_memrefs:
    omp_pipeline_builder.enable_flatten_memrefs()
  if inline_functions:
    omp_pipeline_builder.enable_inline_polygeist_functions(physics_file)
  if imperfect_nested_loops_to_perfect:
    omp_pipeline_builder.enable_imperfect_nested_loops_to_perfect()
  if nested_parallel_ops_to_multidimensional:
    omp_pipeline_builder.enable_nested_parallel_ops_to_multidimensional()
  if nested_parallel_ops_to_for_ops > -1:
    omp_pipeline_builder.enable_nested_parallel_ops_to_for_ops(nested_parallel_ops_to_for_ops)
  if max_patch_size > -1:
    omp_pipeline_builder.enable_memory_reuse(max_patch_size)
  if use_memory_fusion:
    omp_pipeline_builder.enable_memory_fusion()
  if use_loop_fusion:
    omp_pipeline_builder.enable_loop_fusion()
  omp_pipeline = omp_pipeline_builder.generate_opt_commands()
  project.output.makefile.d["OMP_PIPELINE"] = omp_pipeline

  # Set environment variable for dimension detection
  os.environ['MLIR_DIMENSIONS'] = str(dimensions)
  filepath = os.path.realpath(__file__)
  peano_dir = filepath[0:filepath.find("python/")]

  # Add MLIR-specific files and flags
  project.output.makefile.add_cpp_file("MLIRbridge.cpp")
  project.output.makefile.add_CXX_flag("-DUSE_MLIR")
  
  # NOTE: As we are using LLVM toolings for MLIR, we link against LLVM's OpenMP library  
  project.output.makefile.add_linker_flag("-lomp")   
  if use_gpu:
    print("Enabling MLIR GPU kernel compilation support")
    project.output.makefile.add_linker_flag("-lomptarget") 
    gpu_pipeline_builder = DSL.PipelineBuilder()
    gpu_pipeline_builder.enable_gpu(project.output.makefile.d["GPU_VENDOR"], project.output.makefile.d["GPU_ARCH"], physics_file)
    if imperfect_nested_loops_to_perfect:
      gpu_pipeline_builder.enable_imperfect_nested_loops_to_perfect()
    if nested_parallel_ops_to_multidimensional:
      gpu_pipeline_builder.enable_nested_parallel_ops_to_multidimensional()
    if nested_parallel_ops_to_for_ops > -1:
      gpu_pipeline_builder.enable_nested_parallel_ops_to_for_ops(nested_parallel_ops_to_for_ops)
    if max_patch_size > -1:
      gpu_pipeline_builder.enable_memory_reuse(max_patch_size)
    if use_memory_fusion:
      gpu_pipeline_builder.enable_memory_fusion()
    if use_loop_fusion:
      gpu_pipeline_builder.enable_loop_fusion()
    project.output.makefile.d["GPU_PIPELINE"] = gpu_pipeline_builder.generate_opt_commands()

    project.output.makefile.d["MLIR_GPU"] = True
    project.output.makefile.add_CXX_flag("-DMLIR_GPU")
    project.output.makefile.add_linker_flag("-ldl")
    project.output.makefile.add_linker_flag("-pthread")

    # NOTE: The current MLIR GPU linearization and lowering pipeline uses the base MLIR
    #       but may use the OMP version in future. Update here, as appropriate
    project.output.makefile.d["MLIR_SOURCES_GPU"] = f"{filename_root}{dimensions}d.mlir"
    project.output.makefile.d["MLIR_LL_FILE_GPU"] = f"{filename_root}{dimensions}d_gpu.ll" 
    project.output.makefile.d["MLIR_OBJECT_GPU"] = f"{filename_root}{dimensions}d_mlir_gpu.o"
    
    if project.output.makefile.d["GPU_VENDOR"] == "NVIDIA":
      project.output.makefile.d["MLIR_GPU_NVIDIA"] = True
      project.output.makefile.add_CXX_flag("-DMLIR_GPU_NVIDIA")
      CUDA_HOME = os.path.dirname(subprocess.check_output("which nvcc", shell=True).decode().strip())
      project.output.makefile.add_cpp_file("MLIRbridgeCUDAfunctions.cpp")
      project.output.makefile.add_linker_flag("-lcuda")
      project.output.makefile.add_linker_flag("-lcudart")
      project.output.makefile.add_linker_flag("-lmlir_cuda_runtime")
      project.output.makefile.add_linker_flag(f"-L{CUDA_HOME}/../targets/sbsa-linux/lib")
      #peano_dir = 
      cuda_bridge_header_file = f"{peano_dir}python/exahype2/dslhype/MLIRbridgeCUDAfunctions.h"
      cuda_bridge_file = f"{peano_dir}python/exahype2/dslhype/MLIRbridgeCUDAfunctions.cpp"
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
      hip_bridge_header_file = f"{peano_dir}python/exahype2/dslhype/MLIRbridgeHIPfunctions.h"
      hip_bridge_file = f"{peano_dir}python/exahype2/dslhype/MLIRbridgeHIPfunctions.cpp"
      os.system("cp {} MLIRbridgeHIPfunctions.h".format(hip_bridge_header_file))
      os.system("cp {} MLIRbridgeHIPfunctions.cpp".format(hip_bridge_file))
    else:
      raise RuntimeError("Unknown GPU vendor, please set it when configuring Peano with --with-gpu-vendor. Also set the GPU architecture with --with-gpu-arch.")
      
  project.output.makefile.d["MLIR_ENABLED"] = True
  project.output.makefile.d["MLIR_PHYSICS_FILE"] = f"{physics_file}"
  project.output.makefile.d["MLIR_SOURCES"] = f"{filename_root}{dimensions}d.mlir"
  project.output.makefile.d["MLIR_LL_FILE"] = f"{filename_root}{dimensions}d.ll" 
  project.output.makefile.d["MLIR_OBJECT"] = f"{filename_root}{dimensions}d_mlir.o"
  project.output.makefile.d["MLIR_SOURCES_OMP"] = f"{filename_root}{dimensions}d_omp.mlir"
  project.output.makefile.d["MLIR_LL_FILE_OMP"] = f"{filename_root}{dimensions}d_omp.ll" 
  project.output.makefile.d["MLIR_OBJECT_OMP"] = f"{filename_root}{dimensions}d_mlir_omp.o"