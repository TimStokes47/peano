import exahype2.dslhype.printers.CPPPrinter as CPPPrinter
import exahype2.dslhype.SyntaxTree as SyntaxTree
import os
import copy

class OMPPrinter(CPPPrinter):
    def print_function_definition(self, function_definition: SyntaxTree.FunctionDefinition, indent_level = 0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::omp {{"""
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
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::omp {{"""
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
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::omp {{"""
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
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::omp {{"""
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

    def print_memory_allocation(self, memory_allocation: SyntaxTree.MemoryAllocation, indent_level=0):
        if SyntaxTree.Node.use_accelerator == False:
            if len(memory_allocation._dimensions) == 1:
                return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self) + "* " if memory_allocation._specify_type else ""}{memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}[{memory_allocation._dimensions[0].print(self)}];"""
            else:
                return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self)}** {memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}*[{memory_allocation._dimensions[-1].print(self)}];
{memory_allocation._loop.print(self, indent_level)}"""
        else:   
            if len(memory_allocation._dimensions) == 1:
                if SyntaxTree.Node.use_memory_manager == True:
                    return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self) + "* " if memory_allocation._specify_type else ""}{memory_allocation._name.print(self)} = tarch::accelerator::GPUMemoryManager::getInstance().allocate<{memory_allocation._type.print(self)}>({memory_allocation._dimensions[0].print(self)}, targetDevice);"""
                else:
                    return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self) + "* " if memory_allocation._specify_type else ""}{memory_allocation._name.print(self)} = ({memory_allocation._type.print(self)}*)omp_target_alloc(sizeof({memory_allocation._type.print(self)}) * {memory_allocation._dimensions[0].print(self)}, targetDevice);"""
            else:
                if SyntaxTree.Node.use_memory_manager == True:
                    return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self)}* {memory_allocation._name.print(self)} = tarch::accelerator::GPUMemoryManager::getInstance().allocate<{memory_allocation._type.print(self)}>({(memory_allocation._size * memory_allocation._dimensions[-1]).print(self)}, targetDevice);"""
                else:
                    return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self)}* {memory_allocation._name.print(self)} = ({memory_allocation._type.print(self)}*)omp_target_alloc(sizeof({memory_allocation._type.print(self)}) * {(memory_allocation._size * memory_allocation._dimensions[-1]).print(self)}, targetDevice);"""

    def print_memory_deallocation(self, memory_deallocation: SyntaxTree.MemoryDeallocation, indent_level=0):
        if SyntaxTree.Node.use_accelerator == False:
            if len(memory_deallocation._dimensions) == 1:
                return f"""{self.get_indent(indent_level)}delete[] {memory_deallocation._name.print(self)};"""
            else:
                return f"""{memory_deallocation._loop.print(self, indent_level)}
{self.get_indent(indent_level)}delete[] {memory_deallocation._name.print(self)};"""
        else:
            if SyntaxTree.Node.use_memory_manager:
                return f"""{self.get_indent(indent_level)}tarch::accelerator::GPUMemoryManager::getInstance().free({memory_deallocation._name.print(self)}, targetDevice);"""
            else:
                return f"""{self.get_indent(indent_level)}omp_target_free({memory_deallocation._name.print(self)}, targetDevice);"""

    def print_data_block_unary_max(self, unary_max: SyntaxTree.DataBlockUnaryMax, indent_level=0):
        if SyntaxTree.Node.use_accelerator:
            return f"""{self.get_indent(indent_level)}omp_target_memset(maxEigenvalues, 0, N * sizeof(double), targetDevice);
{self.get_indent(indent_level)}for (int {unary_max._loop._iteration_variable.print(self)} = {unary_max._loop._iteration_range[0].print(self)}; {unary_max._loop._iteration_variable.print(self)} < {unary_max._loop._iteration_range[1].print(self)}; {unary_max._loop._iteration_variable.print(self)}++) {{
{self.get_indent(indent_level)}#pragma omp target teams loop collapse(Dimensions) reduction(max:{unary_max._outputVariable.multidimensional_index([unary_max._loop.get_iteration_variable()]).print(self)})
{unary_max._loop._statements[1].print(self, indent_level + 1)}
{self.get_indent(indent_level)}}}
"""
        else:
            return unary_max._loop.print(self, indent_level)
    
    def print_data_block_construction_from_function(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        if SyntaxTree.Node.use_accelerator == True:
            omp_pragma = "#pragma omp target teams loop collapse(Dimensions + 1) device(targetDevice)"
        else:
            omp_pragma = "#pragma omp parallel for simd collapse(Dimensions + 1)"
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{self.get_indent(indent_level)}{omp_pragma}
{construction.initialisation.print(self, indent_level)}"""

    def print_data_block_assignment(self, assignment: SyntaxTree.DataBlockAssignment, indent_level=0):
        if SyntaxTree.Node.use_accelerator == True:
            omp_pragma = "#pragma omp target teams loop collapse(Dimensions + 1) device(targetDevice)"
        else:
            omp_pragma = "#pragma omp parallel for simd collapse(Dimensions + 1)"
        return f"""{self.get_indent(indent_level)}{omp_pragma}
{assignment._loop.print(self, indent_level)}
"""