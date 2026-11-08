from abc import ABC, abstractmethod
import exahype2.dslhype.SyntaxTree as SyntaxTree

class Printer(ABC):
    @abstractmethod
    def print_function_definition(self, function_definition: SyntaxTree.FunctionDefinition, indent_level = 0):
        pass

    @abstractmethod
    def print_function_declaration(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        pass

    @abstractmethod
    def print_function_definition_with_timer(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        pass

    @abstractmethod
    def print_function_declaration_with_timer(self, function_definition: SyntaxTree.FunctionDefinition, indent_level=0):
        pass

    @abstractmethod
    def print_argument(self, argument: SyntaxTree.Argument, indent_level=0):
        pass

    @abstractmethod
    def print_name(self, name: SyntaxTree.Name, indent_level=0):
        pass

    @abstractmethod
    def print_if(self, if_statement: SyntaxTree.If, indent_level=0):
        pass

    @abstractmethod
    def print_for(self, for_statement: SyntaxTree.For, indent_level = 0):
        pass

    @abstractmethod
    def print_comment(self, comment: SyntaxTree.Comment, indent_level=0):
        pass

    @abstractmethod
    def print_function_call(self, function_call: SyntaxTree.FunctionCall, indent_level=0):
        pass
    
    @abstractmethod
    def print_max(self, max: SyntaxTree.Max, indent_level=0):
        pass

    @abstractmethod
    def print_memory_allocation(self, memory_allocation: SyntaxTree.MemoryAllocation, indent_level=0):
        pass

    @abstractmethod
    def print_memory_deallocation(self, memory_deallocation: SyntaxTree.MemoryDeallocation, indent_level=0):
        pass

    @abstractmethod
    def print_assignment(self, assignment: SyntaxTree.Assignment, indent_level = 0):
        pass
    
    @abstractmethod
    def print_construction(self, construction: SyntaxTree.Construction, indent_level=0):
        pass

    @abstractmethod
    def print_data_block_construction_from_existing(self, construction: SyntaxTree.DataBlockConstructionFromExisting, indent_level=0):
        pass

    @abstractmethod
    def print_data_block_construction_from_operation(self, construction: SyntaxTree.DataBlockConstructionFromOperation, indent_level=0):
        pass
    
    @abstractmethod
    def print_data_block_construction_from_function(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_data_block_construction_from_matrix_vector_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_data_block_construction_from_dot_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_diagonal_matrix(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_matrix(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_diagonal_matrix_construction_from_kronecker_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_diagonal_matrix_construction_from_value(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass
    
    @abstractmethod
    def print_matrix_construction_from_kronecker_product(self, construction: SyntaxTree.DataBlockConstructionFromFunction, indent_level=0):
        pass

    @abstractmethod
    def print_data_block_assignment(self, assignment: SyntaxTree.DataBlockAssignment, indent_level=0):
        pass

    @abstractmethod
    def print_integer(self, integer: SyntaxTree.Integer, indent_level = 0):
        pass
    
    @abstractmethod
    def print_boolean(self, boolean: SyntaxTree.Boolean, indent_level = 0):
        pass
    
    @abstractmethod
    def print_string(self, string: SyntaxTree.String, indent_level = 0):
        pass
    
    @abstractmethod
    def print_float(self, float: SyntaxTree.Float, indent_level = 0):
        pass
    
    @abstractmethod
    def print_data_block(self, data_block: SyntaxTree.DataBlock, indent_level=0):
        pass

    @abstractmethod
    def print_subscript(self, subscript: SyntaxTree.Subscript, indent_level=0):
        pass

    @abstractmethod
    def print_reference(self, reference: SyntaxTree.Reference, indent_level=0):
        pass

    @abstractmethod
    def print_vector(self, vector: SyntaxTree.Vector, indent_level=0):
        pass
    
    @abstractmethod
    def print_vector_index(self, vector_index: SyntaxTree.VectorIndex, indent_level=0):
        pass
    
    @abstractmethod
    def print_comparison(self, comparison: SyntaxTree.Comparison, indent_level=0):
        pass

    @abstractmethod
    def print_binary_operation(self, binary_operation: SyntaxTree.BinaryOperation, indent_level=0):
        pass
    
    @abstractmethod
    def print_unary_operation(self, unary_operation: SyntaxTree.UnaryOperation, indent_level=0):
        pass
    
    @abstractmethod
    def print_data_block_unary_max(self, unary_max: SyntaxTree.DataBlockUnaryMax, indent_level=0):
        pass
    
    @abstractmethod
    def print_custom_type(self, type: SyntaxTree.TCustom, indent_level=0):
        pass
    
    @abstractmethod
    def print_void_type(self, type: SyntaxTree.TVoid, indent_level=0):
        pass

    @abstractmethod
    def print_integer_type(self, type: SyntaxTree.TInteger, indent_level=0):
        pass
    
    @abstractmethod
    def print_long_type(self, type: SyntaxTree.TLong, indent_level=0):
        pass
    
    @abstractmethod
    def print_boolean_type(self, type: SyntaxTree.TBoolean, indent_level=0):
        pass
    
    @abstractmethod
    def print_float_type(self, type: SyntaxTree.TFloat, indent_level=0):
        pass
    
    @abstractmethod
    def print_string_type(self, type: SyntaxTree.TString, indent_level=0):
        pass
    
    @abstractmethod
    def print_data_block_type(self, type: SyntaxTree.TDataBlock, indent_level=0):
        pass
    
    def print_vector_type(self, type: SyntaxTree.TVector, indent_level=0):
        pass
#
#    @abstractmethod
#    def print_matrix_type(self, type: SyntaxTree.TMatrix, indent_level=0):
#        pass
#
#    @abstractmethod
#    def print_diagonal_matrix_type(self, type: SyntaxTree.TDiagonalMatrix, indent_level=0):
#        pass
    
    @abstractmethod
    def print_nested_memref_type(self, type: SyntaxTree.TNestedMemref, indent_level=0):
        pass
    
    @abstractmethod
    def print_simple_memref_type(self, type: SyntaxTree.TSimpleMemref, indent_level=0):
        pass
    
    @abstractmethod
    def print_function_type(self, type: SyntaxTree.TFunctionType, indent_level=0):
        pass