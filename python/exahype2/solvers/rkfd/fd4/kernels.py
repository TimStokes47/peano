# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
from exahype2.solvers.PDETerms  import PDETerms

import jinja2

from enum import Enum
import os
import exahype2
import exahype2.dslhype as DSL

from exahype2.solvers.rkfd.kernels import SolverVariant    
from exahype2.solvers.rkfd.kernels import KernelVariant    

def create_FD4InExaGRyPE_kernel_definitions_3d(patch_size, unknowns, auxiliary_variables, ko_sigma, flux_implementation, ncp_implementation, source_term_implementation, compute_max_eigenvalue_of_next_time_step):
  function = exahype2.solvers.rkfd.fd4.timeStepWithFD4InExaGRyPE
  template_parameters = [DSL.SyntaxTree.Argument("NC", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("H", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("NumberOfUnknowns", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("NumberOfAuxiliaryVariables", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("KOsigma", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("EvaluateFlux", DSL.SyntaxTree.TBoolean()),
                         DSL.SyntaxTree.Argument("EvaluateNonconservativeProduct", DSL.SyntaxTree.TBoolean()),
                         DSL.SyntaxTree.Argument("EvaluateSource", DSL.SyntaxTree.TBoolean()),
                         DSL.SyntaxTree.Argument("copyOldTimeStepAndScaleWithTimeStepSize", DSL.SyntaxTree.TBoolean()), #not use actually
                         DSL.SyntaxTree.Argument("SecondOrderFormulation", DSL.SyntaxTree.TBoolean()),
                         DSL.SyntaxTree.Argument("FirstOrderFormulation", DSL.SyntaxTree.TBoolean())]

  functor_arguments = [DSL.SyntaxTree.Argument("flux", DSL.SyntaxTree.TCustom("const Flux&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()", is_function=True),
                       DSL.SyntaxTree.Argument("nonconservativeProduct", DSL.SyntaxTree.TCustom("const NonconservativeProduct&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()", is_function=True),
                       DSL.SyntaxTree.Argument("sourceTerm", DSL.SyntaxTree.TCustom("const Source&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, !llvm.ptr) -> ()", is_function=True),
                       DSL.SyntaxTree.Argument("maxEigenvalue", DSL.SyntaxTree.TCustom("const MaxEigenvalue&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()", is_function=True)]

  solver_includes = """#include "exahype2/CellData.h"
#include "exahype2/VolumeIndex.h"
#include "exahype2/fd/PatchUtils.h"
#include "exahype2/fd/Functors.h"
#include "peano4/utils/Loop.h"
#include "tarch/timing/Measurement.h"
#include "tarch/timing/Watch.h"
"""
  d = {}
  d["KERNEL_NAMESPACE"] = "exahype2::fd::fd4"
  d["SOLVER"] = "FD4"
  d["SOLVER_CLASS"] = "FD4"
  d["KERNEL_NAME"] = "timeStepWithFD4"
  d["SOLVER_INCLUDES"] = solver_includes
  d["SOLVER_CLASS_NAMESPACE"] = "benchmarks::exahype2::kernelbenchmarks"

  d["PATCH_SIZE"] = patch_size
  d["NUMBER_OF_UNKNOWNS"] = unknowns
  d["NUMBER_OF_AUXILIARY_VARIABLES"] = auxiliary_variables
  d["KOSIGMA"] = ko_sigma
  d["FLUX_IMPLEMENTATION"] = flux_implementation
  d["NCP_IMPLEMENTATION"] = ncp_implementation
  d["SOURCE_IMPLEMENTATION"] = source_term_implementation
  d["COMPUTE_MAX_EIGENVALUE"] = compute_max_eigenvalue_of_next_time_step
  d["HALO_SIZE"] = 3

  template_call_jinja = """
    {{SOLVER_CLASS_NAMESPACE}}::{{SOLVER_CLASS}},
    {{PATCH_SIZE}},
    {{HALO_SIZE}},
    {{NUMBER_OF_UNKNOWNS}},
    {{NUMBER_OF_AUXILIARY_VARIABLES}},
    static_cast<int>({{KOSIGMA}}),
    {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
    {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
    {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
    {% if COMPUTE_MAX_EIGENVALUE==False %} false {% else %} true {% endif %}
"""
  d["TEMPLATE_CALL"] = jinja2.Template(template_call_jinja).render(**d)

  template_value_definitions_jinja = """
    int NC = {{PATCH_SIZE}};
    int NumberOfUnknowns = {{NUMBER_OF_UNKNOWNS}};
    int NumberOfAuxiliaryVariables = {{NUMBER_OF_AUXILIARY_VARIABLES}};
    int H = {{HALO_SIZE}};
    int KOsigma = static_cast<int>({{KOSIGMA}});
    bool EvaluateFlux = {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %};
    bool EvaluateNonconservativeProduct = {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %};
    bool EvaluateSource = {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %};
    bool EvaluateMaximumEigenvalueAfterTimeStep = {% if COMPUTE_MAX_EIGENVALUE==False %} false {% else %} true {% endif %};
  """
  d["TEMPLATE_VALUE_DEFINITIONS"] = jinja2.Template(template_value_definitions_jinja).render(**d)

  bridge_functions_jinja = """
    void flux_bridge(void* Q_ptr, void* x_ptr, void* h_ptr, double t, double dt, int normal, void* F_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* F = static_cast<double*>(F_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::flux(Q, *x, *h, t, dt, normal, F, OffloadableType::Yes);
    }
    
    void nonconservativeProduct_bridge(void* Q_ptr, void* deltaQ_ptr, void* x_ptr, void* h_ptr, double t, double dt, int normal, void* result_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        double* deltaQ = static_cast<double*>(deltaQ_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* result = static_cast<double*>(result_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::nonconservativeProduct(Q, deltaQ, *x, *h, t, dt, normal, result, OffloadableType::Yes);
    }
    
    void sourceTerm_bridge(void* Q_ptr, void* x_ptr, void* h_ptr, double t, double dt, void* S_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* S = static_cast<double*>(S_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::sourceTerm(Q, *x, *h, t, dt, S, OffloadableType::Yes);
    }

    void maxEigenvalue_bridge(void* Q_ptr, void* x_ptr, void* h_ptr, double t, double dt, int normal, void* result_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* result = static_cast<double*>(result_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::maxEigenvalue(Q, *x, *h, t, dt, normal, result, OffloadableType::Yes);
    }
"""
  d["BRIDGE_FUNCTIONS"] = jinja2.Template(bridge_functions_jinja).render(**d)

  d["EXTERN_FUNCTION_ARGUMENT_DEFINITIONS"] = """
    void (*flux_fn)(void*, void*, void*, double, double, int, void*),
    void (*ncp_fn)(void*, void*, void*, void*, double, double, int, void*),
    void (*source_fn)(void*, void*, void*, double, double, void*),
    void (*maxeig_fn)(void*, void*, void*, double, double, int, void*)
"""
  enabled_pde_terms = []
  if flux_implementation is not None and flux_implementation != exahype2.solvers.PDETerms.None_Implementation:
    enabled_pde_terms.append("EvaluateFlux")
  if source_term_implementation is not None and source_term_implementation != exahype2.solvers.PDETerms.None_Implementation:
    enabled_pde_terms.append("EvaluateSource")
  if ncp_implementation is not None and ncp_implementation != exahype2.solvers.PDETerms.None_Implementation:
    enabled_pde_terms.append("EvaluateNonconservativeProduct")
  if compute_max_eigenvalue_of_next_time_step:
    enabled_pde_terms.append("EvaluateMaximumEigenvalueAfterTimeStep")
  
  DSL.create_kernel_definitions(
    function,
    template_parameters,
    functor_arguments,
    "FD43d",
    solver_includes,
    ["exahype2", "fd", "fd4"],
    d,
    enabled_pde_terms,
    use_mlir=False
  )

def create_DeriCalInExaGRyPE_kernel_definitions_3d():
  template_parameters = [DSL.SyntaxTree.Argument("NC", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("H", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("NumberOfUnknowns", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("NumberOfAuxiliaryVariables", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("SecondOrderFormulation", DSL.SyntaxTree.TBoolean())]

  functor_arguments = []

  DeriCallInExaGRyPE_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"])
  DeriCallInExaGRyPE_kernel = DeriCallInExaGRyPE_tree.print_cpp()
  DeriCallInExaGRyPE_call_with_measurement = DeriCallInExaGRyPE_tree.print_definition_with_timer()
  DeriCallInExaGRyPE_kernel_declaration = DeriCallInExaGRyPE_tree.print_declaration()
  DeriCallInExaGRyPE_call_with_measurement_declaration = DeriCallInExaGRyPE_tree.print_declaration_with_timer()

  DeriCallInExaGRyPE_stateless_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"], stateless=True)
  DeriCallInExaGRyPE_stateless_kernel = DeriCallInExaGRyPE_stateless_tree.print_cpp()
  DeriCallInExaGRyPE_stateless_call_with_measurement = DeriCallInExaGRyPE_stateless_tree.print_definition_with_timer()
  DeriCallInExaGRyPE_stateless_kernel_declaration = DeriCallInExaGRyPE_stateless_tree.print_declaration()
  DeriCallInExaGRyPE_stateless_call_with_measurement_declaration = DeriCallInExaGRyPE_stateless_tree.print_declaration_with_timer()

  #DeriCallInExaGRyPE_accelerator_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"], True, True).print_cpp()
  #DeriCallInExaGRyPE_accelerator_kernel = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"], True, True).print_cpp()
  #DeriCallInExaGRyPE_accelerator_call_with_measurement = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"], True, True).print_definition_with_timer()
  #DeriCallInExaGRyPE_accelerator_kernel_declaration = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"], True, True).print_declaration()
  #DeriCallInExaGRyPE_accelerator_call_with_measurement_declaration = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4"], True, True).print_declaration_with_timer()

  DeriCallInExaGRyPE_omp_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4", "omp"])
  DeriCallInExaGRyPE_omp_kernel = DeriCallInExaGRyPE_omp_tree.print_omp()
  DeriCallInExaGRyPE_omp_call_with_measurement = DeriCallInExaGRyPE_omp_tree.print_definition_with_timer()
  DeriCallInExaGRyPE_omp_kernel_declaration = DeriCallInExaGRyPE_omp_tree.print_declaration()
  DeriCallInExaGRyPE_omp_call_with_measurement_declaration = DeriCallInExaGRyPE_omp_tree.print_declaration_with_timer()

  DeriCallInExaGRyPE_omp_stateless_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4", "omp"], stateless=True)
  DeriCallInExaGRyPE_omp_stateless_kernel = DeriCallInExaGRyPE_omp_stateless_tree.print_omp()
  DeriCallInExaGRyPE_omp_stateless_call_with_measurement = DeriCallInExaGRyPE_omp_stateless_tree.print_definition_with_timer()
  DeriCallInExaGRyPE_omp_stateless_kernel_declaration = DeriCallInExaGRyPE_omp_stateless_tree.print_declaration()
  DeriCallInExaGRyPE_omp_stateless_call_with_measurement_declaration = DeriCallInExaGRyPE_omp_stateless_tree.print_declaration_with_timer()

  DeriCallInExaGRyPE_omp_accelerator_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4", "omp"], stateless=True, use_accelerator=True)
  DeriCallInExaGRyPE_omp_accelerator_kernel = DeriCallInExaGRyPE_omp_accelerator_tree.print_omp()
  DeriCallInExaGRyPE_omp_accelerator_call_with_measurement = DeriCallInExaGRyPE_omp_accelerator_tree.print_definition_with_timer()
  DeriCallInExaGRyPE_omp_accelerator_kernel_declaration = DeriCallInExaGRyPE_omp_accelerator_tree.print_declaration()
  DeriCallInExaGRyPE_omp_accelerator_call_with_measurement_declaration = DeriCallInExaGRyPE_omp_accelerator_tree.print_declaration_with_timer()

  DeriCallInExaGRyPE_sycl_accelerator_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.DerivativesCalInExaGRyPE, template_parameters, functor_arguments, ["exahype2", "fd", "fd4", "sycl"], stateless=True, use_accelerator=True)
  DeriCallInExaGRyPE_sycl_accelerator_kernel = DeriCallInExaGRyPE_sycl_accelerator_tree.print_sycl()
  DeriCallInExaGRyPE_sycl_accelerator_call_with_measurement = DeriCallInExaGRyPE_sycl_accelerator_tree.print_definition_with_timer()
  DeriCallInExaGRyPE_sycl_accelerator_kernel_declaration = DeriCallInExaGRyPE_sycl_accelerator_tree.print_declaration()
  DeriCallInExaGRyPE_sycl_accelerator_call_with_measurement_declaration = DeriCallInExaGRyPE_sycl_accelerator_tree.print_declaration_with_timer()

  template_parameters = [DSL.SyntaxTree.Argument("NC", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("NumberOfUnknowns", DSL.SyntaxTree.TInteger()),
                         DSL.SyntaxTree.Argument("RKOrder", DSL.SyntaxTree.TInteger())]
  functor_arguments = [DSL.SyntaxTree.Argument("RKWeights", DSL.SyntaxTree.TCustom("const double*"))]

  ComputeLinearCombination_omp_accelerator_tree = DSL.Parser().parse(exahype2.solvers.rkfd.fd4.ComputeLinearCombination, template_parameters, functor_arguments, ["exahype2", "fd", "fd4", "omp"], stateless=True, use_accelerator=True)
  ComputeLinearCombination_omp_accelerator_kernel = ComputeLinearCombination_omp_accelerator_tree.print_omp()
  ComputeLinearCombination_omp_accelerator_call_with_measurement = ComputeLinearCombination_omp_accelerator_tree.print_definition_with_timer()
  ComputeLinearCombination_omp_accelerator_kernel_declaration = ComputeLinearCombination_omp_accelerator_tree.print_declaration()
  ComputeLinearCombination_omp_accelerator_call_with_measurement_declaration = ComputeLinearCombination_omp_accelerator_tree.print_declaration_with_timer()

  if not os.path.exists("kernels"):
      os.makedirs("kernels")
  
  file = open("kernels/DeriCallInExaGRyPE.h", "w")
  file.write(f"""#pragma once
#include "exahype2/CellData.h"
#include "exahype2/VolumeIndex.h"
#include "exahype2/fd/PatchUtils.h"
#include "exahype2/fd/Functors.h"
#include "peano4/utils/Loop.h"
#include "tarch/timing/Measurement.h"
#include "tarch/timing/Watch.h"
             
{DeriCallInExaGRyPE_kernel_declaration}
{DeriCallInExaGRyPE_call_with_measurement_declaration}
{DeriCallInExaGRyPE_stateless_kernel_declaration}
{DeriCallInExaGRyPE_stateless_call_with_measurement_declaration}

#if defined(SharedOMP)
{DeriCallInExaGRyPE_omp_kernel_declaration}
{DeriCallInExaGRyPE_omp_call_with_measurement_declaration}
{DeriCallInExaGRyPE_omp_stateless_kernel_declaration}
{DeriCallInExaGRyPE_omp_stateless_call_with_measurement_declaration}
#endif

#if defined(GPUOffloadingOMP)
{DeriCallInExaGRyPE_omp_accelerator_kernel_declaration}
{DeriCallInExaGRyPE_omp_accelerator_call_with_measurement_declaration}
{ComputeLinearCombination_omp_accelerator_kernel_declaration}
{ComputeLinearCombination_omp_accelerator_call_with_measurement_declaration}
#endif

#if defined(GPUOffloadingSYCL)
{DeriCallInExaGRyPE_sycl_accelerator_kernel_declaration}
{DeriCallInExaGRyPE_sycl_accelerator_call_with_measurement_declaration}
#endif

#include "DeriCallInExaGRyPE.cpph"
""")
  file.close()

  file = open("kernels/DeriCallInExaGRyPE.cpph", "w")
  file.write(DeriCallInExaGRyPE_kernel)
  file.write(DeriCallInExaGRyPE_call_with_measurement)
  file.write(DeriCallInExaGRyPE_stateless_kernel)
  file.write(DeriCallInExaGRyPE_stateless_call_with_measurement)

  file.write("#if defined(SharedOMP)\n")
  file.write(DeriCallInExaGRyPE_omp_kernel)
  file.write(DeriCallInExaGRyPE_omp_call_with_measurement)
  file.write(DeriCallInExaGRyPE_omp_stateless_kernel)
  file.write(DeriCallInExaGRyPE_omp_stateless_call_with_measurement)
  file.write("#endif\n")

  file.write("#if defined(GPUOffloadingOMP)\n")
  file.write(DeriCallInExaGRyPE_omp_accelerator_kernel)
  file.write(DeriCallInExaGRyPE_omp_accelerator_call_with_measurement)
  file.write(ComputeLinearCombination_omp_accelerator_kernel)
  file.write(ComputeLinearCombination_omp_accelerator_call_with_measurement)
  file.write("#endif\n")

  file.write("#if defined(GPUOffloadingSYCL)\n")
  file.write(DeriCallInExaGRyPE_sycl_accelerator_kernel)
  file.write(DeriCallInExaGRyPE_sycl_accelerator_call_with_measurement)
  file.write("#endif\n")
  file.close()

def create_FD4_kernel_definitions_3d(patch_size, unknowns, auxiliary_variables, ko_sigma, flux_implementation, ncp_implementation, source_term_implementation, compute_max_eigenvalue_of_next_time_step):
  function = exahype2.solvers.rkfd.fd4.timeStepWithFD4
  template_parameters = [DSL.SyntaxTree.Argument("NC", DSL.SyntaxTree.TInteger(), "applications::exahype2::ccz4::AbstractCCZ4"),
                         DSL.SyntaxTree.Argument("H", DSL.SyntaxTree.TInteger(), "applications::exahype2"),
                         DSL.SyntaxTree.Argument("NumberOfUnknowns", DSL.SyntaxTree.TInteger(), "applications::exahype2::ccz4::AbstractCCZ4"),
                         DSL.SyntaxTree.Argument("NumberOfAuxiliaryVariables", DSL.SyntaxTree.TInteger(), "applications::exahype2::ccz4::AbstractCCZ4"),
                         DSL.SyntaxTree.Argument("KOsigma", DSL.SyntaxTree.TInteger(), "applications::exahype2::ccz4::AbstractCCZ4"),
                         DSL.SyntaxTree.Argument("EvaluateFlux", DSL.SyntaxTree.TBoolean(), "applications::exahype2::ccz4"),
                         DSL.SyntaxTree.Argument("EvaluateNonconservativeProduct", DSL.SyntaxTree.TBoolean(), "applications::exahype2::ccz4"),
                         DSL.SyntaxTree.Argument("EvaluateSource", DSL.SyntaxTree.TBoolean(), "applications::exahype2::ccz4"),
                         DSL.SyntaxTree.Argument("copyOldTimeStepAndScaleWithTimeStepSize", DSL.SyntaxTree.TBoolean())] #not use actually

  functor_arguments = [DSL.SyntaxTree.Argument("flux", DSL.SyntaxTree.TCustom("const Flux&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()", is_function=True),
                       DSL.SyntaxTree.Argument("nonconservativeProduct", DSL.SyntaxTree.TCustom("const NonconservativeProduct&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()", is_function=True),
                       DSL.SyntaxTree.Argument("sourceTerm", DSL.SyntaxTree.TCustom("const Source&"), "(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, !llvm.ptr) -> ()", is_function=True)]
  
  solver_includes = """
#include "exahype2/CellData.h"
#include "exahype2/VolumeIndex.h"
#include "exahype2/fd/PatchUtils.h"
#include "exahype2/fd/Functors.h"
#include "peano4/utils/Loop.h"
#include "tarch/timing/Measurement.h"
#include "tarch/timing/Watch.h"
"""
  
  d = {}
  d["KERNEL_NAMESPACE"] = "exahype2::fd::fd4"
  d["SOLVER"] = "FD4"
  d["SOLVER_CLASS"] = "FD4"
  d["KERNEL_NAME"] = "timeStepWithFD4"
  d["SOLVER_INCLUDES"] = solver_includes
  d["SOLVER_CLASS_NAMESPACE"] = "benchmarks::exahype2::kernelbenchmarks"

  d["PATCH_SIZE"] = patch_size
  d["NUMBER_OF_UNKNOWNS"] = unknowns
  d["NUMBER_OF_AUXILIARY_VARIABLES"] = auxiliary_variables
  d["KOSIGMA"] = ko_sigma
  d["FLUX_IMPLEMENTATION"] = flux_implementation
  d["NCP_IMPLEMENTATION"] = ncp_implementation
  d["SOURCE_IMPLEMENTATION"] = source_term_implementation
  d["COMPUTE_MAX_EIGENVALUE"] = compute_max_eigenvalue_of_next_time_step
  d["HALO_SIZE"] = 3

  template_call_jinja = """
      {{SOLVER_CLASS_NAMESPACE}}::{{SOLVER_CLASS}},
      {{PATCH_SIZE}},
      {{HALO_SIZE}},
      {{NUMBER_OF_UNKNOWNS}},
      {{NUMBER_OF_AUXILIARY_VARIABLES}},
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if COMPUTE_MAX_EIGENVALUE==False %} false {% else %} true {% endif %}
    """
  d["TEMPLATE_CALL"] = jinja2.Template(template_call_jinja).render(**d)

  template_value_definitions_jinja = """
    int NC = {{PATCH_SIZE}};
    int NumberOfUnknowns = {{NUMBER_OF_UNKNOWNS}};
    int NumberOfAuxiliaryVariables = {{NUMBER_OF_AUXILIARY_VARIABLES}};
    int H = {{HALO_SIZE}};
    int KOsigma = static_cast<int>({{KOSIGMA}});
    bool EvaluateFlux = {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %};
    bool EvaluateNonconservativeProduct = {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %};
    bool EvaluateSource = {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %};
    bool EvaluateMaximumEigenvalueAfterTimeStep = {% if COMPUTE_MAX_EIGENVALUE==False %} false {% else %} true {% endif %};
  """
  d["TEMPLATE_VALUE_DEFINITIONS"] = jinja2.Template(template_value_definitions_jinja).render(**d)

  bridge_functions_jinja = """
    void flux_bridge(void* Q_ptr, void* x_ptr, void* h_ptr, double t, double dt, int normal, void* F_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* F = static_cast<double*>(F_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::flux(Q, *x, *h, t, dt, normal, F, OffloadableType::Yes);
    }
    
    void nonconservativeProduct_bridge(void* Q_ptr, void* deltaQ_ptr, void* x_ptr, void* h_ptr, double t, double dt, int normal, void* result_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        double* deltaQ = static_cast<double*>(deltaQ_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* result = static_cast<double*>(result_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::nonconservativeProduct(Q, deltaQ, *x, *h, t, dt, normal, result, OffloadableType::Yes);
    }
    
    void sourceTerm_bridge(void* Q_ptr, void* x_ptr, void* h_ptr, double t, double dt, void* S_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* S = static_cast<double*>(S_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::sourceTerm(Q, *x, *h, t, dt, S, OffloadableType::Yes);
    }

    void maxEigenvalue_bridge(void* Q_ptr, void* x_ptr, void* h_ptr, double t, double dt, int normal, void* result_ptr) {
        double* Q = static_cast<double*>(Q_ptr);
        tarch::la::Vector<Dimensions, double>* x = static_cast<tarch::la::Vector<Dimensions, double>*>(x_ptr);
        tarch::la::Vector<Dimensions, double>* h = static_cast<tarch::la::Vector<Dimensions, double>*>(h_ptr);
        double* result = static_cast<double*>(result_ptr);
        {{SOLVER_CLASS_NAMESPACE}}::Abstract{{SOLVER_CLASS}}::maxEigenvalue(Q, *x, *h, t, dt, normal, result, OffloadableType::Yes);
    }
"""
  d["BRIDGE_FUNCTIONS"] = jinja2.Template(bridge_functions_jinja).render(**d)

  d["EXTERN_FUNCTION_ARGUMENT_DEFINITIONS"] = """
    void (*flux_fn)(void*, void*, void*, double, double, int, void*),
    void (*ncp_fn)(void*, void*, void*, void*, double, double, int, void*),
    void (*source_fn)(void*, void*, void*, double, double, void*)
"""
  enabled_pde_terms = []
  if flux_implementation is not None and flux_implementation != exahype2.solvers.PDETerms.None_Implementation:
    enabled_pde_terms.append("EvaluateFlux")
  if source_term_implementation is not None and source_term_implementation != exahype2.solvers.PDETerms.None_Implementation:
    enabled_pde_terms.append("EvaluateSource")
  if ncp_implementation is not None and ncp_implementation != exahype2.solvers.PDETerms.None_Implementation:
    enabled_pde_terms.append("EvaluateNonconservativeProduct")
  if compute_max_eigenvalue_of_next_time_step:
    enabled_pde_terms.append("EvaluateMaximumEigenvalueAfterTimeStep")
  
  DSL.create_kernel_definitions(
    function,
    template_parameters,
    functor_arguments,
    "FD43d",
    solver_includes,
    ["exahype2", "fd", "fd4"],
    d,
    enabled_pde_terms
  )

def create_compute_kernel_for_FD4InExaGRyPE_DSL(flux_implementation, 
                                                ncp_implementation, 
                                                source_implementation, 
                                                compute_max_eigenvalue_of_next_time_step,
                                                solver_variant: SolverVariant,
                                                kernel_variant: KernelVariant,
                                                KOSigma,
                                                SecondOrder):
  """
  
  I return only the unqualified function call, i.e. without any namespaces.
  So by setting the right namespace as prefix, you can direct it to particular
  implementations.
  
  """  

  template  = "timeStepWithFD4InExaGRyPE"
  
  if solver_variant == SolverVariant.WithVirtualFunctions:
    template += """<
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}},
      {{NUMBER_OF_UNKNOWNS}},
      {{NUMBER_OF_AUXILIARY_VARIABLES}},
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      false, // Runge-Kutta, so no need to copy old data over or to take dt into account
      {% if SECONDORDER %} true {% else %} false {% endif %}, //if we have SO enabled, the quantities that get advections would be different
      {% if SECONDORDER %} false {% else %} true {% endif %} //if we have SO enabled, the quantities that get advections would be different
    >(patchData,
  [&](
    const double * __restrict__ Q,
    const tarch::la::Vector<Dimensions,double>&  faceCentre,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    int                                          normal,
    double * __restrict__ F
  )->void {
    {% if FLUX_IMPLEMENTATION!="<none>" %}
    repositories::{{SOLVER_INSTANCE}}.flux( Q, faceCentre, volumeH, t, dt, normal, F );
    {% endif %}
  },
  [&](
    const double * __restrict__                  Q,
    const double * __restrict__     deltaQ,
    const tarch::la::Vector<Dimensions,double>&  faceCentre,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    int                                          normal,
    double * __restrict__ BTimesDeltaQ
  )->void {
    {% if NCP_IMPLEMENTATION!="<none>" %}
    repositories::{{SOLVER_INSTANCE}}.nonconservativeProduct( Q, deltaQ, faceCentre, volumeH, t, dt, normal, BTimesDeltaQ );
    {% endif %}
  },
  [&](
    const double * __restrict__ Q,
    const tarch::la::Vector<Dimensions,double>&  volumeX,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    double * __restrict__ S
  )->void {
    {% if SOURCE_IMPLEMENTATION!="<none>" %}
    repositories::{{SOLVER_INSTANCE}}.sourceTerm( Q, volumeX, volumeH, t, dt, S );
    {% endif %}
  }
);
  """
  elif solver_variant == SolverVariant.Stateless:
    template += """Stateless<
      {{SOLVER_NAME}},
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}}, 
      {{NUMBER_OF_UNKNOWNS}}, 
      {{NUMBER_OF_AUXILIARY_VARIABLES}}, 
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      false, // Runge-Kutta, so no need to copy old data over or to take dt into account
      {% if SECONDORDER %} true {% else %} false {% endif %}, //if we have SO enabled, the quantities that get advections would be different
      {% if SECONDORDER %} false {% else %} true {% endif %} //if we have SO enabled, the quantities that get advections would be different
    >(patchData);
  """
  elif solver_variant == SolverVariant.AcceleratorWithExplicitCopy:
    template += """Stateless<
      {{SOLVER_NAME}},
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}}, 
      {{NUMBER_OF_UNKNOWNS}}, 
      {{NUMBER_OF_AUXILIARY_VARIABLES}}, 
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      false, // Runge-Kutta, so no need to copy old data over or to take dt into account
      {% if SECONDORDER %} true {% else %} false {% endif %}, //if we have SO enabled, the quantities that get advections would be different
      {% if SECONDORDER %} false {% else %} true {% endif %} //if we have SO enabled, the quantities that get advections would be different
    >(gpuPatchData.targetDevice, gpuPatchData);
  """
  else:
    assert False, "not supported combination: {} x {}".format( solver_variant, kernel_variant )

  result = jinja2.Template( template, undefined=jinja2.DebugUndefined)
  d= {}
  d[ "FLUX_IMPLEMENTATION" ]         = flux_implementation
  d[ "NCP_IMPLEMENTATION" ]          = ncp_implementation
  d[ "SOURCE_IMPLEMENTATION" ]       = source_implementation
  d[ "COMPUTE_MAX_EIGENVALUE" ]      = compute_max_eigenvalue_of_next_time_step
  d[ "KOSIGMA" ]                     = KOSigma
  d[ "SECONDORDER" ]                 = SecondOrder
  return result.render(**d)

def create_compute_kernel_for_FD4_DSL(flux_implementation, 
                                                ncp_implementation, 
                                                source_implementation, 
                                                compute_max_eigenvalue_of_next_time_step,
                                                solver_variant: SolverVariant,
                                                kernel_variant: KernelVariant,
                                                KOSigma):
  """
  
  I return only the unqualified function call, i.e. without any namespaces.
  So by setting the right namespace as prefix, you can direct it to particular
  implementations.
  
  """  

  template  = "timeStepWithFD4"
  
  if solver_variant == SolverVariant.WithVirtualFunctions:
    template += """<
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}},
      {{NUMBER_OF_UNKNOWNS}},
      {{NUMBER_OF_AUXILIARY_VARIABLES}},
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      false // Runge-Kutta, so no need to copy old data over or to take dt into account
    >(patchData,
  [&](
    const double * __restrict__ Q,
    const tarch::la::Vector<Dimensions,double>&  faceCentre,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    int                                          normal,
    double * __restrict__ F
  )->void {
    {% if FLUX_IMPLEMENTATION!="<none>" %}
    repositories::{{SOLVER_INSTANCE}}.flux( Q, faceCentre, volumeH, t, dt, normal, F );
    {% endif %}
  },
  [&](
    const double * __restrict__                  Q,
    const double * __restrict__     deltaQ,
    const tarch::la::Vector<Dimensions,double>&  faceCentre,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    int                                          normal,
    double * __restrict__ BTimesDeltaQ
  )->void {
    {% if NCP_IMPLEMENTATION!="<none>" %}
    repositories::{{SOLVER_INSTANCE}}.nonconservativeProduct( Q, deltaQ, faceCentre, volumeH, t, dt, normal, BTimesDeltaQ );
    {% endif %}
  },
  [&](
    const double * __restrict__ Q,
    const tarch::la::Vector<Dimensions,double>&  volumeX,
    const tarch::la::Vector<Dimensions,double>&  volumeH,
    double                                       t,
    double                                       dt,
    double * __restrict__ S
  )->void {
    {% if SOURCE_IMPLEMENTATION!="<none>" %}
    repositories::{{SOLVER_INSTANCE}}.sourceTerm( Q, volumeX, volumeH, t, dt, S );
    {% endif %}
  }
);
  """
  elif solver_variant == SolverVariant.Stateless:
    template += """Stateless<
      {{SOLVER_NAME}},
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}}, 
      {{NUMBER_OF_UNKNOWNS}}, 
      {{NUMBER_OF_AUXILIARY_VARIABLES}}, 
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      false // Runge-Kutta, so no need to copy old data over or to take dt into account
    >(patchData);
  """
  elif solver_variant == SolverVariant.AcceleratorWithExplicitCopy:
    template += """Stateless<
      {{SOLVER_NAME}},
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}}, 
      {{NUMBER_OF_UNKNOWNS}}, 
      {{NUMBER_OF_AUXILIARY_VARIABLES}}, 
      static_cast<int>({{KOSIGMA}}),
      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
      false // Runge-Kutta, so no need to copy old data over or to take dt into account
    >(gpuPatchData.targetDevice, gpuPatchData);
  """
  else:
    assert False, "not supported combination: {} x {}".format( solver_variant, kernel_variant )

  result = jinja2.Template( template, undefined=jinja2.DebugUndefined)
  d= {}
  d[ "FLUX_IMPLEMENTATION" ]         = flux_implementation
  d[ "NCP_IMPLEMENTATION" ]          = ncp_implementation
  d[ "SOURCE_IMPLEMENTATION" ]       = source_implementation
  d[ "COMPUTE_MAX_EIGENVALUE" ]      = compute_max_eigenvalue_of_next_time_step
  d[ "KOSIGMA" ]                     = KOSigma
  return result.render(**d)

def create_derivative_calculation_kernel_for_FD4InExaGRyPE_DSL(solver_variant: SolverVariant,
                                                              kernel_variant: KernelVariant,
                                                              SecondOrder):

  template  = "DerivativesCalInExaGRyPE"
  
  if solver_variant == SolverVariant.WithVirtualFunctions:
    template += """<
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}},
      {{NUMBER_OF_UNKNOWNS}},
      {{NUMBER_OF_AUXILIARY_VARIABLES}},
      {% if SECONDORDER %} true {% else %} false {% endif %} //if we have SO enabled, the quantities that get advections would be different
    >(patchData);
  """
  elif solver_variant == SolverVariant.Stateless:
    template += """Stateless<
      {{SOLVER_NAME}},
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}}, 
      {{NUMBER_OF_UNKNOWNS}}, 
      {{NUMBER_OF_AUXILIARY_VARIABLES}}, 
      {% if SECONDORDER %} true {% else %} false {% endif %} //if we have SO enabled, the quantities that get advections would be different
    >(patchData);
  """
  elif solver_variant == SolverVariant.AcceleratorWithExplicitCopy:
    template += """Stateless<
      {{SOLVER_NAME}},
      {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
      {{HALO_SIZE}}, 
      {{NUMBER_OF_UNKNOWNS}}, 
      {{NUMBER_OF_AUXILIARY_VARIABLES}}, 
      {% if SECONDORDER %} true {% else %} false {% endif %} //if we have SO enabled, the quantities that get advections would be different
    >(targetDevice, gpuPatchData);
  """
  else:
    assert False, "not supported combination: {} x {}".format( solver_variant, kernel_variant )

  result = jinja2.Template( template, undefined=jinja2.DebugUndefined)
  d= {}
  d[ "SECONDORDER" ]                 = SecondOrder
  return result.render(**d)

   
#def create_compute_kernel_for_FD4(flux_implementation, 
#                                  ncp_implementation, 
#                                  source_implementation, 
#                                  compute_max_eigenvalue_of_next_time_step, 
#                                  solver_variant,
#                                  kernel_variant,
#                                  KOSigma):
#  """
#  
#  I return only the unqualified function call, i.e. without any namespaces.
#  So by setting the right namespace as prefix, you can direct it to particular
#  implementations.
#  
#  """  
#  KernelCalls = {
#    KernelVariant.PatchWiseAoSHeap:               "timeStep_patchwise_heap",
#    KernelVariant.PatchWiseAoSoAHeap:             "timeStep_patchwise_heap",
#    KernelVariant.PatchWiseSoAHeap:               "timeStep_patchwise_heap",
#    KernelVariant.BatchedAoSHeap:                 "timeStep_batched_heap",
#    KernelVariant.BatchedAoSoAHeap:               "timeStep_batched_heap",
#    KernelVariant.BatchedSoAHeap:                 "timeStep_batched_heap",
#    KernelVariant.TaskGraphAoSHeap:               "timeStep_taskgraph_heap",
#    KernelVariant.TaskGraphAoSoAHeap:             "timeStep_taskgraph_heap",
#    KernelVariant.TaskGraphSoAHeap:               "timeStep_taskgraph_heap",
#  }
#  
#  EnumeratorTemplateTypes = {
#    KernelVariant.PatchWiseAoSHeap:               "::exahype2::enumerator::AoSLexicographicEnumerator",
#    KernelVariant.PatchWiseAoSoAHeap:             "::exahype2::enumerator::AoSoALexicographicEnumerator",
#    KernelVariant.PatchWiseSoAHeap:               "::exahype2::enumerator::SoALexicographicEnumerator",
#    KernelVariant.BatchedAoSHeap:                 "::exahype2::enumerator::AoSLexicographicEnumerator",
#    KernelVariant.BatchedAoSoAHeap:               "::exahype2::enumerator::AoSoALexicographicEnumerator",
#    KernelVariant.BatchedSoAHeap:                 "::exahype2::enumerator::SoALexicographicEnumerator",
#    KernelVariant.TaskGraphAoSHeap:               "::exahype2::enumerator::AoSLexicographicEnumerator",
#    KernelVariant.TaskGraphAoSoAHeap:             "::exahype2::enumerator::AoSoALexicographicEnumerator",
#    KernelVariant.TaskGraphSoAHeap:               "::exahype2::enumerator::SoALexicographicEnumerator",
#  }
#
#  template  = KernelCalls[kernel_variant]
#  
#  if solver_variant == SolverVariant.WithVirtualFunctions:
#    template += """_functors(
#  patchData,
#  {{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},
#  {{HALO_SIZE}},
#  {{NUMBER_OF_UNKNOWNS}},
#  {{NUMBER_OF_AUXILIARY_VARIABLES}},
#  {{KOSIGMA}},
#  {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#  {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#  {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#  false, // Runge-Kutta, so no need to copy old data over or to take dt into account
#  ::exahype2::fd::fd4::DifferentialSourceTermVariant::CentralDifferencesWithLopsidedAdvection,
#  [&](
#    const double * __restrict__ Q,
#    const tarch::la::Vector<Dimensions,double>&  faceCentre,
#    const tarch::la::Vector<Dimensions,double>&  volumeH,
#    double                                       t,
#    double                                       dt,
#    int                                          normal,
#    double * __restrict__ F
#  )->void {
#    {% if FLUX_IMPLEMENTATION!="<none>" %}
#    repositories::{{SOLVER_INSTANCE}}.flux( Q, faceCentre, volumeH, t, dt, normal, F );
#    {% endif %}
#  },
#  [&](
#    const double * __restrict__                  Q,
#    const double * __restrict__     deltaQ,
#    const tarch::la::Vector<Dimensions,double>&  faceCentre,
#    const tarch::la::Vector<Dimensions,double>&  volumeH,
#    double                                       t,
#    double                                       dt,
#    int                                          normal,
#    double * __restrict__ BTimesDeltaQ
#  )->void {
#    {% if NCP_IMPLEMENTATION!="<none>" %}
#    repositories::{{SOLVER_INSTANCE}}.nonconservativeProduct( Q, deltaQ, faceCentre, volumeH, t, dt, normal, BTimesDeltaQ );
#    {% endif %}
#  },
#  [&](
#    const double * __restrict__ Q,
#    const tarch::la::Vector<Dimensions,double>&  volumeX,
#    const tarch::la::Vector<Dimensions,double>&  volumeH,
#    double                                       t,
#    double                                       dt,
#    double * __restrict__ S
#  )->void {
#    {% if SOURCE_IMPLEMENTATION!="<none>" %}
#    repositories::{{SOLVER_INSTANCE}}.sourceTerm( Q, volumeX, volumeH, t, dt, S );
#    {% endif %}
#  }
#);
#  """
#  elif solver_variant == SolverVariant.Stateless:
#    template += """_static_calls<{{SOLVER_NAME}},{{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},{{HALO_SIZE}}, {{NUMBER_OF_UNKNOWNS}}, {{NUMBER_OF_AUXILIARY_VARIABLES}}, {{TEMP_DATA_ENUMERATOR}}>(
#      patchData,
#      {{KOSIGMA}},
#      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      false,     // Runge-Kutta, so no need to copy old data over or to take dt into account
#      ::exahype2::fd::fd4::DifferentialSourceTermVariant::CentralDifferencesWithLopsidedAdvection
#    );
#  """
#  elif solver_variant == SolverVariant.Multicore:
#    template += """_multicore_static_calls<{{SOLVER_NAME}},{{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},{{HALO_SIZE}}, {{NUMBER_OF_UNKNOWNS}}, {{NUMBER_OF_AUXILIARY_VARIABLES}}, {{TEMP_DATA_ENUMERATOR}}>(
#      patchData,
#      {{KOSIGMA}},
#      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      false,     // Runge-Kutta, so no need to copy old data over or to take dt into account
#      ::exahype2::fd::fd4::DifferentialSourceTermVariant::CentralDifferencesWithLopsidedAdvection
#    );
#  """
#  elif solver_variant == SolverVariant.AcceleratorWithExplicitCopy:
#    template += """_static_calls<{{SOLVER_NAME}},{{NUMBER_OF_GRID_CELLS_PER_PATCH_PER_AXIS}},{{HALO_SIZE}}, {{NUMBER_OF_UNKNOWNS}}, {{NUMBER_OF_AUXILIARY_VARIABLES}}, {{TEMP_DATA_ENUMERATOR}}>(
#      targetDevice,
#      patchData,
#      {{KOSIGMA}},
#      {% if FLUX_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      {% if NCP_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      {% if SOURCE_IMPLEMENTATION=="<none>" %} false {% else %} true {% endif %},
#      false,     // Runge-Kutta, so no need to copy old data over or to take dt into account
#      ::exahype2::fd::fd4::DifferentialSourceTermVariant::CentralDifferencesWithLopsidedAdvection
#    );
#  """
#  else:
#    assert False, "not supported combination: {} x {}".format( solver_variant, kernel_variant )
#
#  result = jinja2.Template( template, undefined=jinja2.DebugUndefined)
#  d= {}
#  d[ "FLUX_IMPLEMENTATION" ]         = flux_implementation
#  d[ "NCP_IMPLEMENTATION" ]          = ncp_implementation
#  d[ "SOURCE_IMPLEMENTATION" ]       = source_implementation
#  d[ "COMPUTE_MAX_EIGENVALUE" ]      = compute_max_eigenvalue_of_next_time_step
#  d[ "KOSIGMA" ]                     = KOSigma
#  d[ "TEMP_DATA_ENUMERATOR" ]        = EnumeratorTemplateTypes[kernel_variant]
#  return result.render(**d)
