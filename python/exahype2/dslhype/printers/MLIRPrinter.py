import os
import copy
import exahype2.dslhype.SyntaxTree as SyntaxTree
import exahype2.dslhype.Printer as Printer
from exahype2.dslhype.printers.CPPPrinter import CPPPrinter

## We need to track the context of the code generation to support MLIR
#class Context:
#
#    def __init__(self):
#        # A stack of blocks e.g. for loops where we can store hoisted MLIR operations
#        self._blocks = [ Context.Block() ] 	
#        
#    def push_block(self):
#        self._blocks.append(Context.Block())
#        
#    def pop_block(self):
#        if self._blocks:
#            return self._blocks.pop()
#        else:
#            return Context.Block()
#        
#    def get_block(self):
#        if not self._blocks:
#            self.push_block()
#        return self._blocks[-1]
#
#    def clear_block(self):
#        if self._blocks:
#            self._blocks[-1] = Context.Block()
#
#    # We need to store block-level MLIR that has been hoisted and ensure we don't
#    # repeat casting operators for indices within a block e.g. loops    
#    class Block:
#        
#        def __init__(self):
#            # We use this to hoist MLIR operations out of array indexing, function calls etc.
#            self._mlir = []            
#            # We use a set to store the ids that we have already processed within the block
#            self._ids = {}           
#
#        def push_mlir(self):
#            self._mlir.append([])
#
#        def pop_mlir(self):
#            if self._mlir:
#                return self._mlir.pop()
#            else:
#                return []
#            
#        def get_mlir(self):
#            if not self._mlir:
#                self.push_mlir()
#            return self._mlir[-1]    
#
#        def mlir_append(self, op):
#            self.get_mlir().append(op)  
#            
#        # def add_id(self, id):
#        #     self._ids.update(id)
#
#        # def remove_id(self, id):
#        #     self._ids.remove(id)
#
#        # def get_id(self, id):
#        #     return self._ids.get(id)
#                        
#        # def find_id(self, id):
#        #     return id in self._ids

