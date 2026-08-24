import jinja2
import os
import exahype2.dslhype as DSL

def create_kernel_definitions(
    function, 
    template_parameters, 
    functor_arguments, 
    solver_filename, 
    solver_includes, 
    solver_namespace,
    solver_dictionary
):
  cpp_trees = []
  mlir_trees = []

  cpp_trees.append(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace))
  cpp_trees.append(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, stateless = True))
  cpp_trees.append(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, stateless = True, use_accelerator=True, use_memory_manager=True))

  if not os.path.exists("kernels"):
      os.makedirs("kernels")
  
  declaration_file = open(f"kernels/{solver_filename}.h", "w")
  definition_file = open(f"kernels/{solver_filename}.cpph", "w")
  declaration_file.write("#pragma once\n")
  declaration_file.write(solver_includes)
  for i, tree in enumerate(cpp_trees):
    if not tree.use_accelerator:
      declaration_file.write(tree.print_declaration_cpp())
      declaration_file.write(tree.print_declaration_with_timer_cpp())
      definition_file.write(tree.print_cpp())
      definition_file.write(tree.print_definition_with_timer_cpp())

    if tree.use_accelerator:
      declaration_file.write("#if defined (SharedOMP)\n")
      definition_file.write("#if defined (SharedOMP)\n")
    else:
      declaration_file.write("#if defined (GPUOffloadingOMP)\n")
      definition_file.write("#if defined (GPUOffloadingOMP)\n")
    declaration_file.write(tree.print_declaration_omp())
    declaration_file.write(tree.print_declaration_with_timer_omp())
    definition_file.write(tree.print_omp())
    definition_file.write(tree.print_definition_with_timer_omp())
    declaration_file.write ("#endif\n")
    definition_file.write ("#endif\n")

    declaration_file.write("#if defined (GPUOffloadingSYCL)\n")
    definition_file.write("#if defined (GPUOffloadingSYCL)\n")
    declaration_file.write(tree.print_declaration_sycl())
    declaration_file.write(tree.print_declaration_with_timer_sycl())
    definition_file.write(tree.print_sycl())
    definition_file.write(tree.print_definition_with_timer_sycl())
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
  rusanov_mlir_function_definition = DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace)
  rusanov_mlir_stateless_function_definition = DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace, stateless=True)
  gpu_namespace = solver_namespace.copy()
  gpu_namespace = gpu_namespace + ["mlir", "gpu"]
  rusanov_mlir_gpu_function_definition = DSL.Parser().parse(function, template_parameters, functor_arguments, gpu_namespace, stateless=True, use_accelerator=True)

  mlir_cpu_function_definitions = [
    rusanov_mlir_function_definition,
    rusanov_mlir_stateless_function_definition
  ]

  file = open(f"kernels/{solver_filename}.mlir", "w")
  file.write(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace).print_mlir())
  file.close()

  file = open(f"kernels/{solver_filename}_omp.mlir", "w")
  file.write(DSL.Parser().parse(function, template_parameters, functor_arguments, solver_namespace).print_mlir(use_omp=True))
  file.close()

  # Generate MLIR bridge files
  with open("MLIRbridge.cpph", 'w') as file:
    file.write("""
#include "MemRefDescriptor.cpph"
// MLIR-compatible structure using memref descriptors
struct MLIRCellData {
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
};
void convertToMLIRStruct(exahype2::CellData<double, double>* patchData, MLIRCellData* mlir_data);
void convertToMLIRStructGPU(exahype2::CellData<double, double>* copyCellData, MLIRCellData* mlir_data);
void cleanupMLIRStruct(MLIRCellData* mlir_data);
void cleanupMLIRStructGPU(MLIRCellData* mlir_data);
void copybackMLIRresults(exahype2::CellData<double, double>* patchData, MLIRCellData* mlir_data);
""")
    file.write("""extern "C" void timeStepWithRusanov_MLIR_wrapper(exahype2::CellData<double, double>* patchData, bool use_omp);\n""")
    for definition in mlir_cpu_function_definitions:
      definition._arguments[0]._type = DSL.SyntaxTree.TCustom("CellData<double, double>&")
      file.write(definition.print_declaration_mlir())
      file.write(definition.print_declaration_with_timer_mlir())
      file.write(definition.print_declaration_mlir(use_omp=True))
      file.write(definition.print_declaration_with_timer_mlir(use_omp=True))
      definition._arguments[0]._type = DSL.SyntaxTree.TCustom("MLIRCellData*")
      file.write(definition.print_declaration_mlir())
      file.write(definition.print_declaration_with_timer_mlir())
      file.write(definition.print_declaration_mlir(use_omp=True))
      file.write(definition.print_declaration_with_timer_mlir(use_omp=True))

    file.write("#ifdef MLIR_GPU\n")
    rusanov_mlir_gpu_function_definition._arguments[1]._type = DSL.SyntaxTree.TCustom("MLIRCellData*")
    file.write(rusanov_mlir_gpu_function_definition.print_declaration_cpp())
    file.write(rusanov_mlir_gpu_function_definition.print_declaration_with_timer_cpp())
    file.write("#endif")
  
  
  template_strings = [argument.print_cpp() for argument in rusanov_mlir_function_definition._template]
  solver_dictionary["TEMPLATE_DEFINITION"] = "template <" + ",".join(template_strings) + ">\n"
  filepath = os.path.realpath(__file__)
  filepath_root = filepath[0:filepath.find("dslhype/")]
  with open(filepath_root + "dslhype/MLIRbridge.template.cpp", 'r') as template_file:
    mlir_bridge_template = template_file.read()
  mlir_bridge_cpp = jinja2.Template(mlir_bridge_template).render(**solver_dictionary)
  with open("MLIRbridge.cpp", 'w') as file:
    file.write(mlir_bridge_cpp)
