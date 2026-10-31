import os
import copy
import exahype2.dslhype.SyntaxTree as SyntaxTree
import exahype2.dslhype.Printer as Printer

class CPPPrinter(Printer):
    def __init__(self, spaces_per_tab = 2):
        self._spaces_per_tab = spaces_per_tab

    def get_indent(self, indent_level: int) -> str:
        return ' ' * indent_level * self._spaces_per_tab

    def print_function_definition(self, function_definition: SyntaxTree.FunctionDefinition, indent_level = 0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)} {{"""
        namespace_footer = "}"
        template_string = ""
        if function_definition._template is not None:
            template_strings = [argument.print(self) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"

        current_indent = indent_level + 1  # Indentation increases inside namespace
        statement_prints = [statement.print(self, current_indent + 1) for statement in function_definition._body]
        argument_prints = [argument.print(self) for argument in function_definition._arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self)} {function_definition.id}({', '.join(argument_prints)}) {{
{os.linesep.join(statement_prints)}
{self.get_indent(indent_level)}}}
{namespace_footer}
"""

    def print_function_declaration(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)} {{"""
        namespace_footer = "}"
        template_string = ""
        if function_definition._template is not None:
            template_strings = [argument.print(self) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"

        current_indent = indent_level + 1  # Indentation increases inside namespace
        argument_prints = [argument.print(self) for argument in function_definition._arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self)} {function_definition.id}({', '.join(argument_prints)});
{namespace_footer}
"""

    def print_function_definition_with_timer(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)} {{"""
        namespace_footer = "}"
        template_string = ""
        function_call_string = function_definition.id
        if function_definition._template is not None:
            template_strings = [argument.print(self) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"
            function_call_string += "<" + ",".join([template.split(' ')[1] for template in template_strings]) + ">"

        arguments = copy.deepcopy(function_definition._arguments)
        arguments.append(SyntaxTree.Argument("measurement", SyntaxTree.TCustom("tarch::timing::Measurement&")))

        current_indent = indent_level + 1  # Indentation increases inside namespace
        argument_prints = [argument.print(self) for argument in arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self)} {function_definition.id}({', '.join(argument_prints)}) {{
tarch::timing::Watch watch("{"::".join(function_definition._namespaces)}", "{function_definition.id}", false, true);
{function_call_string}({",".join([argument.print(self).split(' ')[-1] for argument in function_definition._arguments])});
watch.stop();
measurement.setValue(watch.getCalendarTime());
{self.get_indent(indent_level)}}}
{namespace_footer}
"""

    def print_function_declaration_with_timer(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)} {{"""
        namespace_footer = "}"
        template_string = ""
        if function_definition._template is not None:
            template_strings = [argument.print(self) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"

        arguments = copy.deepcopy(function_definition._arguments)
        arguments.append(SyntaxTree.Argument("measurement", SyntaxTree.TCustom("tarch::timing::Measurement&")))

        current_indent = indent_level + 1  # Indentation increases inside namespace
        argument_prints = [argument.print(self) for argument in arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self)} {function_definition.id}({', '.join(argument_prints)});
{namespace_footer}
"""

    def print_argument(self, argument: SyntaxTree.Argument, indent_level=0):
        return self.get_indent(indent_level) + argument._type.print(self) + " " + argument.id

    def print_name(self, name: SyntaxTree.Name, indent_level=0):
        return self.get_indent(indent_level) + name.id

    def print_integer(self, integer: SyntaxTree.Integer, indent_level=0):
        if integer._string is None:
            return self.get_indent(indent_level) + str(integer._value)
        else:
            return self.get_indent(indent_level) + str(integer._string)

    def print_if(self, if_statement: SyntaxTree.If, indent_level=0):
        statement_prints = [statement.print(self, indent_level + 1) for statement in if_statement._statements]
        return self.get_indent(indent_level) + f"""if constexpr ({if_statement._boolean.print(self)}) {{
{os.linesep.join(statement_prints)}
""" + self.get_indent(indent_level) + "}"

    def print_for(self, for_statement: SyntaxTree.For, indent_level = 0):
        statement_prints = [statement.print(self, indent_level + 1) for statement in for_statement._statements]
        if for_statement._use_scheduler:
            return self.get_indent(indent_level) + f"""parallelForWithSchedulerInstructions({for_statement._iteration_variable.print(self)}, {for_statement._iteration_range[1].print(self)}, loopParallelism) {{
{os.linesep.join(statement_prints)}
{self.get_indent(indent_level)}}}
endParallelFor"""
        else:
            return self.get_indent(indent_level) + f"""for (int {for_statement._iteration_variable.print(self)} = {for_statement._iteration_range[0].print(self)}; {for_statement._iteration_variable.print(self)} < {for_statement._iteration_range[1].print(self)}; {for_statement._iteration_variable.print(self)}++) {{
{os.linesep.join(statement_prints)}
""" + self.get_indent(indent_level) + "}"

    def print_comment(self, comment: SyntaxTree.Comment, indent_level=0):
        return self.get_indent(indent_level) + "//" + comment._comment[1:]

    def print_function_call(self, function_call: SyntaxTree.FunctionCall, indent_level=0):
        argument_prints = [argument.print(self) for argument in function_call._arguments]
        return f"""{self.get_indent(indent_level)}{function_call.id}({", ".join(argument_prints)});"""
    
    def print_max(self, max: SyntaxTree.Max, indent_level=0):
        return f"{self.get_indent(indent_level)}std::max({max._lhs.print(self)}, {max._rhs.print(self)});"

    def print_memory_allocation(self, memory_allocation: SyntaxTree.MemoryAllocation, indent_level=0):
        if len(memory_allocation._dimensions) == 1:
            return self.get_indent(indent_level) + f"""{memory_allocation._type.print(self) + "* " if memory_allocation._specify_type else ""}{memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}[{memory_allocation._dimensions[0].print(self)}];"""
        else:
            if SyntaxTree.Node.use_accelerator == False and type(SyntaxTree.Name.variables[memory_allocation._name.id]) is not SyntaxTree.FaceDataBlock:
                return self.get_indent(indent_level) + f"""{memory_allocation._type.print(self)}** {memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}*[{memory_allocation._dimensions[-1].print(self)}];
{memory_allocation._loop.print(self, indent_level)}"""
            else:
                return self.get_indent(indent_level) + f"""{memory_allocation._type.print(self)}* {memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}[{(memory_allocation._size * memory_allocation._dimensions[-1]).print(self)}];"""

    def print_memory_deallocation(self, memory_deallocation: SyntaxTree.MemoryDeallocation, indent_level=0):
        if len(memory_deallocation._dimensions) == 1:
            return self.get_indent(indent_level) + f"""delete[] {memory_deallocation._name.print(self)};"""
        else:
            if SyntaxTree.Node.use_accelerator == False and type(SyntaxTree.Name.variables[memory_deallocation._name.id]) is not SyntaxTree.FaceDataBlock:
                return f"""{memory_deallocation._loop.print(self, indent_level)}
{self.get_indent(indent_level)}delete[] {memory_deallocation._name.print(self)};"""
            else:
                return f"""{self.get_indent(indent_level)}delete[] {memory_deallocation._name.print(self)};"""

    def print_assignment(self, assignment: SyntaxTree.Assignment, indent_level = 0):
        lhs_print = assignment._lhs.print(self)
        rhs_print = assignment._rhs.print(self)
        return f"{self.get_indent(indent_level)}{lhs_print} = {rhs_print};"
    
    def print_construction(self, construction: SyntaxTree.Construction, indent_level=0):
        return self.get_indent(indent_level) + f"{construction._name.get_type().print(self)} {construction._name.print(self)} = {construction._expression.print(self)};"

    def print_data_block_construction_from_existing(self, construction: SyntaxTree.DataBlockConstructionFromExisting, indent_level=0):
        type_print = construction._name.get_type().print(self)
        if isinstance(construction._name.get_type(), SyntaxTree.TDiagonalMatrix) or construction._name.id[-2:] == "1d":
            type_print = "const double*"
        if len(construction._dataBlock._iteration_range) > 1 and SyntaxTree.Node.use_accelerator == False and type(construction._dataBlock) is not SyntaxTree.FaceDataBlock:
            type_print += "*"
        return self.get_indent(indent_level) + f"{type_print} {construction._name.print(self)} = {construction._dataBlock._internal.print(self)};"

    def print_data_block_construction_from_operation(self, construction: SyntaxTree.DataBlockConstructionFromOperation, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.assignment.print(self, indent_level)}"""
    
    def print_data_block_construction_from_function(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.initialisation.print(self, indent_level)}"""

    def print_data_block_construction_from_matrix_vector_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.assignment.print(self, indent_level)}"""
    
    def print_data_block_construction_from_dot_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.assignment.print(self, indent_level)}"""
    
    def print_diagonal_matrix(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        if construction._id is not None:
            return construction._id.print(self, indent_level)
        else:
            return construction._internal.print(self, indent_level)
    
    def print_matrix(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        if construction._id is not None:
            return construction._id.print(self, indent_level)
        else:
            return construction._internal.print(self, indent_level)
    
    def print_diagonal_matrix_construction_from_kronecker_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction._memory_allocation.print(self, indent_level)}
{construction._assignment.print(self, indent_level)}"""
    
    def print_diagonal_matrix_construction_from_value(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction._memory_allocation.print(self, indent_level)}
{construction._assignment.print(self, indent_level)}"""
    
    def print_matrix_construction_from_kronecker_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction._memory_allocation.print(self, indent_level)}
{construction._initialisation.print(self, indent_level)}
{construction._assignment.print(self, indent_level)}"""

    def print_data_block_assignment(self, assignment: SyntaxTree.DataBlockAssignment, indent_level=0):
        return assignment._loop.print(self, indent_level)

    def print_boolean(self, boolean: SyntaxTree.Boolean, indent_level = 0):
        return self.get_indent(indent_level) + str(boolean.value)
    
    def print_string(self, string: SyntaxTree.String, indent_level = 0):
        return self.get_indent(indent_level) + str(string._value)
    
    def print_float(self, float: SyntaxTree.Float, indent_level = 0):
        if float._string is None:
            return self.get_indent(indent_level) + str(float._value)
        else:
            return self.get_indent(indent_level) + str(float._string)
    
    def print_data_block(self, data_block: SyntaxTree.DataBlock, indent_level=0):
        if data_block.id is None:
            return self.get_indent(indent_level) + data_block._internal.print(self)
        else:
            return self.get_indent(indent_level) + data_block.id

    def print_subscript(self, subscript: SyntaxTree.Subscript, indent_level=0):
        return f"{self.get_indent(indent_level)}{subscript._value.print(self)}[{subscript._index.print(self)}]"

    def print_reference(self, reference: SyntaxTree.Reference, indent_level=0):
        return f"{self.get_indent(indent_level)}&{reference._expression.print(self)}"

    def print_vector(self, vector: SyntaxTree.Vector, indent_level=0):
        if vector._string is None:
            element_prints = [element.print(self) for element in vector._elements]
            return f"""{self.get_indent(indent_level)}tarch::la::Vector<{len(vector._elements)}, {vector._type.print(self)}>{{{", ".join(element_prints)}}}"""
        return vector._string
    
    def print_vector_index(self, vector_index: SyntaxTree.VectorIndex, indent_level=0):
        return f"{self.get_indent(indent_level)}{vector_index._value.print(self)}({vector_index._index.print(self)})"
    
    def print_comparison(self, comparison: SyntaxTree.Comparison, indent_level=0):
        return f"{self.get_indent(indent_level)}({comparison._lhs.print(self)} {comparison._operation} {comparison._rhs.print(self)})"

    def print_binary_operation(self, binary_operation: SyntaxTree.BinaryOperation, indent_level=0):
        return f"{self.get_indent(indent_level)}({binary_operation._lhs.print(self)} {binary_operation._operation} {binary_operation._rhs.print(self)})"
    
    def print_unary_operation(self, unary_operation: SyntaxTree.UnaryOperation, indent_level=0):
        return f"{self.get_indent(indent_level)}({unary_operation._operation} {unary_operation._operand.print(self)})"
    
    def print_data_block_unary_max(self, unary_max: SyntaxTree.DataBlockUnaryMax, indent_level=0):
        return unary_max._loop.print(self, indent_level)
    
    def print_custom_type(self, type: SyntaxTree.TCustom, indent_level=0):
        return self.get_indent(indent_level) + type._type
    
    def print_void_type(self, type: SyntaxTree.TVoid, indent_level=0):
        return self.get_indent(indent_level) + "void"

    def print_integer_type(self, type: SyntaxTree.TInteger, indent_level=0):
        return self.get_indent(indent_level) + "int"
    
    def print_long_type(self, type: SyntaxTree.TLong, indent_level=0):
        return self.get_indent(indent_level) + "long"
    
    def print_boolean_type(self, type: SyntaxTree.TBoolean, indent_level=0):
        return self.get_indent(indent_level) + "bool"
    
    def print_float_type(self, type: SyntaxTree.TFloat, indent_level=0):
        return self.get_indent(indent_level) + "double" + ("&" if type._reference else "")
    
    def print_string_type(self, type: SyntaxTree.TString, indent_level=0):
        return self.get_indent(indent_level) + "const char*"
    
    def print_data_block_type(self, type: SyntaxTree.TDataBlock, indent_level=0):
        return self.get_indent(indent_level) + type._underlying_type.print(self) + "*"
    
    def print_matrix_type(self, type: SyntaxTree.TMatrix, indent_level=0):
        return self.get_indent(indent_level) + type._underlying_type.print(self) + "*"
    
    def print_diagonal_matrix_type(self, type: SyntaxTree.TDiagonalMatrix, indent_level=0):
        return self.get_indent(indent_level) + type._underlying_type.print(self) + "*"
    
    def print_nested_memref_type(self, type: SyntaxTree.TNestedMemref, indent_level=0):
        return self.get_indent(indent_level) + "double**"
    
    def print_simple_memref_type(self, type: SyntaxTree.TSimpleMemref, indent_level=0):
        return self.get_indent(indent_level) + "void*"
    
    def print_function_type(self, type: SyntaxTree.TFunctionType, indent_level=0):
        return self.get_indent(indent_level) + "void*"