class MLIRPrinter(Printer):
    def __init__(self, spaces_per_tab = 2, use_omp = False):
        self._spaces_per_tab = spaces_per_tab
        self.use_omp = use_omp
        self.cpp_printer = CPPPrinter(spaces_per_tab)

    def get_indent(self, indent_level: int) -> str:
        return ' ' * indent_level * self._spaces_per_tab
    
    def print_function_definition(self, function_definition: SyntaxTree.FunctionDefinition, indent_level = 0):
        if function_definition._top_level:
            for argument in function_definition._arguments:
                SyntaxTree.Node.type_map.update({argument.id: argument._namespace})
                if SyntaxTree.Node.type_map.get(argument._type.print(self.cpp_printer)) is None:
                    SyntaxTree.Node.type_map.update({argument._type.print(self.cpp_printer): argument._namespace})
        SyntaxTree.Node.use_accelerator = function_definition.use_accelerator
        SyntaxTree.Node.use_mlir_omp = self.use_omp
        indent_level += 1  # Increase indentation for MLIR
        
        if function_definition._top_level:
            SyntaxTree.Node.reset_mlir_ids()  # Reset the MLIR id count for the top-level function

            # NOTE: We used to generate MLIRbridge.cpp using the dedicated MLIRBridgeGenerator module
            globals_mlir = []
            
            # Add external function declarations for bridge functions using dynamic signatures
            # Extract function signatures directly from function_definition._template (functors only, not constants)
            if function_definition._template is not None:
                for argument in function_definition._arguments:
                    mlir_signature = argument._namespace #SyntaxTree.Node.type_map[cpp_type_str]
                    
                    # Only process functors, skip template constants like "NumberOfVolumesPerAxisInPatch"
                    if argument._is_function:
                        # Create the bridge function declaration in MLIR with _bridge suffix
                        # This matches the C++ wrapper function names (flux_bridge, etc.)
                        func_decl = f"func.func private @{argument.id}_bridge{mlir_signature}"
                        globals_mlir.append(' ' * (indent_level) * SyntaxTree.Node.spaces_per_tab + func_decl)

                    
            else:
                # No template means no MLIR bridge functions are needed
                raise ValueError(
                    "MLIR code generation requires template parameters to determine bridge function signatures. "
                    "Ensure the function has template parameters that specify the required functors."
                )
            
            # Add addressof/loads for globals inside the function
            function_global_loads = []
            if function_definition._template is not None:
                for argument in function_definition._template:
                    # Generate globals for ALL template parameters (including boolean evaluation flags)
                    new_id = SyntaxTree.Node.get_mlir_id()
                    globals_mlir.append(' ' * (indent_level) * SyntaxTree.Node.spaces_per_tab +f"llvm.mlir.global external @{argument.id}() : {argument._type.print(self)}")
                    function_global_loads.append(' ' * (indent_level + 1) * SyntaxTree.Node.spaces_per_tab + f"%{new_id} = llvm.mlir.addressof @{argument.id} : !llvm.ptr")
                    
                    # Get the MLIR type for the argument
                    arg_mlir_type = argument._type.print(self)
                    
                    # Skip global loading for variables with empty types (be more thorough)
                    if arg_mlir_type and arg_mlir_type.strip() != '' and len(arg_mlir_type.strip()) > 0:
                        function_global_loads.append(' ' * (indent_level + 1) * SyntaxTree.Node.spaces_per_tab + f"%{argument.id} = llvm.load %{new_id} {{name = \"{argument.id.strip('%')}\"}} : !llvm.ptr -> {arg_mlir_type}")
                    else:
                        # For variables with empty types, just create a comment
                        function_global_loads.append(' ' * (indent_level + 1) * SyntaxTree.Node.spaces_per_tab + f"// Skipping load for {argument.id} (empty type: '{arg_mlir_type}')")
            else:
                globals_mlir = []

            SyntaxTree.Node.context.push_block()
            argument_prints = [argument.print(self, indent_level).strip('') for argument in function_definition._arguments]
            SyntaxTree.Node.context.push_block()

            statement_prints = []
            for statement in function_definition._body:
                statement_prints.append(statement.print(self, indent_level + 1))
            statements = SyntaxTree.Node.context.pop_block().get_mlir()
            statements += statement_prints
            
            return_type = function_definition._return_type.print(self)
            return_type_str = f" -> ({return_type})" if return_type != 'void' else ''

            return f"""
builtin.module {{
{os.linesep.join(globals_mlir)}
    func.func @{function_definition.id + "_omp" if SyntaxTree.Node.use_mlir_omp else function_definition.id}({', '.join(argument_prints)}){return_type_str} {{
{os.linesep.join(function_global_loads)}
{os.linesep.join(statements)}
func.return
    }}
}}
"""
        # Non-top-level case
        SyntaxTree.Node.context.push_block()
        argument_prints = [argument.print(self, indent_level) for argument in function_definition._arguments]
        SyntaxTree.Node.context.push_block()
        statement_prints = [statement.print(self, indent_level + 1) for statement in function_definition._body]
        statements = SyntaxTree.Node.context.pop_block().get_mlir()
        statements += statement_prints
        return_type = function_definition._return_type.print(self, indent_level)
        return_type_str = f" -> ({return_type})" if return_type != 'void' else ''
        return f"""
    func.func @{function_definition.id}({', '.join(argument_prints)}){return_type_str} {{
{os.linesep.join(statements)}
func.return
    }}
"""

    def print_function_declaration(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        if self.use_omp:
            extra_namespace = "omp::mlir"
        else:
            extra_namespace = "mlir"
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::{extra_namespace} {{"""
        namespace_footer = "}"
        template_string = ""
        if function_definition._template is not None:
            template_strings = [argument.print(self.cpp_printer) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"

        current_indent = indent_level + 1  # Indentation increases inside namespace
        argument_prints = [argument.print(self.cpp_printer) for argument in function_definition._arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self.cpp_printer)} {function_definition.id}({', '.join(argument_prints)});
{namespace_footer}
"""

    def print_function_definition_with_timer(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        if self.use_omp:
            extra_namespace = "omp::mlir"
        else:
            extra_namespace = "mlir"
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::{extra_namespace} {{"""
        namespace_footer = "}"
        template_string = ""
        function_call_string = function_definition.id
        if function_definition._template is not None:
            template_strings = [argument.print(self.cpp_printer) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"
            function_call_string += "<" + ",".join([template.split(' ')[1] for template in template_strings]) + ">"

        arguments = copy.deepcopy(function_definition._arguments)
        arguments.append(SyntaxTree.Argument("measurement", SyntaxTree.TCustom("tarch::timing::Measurement&")))

        current_indent = indent_level + 1  # Indentation increases inside namespace
        argument_prints = [argument.print(self.cpp_printer) for argument in arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self.cpp_printer)} {function_definition.id}({', '.join(argument_prints)}) {{
tarch::timing::Watch watch("{"::".join(function_definition._namespaces)}", "{function_definition.id}", false, true);
{function_call_string}({",".join([argument.print(self.cpp_printer).split(' ')[-1] for argument in function_definition._arguments])});
watch.stop();
measurement.setValue(watch.getCalendarTime());
{self.get_indent(indent_level)}}}
{namespace_footer}
"""

    def print_function_declaration_with_timer(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        if self.use_omp:
            extra_namespace = "omp::mlir"
        else:
            extra_namespace = "mlir"
        namespace_header = f"""namespace {"::".join(function_definition._namespaces)}::{extra_namespace} {{"""
        namespace_footer = "}"
        template_string = ""
        if function_definition._template is not None:
            template_strings = [argument.print(self.cpp_printer) for argument in function_definition._template]
            template_string = "template <" + ",".join(template_strings) + ">\n"

        arguments = copy.deepcopy(function_definition._arguments)
        arguments.append(SyntaxTree.Argument("measurement", SyntaxTree.TCustom("tarch::timing::Measurement&")))

        current_indent = indent_level + 1  # Indentation increases inside namespace
        argument_prints = [argument.print(self.cpp_printer) for argument in arguments]

        return f"""{namespace_header}
{template_string}{self.get_indent(indent_level)}{function_definition._return_type.print(self.cpp_printer)} {function_definition.id}({', '.join(argument_prints)});
{namespace_footer}
"""

    def print_argument(self, argument: SyntaxTree.Argument, indent_level=0):
        # NOTE: Arguments are inserted in-line into 'func.func' op
        return f"%{argument.id} : {argument._type.print(self)}"

    def print_name(self, name: SyntaxTree.Name, indent_level=0):
        return f"%{name.id}"

    def print_integer(self, integer: SyntaxTree.Integer, indent_level=0):
        indent = self.get_indent(indent_level)
        if integer._string is None:
            value = str(integer._value)
            if integer.hoist():
                integer.mlir_id(SyntaxTree.Node.get_mlir_id())
                mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
                mlir_str += indent + f"%{integer.mlir_id()} = arith.constant {integer._value} : {integer.get_type().print(self)}\n"
                SyntaxTree.Node.context.push_block()
                SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                id = f"%{integer.mlir_id()}"
                return id
            return value
        else:
            # NOTE: We use Strings for the CellData structs (patchData).
            #	So, we check the name here and we generate the MLIR to 
            #	extract the struct from LLVM pointer
            # TODO: We need to raise this to where we make the assignment 
            match integer._string:
                case x if '.' in x:
                    struct, field = x.split('.')
                    if struct in SyntaxTree.Node.struct_map:
                        struct_list = SyntaxTree.Node.struct_map[struct]
                        idx = [item[0] for item in struct_list].index(field)
                        mlir_type = struct_list[idx][1]
                        # Extract just the struct type (everything after the separator)
                        mlir_type_struct = mlir_type.split('|||')[1] if '|||' in mlir_type else mlir_type
                        # getelementptr returns !llvm.ptr and takes struct type as parameter
                        mlir_str = f"llvm.getelementptr %{struct}[0, {idx}] : (!llvm.ptr) -> !llvm.ptr, {mlir_type_struct}"
                        return mlir_str
            return integer._string

    def print_if(self, if_statement: SyntaxTree.If, indent_level=0):
        indent = self.get_indent(indent_level)
        SyntaxTree.Node.context.push_block()
        statement_prints = [statement.print(self, indent_level + 1) for statement in if_statement._statements]
        body_statements = ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
        body_statements += ''.join(statement_prints)
        return indent + f"""scf.if {if_statement._boolean.print(self, indent_level)} {{
{body_statements}""" + indent + "}"

    def print_for(self, for_statement: SyntaxTree.For, indent_level = 0):
        indent = self.get_indent(indent_level)
        bounds_casting = ''
            
        if isinstance(for_statement._iteration_range[0], list) or isinstance(for_statement._iteration_range[0], tuple):
            dims = len(for_statement._iteration_range)
            iter_vars = []
            lwbs = []
            upbs = []
            steps = []
            for d in range(dims):
                var = For._iteration_variable_names[d]
                iter_vars.append(f"%{var}")
                for_statement._iteration_range[d][0].hoist(True)
                for_statement._iteration_range[d][1].hoist(True)
                if isinstance(for_statement._iteration_range[d][0],  SyntaxTree.Integer):
                    mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    bounds_casting += indent + f"{mlir_id} = arith.constant {for_statement._iteration_range[d][0].print(self, indent_level)} : index\n"
                    lwb = mlir_id
                else:
                    lwb = for_statement._iteration_range[d][0].print(self, indent_level).strip()
                if isinstance(for_statement._iteration_range[d][1],  SyntaxTree.Integer):
                    mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    bounds_casting += indent + f"{mlir_id} = arith.constant {for_statement._iteration_range[d][1].print(self, indent_level)} : index\n"
                    upb = mlir_id
                else:
                    upb = for_statement._iteration_range[d][1].print(self, indent_level).strip()
                lwbs.append(lwb)
                upbs.append(upb)
                mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                bounds_casting += indent + f"{mlir_id} = arith.constant 1 : index\n"
                steps.append(mlir_id)
        else:
            for_statement._iteration_range[0].hoist(True)
            for_statement._iteration_range[1].hoist(True)
            if not for_statement._iteration_variable._index_type:
                for_statement._iteration_variable.hoist(True)
                mlir_str = for_statement._iteration_variable.print(self, indent_level).strip()
                mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                bounds_casting += ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
                SyntaxTree.Node.context.push_block()
                bounds_casting += indent + f"{mlir_id} = arith.index_cast {mlir_str} : {for_statement._iteration_variable.get_type().print(self)} to index\n"
                iter_vars = [mlir_id]
            else:
                iter_vars = [for_statement._iteration_variable.print(self, indent_level).strip()]
                
            if not for_statement._iteration_range[0]._index_type:
                for_statement._iteration_range[0].hoist(True)
                mlir_str = for_statement._iteration_range[0].print(self, indent_level).strip()
                mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                bounds_casting += ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
                SyntaxTree.Node.context.push_block()
                bounds_casting += indent + f"{mlir_id} = arith.index_cast {mlir_str} : {for_statement._iteration_range[0].get_type().print(self, indent_level).strip()} to index\n"
                lwbs = [mlir_id]
            else:
                lwbs += [for_statement._iteration_range[0].print(self, indent_level).strip()]
                
            if not for_statement._iteration_range[1]._index_type:
                for_statement._iteration_range[1].hoist(True)
                mlir_str = for_statement._iteration_range[1].print(self, indent_level).strip()
                mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                bounds_casting += ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
                SyntaxTree.Node.context.push_block()
                bounds_casting += indent + f"{mlir_id} = arith.index_cast {mlir_str} : {for_statement._iteration_range[1].get_type().print(self, indent_level).strip()} to index\n"
                upbs = [mlir_id]
            else:
                upbs += [for_statement._iteration_range[1].print(self, indent_level).strip()]
                
            bounds_casting += ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
            mlir_id = f"%{SyntaxTree.Node.get_mlir_id()}"
            bounds_casting += indent + f"{mlir_id} = arith.constant 1 : index\n"
            steps = [mlir_id]

        # Collect body
        SyntaxTree.Node.context.push_block()
        statement_prints = [statement.print(self, indent_level + 1) for statement in for_statement._statements]
        statements = ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
        statements += ''.join(statement_prints)

        # Check if loop contains memory allocation (nested memref allocation must be sequential)
        # Use scf.for for allocation loops, scf.parallel for computation loops
        has_allocation = any(isinstance(stmt, SyntaxTree.MemoryAllocation) for stmt in for_statement._statements)
        
        if has_allocation:
            # Use sequential scf.for for memory allocation (cannot parallelize)
            loop_header = (
                f"{indent}scf.for {', '.join(iter_vars)} = "
                f"{', '.join(lwbs)} to {', '.join(upbs)} step {', '.join(steps)} {{\n"
            )
        else:
            # Use scf.parallel for computation loops (safe to parallelize)
            loop_header = (
                f"{indent}scf.parallel ({', '.join(iter_vars)}) = "
                f"({', '.join(lwbs)}) to ({', '.join(upbs)}) step ({', '.join(steps)}) {{\n"
            )
        loop_footer = "\n" + indent + "}\n"

        return bounds_casting + loop_header + statements + loop_footer

    def print_comment(self, comment: SyntaxTree.Comment, indent_level=0):
        return self.get_indent(indent_level) + "//" + comment._comment[1:]

    def print_function_call(self, function_call: SyntaxTree.FunctionCall, indent_level=0):
        indent = self.get_indent(indent_level)
        SyntaxTree.Node.context.push_block()
        
        # Detect if this is a bridge function call
        is_bridge_call = function_call.id in ['sourceTerm', 'flux', 'nonconservativeProduct']
        
        # Process arguments and handle array subscripts
        argument_prints = []
        for i, argument in enumerate(function_call._arguments):
            argument.hoist(True)
            
            # Check if this is a Reference to a 2D subscript for a bridge call
            is_reference_to_2d_subscript = (
                is_bridge_call and
                isinstance(argument, SyntaxTree.Reference) and
                isinstance(argument._expression, SyntaxTree.Subscript) and
                isinstance(argument._expression._value, SyntaxTree.Subscript)
            )
            
            if is_reference_to_2d_subscript:
                # This is Reference(Subscript[Subscript[2D_memref]])
                # Extract the nested structure
                outer_subscript = argument._expression  # Subscript[Subscript[...], col_index]
                inner_subscript = outer_subscript._value  # Subscript[2D_memref, row_index]
                memref_2d = inner_subscript._value  

                outer_subscript.hoist(True)
                outer_mlir = outer_subscript.print(self, indent_level)
                mlir_str_from_outer = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
                SyntaxTree.Node.context.get_block().mlir_append(mlir_str_from_outer)
                
                # Get MLIR representations
                memref_2d.hoist(True)
                memref_2d_mlir = memref_2d.print(self, indent_level).strip(' ')

                # Get the base memref variable name (strip % prefix)
                base_mlir_name = memref_2d_mlir.lstrip('%')
                is_nested_base = False
                if base_mlir_name in SyntaxTree.Node.type_map and SyntaxTree._is_nested_memref(SyntaxTree.Node.type_map[base_mlir_name]):
                    is_nested_base = True
                
                # Use unified method from SyntaxTree.Node to generate pointer arithmetic
                ptr_code, final_ptr = SyntaxTree.Node.generate_pointer_to_2d_element(
                    memref_2d_mlir, inner_subscript._new_index, outer_subscript._new_index, is_nested_base, indent_level
                )
                SyntaxTree.Node.context.get_block().mlir_append(ptr_code)
                arg_mlir = final_ptr
            else:
                # Not a 2D subscript for bridge call - use standard approach
                arg_mlir = argument.print(self, indent_level).strip(' ')
            
            # Regular subscript (array indexing) that needs loading
            if not is_reference_to_2d_subscript and isinstance(argument, SyntaxTree.Subscript):
                # Get the base array and index
                base_value = arg_mlir  # This is the array reference
                index = argument._new_index  # This is the converted index
                base_type = argument._value.get_type()
                
                if isinstance(base_type, SyntaxTree.TDataBlock):
                    # Check if this is a multi-dimensional (2D+) DataBlock
                    if isinstance(base_type._dimensions, int) and base_type._dimensions >= 2:
                        # This is a flat multi-dimensional memref (like memref<?x?xf64>)
                        # When passed to a function expecting memref<?xf64>, just pass it as-is
                        arg_mlir = f"{base_value}[{index}]"
                    else:
                        # Single-dimensional DataBlock
                        element_type_class, is_vector, mlir_element_type = base_type.get_array_element_info()

                        if is_vector:
                            # This is an array of vectors - generate a load to get the vector pointer as i64, then cast to !llvm.ptr
                            result_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                            load_op = indent + f"{result_id} = memref.load {base_value}[{index}] {{name = \"{base_value.strip('%')}\"}} : memref<?x{mlir_element_type}>\n"
                            SyntaxTree.Node.context.get_block().mlir_append(load_op)
                            # Now cast the i64 to !llvm.ptr
                            ptr_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                            cast_op = indent + f"{ptr_id} = llvm.inttoptr {result_id} : {mlir_element_type} to !llvm.ptr\n"
                            SyntaxTree.Node.context.get_block().mlir_append(cast_op)
                            arg_mlir = ptr_id
                        else:
                            # Regular array of scalars - generate a load to get the scalar value
                            result_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                            load_op = indent + f"{result_id} = memref.load {base_value}[{index}] {{name = \"{base_value.strip('%')}\"}} : memref<?x{mlir_element_type}>\n"
                            SyntaxTree.Node.context.get_block().mlir_append(load_op)
                            arg_mlir = result_id
                else:
                    # Handle non-SyntaxTree.TDataBlock types (like nested memrefs)
                    if str(base_type).startswith("memref<?xmemref<?xf64>>"):
                        # This is an array of vectors (stored as memref of memrefs)
                        result_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        load_op = indent + f"{result_id} = memref.load {base_value}[{index}] {{name = \"{base_value.strip('%')}\"}} : memref<?xmemref<?xf64>>\n"
                        SyntaxTree.Node.context.get_block().mlir_append(load_op)
                        arg_mlir = result_id
                    else:
                        # For other types, just pass the reference
                        arg_mlir = f"{base_value}[{index}]"
                
            argument_prints.append(arg_mlir)
        
        argument_block = SyntaxTree.Node.context.pop_block().get_mlir()
        for op in argument_block:
            SyntaxTree.Node.context.get_block().mlir_append(' ' * indent_level * SyntaxTree.Node.spaces_per_tab + op.strip(' ')) 
        
        # NOTE: we lookup the function pointer name in type_map to get the type
        id = function_call.id[0].upper() + function_call.id[1:]
        type_entry = SyntaxTree.Node.type_map[function_call.id]
        # Handle both Type objects and strings
        if isinstance(type_entry, str):
            fn_type = type_entry
        else:
            fn_type = type_entry.print(self).strip()
        
        bridge_function_name = f"{function_call.id}_bridge"
        call_statement = f"""func.call @{bridge_function_name}({", ".join(argument_prints)}) : {fn_type.strip(' ')}"""
        
        SyntaxTree.Node.context.get_block().mlir_append(indent + call_statement)
        return '' 
    
    def print_max(self, max: SyntaxTree.Max, indent_level=0):
        indent = self.get_indent(indent_level)
        
        def unpack_nested_memref_access(expr, indent_level):
            indent = ' ' * indent_level * SyntaxTree.Node.spaces_per_tab
            
            if not (isinstance(expr, SyntaxTree.Subscript) and isinstance(expr._value, SyntaxTree.Subscript)):
                return expr.print(self, indent_level).strip(' ')
            
            outer_subscript = expr._value
            
            if not hasattr(outer_subscript, '_new_index'):
                outer_subscript.print(self, indent_level)
            if not hasattr(expr, '_new_index'):
                expr.print(self, indent_level)
            
            outer_index = getattr(outer_subscript, '_new_index', outer_subscript._index.print(self, indent_level))
            inner_index = getattr(expr, '_new_index', expr._index.print(self, indent_level))
            
            inner_memref_id = f"%{SyntaxTree.Node.get_mlir_id()}"

            # Get the outer memref's MLIR variable name and check type_map
            outer_memref_var = outer_subscript._value.print(self, indent_level).strip(' ')
            
            # Try to determine the outer memref type
            # First, check if this variable is registered in type_map
            try:
                # Get the cpp name to check type_map
                outer_var_cpp = outer_subscript._value.print(self.cpp_printer).strip()
                if outer_var_cpp in SyntaxTree.Node.type_map:
                    type_entry = SyntaxTree.Node.type_map[outer_var_cpp]
                    # Handle both Type objects and strings
                    if isinstance(type_entry, str):
                        outer_memref_type = type_entry
                    else:
                        # Type object - call print(self)
                        outer_memref_type = type_entry.print(self).strip()
                else:
                    # Fallback to default nested memref type
                    outer_memref_type = "memref<?xmemref<?xf64>>"
            except:
                outer_memref_type = "memref<?xmemref<?xf64>>"

            mlir_str = indent + f"{inner_memref_id} = memref.load {outer_memref_var}[{outer_index}] {{name = \"{outer_memref_var.strip('%')}\"}} : {outer_memref_type}\n"
            value_id = f"%{SyntaxTree.Node.get_mlir_id()}"
            mlir_str += indent + f"{value_id} = memref.load {inner_memref_id}[{inner_index}] {{name = \"{outer_memref_var.strip('%')}\"}} : memref<?xf64>\n"
            SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
  
            return value_id
        
        SyntaxTree.Node.context.push_block()
        
        if type(max._lhs.get_type()) is SyntaxTree.TDataBlock:
            max._lhs.hoist(True)
        if type(max._rhs.get_type()) is SyntaxTree.TDataBlock:
            max._rhs.hoist(True)
            
        const_zero_id = None
        if isinstance(max._lhs, SyntaxTree.Subscript) and isinstance(max._lhs._value, SyntaxTree.Subscript):
            lhs = unpack_nested_memref_access(max._lhs, indent_level)
        else:
            lhs = max._lhs.print(self, indent_level).strip(' ')
            if SyntaxTree.TDataBlock.needs_memref_load(max._lhs):
                const_zero_id = SyntaxTree.TDataBlock.ensure_const_zero(const_zero_id, indent_level)
                lhs = SyntaxTree.TDataBlock.generate_memref_load(max._lhs, lhs, max._lhs._index.print(self), indent_level)
            
        if isinstance(max._rhs, SyntaxTree.Subscript) and isinstance(max._rhs._value, SyntaxTree.Subscript):
            rhs = unpack_nested_memref_access(max._rhs, indent_level)
        else:
            rhs = max._rhs.print(self, indent_level).strip(' ')
            if SyntaxTree.TDataBlock.needs_memref_load(max._rhs):
                const_zero_id = SyntaxTree.TDataBlock.ensure_const_zero(const_zero_id, indent_level)
                rhs = SyntaxTree.TDataBlock.generate_memref_load(max._rhs, rhs, const_zero_id, indent_level)

        argument_block = SyntaxTree.Node.context.pop_block().get_mlir()
        for op in argument_block:
            SyntaxTree.Node.context.get_block().mlir_append(op)  
  
        # Use the unified type determination and operation mapping from SyntaxTree.TDataBlock
        op_type_class = SyntaxTree.TDataBlock.get_mlir_type_for_operation(max._lhs, max._rhs)
        result_type = op_type_class().print(self)  

        if op_type_class is SyntaxTree.TInteger:
            mlir_op = "arith.maxsi"
        else:
            mlir_op = "arith.maximumf"  # Default to float
            
        new_id = f"%{SyntaxTree.Node.get_mlir_id()}"
        mlir_str = indent + f"{new_id} = {mlir_op} {lhs}, {rhs} : {result_type}\n"
        SyntaxTree.Node.context.get_block().mlir_append(mlir_str)         
           
        return new_id

    def print_memory_allocation(self, memory_allocation: SyntaxTree.MemoryAllocation, indent_level=0):
        indent = self.get_indent(indent_level)
        
        # Comprehensive tracing for MemoryAllocation
        name_mlir = memory_allocation._name.print(self)
        name_type = memory_allocation._name.get_type()
        is_datablock = isinstance(name_type, SyntaxTree.TDataBlock)
        var_name_for_map = memory_allocation._name.print(self.cpp_printer).strip()
        in_type_map = var_name_for_map in SyntaxTree.Node.type_map

        if len(memory_allocation._dimensions) == 1:
            # Process the dimension expression first and collect any intermediate operations
            SyntaxTree.Node.context.push_block()
            index_val = memory_allocation._dimensions[0].print(self, indent_level)
            intermediate_ops = SyntaxTree.Node.context.pop_block().get_mlir()
            
            # Now build the allocation operations
            index_id = f"%{SyntaxTree.Node.get_mlir_id()}"
            decl = ''.join(intermediate_ops)
            
            # Add the index cast and allocation with correct source type
            index_type = memory_allocation._dimensions[0].get_type()
            index_mlir_type = index_type.print(self).strip()
            decl += indent + f"{index_id} = arith.index_cast {index_val} : {index_mlir_type} to index\n"
            new_id = f"%{SyntaxTree.Node.get_mlir_id()}"
            
            # For 1D allocation, we should always generate 1D memref type
            # Get the element type correctly
            name_type = memory_allocation._name.get_type()
            if isinstance(name_type, SyntaxTree.TDataBlock) or isinstance(name_type, SyntaxTree.TDiagonalMatrix)  or isinstance(name_type, SyntaxTree.TMatrix):
                # Get the element type of the DataBlock
                element_type_class = name_type.get_element_type()
                element_type = element_type_class().print(self)
                mlir_type = f"memref<?x{element_type}>"
            else:
                mlir_type = name_type.print(self)

            if mlir_type is None and SyntaxTree.Node.type_map[var_name_for_map] == "double*":
                mlir_type = "memref<?xf64>"
            matrix_names = ["identity_matrix", "mass_matrix_2d"]
            if name_mlir.strip("%") in matrix_names:
                new_id = "%" + name_mlir.strip("%")
                SyntaxTree.MemoryAllocation.allocated_types[memory_allocation._name.print(self)] = mlir_type
            decl += indent + f"{new_id} = memref.alloc({index_id}) {{name = \"{name_mlir.strip('%')}\"}} : {mlir_type}\n"
            # Track the allocated type for deallocation
            SyntaxTree.MemoryAllocation.allocated_types[new_id] = mlir_type
            
            # Check if this is an inner allocation that needs to be stored in an outer array
            # This happens when memory_allocation._name is a Subscript (indicating arr[i] = allocated_inner_array)
            if isinstance(memory_allocation._name, SyntaxTree.Subscript) and not memory_allocation._specify_type:
                # This is an inner allocation for nested memref, add the store operation
                # Get the outer array name and index
                outer_array = memory_allocation._name._value  # This should be the outer array name
                outer_index = memory_allocation._name._index  
                
                # Process the outer array and index expressions to collect any additional operations
                SyntaxTree.Node.context.push_block()
                outer_array_mlir = outer_array.print(self, indent_level)
                outer_index_mlir = outer_index.print(self, indent_level)
                additional_ops = SyntaxTree.Node.context.pop_block().get_mlir()
                
                # Add any additional operations
                decl += ''.join(additional_ops)
                
                # When storing a memref into a nested memref array, we need to use memref<?xmemref<?xf64>>
                type_mlir = SyntaxTree.Node.get_mlir_type(memory_allocation._type)
                if type_mlir is None:
                    type_entry = SyntaxTree.Node.type_map[memory_allocation._type.print(self.cpp_printer)]
                    # Handle both Type objects and strings
                    if isinstance(type_entry, str):
                        type_mlir = type_entry.strip(' ')
                    else:
                        # Type object - call print(self)
                        type_mlir = type_entry.print(self).strip()
                outer_type = f"memref<?xmemref<?x{type_mlir}>>"
                
                # Add the store operation
                decl += indent + f"memref.store {new_id}, {outer_array_mlir}[{outer_index_mlir}] {{name = \"{outer_array_mlir.strip('%')}\"}} : {outer_type}"
            return decl
        else:
            # For multi-dimensional arrays, check type_map first (ordering is important)
            # This allows overriding default DataBlock behavior for nested memref types
            var_name_for_type_check = memory_allocation._name.print(self.cpp_printer).strip()
            in_type_map = var_name_for_type_check in SyntaxTree.Node.type_map
            
            # Use nested memref if in type_map, otherwise check if DataBlock flat
            if in_type_map:
                # Variable has explicit type specification - use nested memref path
                is_datablock = False  # Force nested path
                actual_dims_calc = -1
            else:
                # For multi-dimensional arrays, check if this is a DataBlock
                # If so, use flat memref with the correct number of dimensions instead of nested memref
                is_datablock = isinstance(memory_allocation._name.get_type(), SyntaxTree.TDataBlock)
                
                # Calculate actual_dims based on DataBlock iteration_range
                if is_datablock:
                    datablock_obj = memory_allocation._name.get_type()

                    if hasattr(datablock_obj, '_iteration_range'):
                        actual_dims_calc = len(datablock_obj._iteration_range)
                    elif isinstance(datablock_obj._dimensions, int):
                        actual_dims_calc = 1 if datablock_obj._dimensions == 1 else 2
                    elif isinstance(datablock_obj._dimensions, list):
                        actual_dims_calc = len(datablock_obj._dimensions) if len(datablock_obj._dimensions) > 0 else 1
                    else:
                        actual_dims_calc = 1
                else:
                    actual_dims_calc = -1  # Not a DataBlock
                
            if is_datablock and actual_dims_calc >= 2:
                # Multi-dimensional DataBlock case: allocate as flat N-D memref
                element_type_class = memory_allocation._name.get_type().get_element_type()
                element_type = element_type_class().print(self)

                # We need actual_dims_calc dimensions from memory_allocation._dimensions
                # For a 2D array: _dimensions might have 2, 3, or 4 elements (depends on iteration_range structure)
                # Take the first actual_dims_calc elements
                dim_exprs = memory_allocation._dimensions[:actual_dims_calc]
                if len(dim_exprs) < actual_dims_calc:
                    # Fallback: if we don't have enough dimensions, use what we have
                    dim_exprs = memory_allocation._dimensions
                
                intermediate_decl = ""
                dim_ids = []
                
                for i, dim_expr in enumerate(dim_exprs):
                    # Evaluate the dimension expression - this appends its operations to context
                    dim_val = dim_expr.print(self, indent_level)
                    dim_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    dim_ids.append(dim_id)
                    dim_type = dim_expr.get_type()
                    dim_mlir_type = dim_type.print(self).strip()
                    
                    # Generate the index cast (will be appended after dimension expressions)
                    intermediate_decl += indent + f"{dim_id} = arith.index_cast {dim_val} : {dim_mlir_type} to index\n"
                
                # These must come before the index_cast operations
                dim_expr_ops = SyntaxTree.Node.context.pop_block().get_mlir()
                intermediate_decl = ''.join(dim_expr_ops) + intermediate_decl
                
                # Re-push a new block for remaining operations
                SyntaxTree.Node.context.push_block()
                
                # Process name and allocate
                name_mlir = memory_allocation._name.print(self, indent_level)
                
                # Build memref type: memref<?x?x...xf64> with actual_dims_calc '?'s
                dimension_list = ["?"] * actual_dims_calc
                memref_dims = "x".join(dimension_list)
                alloc_args = ", ".join(dim_ids)
                
                # Allocate as flat N-D memref
                alloc_line = indent + f"{name_mlir} = memref.alloc({alloc_args}) {{name = \"{name_mlir.strip('%')}\"}} : memref<{memref_dims}x{element_type}>\n"
                
                # Track the allocated memref type so it can be used in load/store operations
                memref_type_full = f"memref<{memref_dims}x{element_type}>"
                SyntaxTree.Node.mlir_memref_types[name_mlir] = memref_type_full
                SyntaxTree.MemoryAllocation.allocated_types[name_mlir] = memref_type_full
                
                return intermediate_decl + alloc_line
            else:
                # For non-DataBlock multi-dimensional arrays, use nested memref with loop
                # Use the existing loop structure created in the constructor
                size = memory_allocation._dimensions[-2]
                for i in range(len(memory_allocation._dimensions) - 3, -1, -1):
                    size = size * memory_allocation._dimensions[i]
                    
                # Process the outer dimension expression first and collect any intermediate operations
                SyntaxTree.Node.context.push_block()
                outer_size_val = memory_allocation._dimensions[-1].print(self, indent_level)
                intermediate_ops = SyntaxTree.Node.context.pop_block().get_mlir()
                
                # Build the allocation operations
                decl = ''.join(intermediate_ops)
                
                # Add the index cast and allocation with correct source type
                outer_size_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                outer_size_type = memory_allocation._dimensions[-1].get_type()
                outer_size_mlir_type = outer_size_type.print(self).strip()
                decl += indent + f"{outer_size_id} = arith.index_cast {outer_size_val} : {outer_size_mlir_type} to index\n"
                
                # Determine the correct type for the outer array
                # Check if this variable name is in type_map (e.g., maxEigenvaluesX, maxEigenvaluesY)
                var_name = memory_allocation._name.print(self.cpp_printer).strip()

                # NOTE: We need to check if the variable name is in type_map first
                if var_name in SyntaxTree.Node.type_map:
                    # Use the explicitly registered type from type_map
                    type_entry = SyntaxTree.Node.type_map[var_name]

                    # Handle both Type objects and strings
                    if isinstance(type_entry, str):
                        outer_type = type_entry
                    else:
                        # Type object (e.g., TNestedMemref) 
                        outer_type = type_entry.print(self).strip()
                elif isinstance(memory_allocation._name.get_type(), SyntaxTree.TDataBlock):
                    # Get the element type correctly for the inner memref
                    element_type_class = memory_allocation._name.get_type().get_element_type()
                    element_type = element_type_class().print(self)
                    outer_type = f"memref<?xmemref<?x{element_type}>>"
                else:
                    # Fallback for non-DataBlock types
                    name_type_mlir = memory_allocation._name.get_type().print(self) 

                    if name_type_mlir is None:
                        type_entry = SyntaxTree.Node.type_map[memory_allocation._name.get_type().print(self.cpp_printer)]
                        # Handle both Type objects and strings
                        if isinstance(type_entry, str):
                            name_type_mlir = type_entry.strip(' ')
                        else:
                            # Type object 
                            name_type_mlir = type_entry.print(self).strip()
                    outer_type = f"memref<?x{name_type_mlir}>"
                
                # Process the name expression to collect any additional operations
                SyntaxTree.Node.context.push_block()
                name_mlir = memory_allocation._name.print(self, indent_level)
                name_ops = SyntaxTree.Node.context.pop_block().get_mlir()
                
                # Add any name-related operations
                decl += ''.join(name_ops)
                decl += indent + f"{name_mlir} = memref.alloc({outer_size_id}) {{name = \"{name_mlir.strip('%')}\"}} : {outer_type}\n"
                # Track the allocated type for deallocation
                SyntaxTree.MemoryAllocation.allocated_types[name_mlir] = outer_type
                
                # Use the existing loop structure for inner allocations
                loop_mlir = memory_allocation._loop.print(self, indent_level)
                
                return decl + loop_mlir

    def print_memory_deallocation(self, memory_deallocation: SyntaxTree.MemoryDeallocation, indent_level=0):
        indent = self.get_indent(indent_level)
        
        # Get the MLIR variable name
        name_mlir = memory_deallocation._name.print(self, indent_level)
        # Use the stored allocated type if available
        if name_mlir in SyntaxTree.MemoryAllocation.allocated_types:
            name_type_mlir = SyntaxTree.MemoryAllocation.allocated_types[name_mlir]
        elif memory_deallocation._allocated_type is not None:
            # Use the allocated type if it was set explicitly
            name_type_mlir = memory_deallocation._allocated_type
        else:
            # Fallback: try to reconstruct the type from the allocation dimensions
            # For nested memrefs (multi-dimensional arrays), we need memref<?xmemref<?xf64>>
            # For 1D arrays, we need memref<?xf64>
            name_element_type = memory_deallocation._name.get_type()
            element_type = name_element_type.print(self).strip()
            
            if len(memory_deallocation._dimensions) == 1:
                name_type_mlir = f"memref<?x{element_type}>"
            else:
                # For multidimensional, assume nested memref
                name_type_mlir = f"memref<?xmemref<?x{element_type}>>"
        
        if name_mlir[0] != "%":
            name_mlir = "%" + name_mlir
        return "\n" + indent + f"""memref.dealloc {name_mlir} : {name_type_mlir}\n"""

    def print_assignment(self, assignment: SyntaxTree.Assignment, indent_level = 0):
        indent = self.get_indent(indent_level)
        mlir_str = ''
        SyntaxTree.Node.context.push_block()
        rhs = assignment._rhs.print(self, indent_level)
        
        rhs_indices = []
        rhs_dims = []
        rhs_casting_mlir = ''
                
        if isinstance(assignment._rhs, SyntaxTree.Subscript):
            mlir_list = SyntaxTree.Node.context.get_block().get_mlir()
            for i in range(assignment._rhs._dimensions):
                rhs_dims.append('?')
                if len(mlir_list) > 0:
                    rhs_indices.insert(0, assignment._rhs._new_index)
                    rhs_casting_mlir += mlir_list.pop()

            mlir_str += ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
            mlir_str += rhs_casting_mlir 
        else:
            # Assume scalar here
            tmp = ''.join(SyntaxTree.Node.context.pop_block().get_mlir())        
            mlir_str += tmp

        SyntaxTree.Node.context.push_block()
        lhs = assignment._lhs.print(self, indent_level)
        
        lhs_indices = []
        lhs_dims = []
        lhs_casting_mlir = ''
        
        if isinstance(assignment._lhs, SyntaxTree.Subscript):
            mlir_list = SyntaxTree.Node.context.get_block().get_mlir()
            for i in range(assignment._lhs._dimensions):
                lhs_dims.append('?')
                if len(mlir_list) > 0:
                    popped = mlir_list.pop()
                    lhs_indices.insert(0, assignment._lhs._new_index)
                    lhs_casting_mlir += popped

            mlir_str += ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
            mlir_str += lhs_casting_mlir
        else:
            tmp = ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
            mlir_str += tmp
        
        # Check if LHS is a flat 2D memref (Subscript[Subscript] on String-based field)
        lhs_is_flat_2d = False
        lhs_memref_name = None
        all_lhs_indices = []  
        
        if isinstance(assignment._lhs, SyntaxTree.Subscript) and isinstance(assignment._lhs._value, SyntaxTree.Subscript):
            # Check if the base is a String-based field
            base_base = assignment._lhs._value._value
            if isinstance(base_base, SyntaxTree.DataBlock) and isinstance(base_base._internal, SyntaxTree.String):
                # This is a String-based field which is now a flat 2D memref
                lhs_is_flat_2d = True
                dsl_name = base_base._internal._value
                lhs_memref_name = SyntaxTree.Node.mlir_symbol_table[dsl_name]

                outer_sub = assignment._lhs
                inner_sub = assignment._lhs._value
                
                if hasattr(inner_sub, '_new_index'):
                    all_lhs_indices.append(inner_sub._new_index)
                if hasattr(outer_sub, '_new_index'):
                    all_lhs_indices.append(outer_sub._new_index)
        
        if lhs_is_flat_2d:
            rhs_value = rhs
            
            lhs_index_str = '[' + ','.join(all_lhs_indices) + ']'
            lhs_type = f"memref<?x?xf64>"
            mlir_str += indent + f"memref.store {rhs_value}, {lhs_memref_name}{lhs_index_str} {{name = \"{lhs_memref_name.strip('%')}\"}} : {lhs_type}\n"
        elif isinstance(assignment._lhs, SyntaxTree.Subscript) and isinstance(assignment._lhs._value, SyntaxTree.Subscript):
            base_base = assignment._lhs._value._value
            
            # If base is not a String field, it's a true nested memref (e.g. lambdaLeft[i][j])
            if not (isinstance(base_base, SyntaxTree.DataBlock) and isinstance(base_base._internal, SyntaxTree.String)):
                # This is a nested memref store (e.g., lambdaLeft[i][j] = value)
                rhs_value = rhs
                if isinstance(assignment._rhs,  SyntaxTree.Float):
                    const_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    mlir_str += indent + f"{const_id} = arith.constant {assignment._rhs._value} : f64\n"
                    rhs_value = const_id
                elif isinstance(assignment._rhs,  SyntaxTree.Integer):
                    const_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    mlir_str += indent + f"{const_id} = arith.constant {assignment._rhs._value} : i32\n"
                    rhs_value = const_id
                elif isinstance(assignment._rhs, SyntaxTree.Subscript):
                    rhs_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    rhs_index_str = '[' + ','.join(rhs_indices) + ']'
                    rhs_type = "memref<?xf64>" if not rhs_dims else f"memref<{ 'x'.join(rhs_dims) }xf64>"
                    mlir_str += indent + f"{rhs_id} = memref.load {rhs}{rhs_index_str} {{name = \"{rhs.strip('%')}\"}} : {rhs_type.strip(' ')}\n"
                    rhs_value = rhs_id
                
                # Extract indices from nested subscript chain
                # lhs._value is the inner subscript, lhs is the outer subscript
                outer_index = assignment._lhs._new_index           
                inner_memref_var = assignment._lhs._value._value.print(self, indent_level)  # Base memref name
                
                # Load the inner memref from the outer nested structure
                inner_memref_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                # Get the actual outer memref type from type_map
                base_var = assignment._lhs._value._value
                type_entry = SyntaxTree.Node.type_map[base_var.id]

                if isinstance(type_entry, str):
                    outer_memref_type = type_entry
                else:
                    outer_memref_type = type_entry.print(self).strip()

                # Load inner memref with first index
                inner_sub_index = assignment._lhs._value._new_index
                mlir_str += indent + f"{inner_memref_id} = memref.load {inner_memref_var}[{inner_sub_index}] {{name = \"{inner_memref_var.strip('%')}\"}} : {outer_memref_type}\n"
                
                # Store directly into the inner memref with the second index
                inner_memref_type = "memref<?xf64>"  # Default inner arrays are 1D
                mlir_str += indent + f"memref.store {rhs_value}, {inner_memref_id}[{outer_index}] {{name = \"{inner_memref_var.strip('%')}\"}} : {inner_memref_type}\n"
        else:
            # Handle regular flat memref assignment (non-nested)
            rhs_id = f"%{SyntaxTree.Node.get_mlir_id()}"         

            lhs_index_str = '[' + ','.join(lhs_indices) + ']'
            # Ensure at least one dimension for memref type
            if lhs_dims:
                lhs_type = f"memref<{ 'x'.join(lhs_dims) }xf64>"
            else:
                lhs_type = "memref<?xf64>"
            rhs_index_str = '[' + ','.join(rhs_indices) + ']'            
            if rhs_dims:
                rhs_type = f"memref<{ 'x'.join(rhs_dims) }x f64>"
            else:
                rhs_type = "memref<?xf64>"
            
            if isinstance(assignment._rhs, SyntaxTree.Subscript):
                mlir_str += indent + f"{rhs_id} = memref.load {rhs}{rhs_index_str} {{name = \"{rhs.strip('%')}\"}} : {rhs_type.strip(' ')}\n"
                rhs_value = rhs_id
            else:
                rhs_value = rhs

            if isinstance(assignment._rhs,  SyntaxTree.Float):
                const_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                mlir_str += indent + f"{const_id} = arith.constant {assignment._rhs._value} : f64\n"
                rhs_value = const_id
            elif isinstance(assignment._rhs,  SyntaxTree.Integer):
                const_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                mlir_str += indent + f"{const_id} = arith.constant {assignment._rhs._value} : i32\n"
                rhs_value = const_id

            if isinstance(assignment._lhs, SyntaxTree.Subscript):
                mlir_str += indent + f"memref.store {rhs_value}, {lhs}{lhs_index_str} {{name = \"{lhs.strip('%')}\"}} : {lhs_type.strip(' ')}\n"
            else:
                lhs_type_obj = assignment._lhs.get_type()
                lhs_mlir_type = lhs_type_obj.print(self, indent_level).strip(' ')
                mlir_str += indent + f"memref.store {rhs_value}, {lhs} {{name = \"{lhs.strip('%')}\"}} : {lhs_mlir_type}\n"
                
        return mlir_str
    
    def print_construction(self, construction: SyntaxTree.Construction, indent_level=0):
        indent = self.get_indent(indent_level)

        if isinstance(construction._expression, SyntaxTree.Integer):
            if construction._expression._string is None:
                expr_mlir = construction._expression.print(self, indent_level)
                
                # Get any operations that were added to the context
                mlir_str = ''.join(SyntaxTree.Node.context.get_block().pop_mlir())
                mlir_str += indent + f"{construction._name.print(self, indent_level)} = {expr_mlir} : {construction._name.get_type().print(self, indent_level).strip(' ')}\n"
            else:        
                # Get another variable id and load the pointer into that, then load the value into the actual variable (construction)
                new_id = SyntaxTree.Node.get_mlir_id()
                mlir_str =  indent + f"%{new_id} = {construction._expression.print(self, indent_level)}\n"

                # Use the expression's type, not the name's type
                expr_type = construction._expression.get_type().print(self, indent_level).strip(' ')
                mlir_str += indent + f"{construction._name.print(self, indent_level)} = llvm.load %{new_id} {{name = \"{construction._name.print(self, indent_level).strip('%')}\"}} : !llvm.ptr -> {expr_type}\n"
        else:
            mlir_str =  f"{self.get_indent(indent_level)}{construction._name.print(self)} = {construction._expression.print(self, indent_level)}\n"
        SyntaxTree.Node.context.get_block().mlir_append(mlir_str)            
        return ''

    def print_data_block_construction_from_existing(self, construction: SyntaxTree.DataBlockConstructionFromExisting, indent_level=0):
        indent = self.get_indent(indent_level)
        #if isinstance(construction._name.get_type(), SyntaxTree.TDiagonalMatrix):
        if construction._name.id[-2:] == "1d":
            return f"""{self.get_indent(indent_level)}{construction._name.print(self)} = builtin.unrealized_conversion_cast %{construction._dataBlock._internal._value} : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> to memref<?xf64> {{name = "{construction._name.print(self.cpp_printer)}"}} """
            #return f"{self.get_indent(indent_level)}{construction._name.print(self)} = %{construction._dataBlock._internal._value}"
        else:
            type_print = construction._name.get_type().print(self, indent_level)

        dims = []
        
        for i in range(len(construction._dataBlock._iteration_range) - 3, -1, -1):
                dims.append(construction._dataBlock._iteration_range[i])
                
        if len(construction._dataBlock._iteration_range) > 1:
            type_print = "!llvm.ptr " #+ f"{construction._dataBlock._underlying_type} dims: {dims})"
    
        # NOTE: We encode structs as Strings, so check here if they are a struct and generate accordingly
        # Set the target variable name so String.print(self) can use it
        target_var_name = construction._name.print(self, indent_level)
        SyntaxTree.Node.target_variable_name = target_var_name
        
        # Store the mapping from DSL name to MLIR variable name for later use in assignments
        # TODO: This is clumsy - it is assuming the exact flow here...
        dsl_name = construction._dataBlock._internal._value
        SyntaxTree.Node.mlir_symbol_table[dsl_name] = target_var_name

        # Generate the struct field access operations with the target variable name
        struct_mlir = construction._dataBlock._internal.print(self, indent_level).strip(' ')
        mlir_str = ''.join(SyntaxTree.Node.context.pop_block().get_mlir())
        
        # Clear the target variable name - we've finished generating the MLIR for this SyntaxTree.Node
        SyntaxTree.Node.target_variable_name = None
        
        return mlir_str 

    def print_data_block_construction_from_operation(self, construction: SyntaxTree.DataBlockConstructionFromOperation, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.assignment.print(self, indent_level)}"""
    
    def print_data_block_construction_from_function(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        SyntaxTree.Node.context.push_block()
        mem_alloc = construction.memoryAllocation.print(self, indent_level)
        init = construction.initialisation.print(self, indent_level)
        alloc_block = SyntaxTree.Node.context.pop_block().get_mlir()
        for op in alloc_block:
            SyntaxTree.Node.context.get_block().mlir_append(op) 
            
        return f"""{mem_alloc}
{init}"""

    def print_data_block_construction_from_matrix_vector_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.assignment.print(self, indent_level)}"""
    
    def print_data_block_construction_from_dot_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        return f"""{construction.memoryAllocation.print(self, indent_level)}
{construction.assignment.print(self, indent_level)}"""
    
    def print_diagonal_matrix(self, matrix: SyntaxTree.DiagonalMatrix, indent_level=0):
        if matrix._id is None:
            return matrix._internal.print(self, indent_level)
        else:
            print(matrix._id)
            return "%" + str(matrix._id)
    
    def print_matrix(self, construction: SyntaxTree.Matrix, indent_level=0):
        if construction.id is None:
            return construction._internal.print(self, indent_level)
        else:
            return "%" + construction.id
    
    def print_diagonal_matrix_construction_from_kronecker_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        SyntaxTree.Node.type_map.update({construction._matrix.print(self.cpp_printer): SyntaxTree.TDiagonalMatrix().print(self.cpp_printer)})
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
        # NOTE: Literals are returned and used in-line, no hoisting required
        return str(boolean._value) 
    
    def print_string(self, string: SyntaxTree.String, indent_level = 0):
        indent = self.get_indent(indent_level) 

        match string._value:
            case x if '.' in x:
                struct, field = x.split('.')
                if struct in SyntaxTree.Node.struct_map:
                    struct_list = SyntaxTree.Node.struct_map[struct]
                    idx = [item[0] for item in struct_list].index(field)
                    mlir_type_target, mlir_type_struct = struct_list[idx][1].split('|||')

                    # Generate the MLIR operations
                    struct_val_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    
                    # TODO: Special case: maxEigenvalue is also a function parameter, so we have to hack it here
                    if field == "maxEigenvalue":
                        field_id = f"%{field}s"
                    else:
                        field_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    
                    # Build the basic MLIR operations
                    mlir_str = indent + f"{struct_val_id} = llvm.load %{struct} {{name = \"{struct.strip('%')}\"}}: !llvm.ptr -> {mlir_type_struct}\n"
                    
                    # If the extracted field is a raw pointer (not a descriptor struct),
                    # extract it directly with a _ptr suffix to distinguish from the final memref
                    if not mlir_type_target.startswith("!llvm.struct<"):
                        # For raw pointers (like maxEigenvalue field [11]), use _ptr naming
                        ptr_field_id = f"%{field}{'_ptr' if field != 'maxEigenvalue' else '_ptr'}"
                        mlir_str += indent + f"{ptr_field_id} = llvm.extractvalue {struct_val_id}[{idx}] : {mlir_type_struct}\n"
                        field_id = ptr_field_id
                    else:
                        # For descriptor structs, extract normally (will be processed below)
                        mlir_str += indent + f"{field_id} = llvm.extractvalue {struct_val_id}[{idx}] : {mlir_type_struct}\n"

                    final_id = field_id

                    # If the extracted field is a descriptor struct, convert it to appropriate memref
                    if mlir_type_target.startswith("!llvm.struct<"):
                        # Use getelementptr[0, field_idx] to properly access struct fields
                        ptr_getelementptr_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        ptr_load_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        mlir_str += indent + f"{ptr_getelementptr_id} = llvm.getelementptr %{struct}[0, {idx}] : (!llvm.ptr) -> !llvm.ptr, {mlir_type_struct}\n"
                        mlir_str += indent + f"{ptr_load_id} = llvm.load {ptr_getelementptr_id} {{name = \"{struct.strip('%')}\"}}: !llvm.ptr -> {mlir_type_target}\n"
                        
                        # Extract all 5 descriptor fields
                        alloc_ptr_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        aligned_ptr_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        offset_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        sizes_0_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        strides_0_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                        
                        mlir_str += indent + f"{alloc_ptr_id} = llvm.extractvalue {ptr_load_id}[0] : {mlir_type_target}\n"
                        mlir_str += indent + f"{aligned_ptr_id} = llvm.extractvalue {ptr_load_id}[1] : {mlir_type_target}\n"
                        mlir_str += indent + f"{offset_id} = llvm.extractvalue {ptr_load_id}[2] : {mlir_type_target}\n"
                        mlir_str += indent + f"{sizes_0_id} = llvm.extractvalue {ptr_load_id}[3, 0] : {mlir_type_target}\n"
                        mlir_str += indent + f"{strides_0_id} = llvm.extractvalue {ptr_load_id}[4, 0] : {mlir_type_target}\n"
                        
                        # Determine target memref type based on descriptor dimensions
                        is_2d = "array<2 x i64>" in mlir_type_target
                        
                        # For 2D descriptors, also extract sizes[1] and strides[1]
                        sizes_1_id = None
                        strides_1_id = None
                        
                        if is_2d:
                            sizes_1_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                            strides_1_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                            mlir_str += indent + f"{sizes_1_id} = llvm.extractvalue {ptr_load_id}[3, 1] : {mlir_type_target}\n"
                            mlir_str += indent + f"{strides_1_id} = llvm.extractvalue {ptr_load_id}[4, 1] : {mlir_type_target}\n"
                        
                        # Use type_map to get the correct memref type for this field
                        # For fields like cellCentre, cellSize, t, dt, we need to use their correct types from type_map
                        if field in SyntaxTree.Node.type_map:
                            type_entry = SyntaxTree.Node.type_map[field]
                            # Handle both Type objects and strings
                            if isinstance(type_entry, str):
                                target_memref_type = type_entry
                            else:
                                # Type object - call print(self)
                                target_memref_type = type_entry.print(self).strip()
                        else:
                            # Fallback to descriptor-based inference
                            if is_2d:
                                # 2D descriptor
                                target_memref_type = "memref<?x?xf64>"
                            else:
                                # 1D descriptor (assume array<1 x i64>)
                                target_memref_type = "memref<?xf64>"

                        # Create memref descriptor from the extracted fields using the new method
                        conversion_mlir, memref_id = SyntaxTree.Node.create_memref_from_extracted_descriptor(
                            alloc_ptr_id,
                            aligned_ptr_id,
                            offset_id,
                            sizes_0_id,
                            sizes_1_id,
                            strides_0_id,
                            strides_1_id,
                            target_memref_type,
                            mlir_type_target,
                            indent_level,
                            target_var_name=SyntaxTree.Node.target_variable_name,
                            is_2d=is_2d)
                        mlir_str += conversion_mlir
                        final_id = memref_id
                    elif 'memref' in mlir_type_target or mlir_type_target == '!llvm.ptr':
                        # For explicit memref types or raw pointers (like maxEigenvalue), 
                        # use create_memref_from_ptr with the extracted pointer (_ptr version)
                        clean_target_type = mlir_type_target.strip('()')
                        # Use the _ptr field_id for raw pointer types
                        conversion_mlir, memref_id = SyntaxTree.Node.create_memref_from_ptr(
                            field_id,  # This is now %maxEigenvalues_ptr for raw pointers
                            clean_target_type, 
                            indent_level, 
                            target_var_name=SyntaxTree.Node.target_variable_name)
                        mlir_str += conversion_mlir
                        final_id = memref_id

                    # Always append to context and return variable ID
                    SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                    return final_id
        return string._value
    
    def print_float(self, float: SyntaxTree.Float, indent_level = 0):
        indent = self.get_indent(indent_level) 

        if float._string is None:
            value = str(float._value)
            if float.hoist():
                float.mlir_id(SyntaxTree.Node.get_mlir_id())
                mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
                mlir_str += indent + f"%{float.mlir_id()} = arith.constant {float._value} : {float.get_type().print(self)}\n"
                SyntaxTree.Node.context.push_block()
                SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                id = f"%{float.mlir_id()}"
                return id
            return value
        else:
            return str(float._string) 
    
    def print_data_block(self, data_block: SyntaxTree.DataBlock, indent_level=0):
        if data_block.id is None:
            return data_block._internal.print(self, indent_level)
        else:
            return "%" + data_block.id

    def print_subscript(self, subscript: SyntaxTree.Subscript, indent_level=0):
        indent = self.get_indent(indent_level)
        dims = 1
        tmp = subscript._value
        while isinstance(tmp, SyntaxTree.Subscript):
            dims += 1
            tmp = tmp._value

        subscript._dimensions = dims
        value_type = subscript._value.get_type()
        type_str = value_type.print(self.cpp_printer).strip()

        mlir_type = SyntaxTree.Node.type_map.get(type_str)
        if mlir_type is None:
            # Fallback: try to get the MLIR type from the type object itsubscript, or use a default
            mlir_type = '' + value_type.print(self)
                
        mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())

        subscript._index.hoist(True)
        index = subscript._index.print(self, indent_level)
        mlir_str += "".join(SyntaxTree.Node.context.get_block().pop_mlir())

        if not getattr(subscript._index, '_index_type', False):
            new_index = f"%{SyntaxTree.Node.get_mlir_id()}"
            # Determine the actual type of the index expression
            index_expr_type = subscript._index.get_type()
            mlir_type_cast = index_expr_type.print(self).strip()
            mlir_str += indent + new_index + f" = arith.index_cast {index.strip(' ')}: {mlir_type_cast} to index\n"
            subscript._new_index = new_index
        else:
            subscript._new_index = index

        # NOTE: special handling: Strings might represent struct field access
        if isinstance(subscript._value, SyntaxTree.String):
            # When indexing a String field (like patchData.input[i]), we need to handle the index
            # Get the field type to determine if it's 2D or 1D
            base_mlir = subscript._value.print(self, indent_level)
            base_type = subscript._value.get_type()
            
            if isinstance(base_type, SyntaxTree.TDataBlock):
                memref_type = base_type.print(self).strip()
                
                # Check if it's a 2D flat or 1D nested memref
                if "x?x" in memref_type:
                    # 2D flat memref - but we only have one index, which means we need a second one
                    # This shouldn't happen for single-level subscripts on 2D arrays
                    result_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    mlir_str += indent + f"{result_id} = memref.load {base_mlir}[%0, {subscript._new_index}] {{name = \"{base_mlir.strip('%')}\"}} : {memref_type}\n"
                    SyntaxTree.Node.context.push_block()
                    SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                    return result_id
                else:
                    # 1D memref - can be nested (memref<?xmemref<?xf64>>) or plain (memref<?xf64>)
                    result_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                    mlir_str += indent + f"{result_id} = memref.load {base_mlir}[{subscript._new_index}] {{name = \"{base_mlir.strip('%')}\"}} : {memref_type}\n"
                    SyntaxTree.Node.context.push_block()
                    SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                    return result_id
            else:
                # Not a memref type, just return the base
                return base_mlir

        SyntaxTree.Node.context.push_block()
        SyntaxTree.Node.context.get_block().mlir_append(mlir_str)

        subscript._value.hoist(True)
        base_value = subscript._value.print(self, indent_level)

        # Handle nested subscripts (like source[i][j]) by generating proper load operations
        if subscript.is_nested_subscript():
            result_id = f"%{SyntaxTree.Node.get_mlir_id()}"

            base_expr = tmp  # The base expression without subscripts
            outer_base = base_expr.print(self, indent_level)  # The base array
            
            # Get the actual type of the base expression - check mlir_memref_types FIRST
            if outer_base in SyntaxTree.Node.mlir_memref_types:
                actual_memref_type = SyntaxTree.Node.mlir_memref_types[outer_base]
            else:
                # Fallback to computing from type
                base_expr_type = base_expr.get_type()
                if isinstance(base_expr_type, SyntaxTree.TDataBlock):
                    actual_memref_type = base_expr_type.print(self).strip()
                else:
                    actual_memref_type = "memref<?x?xf64>"  # Fallback
            
            # Check if base expression has an explicit type in type_map (nested memref override)
            base_name = None
            if isinstance(base_expr, SyntaxTree.Name):
                base_name = base_expr.id
            elif hasattr(base_expr, 'id'):
                # DataBlock or similar object with id attribute
                base_name = base_expr.id
            
            if base_name and base_name in SyntaxTree.Node.type_map and SyntaxTree._is_nested_memref(SyntaxTree.Node.type_map[base_name]):
                actual_memref_type = SyntaxTree.Node.type_map[base_name]
            
            # Collect both indices from the nested structure
            indices = []
            current = subscript
            while isinstance(current, SyntaxTree.Subscript):
                indices.insert(0, current._new_index)
                current = current._value
            
            # Generate load based on memref type structure
            indices_str = ", ".join(indices)
            mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
            
            # Check if it's a nested memref (memref<?xmemref<...>>) vs flat multi-D (memref<?x?x...>)
            # Handle both Type objects and strings
            if isinstance(actual_memref_type, str):
                is_nested = actual_memref_type.count("memref<") > 1
                actual_memref_type_str = actual_memref_type
            else:
                # Type object (e.g., TNestedMemref)
                is_nested = actual_memref_type.is_nested_memref()
                actual_memref_type_str = actual_memref_type.print(self).strip()
            
            if is_nested and len(indices) >= 2:
                # This is a nested memref (memref<?xmemref<?xf64>>)
                # Need to load in two steps: first get the inner memref, then load from it
                inner_memref_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                # Extract the inner memref type from the outer type
                if actual_memref_type_str.startswith("memref<?x") and actual_memref_type_str.endswith(">>"):
                    inner_type = actual_memref_type_str[9:-1]  # Remove "memref<?x" and final ">"
                else:
                    inner_type = "memref<?xf64>"  # Fallback
                
                mlir_str += indent + f"{inner_memref_id} = memref.load {outer_base}[{indices[0]}] {{name = \"{outer_base.strip('%')}\"}} : {actual_memref_type_str}\n"
                # Now load from the inner memref with remaining indices
                remaining_indices = ", ".join(indices[1:])
                mlir_str += indent + f"{result_id} = memref.load {inner_memref_id}[{remaining_indices}] {{name = \"{outer_base.strip('%')}\"}} : {inner_type}\n"
            else:
                # This is a flat multi-D memref (memref<?x?xf64>, memref<?x?x?xf64>, etc.)
                mlir_str += indent + f"{result_id} = memref.load {outer_base}[{indices_str}] {{name = \"{outer_base.strip('%')}\"}} : {actual_memref_type_str}\n"

            SyntaxTree.Node.context.push_block()
            SyntaxTree.Node.context.get_block().mlir_append(mlir_str)

            return result_id

        # Default: just return the base value (may be a memref, pointer, or value)
        return base_value

    def print_reference(self, reference: SyntaxTree.Reference, indent_level=0):
        result_id = None
        indent = self.get_indent(indent_level)
        mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
        
        reference._expression.hoist(True)
               
        # Handle references to subscripted expressions (array elements)
        if isinstance(reference._expression, SyntaxTree.Subscript):
            base_expr = reference._expression._value
            index_expr = reference._expression._index

            base_expr.hoist(True)
            base_mlir = base_expr.print(self, indent_level)
            
            mlir_str += "".join(SyntaxTree.Node.context.get_block().pop_mlir())
            
            if not hasattr(reference._expression, '_new_index'):
                # If _new_index doesn't exist, process index and generate it now
                index_expr.hoist(True)
                index_mlir = index_expr.print(self, indent_level)
                mlir_str += "".join(SyntaxTree.Node.context.get_block().pop_mlir())
                
                # Create new index with proper type cast
                new_index = f"%{SyntaxTree.Node.get_mlir_id()}"
                mlir_type = ('index' if hasattr(index_expr, '_index_type') and index_expr._index_type else 'i32')
                mlir_str += indent + new_index + f" = arith.index_cast {index_mlir.strip(' ')}: {mlir_type} to index\n"
                reference._expression._new_index = new_index
            else:
                new_index = reference._expression._new_index
            
            # Determine dimensions and type of the memref
            base_type = base_expr.get_type()          
            if isinstance(base_type, SyntaxTree.TDataBlock):
                result_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                
                # For nested memrefs (like double**), handle the inner memref properly
                if isinstance(base_expr, SyntaxTree.Subscript):
                    # base_expr is Subscript[DataBlock, i] 
                    # We need to check if the base_expr itreference is a nested subscript or just single-level
                    if isinstance(base_expr._value, SyntaxTree.Subscript):
                        result_id = base_mlir
                    else:
                        # Check if this variable has an explicit nested type in `type_map`
                        base_mlir_name = base_mlir.lstrip('%')
                        is_nested_memref = False
                        if base_mlir_name in SyntaxTree.Node.type_map and SyntaxTree._is_nested_memref(SyntaxTree.Node.type_map[base_mlir_name]):
                            is_nested_memref = True
                        
                        outer_index = base_expr._new_index
                        
                        # Use unified method from SyntaxTree.Node to generate pointer arithmetic
                        ptr_code, result_id = SyntaxTree.Node.generate_pointer_to_2d_element(
                            base_mlir, outer_index, new_index, is_nested_memref, indent_level
                        )

                        mlir_str += ptr_code
                else:
                    element_type = "f64"  # Default to f64 - modify if needed for other types
                    mlir_str += indent + f"{result_id} = memref.subview {base_mlir}[{new_index}][1][1] : memref<?x{element_type}> to memref<?x{element_type}>\n"
        
        # Default behavior for non-subscript references
        SyntaxTree.Node.context.push_block()
        SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
        
        # Process the expression and return its ID
        return result_id if result_id is not None else reference._expression.print(self, indent_level)

    def print_vector(self, vector: SyntaxTree.Vector, indent_level=0):
        pass
    
    def print_vector_index(self, vector_index: SyntaxTree.VectorIndex, indent_level=0):
        indent = self.get_indent(indent_level)
        mlir_str = ""

        if isinstance(vector_index._value, SyntaxTree.Subscript):
            array_base = vector_index._value._value
            array_index = vector_index._value._index
            array_base_mlir = array_base.print(self, indent_level)
            array_index_mlir = array_index.print(self, indent_level)
            # Ensure array_index is of type 'index' with correct source type
            if not getattr(array_index, '_index_type', False):
                array_index_cast = f"%{SyntaxTree.Node.get_mlir_id()}"
                array_index_type = array_index.get_type()
                array_index_mlir_type = array_index_type.print(self).strip()
                mlir_str += indent + f"{array_index_cast} = arith.index_cast {array_index_mlir} : {array_index_mlir_type} to index\n"
            else:
                array_index_cast = array_index_mlir

        raw_ptr = f"%{SyntaxTree.Node.get_mlir_id()}"
        mlir_str += indent + f"{raw_ptr} = memref.load {array_base_mlir}[{array_index_cast}] {{name = \"{array_base_mlir.strip('%')}\"}} : memref<?xi64>\n"

        vec_ptr = f"%{SyntaxTree.Node.get_mlir_id()}"
        mlir_str += indent + f"{vec_ptr} = llvm.inttoptr {raw_ptr} {{name = \"{array_base_mlir.strip('%')}\"}} : i64 to !llvm.ptr\n"

        func_param_mlir = vector_index._index.print(self, indent_level)
        # Ensure func_param is i32 (emit arith.constant for Integer, cast otherwise)
        if isinstance(vector_index._index,  SyntaxTree.Integer):
            func_param_i32 = f"%{SyntaxTree.Node.get_mlir_id()}"
            mlir_str += indent + f"{func_param_i32} = arith.constant {int(vector_index._index._value)} : i32\n"
        elif not getattr(vector_index._index, '_index_type', False):
            func_param_i32 = f"%{SyntaxTree.Node.get_mlir_id()}"
            mlir_str += indent + f"{func_param_i32} = arith.index_cast {func_param_mlir} : index to i32\n"
        else:
            func_param_i32 = func_param_mlir

        result = f"%{SyntaxTree.Node.get_mlir_id()}"
        # Convert i32 index to i64 for pointer arithmetic
        func_param_i64 = f"%{SyntaxTree.Node.get_mlir_id()}"
        mlir_str += indent + f"{func_param_i64} = arith.extsi {func_param_i32} : i32 to i64\n"
        # Compute pointer to array element
        velem_ptr = f"%{SyntaxTree.Node.get_mlir_id()}"
        mlir_str += indent + f"{velem_ptr} = llvm.getelementptr {vec_ptr}[{func_param_i64}] {{name = \"{array_base_mlir.strip('%')}\"}} : (!llvm.ptr, i64) -> !llvm.ptr, f64\n"
        # Load the value from the computed pointer
        mlir_str += indent + f"{result} = llvm.load {velem_ptr} {{name = \"{array_base_mlir.strip('%')}\"}} : !llvm.ptr -> f64\n"
        SyntaxTree.Node.context.get_block().mlir_append(mlir_str)

        return result
    
    def print_comparison(self, comparison: SyntaxTree.Comparison, indent_level=0):
        op_type_class = SyntaxTree.TDataBlock.get_mlir_type_for_operation(comparison._lhs, comparison._rhs)
        
        mlir_operation = Comparison.get_mlir_comparison_op(comparison._operation, op_type_class)
        result_type = "i1"  # Comparisons always return boolean
        
        if mlir_operation is None:
            raise ValueError(f"Unsupported comparison operation: {comparison._operation}")
        
        const_zero_id = None
        
        lhs_mlir = comparison._lhs.print(self, indent_level)
        if SyntaxTree.TDataBlock.needs_memref_load(comparison._lhs):
            const_zero_id = SyntaxTree.TDataBlock.ensure_const_zero(const_zero_id, indent_level)
            lhs_mlir = SyntaxTree.TDataBlock.generate_memref_load(comparison._lhs, lhs_mlir, const_zero_id, indent_level)
            
        rhs_mlir = comparison._rhs.print(self, indent_level)
        if SyntaxTree.TDataBlock.needs_memref_load(comparison._rhs):
            const_zero_id = SyntaxTree.TDataBlock.ensure_const_zero(const_zero_id, indent_level)
            rhs_mlir = SyntaxTree.TDataBlock.generate_memref_load(comparison._rhs, rhs_mlir, const_zero_id, indent_level)

        return self.get_indent(indent_level) + f"""{mlir_operation} {lhs_mlir}, {rhs_mlir} : {result_type}"""

    def print_binary_operation(self, binary_operation: SyntaxTree.BinaryOperation, indent_level=0):
        indent = self.get_indent(indent_level)
        binary_operation._lhs.hoist(True)
        binary_operation._rhs.hoist(True)  
        
        op_type_class = SyntaxTree.TDataBlock.get_mlir_type_for_operation(binary_operation._lhs, binary_operation._rhs)
        result_type = op_type_class().print_mlir()
        
        op_type_class = SyntaxTree.TDataBlock.get_mlir_type_for_operation(binary_operation._lhs, binary_operation._rhs)
        result_type = op_type_class().print(self)

        const_zero_id = None

        binary_operation._lhs.hoist(True)
        lhs = binary_operation._lhs.print(self, indent_level).strip(' ')
        
        if SyntaxTree.TDataBlock.needs_memref_load(binary_operation._lhs):
            const_zero_id = SyntaxTree.TDataBlock.ensure_const_zero(const_zero_id, indent_level)
            lhs = SyntaxTree.TDataBlock.generate_memref_load(binary_operation._lhs, lhs, const_zero_id, indent_level)
            
        lhs = SyntaxTree.BinaryOperation.cast_operand(lhs, binary_operation._lhs, op_type_class, indent_level)

        binary_operation._rhs.hoist(True)
        rhs = binary_operation._rhs.print(self, indent_level).strip(' ')
        
        if SyntaxTree.TDataBlock.needs_memref_load(binary_operation._rhs):
            const_zero_id = SyntaxTree.TDataBlock.ensure_const_zero(const_zero_id, indent_level)
            rhs = SyntaxTree.TDataBlock.generate_memref_load(binary_operation._rhs, rhs, const_zero_id, indent_level)

        rhs = SyntaxTree.BinaryOperation.cast_operand(rhs, binary_operation._rhs, op_type_class, indent_level)

        mlir_str = ''
        for mlir in SyntaxTree.Node.context.pop_block().get_mlir():
            mlir_str += indent + mlir.strip(' ')
        
        binary_operation.mlir_id(f"%{SyntaxTree.Node.get_mlir_id()}")
        
        mlir_str += indent + f"""{binary_operation.mlir_id()} = {binary_operation.get_mlir_op()} {lhs}, {rhs} : {result_type}\n"""        
        SyntaxTree.Node.context.push_block()
        SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
            
        return binary_operation.mlir_id()
    
    def print_unary_operation(self, unary_operation: SyntaxTree.UnaryOperation, indent_level=0):
        indent = self.get_indent(indent_level)
        unary_operation._operand.hoist(True)

        operand_type = unary_operation._operand.get_type()
        if isinstance(operand_type, SyntaxTree.TDataBlock):
            element_type_class = operand_type.get_element_type()
            operand_type = element_type_class() if element_type_class else operand_type

        if isinstance(operand_type, SyntaxTree.TFloat):
            if unary_operation._operation == "+":
                SyntaxTree.Node.context.get_block().mlir_append(unary_operation._operand.print(self, indent_level))
                return unary_operation.mlir_id()
            elif unary_operation._operation == "-":
                op_mlir = unary_operation._operand.print(self, indent_level).strip(' ')
                mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
                neg_one_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                mlir_str += indent + f"{neg_one_id} = arith.constant -1.0 : f64\n"

                unary_operation.mlir_id(f"%{SyntaxTree.Node.get_mlir_id()}")
                mlir_str += indent + f"{unary_operation.mlir_id()} = arith.mulf {op_mlir}, {neg_one_id} : f64\n"
                SyntaxTree.Node.context.push_block()
                SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                return unary_operation.mlir_id()
        else:
            if unary_operation._operation == "+":
                SyntaxTree.Node.context.get_block().mlir_append(unary_operation._operand.print(self, indent_level))
                return unary_operation.mlir_id()
            elif unary_operation._operation == "-":
                op_mlir = unary_operation._operand.print(self, indent_level).strip(' ')
                mlir_str = "".join(SyntaxTree.Node.context.get_block().pop_mlir())
                neg_one_id = f"%{SyntaxTree.Node.get_mlir_id()}"
                mlir_str += indent + f"{neg_one_id} = arith.constant -1 : i32\n"

                unary_operation.mlir_id(f"%{SyntaxTree.Node.get_mlir_id()}")
                mlir_str += indent + f"{unary_operation.mlir_id()} = arith.muli {op_mlir}, {neg_one_id} : i32\n"
                SyntaxTree.Node.context.push_block()
                SyntaxTree.Node.context.get_block().mlir_append(mlir_str)
                return unary_operation.mlir_id()
    
    def print_data_block_unary_max(self, unary_max: SyntaxTree.DataBlockUnaryMax, indent_level=0):
        return unary_max._loop.print(self, indent_level)
    
    def print_custom_type(self, type: SyntaxTree.TCustom, indent_level=0):
        mlir_type = SyntaxTree.Node.get_mlir_type(type._type)
        if mlir_type is not None:
            return ' ' * indent_level * SyntaxTree.Node.spaces_per_tab + mlir_type
        else:
            # Fallback to type_map for complex types
            type_entry = SyntaxTree.Node.type_map[f"{type._type}"]
            if isinstance(type_entry, str):
                return self.get_indent(indent_level) + type_entry
            else:
                # Type object (e.g., TVoid, TFloat, etc.)
                return self.get_indent(indent_level) + type_entry.print(self)
    
    def print_void_type(self, type: SyntaxTree.TVoid, indent_level=0):
        return self.get_indent(indent_level) + 'void'

    def print_integer_type(self, type: SyntaxTree.TInteger, indent_level=0):
        return self.get_indent(indent_level) + "i32"
    
    def print_long_type(self, type: SyntaxTree.TLong, indent_level=0):
        return self.get_indent(indent_level) + "i64"
    
    def print_boolean_type(self, type: SyntaxTree.TBoolean, indent_level=0):
        return self.get_indent(indent_level) + "i1"
    
    def print_float_type(self, type: SyntaxTree.TFloat, indent_level=0):
        return self.get_indent(indent_level) + "f64"
    
    def print_string_type(self, type: SyntaxTree.TString, indent_level=0):
        pass
    
    def print_data_block_type(self, type: SyntaxTree.TString, indent_level=0):
        dims = []
        
        if isinstance(type._dimensions, int):
            dims.append(str(type._dimensions))
        else:
            for dim in type._dimensions:
                match dim:
                    case int():
                        dims.append(str(dim))                    
                    case Expression():
                        dims.append(dim.print(self, indent_level))
                    case list():
                        SyntaxTree.Node.context.push_block()
                        for op in dim:
                            op.print(self, indent_level)
                        dims.append(''.join(SyntaxTree.Node.context.pop_block().get_mlir()))
                                    
        # NOTE: The underlying type isn't an MLIR type here but a string representing the C++ type. So...
        # 	We look up the C++ type string in a types map in SyntaxTree.Node and get the MLIR type
        if isinstance(type._underlying_type, SyntaxTree.Type):
            type_name = type._underlying_type.print(self, indent_level)
        else:
            # We have a String type here
            type_name = SyntaxTree.Node.get_mlir_type(type._underlying_type._value)
            if type_name is None:
                # Fallback to type_map for complex types
                type_name = SyntaxTree.Node.type_map[f"{type._underlying_type}"]
        
        # If type_name is already a memref type (e.g., from Vector types that map to 'memref<?xi64>'),
        # return it directly instead of wrapping it again. This prevents creating invalid nested memrefs like 'memref<?xmemref<?xi64>>'.
        if isinstance(type_name, str) and type_name.startswith("memref<"):
            return ' ' * indent_level * SyntaxTree.Node.spaces_per_tab + type_name
        
        # Build the memref type with the correct number of dimensions
        # NOTE: type._dimensions may include halo dimensions, so we need to infer actual data dimensions
        # For now, we use a heuristic: if 1D field, return 1D; if multi-D, assume 2D (common case)
        # The type_map provides the correct types for named fields, so this should rarely be used
        
        if isinstance(type._dimensions, int):
            if type._dimensions == 1:
                actual_dims = 1
            else:
                # Multi-dimensional iteration ranges with halo are typically 2D or 3D data
                # For now, assume 2D as fallback
                actual_dims = 2
        elif isinstance(type._dimensions, list):
            if len(type._dimensions) <= 1:
                actual_dims = 1
            else:
                actual_dims = 2  # Fallback for multi-dim
        else:
            actual_dims = 1  # Fallback
        
        # Build memref with ? for each actual dimension
        dimension_list = ["?"] * actual_dims
        memref_dims = "x".join(dimension_list)
        
        return self.get_indent(indent_level) + f"""memref<{memref_dims}x{type_name}>"""
    
    def print_vector_type(self, type: SyntaxTree.TVector, indent_level=0):
        return self.get_indent(indent_level) + f'memref<?x{type._element_type.print(self).strip()}>'

    def print_nested_memref_type(self, type: SyntaxTree.TNestedMemref, indent_level=0):
        return self.get_indent(indent_level) + f'memref<?xmemref<?x{type._element_type.print(self).strip()}>>'
    
    def print_simple_memref_type(self, type: SyntaxTree.TSimpleMemref, indent_level=0):
        # Build memref<?x...xf64> with appropriate number of dimensions
        dims = 'x'.join(['?'] * type._dimensions)
        return self.get_indent(indent_level) + f'memref<{dims}x{type._element_type.print(self).strip()}>'  # Use print_mlir for element type
    
    def print_function_type(self, type: SyntaxTree.TFunctionType, indent_level=0):
        return self.get_indent(indent_level) + type._signature
