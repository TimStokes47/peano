import exahype2.api.dslhype2 as dslhype
from exahype2.api import Solver, Target, PDETerm
import os
from enum import Enum



def translate_dslhype_function(
    #fully_qualified_solver_name: str,
    function,
    dimensions: int,
    function_namespace: list[str],
    function_map: dict[dslhype.Name, dslhype.TFunction],
    external_variables, 
    pde_terms: list[PDETerm],
    output_file_name: str,
    target: Target,
    stateless: bool,
    function_name: str | None = None,
    output_directory: str = ".",
):
    """!
    function_name: str
      Name of the generated C++ function. Defaults to the name of the
      Python DSL function passed in as function.

    output_directory: str
      Directory the kernels/ subdirectory with the generated files is
      created in, usually the Peano project's directory.
    """
    if target == Target.Accelerator and stateless == False:
        raise Exception("Accelerated kernels must be stateless")

    reduced_functor_parameters: list[dslhype.Argument] = []
    for functor_name, functor_type in function_map.items():
        function_in_pde_terms = False
        for pde_term in pde_terms:
            if pde_term.NAME == functor_name.id:
                function_in_pde_terms = True
        if function_in_pde_terms:
            reduced_functor_parameters.append(dslhype.Argument(functor_name, functor_type))

    # Name the type according to the C++ type, since MLIR uses !llvm.ptr for structs
    # Add fields in the order specified by the MLIR type since only MLIR needs this information
    cell_data_type = dslhype.TStruct("::exahype2::CellData<double, double>&")
    cell_data_type.add_field("QIn", dslhype.TDataBlock(2, dslhype.TFloat(), flatten_array=True))
    cell_data_type.add_field("QOut", dslhype.TDataBlock(2, dslhype.TFloat(), flatten_array=True))
    cell_data_type.add_field("cellCentre", dslhype.TDataBlock(1, dslhype.TVector(dslhype.TFloat(), dimensions)))
    cell_data_type.add_field("cellSize", dslhype.TDataBlock(1, dslhype.TVector(dslhype.TFloat(), dimensions)))
    cell_data_type.add_field("t", dslhype.TArray(dslhype.TFloat(), 1))
    cell_data_type.add_field("dt", dslhype.TArray(dslhype.TFloat(), 1))
    cell_data_type.add_field("id", dslhype.TArray(dslhype.TInteger(), 1))
    cell_data_type.add_field("maxEigenvalue", dslhype.TArray(dslhype.TFloat(), 1))
    cell_data_type.add_field("numberOfCells", dslhype.TInteger())

    arguments = [
        dslhype.Argument(dslhype.Name("patchData"), cell_data_type)
    ]

    #if output_language == dslhype.DSLOutputLanguage.MLIR:
    #    mlir_cell_data_type = dslhype.TStruct("::exahype2::MLIRCellDataFlattened&")
    #    mlir_cell_data_type.add_field("QIn", dslhype.TDataBlock(2, dslhype.TFloat(), flatten_array=True))
    #    mlir_cell_data_type.add_field("QOut", dslhype.TDataBlock(2, dslhype.TFloat(), flatten_array=True))
    #    mlir_cell_data_type.add_field("cellCentre", dslhype.TDataBlock(1, dslhype.TVector(dslhype.TFloat(), dimensions)))
    #    mlir_cell_data_type.add_field("cellSize", dslhype.TDataBlock(1, dslhype.TVector(dslhype.TFloat(), dimensions)))
    #    mlir_cell_data_type.add_field("numberOfCells", dslhype.TInteger())
    #
    #    arguments = [
    #        dslhype.Argument(dslhype.Name("patchData"), mlir_cell_data_type)
    #    ]
    #
    if not stateless:
        arguments.extend(reduced_functor_parameters)


    parser = dslhype.Parser()
    function = parser.parse(function, function_id=function_name + ("Stateless" if stateless else ""), function_namespace=function_namespace, function_arguments=arguments, function_map=function_map, external_variables=external_variables, enabled_pde_terms=pde_terms, stateless=stateless)
    
    cpp_printer = dslhype.CPPPrinter(4)
    function_declaration = function.print_declaration(cpp_printer)
    function_definition = function.print_definition(cpp_printer)
    function_declaration_with_timer = function.print_declaration_with_timer(cpp_printer)
    function_definition_with_timer = function.print_definition_with_timer(cpp_printer)

    kernels_directory = os.path.join(output_directory, "kernels")
    os.makedirs(kernels_directory, exist_ok=True)
    file = open(os.path.join(kernels_directory, f"{output_file_name}.h"), "a")
    file.write(function_declaration)
    file.write(function_declaration_with_timer)

    if stateless == True:
        file = open(os.path.join(kernels_directory, f"{output_file_name}.cpph"), "a")
    else:
        file = open(os.path.join(kernels_directory, f"{output_file_name}.cpp"), "a")

    file.write(function_definition)
    file.write(function_definition_with_timer)
    file.close()

#    if output_language == DSLOutputLanguage.MLIR:
#        printer = dslhype.MLIRPrinter(4)
#        function_declaration = function.print_declaration(printer)
#        function_definition = function.print_definition(printer)
#        function_bridge = printer.print_function_bridge(function)
#        function_declaration_with_timer = function.print_declaration_with_timer(printer)
#        function_definition_with_timer = function.print_definition_with_timer(printer)
#
#        kernels_directory = os.path.join(output_directory, "kernels")
#        os.makedirs(kernels_directory, exist_ok=True)
#
#        file = open(os.path.join(kernels_directory, f"{output_file_name}.h"), "a")
#        file.write(function_declaration)
#        file.write(function_declaration_with_timer)
#        file.close()
#
#        file = open(os.path.join(kernels_directory, f"{output_file_name}.mlir"), "w")
#        file.write(function_definition)
#        file.close()
#
#
#        # Generate PDE function calls
#        term_definitions = """extern "C" {\n"""
#        for pde_term_parameter in reduced_functor_parameters:
#            pde_term_name = pde_term_parameter.name
#            pde_term_type = pde_term_parameter.type
#            assert(isinstance(pde_term_type, dslhype.TFunction))
#            term_prints: list[str] = []
#            for parameter_name, parameter_type in pde_term_type.argument_types.items():
#                if isinstance(parameter_type, dslhype.TArray):
#                    term_prints.append(f"void* {parameter_name.id}_ptr")
#                else:
#                    term_prints.append(f"{parameter_type.print(cpp_printer)} {parameter_name.id}")
#
#            type_casts = []
#            for parameter_name, parameter_type in pde_term_type.argument_types.items():
#                if isinstance(parameter_type, dslhype.TArray):
#                    type_casts.append(f"{parameter_type.element_type.print(cpp_printer)}* {parameter_name.id} = static_cast<{parameter_type.element_type.print(cpp_printer)}*>({parameter_name.id}_ptr);")
#            argument_prints = [argument.print(cpp_printer) for argument in pde_term_type.argument_types.keys()]
#
#            term_definitions += f"""void {pde_term_name.id}({", ".join(term_prints)}){{
#{os.linesep.join(type_casts)}
#{fully_qualified_solver_name}::{pde_term_name.id}(nullptr, {0, 0}, 0, {", ".join(argument_prints)}, ::exahype2::Solver::Offloadable::Yes);
#}}
#"""
#        term_definitions += "}"
#        
#        file = open(os.path.join(kernels_directory, f"{output_file_name}.cpp"), "a")
#        file.write(function_bridge)
#        file.write(function_definition_with_timer)
#        file.write(term_definitions)
#        file.close()