import exahype2.dslhype.printers.CPPPrinter as CPPPrinter
import exahype2.dslhype.SyntaxTree as SyntaxTree
import os
import copy

class SYCLPrinter(CPPPrinter):
    def print_function_definition(self, function_definition: SyntaxTree.FunctionDefinition, indent_level = 0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::sycl {{"""
        namespace_footer = "}"
        template_string = ""
        if function_definition._template is not None:
            template_strings = [argument.print(self) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"

        current_indent = indent_level + 1  # Indentation increases inside namespace
        statement_prints = [statement.print(self, current_indent + 1) for statement in function_definition._body]
        argument_prints = [argument.print(self) for argument in function_definition._arguments]
        statement_prints.insert(0, "::sycl::queue& queue = tarch::accelerator::getSYCLQueue(targetDevice);")
        statement_prints.insert(1, "size_t range0, range1, range2;")

        data_copy_prints = []
        for dataBlockCreation in SyntaxTree.FunctionDefinition._syclDataToCopy:
            if len(dataBlockCreation._dataBlock._iteration_range) > 1:
                step_size = SyntaxTree.Integer(1)
                for d in dataBlockCreation._dataBlock._iteration_range[0:-1]:
                    step_size = step_size * (d[1] - d[0])
                size = step_size * (dataBlockCreation._dataBlock._iteration_range[-1][1] - dataBlockCreation._dataBlock._iteration_range[-1][0])

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self)} {function_definition.id}({', '.join(argument_prints)}) {{
{os.linesep.join(statement_prints)}
{self.get_indent(indent_level)}}}
{namespace_footer}
"""

    def print_function_declaration(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::sycl {{"""
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
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::sycl {{"""
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
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::sycl {{"""
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
        if len(memory_allocation._dimensions) == 1:
            return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self) + "* " if memory_allocation._specify_type else ""}{memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}[{memory_allocation._dimensions[0].print(self)}];"""
        else:
            return f"""{self.get_indent(indent_level)}{memory_allocation._type.print(self)}** {memory_allocation._name.print(self)} = new {memory_allocation._type.print(self)}*[{memory_allocation._dimensions[-1].print(self)}];
{memory_allocation._loop.print(self, indent_level)}"""

    def print_memory_deallocation(self, memory_deallocation: SyntaxTree.MemoryDeallocation, indent_level=0):
        return f"""{self.get_indent(indent_level)}::sycl::free({memory_deallocation._name.print(self)}, queue);"""

    def print_data_block_unary_max(self, unary_max: SyntaxTree.DataBlockUnaryMax, indent_level=0):
        ranges = [f"range{i} = {loop.get_interval_size().print(self)};" for i, loop in enumerate(unary_max._loops[0:3])]
        indexing = [f"int {loop.get_iteration_variable().print(self)} = index[{i}];" for i, loop in enumerate(unary_max._loops[0:3])]
        statement_prints = [statement.print(self, indent_level + 1) for statement in unary_max._loops[2]._statements]
        return os.linesep.join(ranges) + "\n" + self.get_indent(indent_level) + f"""queue.submit([&](::sycl::handler& handler) {{handler.memset({unary_max._outputVariable.print(self)}, 0, sizeof(double) * {unary_max._loop.get_interval_size().print(self)}); }}).wait();
{self.get_indent(indent_level)}queue.submit([&](::sycl::handler& handler) {{
{self.get_indent(indent_level)}handler.parallel_for(::sycl::range<3>{{range0, range1, range2}}, [=](::sycl::item<3> index) {{
{os.linesep.join(indexing)}
{unary_max._loops[3].print(self, indent_level + 1)}
{self.get_indent(indent_level + 1)}}});
{self.get_indent(indent_level)}}}).wait();"""
    
    def print_max(self, max: SyntaxTree.Max, indent_level=0):
        return f"{self.get_indent(indent_level)}sycl::max({max._lhs.print(self)}, {max._rhs.print(self)});"

    def print_data_block_construction_from_function(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        ranges = [f"range{i} = {loop.get_interval_size().print(self)};" for i, loop in enumerate(construction._loops[0:3])]
        indexing = [f"int {loop.get_iteration_variable().print(self)} = index[{i}];" for i, loop in enumerate(construction._loops[0:3])]
        statement_prints = [statement.print(self, indent_level + 1) for statement in construction._loops[2]._statements]
        return construction.memoryAllocation.print(self, indent_level) + os.linesep.join(ranges) + "\n" + self.get_indent(indent_level) + f"""queue.submit([&](::sycl::handler& handler) {{
{self.get_indent(indent_level)}handler.parallel_for(::sycl::range<3>{{range0, range1, range2}}, [=](::sycl::item<3> index) {{
{os.linesep.join(indexing)}
{os.linesep.join(statement_prints)}
{self.get_indent(indent_level + 1)}}});
{self.get_indent(indent_level)}}}).wait();"""

    def print_data_block_assignment(self, assignment: SyntaxTree.DataBlockAssignment, indent_level=0):
        ranges = [f"range{i} = {loop.get_interval_size().print(self)};" for i, loop in enumerate(assignment._loops[0:3])]
        indexing = [f"int {loop.get_iteration_variable().print(self)} = index[{i}];" for i, loop in enumerate(assignment._loops[0:3])]
        statement_prints = [statement.print(self, indent_level + 1) for statement in assignment._loops[2]._statements]
        return os.linesep.join(ranges) + "\n" + self.get_indent(indent_level) + f"""queue.submit([&](::sycl::handler& handler) {{
{self.get_indent(indent_level)}handler.parallel_for(::sycl::range<3>{{range0, range1, range2}}, [=](::sycl::item<3> index) {{
{os.linesep.join(indexing)}
{os.linesep.join(statement_prints)}
{self.get_indent(indent_level + 1)}}});
{self.get_indent(indent_level)}}}).wait();"""