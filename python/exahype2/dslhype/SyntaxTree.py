from abc import ABC, abstractmethod
import os
import copy
from enum import Enum
from operator import itemgetter
import exahype2.dslhype.Printer as Printer

# We need to track the context of the code generation to support MLIR
class Context:

    def __init__(self):
        # A stack of blocks e.g. for loops where we can store hoisted MLIR operations
        self._blocks = [ Context.Block() ] 	
        
    def push_block(self):
        self._blocks.append(Context.Block())
        
    def pop_block(self):
        if self._blocks:
            return self._blocks.pop()
        else:
            return Context.Block()
        
    def get_block(self):
        if not self._blocks:
            self.push_block()
        return self._blocks[-1]

    def clear_block(self):
        if self._blocks:
            self._blocks[-1] = Context.Block()

    # We need to store block-level MLIR that has been hoisted and ensure we don't
    # repeat casting operators for indices within a block e.g. loops    
    class Block:
        
        def __init__(self):
            # We use this to hoist MLIR operations out of array indexing, function calls etc.
            self._mlir = []            
            # We use a set to store the ids that we have already processed within the block
            self._ids = {}           

        def push_mlir(self):
            self._mlir.append([])

        def pop_mlir(self):
            if self._mlir:
                return self._mlir.pop()
            else:
                return []
            
        def get_mlir(self):
            if not self._mlir:
                self.push_mlir()
            return self._mlir[-1]    

        def mlir_append(self, op):
            self.get_mlir().append(op)  
            
        # def add_id(self, id):
        #     self._ids.update(id)

        # def remove_id(self, id):
        #     self._ids.remove(id)

        # def get_id(self, id):
        #     return self._ids.get(id)
                        
        # def find_id(self, id):
        #     return id in self._ids


        
class Node(ABC):
    output_format = None
    spaces_per_tab = 4
    use_accelerator = False
    use_memory_manager = False
    mlir_id_count = 0	# We need a variable to keep track of the MLIR ids %0, %1 etc.
    type_map = {}
    struct_map = {}
    context = Context()
    target_variable_name = None  # For tracking when we're generating a specific variable name
    mlir_symbol_table = {}  # Maps DSL names (e.g., "patchData.QOut") to MLIR variable names (e.g., "%output")
    mlir_memref_types = {}  # Maps MLIR variable names (e.g., "%output") to their memref types (e.g., "memref<?x?xf64>")
   
    # Initialise the type, struct and function maps
    # NOTE: Populated lazily after Type classes are defined (see __init_type_map below)
    type_map.update({'void': 'void'})  # Will be replaced with TVoid() in Phase 2a
    type_map.update({'int': 'i64'})  # Match TInteger class and C++ kernel expectations
    type_map.update({'double': 'f64'})  # Will be replaced with TFloat() in Phase 2a
    type_map.update({'double*': 'memref<?xf64>'})
    type_map.update({'double**': 'memref<?xmemref<?xf64>>'})
    type_map.update({'tarch::la::Vector<2, double>': 'memref<?xi64>'}) # Using memref instead of !llvm.ptr for standard indexing
    type_map.update({'tarch::la::Vector<3, double>': 'memref<?xi64>'}) # Using memref instead of !llvm.ptr for standard indexing
    type_map.update({'CellData<double, double>&': '!llvm.ptr'}) #'memref<?xstruct<f64, f64>>'})
    #type_map.update({'const FluxFunctor&': '(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()'})
    #type_map.update({'const SourceFunctor&': '(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, !llvm.ptr) -> ()'})
    #type_map.update({'const NonconservativeProductFunctor&': '(!llvm.ptr, !llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()'})   
    #type_map.update({'const MaxEigenvalueFunctorInPlace&': '(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()'})
    #type_map.update({'const MaxEigenvalueFunctor&': '(!llvm.ptr, !llvm.ptr, !llvm.ptr, f64, f64, i32, !llvm.ptr) -> ()'})
    #type_map.update({'peano4::utils::LoopPlacement': 'i32'})
    # Note: QIn, QOut, cellCentre, cellSize, t, dt, id, numberOfCells, etc. are populated from struct_map below
    
    # Functor function MLIR signatures for individual function calls
    #type_map.update({'flux': '(memref<?xf64>, !llvm.ptr, !llvm.ptr, f64, f64, i32, memref<?xf64>) -> ()'})
    #type_map.update({'nonconservativeProduct': '(memref<?xf64>, memref<?xf64>, !llvm.ptr, !llvm.ptr, f64, f64, i32, memref<?xf64>) -> ()'})
    #type_map.update({'sourceTerm': '(memref<?xf64>, !llvm.ptr, !llvm.ptr, f64, f64, memref<?xf64>) -> ()'})
    #type_map.update({'maxEigenvalue': '(memref<?xf64>, !llvm.ptr, !llvm.ptr, f64, f64, i32, memref<?xf64>) -> ()'})    

    # Local variable storage types (for runtime-allocated temporary variables)
    type_map.update({'source': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesX': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesY': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesZ': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesX2': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesY2': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesZ2': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesXY': 'memref<?xmemref<?xf64>>'})
    type_map.update({'maxEigenvaluesPerVolume': 'memref<?xmemref<?xf64>>'})
    
    # Lambda/flux arrays (2D double** patterns) - These are ASSIGNED to with nested subscripts
    # so they need nested memref type mapping
    type_map.update({'lambdaLeft': 'memref<?xmemref<?xf64>>'})
    type_map.update({'lambdaRight': 'memref<?xmemref<?xf64>>'})
    type_map.update({'lambdaBottom': 'memref<?xmemref<?xf64>>'})
    type_map.update({'lambdaTop': 'memref<?xmemref<?xf64>>'})
    type_map.update({'lambdaNear': 'memref<?xmemref<?xf64>>'})
    type_map.update({'lambdaFar': 'memref<?xmemref<?xf64>>'})
    type_map.update({'fluxLeft': 'memref<?xmemref<?xf64>>'})
    type_map.update({'fluxRight': 'memref<?xmemref<?xf64>>'})
    type_map.update({'fluxBottom': 'memref<?xmemref<?xf64>>'})
    type_map.update({'fluxTop': 'memref<?xmemref<?xf64>>'})
    type_map.update({'fluxNear': 'memref<?xmemref<?xf64>>'})
    type_map.update({'fluxFar': 'memref<?xmemref<?xf64>>'})
    
    # Bridge arrays: Used as temporary storage, allocated with outer dimension then inner arrays
    # Like lambdaLeft, these are semantically "array of arrays" in the C++ kernel
    # Must use nested memref type to match the allocation pattern
    type_map.update({'flux_x': 'memref<?xmemref<?xf64>>'})
    type_map.update({'flux_y': 'memref<?xmemref<?xf64>>'})
    type_map.update({'flux_z': 'memref<?xmemref<?xf64>>'})
    type_map.update({'average_x': 'memref<?xmemref<?xf64>>'})
    type_map.update({'average_y': 'memref<?xmemref<?xf64>>'})
    type_map.update({'average_z': 'memref<?xmemref<?xf64>>'})
    type_map.update({'delta_x': 'memref<?xmemref<?xf64>>'})
    type_map.update({'delta_y': 'memref<?xmemref<?xf64>>'})
    type_map.update({'delta_z': 'memref<?xmemref<?xf64>>'})
    type_map.update({'nonconservative_x': 'memref<?xmemref<?xf64>>'})
    type_map.update({'nonconservative_y': 'memref<?xmemref<?xf64>>'})
    type_map.update({'nonconservative_z': 'memref<?xmemref<?xf64>>'})

    # MLIR to C++ type mappings for function signature generation
    type_map.update({'memref<?xf64>': 'void*'})
    type_map.update({'!llvm.ptr': 'void*'})
    type_map.update({'f64': 'double'})
    type_map.update({'i32': 'int'})
    type_map.update({'i64': 'long'})
    type_map.update({'i1': 'bool'})

    # MLIRCellData struct layout with MemRefDescriptor fields
    descriptor2DType = "!llvm.struct<(ptr, ptr, i64, array<2 x i64>, array<2 x i64>)>"
    descriptor1DType = "!llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>"
    
    # Updated patchTypes: 7 descriptors + 3 i64 scalars + 2 ptrs = 12 fields
    patchTypes = [descriptor2DType, descriptor2DType, descriptor1DType, descriptor1DType, 
                  descriptor1DType, descriptor1DType, descriptor1DType, 'i64', 'i64', 'i64', 'ptr', 'ptr']
    
    struct_map.update({'patchData': [ 
            ('QIn' , f"{descriptor2DType}|||!llvm.struct<({', '.join(patchTypes)})>"),                    # Field 0: 2D descriptor
            ('QOut' , f"{descriptor2DType}|||!llvm.struct<({', '.join(patchTypes)})>"),                   # Field 1: 2D descriptor
            ('cellCentre' , f"{descriptor1DType}|||!llvm.struct<({', '.join(patchTypes)})>"),               # Field 2: 1D descriptor
            ('cellSize' , f"{descriptor1DType}|||!llvm.struct<({', '.join(patchTypes)})>"),                 # Field 3: 1D descriptor
            ('t' , f"{descriptor1DType}|||!llvm.struct<({', '.join(patchTypes)})>"),                        # Field 4: 1D descriptor
            ('dt' , f"{descriptor1DType}|||!llvm.struct<({', '.join(patchTypes)})>"),                       # Field 5: 1D descriptor
            ('id' , f"{descriptor1DType}|||!llvm.struct<({', '.join(patchTypes)})>"),                       # Field 6: 1D descriptor
            ('numberOfCells' , f"i64|||!llvm.struct<({', '.join(patchTypes)})>"),                      # Field 7: i64
            ('memoryLocation' , f"i64|||!llvm.struct<({', '.join(patchTypes)})>"),                     # Field 8: i64
            ('targetDevice' , f"i64|||!llvm.struct<({', '.join(patchTypes)})>"),                       # Field 9: i64
            ('QOut_legacy' , f"!llvm.ptr|||!llvm.struct<({', '.join(patchTypes)})>"),                  # Field 10: function pointer
            ('maxEigenvalue' , f"memref<?xf64>|||!llvm.struct<({', '.join(patchTypes)})>")]})              # Field 11: function pointer

    # Populate type_map with descriptor field types from struct_map
    # This maps field names to their converted MLIR memref types
    for struct_name, fields in struct_map.items():
        for field_name, field_info in fields:
            target_type = field_info.split('|||')[0]
            # Convert descriptor types to their memref equivalents
            if target_type == descriptor2DType:
                type_map.update({field_name: 'memref<?x?xf64>'})
            elif target_type == descriptor1DType:
                # Distinguish based on field name:
                # - cellCentre, cellSize, h, x: these are Vector<2,double> fields (stored as i64 pointers)
                # - t, dt, id: these are scalar f64 fields
                if field_name in ['cellCentre', 'cellSize', 'h', 'x']:
                    type_map.update({field_name: 'memref<?xi64>'})  # Vector fields (pointers to vectors)
                else:
                    type_map.update({field_name: 'memref<?xf64>'})  # Scalar fields (f64 values)
            elif target_type == 'i64':
                type_map.update({field_name: 'i64'})
            elif target_type == '!llvm.ptr':
                # For MLIR pointer types, keep them as-is (they're function pointers, not data pointers)
                type_map.update({field_name: '!llvm.ptr'})
            else:
                # For other types, use the target_type directly
                type_map.update({field_name: target_type})

    def __init__(self):
        self._parent = None
        self._on_left = False
        self._hoist = False 
        self._index_type = False
        
    def parent(self, parent = None):
        if parent is not None:
          self._parent = parent
        return self._parent
                  
    def on_left(self, left = None):
        if left is not None:
          self._on_left = left
        return self._on_left 

    def hoist(self, flag = None):
        if flag is not None:
          self._hoist = flag
        return self._hoist
              
    @classmethod    
    def get_mlir_id(cls):
        current_reg = cls.mlir_id_count
        cls.mlir_id_count += 1       
        return current_reg
   
    @classmethod    
    def reset_mlir_ids(cls):
        cls.mlir_id_count = 0
 
    @staticmethod
    def get_mlir_type(type_string):
        # Handle string representations by creating temporary type objects
        if isinstance(type_string, str):
            if type_string == "int":
                return TInteger().print_mlir(0)
            elif type_string == "double":
                return TFloat().print_mlir(0)
            elif type_string == "bool":
                return TBoolean().print_mlir(0)
        return None

    @staticmethod
    def generate_pointer_to_2d_element(memref_mlir, row_index, col_index, is_nested, indent_level=0):
        indent = ' ' * indent_level * Node.spaces_per_tab
        mlir_code = ""
        
        if is_nested:
            # Load inner memref from outer array, then offset within it
            inner_memref_id = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{inner_memref_id} = memref.load {memref_mlir}[{row_index}] {{name = \"{memref_mlir.strip('%')}\"}} : memref<?xmemref<?xf64>>\n"
            
            # Extract pointer as index from inner memref
            ptr_idx_id = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{ptr_idx_id} = memref.extract_aligned_pointer_as_index {inner_memref_id} : memref<?xf64> -> index {{name = \"{memref_mlir.strip('%')}\"}}\n"
            
            # Byte offset calculation (col * 8 for f64 elements)
            element_size_bytes = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{element_size_bytes} = arith.constant 8 : index\n"
            
            offset_bytes_id = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{offset_bytes_id} = arith.muli {col_index}, {element_size_bytes} : index\n"
            
            # Add offset to base pointer
            ptr_with_offset = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{ptr_with_offset} = arith.addi {ptr_idx_id}, {offset_bytes_id} : index\n"
            
            # Convert to i64, then to llvm.ptr
            ptr_i64 = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{ptr_i64} = arith.index_cast {ptr_with_offset} : index to i64\n"
            
            result_ptr = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{result_ptr} = llvm.inttoptr {ptr_i64} : i64 to !llvm.ptr\n"
        else:
            # Flat 2D memref: compute (row * width + col) offset
            col_width_id = f"%{Node.get_mlir_id()}"
            one_const = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{one_const} = arith.constant 1 : index\n"
            mlir_code += indent + f"{col_width_id} = memref.dim {memref_mlir}, {one_const} : memref<?x?xf64>\n"
            
            # Calculate row offset (row * width)
            row_offset = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{row_offset} = arith.muli {row_index}, {col_width_id} : index\n"
            
            # Add column to row offset
            elem_offset = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{elem_offset} = arith.addi {row_offset}, {col_index} : index\n"
            
            # Convert element offset to bytes
            element_size_bytes = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{element_size_bytes} = arith.constant 8 : index\n"
            
            offset_bytes_id = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{offset_bytes_id} = arith.muli {elem_offset}, {element_size_bytes} : index\n"
            
            # Extract base pointer from memref
            base_ptr_idx = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{base_ptr_idx} = memref.extract_aligned_pointer_as_index {memref_mlir} : memref<?x?xf64> -> index {{name = \"{memref_mlir.strip('%')}\"}}\n"
            
            # Add byte offset to base
            ptr_with_offset = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{ptr_with_offset} = arith.addi {base_ptr_idx}, {offset_bytes_id} : index\n"
            
            # Convert to i64, then to llvm.ptr
            ptr_i64 = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{ptr_i64} = arith.index_cast {ptr_with_offset} : index to i64\n"
            
            result_ptr = f"%{Node.get_mlir_id()}"
            mlir_code += indent + f"{result_ptr} = llvm.inttoptr {ptr_i64} {{name = \"{memref_mlir.strip('%')}\"}} : i64 to !llvm.ptr\n"
        
        return mlir_code, result_ptr

    @classmethod
    def create_memref_from_ptr(cls, ptr_id, target_type, indent_level=0, size_hint=None, target_var_name=None):
        indent = ' ' * indent_level * cls.spaces_per_tab
        
        # Generate unique IDs for all the operations
        desc_id = f"%{cls.get_mlir_id()}"
        desc_0_id = f"%{cls.get_mlir_id()}"
        desc_1_id = f"%{cls.get_mlir_id()}"
        desc_2_id = f"%{cls.get_mlir_id()}"
        desc_3_id = f"%{cls.get_mlir_id()}"
        desc_4_id = f"%{cls.get_mlir_id()}"
        
        # Use target variable name if provided, otherwise generate a new one
        if target_var_name:
            memref_id = target_var_name
        else:
            memref_id = f"%{cls.get_mlir_id()}"
        
        # Create constants we need
        c0_i64_id = f"%{cls.get_mlir_id()}"
        c1_i64_id = f"%{cls.get_mlir_id()}"
        
        # Use standard LLVM memref descriptor format - this is part of the fix supplied by Nick Brown
        # For 2D memrefs, use 2D descriptor; for 1D memrefs, use 1D descriptor
        if target_type.startswith('memref<?x?'):
            # 2D memref needs 2D descriptor
            struct_type = '!llvm.struct<(ptr, ptr, i64, array<2 x i64>, array<2 x i64>)>'
            num_dimensions = 2
        else:
            # 1D memref needs 1D descriptor
            struct_type = '!llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>'
            num_dimensions = 1
        
        # Determine size based on target type or hint
        if target_type.startswith('memref<?xmemref') or size_hint == 1:
            size_val = "1"
        elif target_type.startswith('memref<?xi64>') and size_hint == 2:
            size_val = "2"
        elif target_type.startswith('memref<?xi64>'):
            size_val = "2"  # Default for vector types
        else:
            size_val = "1"  # Default for scalar-like types
            
        mlir_str = ""
        
        # Create the required constants
        mlir_str += indent + f"{c0_i64_id} = arith.constant 0 : i64\n"
        mlir_str += indent + f"{c1_i64_id} = arith.constant 1 : i64\n"
        
        # If we need a size constant, create it
        if size_val != "1":
            size_id = f"%{cls.get_mlir_id()}"
            size_i64_id = f"%{cls.get_mlir_id()}"
            mlir_str += indent + f"{size_id} = arith.constant {size_val} : index\n"
            mlir_str += indent + f"{size_i64_id} = arith.index_cast {size_id} : index to i64\n"
            size_ref = size_i64_id
        else:
            size_ref = c1_i64_id
            
        # Create the LLVM memref descriptor
        mlir_str += indent + f"{desc_id} = llvm.mlir.undef : {struct_type}\n"
        mlir_str += indent + f"{desc_0_id} = llvm.insertvalue {ptr_id}, {desc_id}[0] : {struct_type}\n"
        mlir_str += indent + f"{desc_1_id} = llvm.insertvalue {ptr_id}, {desc_0_id}[1] : {struct_type}\n"
        mlir_str += indent + f"{desc_2_id} = llvm.insertvalue {c0_i64_id}, {desc_1_id}[2] : {struct_type}\n"
        mlir_str += indent + f"{desc_3_id} = llvm.insertvalue {size_ref}, {desc_2_id}[3, 0] : {struct_type}\n"
        if num_dimensions == 2:
            # For 2D descriptor, set [3, 1], [4, 0] and [4, 1]
            desc_3_1_id = f"%{cls.get_mlir_id()}"
            mlir_str += indent + f"{desc_3_1_id} = llvm.insertvalue {size_ref}, {desc_3_id}[3, 1] : {struct_type}\n"
            mlir_str += indent + f"{desc_4_id} = llvm.insertvalue {c1_i64_id}, {desc_3_1_id}[4, 0] : {struct_type}\n"
            desc_final_id = f"%{cls.get_mlir_id()}"
            mlir_str += indent + f"{desc_final_id} = llvm.insertvalue {c1_i64_id}, {desc_4_id}[4, 1] : {struct_type}\n"
        else:
            # For 1D descriptor, just set [4, 0]
            mlir_str += indent + f"{desc_4_id} = llvm.insertvalue {c1_i64_id}, {desc_3_id}[4, 0] : {struct_type}\n"
            desc_final_id = desc_4_id
        mlir_str += indent + f"{memref_id} = builtin.unrealized_conversion_cast {desc_final_id} : {struct_type} to {target_type} {{name = \"{memref_id.strip('%')}\"}}\n"
        
        return mlir_str, memref_id

    @classmethod
    def create_memref_from_extracted_descriptor(cls, alloc_ptr_id, aligned_ptr_id, offset_id, sizes_0_id, sizes_1_id, strides_0_id, strides_1_id, target_type, struct_type, indent_level=0, target_var_name=None, is_2d=False):
        indent = ' ' * indent_level * cls.spaces_per_tab
        
        # Use target variable name if provided
        if target_var_name:
            memref_id = target_var_name
        else:
            memref_id = f"%{cls.get_mlir_id()}"
        
        # Generate unique IDs for all the operations
        desc_id = f"%{cls.get_mlir_id()}"
        desc_0_id = f"%{cls.get_mlir_id()}"
        desc_1_id = f"%{cls.get_mlir_id()}"
        desc_2_id = f"%{cls.get_mlir_id()}"
        desc_3_id = f"%{cls.get_mlir_id()}"
        desc_4_id = f"%{cls.get_mlir_id()}"
        
        c1_i64_id = f"%{cls.get_mlir_id()}"
        
        mlir_str = ""
        
        # Create constants we need
        mlir_str += indent + f"{c1_i64_id} = arith.constant 1 : i64\n"
        
        # Create the LLVM memref descriptor using the extracted fields
        # We preserve the extracted pointers but hardcode sizes and strides to 1
        mlir_str += indent + f"{desc_id} = llvm.mlir.undef : {struct_type}\n"
        mlir_str += indent + f"{desc_0_id} = llvm.insertvalue {alloc_ptr_id}, {desc_id}[0] : {struct_type}\n"
        mlir_str += indent + f"{desc_1_id} = llvm.insertvalue {aligned_ptr_id}, {desc_0_id}[1] : {struct_type}\n"
        mlir_str += indent + f"{desc_2_id} = llvm.insertvalue {offset_id}, {desc_1_id}[2] : {struct_type}\n"
        mlir_str += indent + f"{desc_3_id} = llvm.insertvalue {sizes_0_id}, {desc_2_id}[3, 0] : {struct_type}\n"
        
        # Check if 2D or 1D 
        if is_2d:
            # 2D descriptor - use all extracted components
            desc_3_1_id = f"%{cls.get_mlir_id()}"
            mlir_str += indent + f"{desc_3_1_id} = llvm.insertvalue {sizes_1_id}, {desc_3_id}[3, 1] : {struct_type}\n"
            mlir_str += indent + f"{desc_4_id} = llvm.insertvalue {strides_0_id}, {desc_3_1_id}[4, 0] : {struct_type}\n"
            desc_final_id = f"%{cls.get_mlir_id()}"
            mlir_str += indent + f"{desc_final_id} = llvm.insertvalue {strides_1_id}, {desc_4_id}[4, 1] : {struct_type}\n"
        else:
            # 1D descriptor - use strides_0_id
            mlir_str += indent + f"{desc_4_id} = llvm.insertvalue {strides_0_id}, {desc_3_id}[4, 0] : {struct_type}\n"
            desc_final_id = desc_4_id
        
        mlir_str += indent + f"{memref_id} = builtin.unrealized_conversion_cast {desc_final_id} : {struct_type} to {target_type} {{name = \"{memref_id.strip('%')}\"}}\n"
        
        return mlir_str, memref_id        

class Statement(Node):
    def __init__(self):
        super().__init__()


class Expression(Node):
    def __init__(self):
        super().__init__()
        self._mlir_id = None

    def mlir_id(self, id = None):
        if id is not None:
            self._mlir_id = id
        return self._mlir_id
        
    def multidimensional_index(self, index_list, start_index=0, dimensions = []):
        return self
    
    @abstractmethod
    def get_type(self):
        pass


class Type(Node):

    def print_tree(self, indent_level=0):
        return ' ' 

    def get_element_type(self):
        return None


class TVoid(Type):
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_void(self, indent_level)
    
    def is_function(self):
        return False
    
 

class Argument(Node):
    def __init__(self, id, argument_type: Type, namespace = None, is_function = False):
        super().__init__()
        self.id = id
        self._type = argument_type
        self._type.parent(self)
        self._namespace = namespace
        self._is_function = is_function
        if Node.use_accelerator == True:
            if type(self._type) is TCustom and self._type._type.find("CellData") != -1:
                index = self._type._type.find("CellData")
                self._type._type = self._type._type[0:index] + "CopyCellDataGPU" + self._type._type[index + 8:]
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_argument(self, indent_level)

    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"Argument: {self.id}"


class FunctionDefinition(Node):
    _stateless = False
    _syclDataToCopy = []

    def __init__(self, id, return_type: Type, template = None, namespaces = [], use_accelerator = False, top_level = False):
        super().__init__()
        self.id = id
        self._return_type = return_type
        self._return_type.parent(self)
        self._template = template
        self._arguments = []
        self._body = []
        self.use_accelerator = use_accelerator
        self._namespaces = namespaces
        self._top_level = top_level

    def top_level(self, top = None):
        if top is not None:
          self._top_level = top
        return self._top_level                

    def add_statement(self, statement: Statement):
        if statement is not None:
            self._body.append(statement)

    def add_argument(self, argument: Argument):
        self._arguments.append(argument)
        if self._top_level:
            # NOTE: For now, we assume that the top-level function is always the main entry point
            #       and set global values in Node here - 'namespace' is MLIR type here
            Node.type_map.update({argument.id: argument._namespace})
            if Node.type_map.get(argument._type.print_cpp()) is None:
                Node.type_map.update({argument._type.print_cpp(): argument._namespace})

    def print_declaration(self, printer: Printer, indent_level = 0):
        Node.use_accelerator = self.use_accelerator
        return printer.print_function_declaration(self, indent_level)

    def print_declaration_with_timer(self, printer: Printer, indent_level = 0):
        Node.use_accelerator = self.use_accelerator
        return printer.print_function_declaration_with_timer(self, indent_level)

    def print_definition_with_timer(self, printer: Printer, indent_level = 0):
        Node.use_accelerator = self.use_accelerator
        return printer.print_function_definition_with_timer(self, indent_level)

    def print(self, printer: Printer, indent_level = 0):
        Node.use_accelerator = self.use_accelerator
        return printer.print_function_definition(self, indent_level)

    def print_tree(self, indent_level = 0):
        Node.use_accelerator = self.use_accelerator
        statement_prints = [statement.print_tree(indent_level + 1) for statement in self._body]
        argument_prints = [argument.print_tree(indent_level + 1) for argument in self._arguments]
        return f"""
FunctionDefinition: {self.id}:
{os.linesep.join(argument_prints)}
{os.linesep.join(statement_prints)}"""

class LogToFile(Statement):
    def __init__(self, statementToLog, outputStream):
        self._statement = statementToLog
        self._output = outputStream

    def print_cpp(self, indent_level=0):
        return f"""{self._output.print_cpp()} << {self._statement.print_cpp()};"""
    
    def print_omp(self, indent_level=0):
        return f"""{self._output.print_omp()} << {self._statement.print_omp()};"""
    
    def print_sycl(self, indent_level=0):
        return f"""{self._output.print_sycl()} << {self._statement.print_sycl()};"""
    
    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return "LogToFile"


# Expressions
class Name(Expression):
    variables = dict()

    def __init__(self, id, type = None):
        super().__init__()
        self.id = id
        self.type = type
        if self.type is None:
            self.type = Name.variables[self.id].get_type()
        self.type._parent = self

    def multidimensional_index(self, index_list, start_index=0, dimensions = []):
        if type(Name.variables[self.id].get_type()) is TDataBlock or type(Name.variables[self.id].get_type()) is TDiagonalMatrix:
            return Name.variables[self.id].multidimensional_index(index_list, start_index, dimensions)
        else:
            return self

    def index(self, i):
        return Name.variables[self.id].index(i)
    
    def index(self, i, j):
        return Name.variables[self.id].index(i, j)

    def __add__(self, rhs):
        return BinaryOperation("+", self, rhs)

    def __sub__(self, rhs):
        return BinaryOperation("-", self, rhs)
    
    def __mul__(self, rhs):
        return BinaryOperation("*", self, rhs)
    
    def __div__(self, rhs):
        return BinaryOperation("/", self, rhs)
    
    def __neg__(self):
        return UnaryOperation("-", self)

    def __gt__(self, rhs):
        if type(rhs) is Name:
            return (Name.variables[self.id] > Name.variables[rhs.id])
        return (Name.variables[self.id] > rhs)

    def __eq__(self, rhs):
        if type(rhs) is Name:
            if rhs.id in Name.variables and self.id in Name.variables:
                return (self._value == Name.variables[rhs.id])
            else:
                return False
        else:
            if self.id in Name.variables:
                return (Name.variables[self.id] == rhs)
            else:
                return False

    def get_type(self):
        return self.type
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_name(self, indent_level)
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "Name:" + self.id

class Index:
    pass

class Integer(Expression):
    def __init__(self, value, string = None):
        super().__init__()
        self._value = int(value)
        self._string = string
    
    def __add__(self, rhs):
        if self._value == 0:
            return rhs
        if type(rhs) is Integer:
            return Integer(self._value + rhs._value)
        return BinaryOperation("+", self, rhs)

    def __sub__(self, rhs):
        if self._value == 0:
            if type(rhs) is Integer:
                return Integer(-rhs._value)
            if type(rhs) is UnaryOperation and rhs._operation == "-":
                return rhs._operand
            return -rhs
        if type(rhs) is Integer:
            return Integer(self._value - rhs._value)
        return BinaryOperation("-", self, rhs)
    
    def __mul__(self, rhs):
        return BinaryOperation("*", self, rhs)
    
    def __div__(self, rhs):
        return BinaryOperation("/", self, rhs)

    def __neg__(self):
        if self._value == 0:
            return self
        else:
            return UnaryOperation("-", self)

    def __lt__(self, rhs):
        if type(rhs) is Name:
            if rhs.id in Name.variables:
                return (self._value < Name.variables[rhs.id])
            else:
                return False
        return (self._value < rhs)
    
    def __gt__(self, rhs):
        if type(rhs) is Name:
            if rhs.id in Name.variables:
                return (self._value > Name.variables[rhs.id])
            else:
                return False
        return (self._value > rhs)

    def __eq__(self, rhs):
        if type(rhs) is Integer:
            return (self._value == rhs._value)

    def __int__(self):
        return self._value

    def get_type(self):
        # Check if this Integer references a struct field in struct_map
        if self._string is not None and '.' in self._string:
            struct, field = self._string.split('.')
            if struct in Node.struct_map:
                struct_list = Node.struct_map[struct]
                # Find the field and get its type
                for item in struct_list:
                    if item[0] == field:
                        # Extract the target type using '|||' separator (struct_map format: "target_type|||outer_struct_type")
                        mlir_type = item[1]
                        target_type = mlir_type.split('|||')[0] if '|||' in mlir_type else mlir_type
                        # Create a type object based on the target type
                        if target_type == 'i64':
                            # Return a wrapper that will generate i64
                            return TLong()
                        elif target_type == 'i32':
                            return TInteger()
                        elif target_type == 'f64':
                            return TFloat()
        # Default: return regular i32 TInteger
        return TInteger()
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_integer(self, indent_level)
        
    def print_tree(self, indent_level=0):
        if self._string is None:
            return ' ' * indent_level * Node.spaces_per_tab + "Integer: " + str(self._value)
        else:
            return ' ' * indent_level * Node.spaces_per_tab + "Integer: " + str(self._string)


class Boolean(Expression):
    def __init__(self, value):
        super().__init__()
        self._value = value
    
    def __str__(self):
        return str(self._value)
    
    def get_type(self):
        return TBoolean()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_boolean(self, indent_level)

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "Boolean: " + str(self._value)

class String(Expression):
    def __init__(self, value):
        super().__init__()
        self._value = value
    
    def __str__(self):
        return self._value

    def get_type(self):
        return TString()
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_string(self, indent_level)

    def print_cpp(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + self._value
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "String: " + self._value


class Float(Expression):
    def __init__(self, value,  string = None, reference = False):
        super().__init__()
        self._value = value
        self._string = string
        self._reference = reference

    
    def __float__(self):
        return self._value

    def get_type(self):
        return TFloat(self._reference)
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_float(self, indent_level)
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "Float: " + str(self._value)
    

class DataBlock(Expression):
    def __init__(self, iteration_range, internal, requires_memory_allocation, id = None, underlying_type = String("double")):
        super().__init__()
        self._iteration_range = iteration_range
        self._internal = internal
        self.requires_memory_allocation = requires_memory_allocation
        self._underlying_type = underlying_type
        self.id = id
        self._offset = []
        self._memory_range = []
        for [x, y] in self._iteration_range:
            self._offset.append(Integer(0))
            self._memory_range.append([Integer(0), y - x])
    
    def offset(self, offset):
        output = DataBlock(copy.deepcopy(self._iteration_range), copy.deepcopy(self._internal), True)
        output.id = self.id

        for i in range(len(offset)):
            if type(offset[i]) is list:
                if offset[i][0] is None:
                    offset[i][0] = self._iteration_range[i][0]
                if offset[i][1] is None:
                    offset[i][1] = self._iteration_range[i][1]

                output._offset[i] = offset[i][0] - self._iteration_range[i][0]
                output._iteration_range[i][1] = offset[i][1]
                output._iteration_range[i][0] = offset[i][0]
            else:
                raise Exception("Invalid index")
        
        for i in range(len(output._offset), len(output._iteration_range)):
            output._offset.append(Integer(0))

        return output

    def linearise_index(self, indices, dimensions, offset_start_index = 0):
        factors = [Integer(1)]
        for i in range(0, len(dimensions) - 1):
            factors.append(factors[-1] * (self._memory_range[i][1]))
        if Node.use_accelerator == False and type(self) is not FaceDataBlock:
            index_list = indices[0:-1]
        else:
            index_list = indices
            
        if type(dimensions[-len(index_list)][1] - dimensions[-len(index_list)][0]) is Integer and (dimensions[-len(index_list)][1] - dimensions[-len(index_list)][0])._value == 1:
            index = self._offset[offset_start_index]
        else:
            if type(self._offset[offset_start_index + 0].get_type()) is TDataBlock:
                index = index_list[0] + self._offset[offset_start_index + 0].multidimensional_index(index_list, offset_start_index)
            else:
                index = index_list[0] + self._offset[offset_start_index + 0]

        index = factors[-len(index_list)] * index
        for i in range(1, len(index_list)):
            if type(self._offset[offset_start_index + i].get_type()) is TDataBlock:
                temp = index_list[i] + self._offset[offset_start_index + i].multidimensional_index(indices, offset_start_index)
            else:
                temp = index_list[i] + self._offset[offset_start_index + i]
            index = index + factors[-len(index_list) + i] * temp
        return index
    
    def multidimensional_index(self, index_list, start_index = 0, dimensions = []):
        if len(self._iteration_range) == 1:
            return Subscript(self, index_list[-1])
        else:
            if Node.use_accelerator == False:
                return Subscript(Subscript(self, index_list[-1]), self.linearise_index(index_list, self._iteration_range[0:-1], start_index))
            else:
                return Subscript(self, self.linearise_index(index_list, self._iteration_range, start_index))

    def get_type(self):
        db_type = TDataBlock(len(self._iteration_range), self._underlying_type)
        return db_type
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block(self, indent_level)
        
    def print_tree(self, indent_level=0):
        if self.id is None:
            return ' ' * indent_level * Node.spaces_per_tab + f"""DataBlock:
{self._internal.print_tree(indent_level + 1)}"""
        else:
            return ' ' * indent_level * Node.spaces_per_tab + "DataBlock: " + self.id

"""
FaceDataBlock distinguishes itself from normal DataBlocks by the fact that its internal array is always 1d.
"""
class FaceDataBlock(DataBlock):
    def __init__(self, iteration_range, internal, requires_memory_allocation, id = None, underlying_type = String("double")):
        super().__init__(iteration_range, internal, requires_memory_allocation, id, underlying_type)

    def offset(self, offset):
        output = FaceDataBlock(copy.deepcopy(self._iteration_range), copy.deepcopy(self._internal), True)
        output.id = self.id

        for i in range(len(offset)):
            if type(offset[i]) is list:
                if offset[i][0] is None:
                    offset[i][0] = self._iteration_range[i][0]
                if offset[i][1] is None:
                    offset[i][1] = self._iteration_range[i][1]

                output._offset[i] = offset[i][0] - self._iteration_range[i][0]
                output._iteration_range[i][1] = offset[i][1]
                output._iteration_range[i][0] = offset[i][0]
            else:
                raise Exception("Invalid index")
        
        for i in range(len(output._offset), len(output._iteration_range)):
            output._offset.append(Integer(0))

        return output
    
    def multidimensional_index(self, index_list, start_index = 0, dimensions = []):
        return Subscript(self, self.linearise_index(index_list, self._iteration_range, start_index))
    

class LogDataBlock(Statement):
    def __init__(self, dataBlock, filename):
        self._filename = filename

        self._dataBlock = dataBlock

        loops = []
        index = []

        loops.append(For([Integer(0), self._dataBlock._iteration_range[-1][1], - self._dataBlock._iteration_range[-1][0]]))
        for i in range(len(self._dataBlock._memory_range) - 2, -1, -1):
            loops.append(For([Integer(0), self._dataBlock._iteration_range[i][1] - self._dataBlock._iteration_range[i][0]]))
       
        for i in range(len(self._dataBlock._memory_range) - 1, -1, -1):
            index.append(loops[i].get_iteration_variable())                            

        loops[-1].add_statement(LogToFile(self._dataBlock.multidimensional_index(index), String("log")))
        loops[-1].add_statement(LogToFile(String("\" \""), String("log")))
        for i in range(0, len(loops) - 1):
            loops[i].add_statement(loops[i + 1])

        loops[-2].add_statement(LogToFile(String("\", \""), String("log")))
        for i in range(0, len(self._dataBlock._memory_range) - 2):
            loops[i].add_statement(LogToFile(String("\"\\n\""), String("log")))

        for loop in loops[::-1]:
            loop.close_scope()
        self._loop = loops[0]
    
    def print_cpp(self, indent_level=0):
        return f"""log.open("{self._filename.print_cpp()}");
{self._loop.print_cpp()}
log.close();
"""
    
    def print_omp(self, indent_level=0):
        return ""

    def print_sycl(self, indent_level=0):
        return ""

    def print_mlir(self, indent_level=0):
        return ""

    def print_tree(self, indent_level=0):
        return ""

class Subscript(Expression):
    def __init__(self, value: Expression, index: Expression):
        super().__init__()
        self._value = value
        self._index = index
    
    def __neg__(self):
        return UnaryOperation("-", self)

    def is_nested_subscript(self):
        return isinstance(self._value, Subscript)

    def get_type(self):
        return self._value.get_type()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_subscript(self, indent_level)

    def print_tree(self, indent_level=0):
        return f"""Subscript:
{self._value.print_tree(indent_level + 1)}
{self._index.print_tree(indent_level + 1)}"""


class Reference(Expression):
    def __init__(self, expression: Expression):
        self._expression = expression
    
    def get_type(self):
        return self._expression.get_type()
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_reference(self, indent_level)

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""Reference:
{self._expression.print_tree(indent_level + 1)}"""


class Vector(Expression):
    def __init__(self, elements, element_type, string = None):
        self._elements = elements
        self._type = element_type
        self._string = string

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_vector(self, indent_level)

    def print_tree(self, indent_level=0):
        return "Vector"
    
    def get_type(self):
        if self._string is not None:
            return TCustom("auto")
        return TCustom(f"tarch::la::Vector<{len(self._elements)}, {self._type.print_cpp()}>")


class DiagonalMatrixConstructionFromExisting(Statement):
    def __init__(self, id, matrix):
        self.id = id
        self.matrix = matrix
        self.data_block = DataBlock([[Integer(0), matrix._width]], matrix._internal, False, id = id)
        self.data_block_construction = DataBlockConstructionFromExisting(Name(self.id, self.matrix.get_type()), self.matrix.data_block)
        self.matrix._data_block = self.data_block
        Name.variables[self.id] = self.matrix
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_construction_from_existing(self.data_block_construction, indent_level)

class DiagonalMatrix(Expression):
    def __init__(self, width, internal, index_dimensions):
        self._internal = internal
        self._width = width
        self._index_dimensions = index_dimensions
        self.data_block = DataBlock([[Integer(0), width]], internal, False)
        self._id = None

    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        diagonal_index = index[1]
        factor = Integer(1)
        for i in range(2, len(index) - 1):
            factor = factor * (dimensions[i - 1][1] - dimensions[i - 1][0])
            diagonal_index = diagonal_index + factor * index[i]
        return Subscript(self.data_block, diagonal_index)

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_diagonal_matrix(self, indent_level)

    def print_cpp(self, indent_level=0):
        if self._id is not None:
            return self._id.print_cpp(indent_level)
        else:
            return self._internal.print_cpp(indent_level)

    def print_omp(self, indent_level=0):
        if self._id is not None:
            return self._id.print_cpp(indent_level)
        else:
            return self._internal.print_cpp(indent_level)

    def print_sycl(self, indent_level=0):
        pass

    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "DiagonalMatrix"

    def get_type(self):
        return TDiagonalMatrix()


class Matrix(Expression):
    def __init__(self, dimensions, internal, index_dimensions):
        self._internal = internal
        self._dimensions = dimensions
        self._index_dimensions = index_dimensions
        self.data_block = DataBlock([[Integer(0), BinaryOperation("*", dimensions[0], dimensions[1])]], internal, False)
        self._id = None

    def index(self, row_index, column_index):
        return Subscript(self, row_index * self._dimensions[1] + column_index)

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_matrix(self, indent_level)

    def print_cpp(self, indent_level=0):
        if self._id is not None:
            return self._id.print_cpp(indent_level)
        else:
            return self._internal.print_cpp(indent_level)

    def print_omp(self, indent_level=0):
        if self._id is not None:
            return self._id.print_cpp(indent_level)
        else:
            return self._internal.print_cpp(indent_level)

    def print_sycl(self, indent_level=0):
        pass

    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "Matrix"

    def get_type(self):
        return TMatrix()
    

class DiagonalMatrixConstructionFromKroneckerProduct(Expression):
    def __init__(self, id, matrix):
        self._id = String(id)
        if type(matrix) is Name:
            self._matrix = Name.variables[matrix.id]
        else:
            self._matrix = matrix

        self._matrix.data_block.id = id
        self._memory_allocation = MemoryAllocation(self._id, TFloat(), [self._matrix._width])

        lhs_matrix = self._matrix._internal._arguments[0]
        if type(lhs_matrix) is Name:
            lhs_matrix = Name.variables[lhs_matrix.id]

        rhs_matrix = self._matrix._internal._arguments[1]
        if type(rhs_matrix) is Name:
            rhs_matrix = Name.variables[rhs_matrix.id]

        self._assignment = For([Integer(0), lhs_matrix._width])
        internal_loop = For([Integer(0), rhs_matrix._width])
        index = self._assignment.get_iteration_variable() * rhs_matrix._width + internal_loop.get_iteration_variable()
        lhs_index = Subscript(lhs_matrix.data_block, self._assignment.get_iteration_variable())
        if type(lhs_matrix._internal) is Float or type(lhs_matrix._internal) is Integer:
            lhs_index = lhs_matrix._internal
        rhs_index = Subscript(rhs_matrix.data_block, internal_loop.get_iteration_variable())
        if type(rhs_matrix._internal) is Float or type(rhs_matrix._internal) is Integer:
            rhs_index = rhs_matrix._internal
        internal_loop.add_statement(Assignment(Subscript(self._matrix.data_block, index), BinaryOperation("*", lhs_index, rhs_index)))
        self._assignment.add_statement(internal_loop)
        internal_loop.close_scope()
        self._assignment.close_scope()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_diagonal_matrix_construction_from_kronecker_product(self, indent_level)

    def print_cpp(self, indent_level=0):
        return f"""{self._memory_allocation.print_cpp(indent_level)}
{self._assignment.print_cpp(indent_level)}"""

    def print_omp(self, indent_level=0):
        return f"""{self._memory_allocation.print_omp(indent_level)}
{self._assignment.print_omp(indent_level)}"""

    def print_sycl(self, indent_level=0):
        pass

    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "DiagonalKroneckerProduct"

    def get_type(self):
        return TDiagonalMatrix()


class DiagonalMatrixConstructionFromValue(Expression):
    def __init__(self, id, matrix):
        self._id = String(id)
        if type(matrix) is Name:
            self._matrix = Name.variables[matrix.id]
        else:
            self._matrix = matrix

        self._matrix.data_block.id = id
        self._memory_allocation = MemoryAllocation(self._id, TFloat(), [self._matrix._width])

        self._assignment = For([Integer(0), self._matrix._width])
        self._assignment.add_statement(Assignment(Subscript(self._matrix.data_block, self._assignment.get_iteration_variable()), self._matrix._internal))
        self._assignment.close_scope()

        Name.variables[id] = self._matrix
        Node.type_map.update({id: self._matrix.get_type().print_cpp()})

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_diagonal_matrix_construction_from_value(self, indent_level)

    def print_cpp(self, indent_level=0):
        return f"""{self._memory_allocation.print_cpp(indent_level)}
{self._assignment.print_cpp(indent_level)}"""

    def print_omp(self, indent_level=0):
        return f"""{self._memory_allocation.print_omp(indent_level)}
{self._assignment.print_omp(indent_level)}"""

    def print_sycl(self, indent_level=0):
        pass

    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "DiagonalKroneckerProduct"

    def get_type(self):
        return TDiagonalMatrix()


class MatrixConstructionFromKroneckerProduct(Expression):
    def __init__(self, id, matrix):
        self._id = String(id)
        if type(matrix) is Name:
            self._matrix = Name.variables[matrix.id]
        else:
            self._matrix = matrix

        self._matrix.data_block.id = id
        self._memory_allocation = MemoryAllocation(self._id, TFloat(), [self._matrix._dimensions[0] * self._matrix._dimensions[1]])

        self._initialisation = For([Integer(0), self._matrix._dimensions[0] * self._matrix._dimensions[1]])
        self._initialisation.add_statement(Assignment(Subscript(self._id, self._initialisation.get_iteration_variable()), Float(0.0)))
        self._initialisation.close_scope()

        lhs_matrix = self._matrix._internal._arguments[0]
        if type(lhs_matrix) is Name:
            lhs_matrix = Name.variables[lhs_matrix.id]

        rhs_matrix = self._matrix._internal._arguments[1]
        if type(rhs_matrix) is Name:
            rhs_matrix = Name.variables[rhs_matrix.id]

        if type(lhs_matrix.get_type()) is TMatrix:
            lhs_dimensions = lhs_matrix._dimensions
        else:
            lhs_dimensions = [lhs_matrix._width, lhs_matrix._width]

        if type(rhs_matrix.get_type()) is TMatrix:
            rhs_dimensions = rhs_matrix._dimensions
        else:
            rhs_dimensions = [rhs_matrix._width, rhs_matrix._width]

        loops = []
        if type(lhs_matrix.get_type()) is TMatrix:
            loops.append(For([Integer(0), lhs_matrix._dimensions[0]]))
            loops.append(For([Integer(0), lhs_matrix._dimensions[1]]))
            lhs_index = loops[0].get_iteration_variable() * lhs_dimensions[1] + loops[1].get_iteration_variable()
            startIndex = loops[0].get_iteration_variable() * lhs_dimensions[1] * rhs_dimensions[1] * rhs_dimensions[0] + loops[1].get_iteration_variable() * rhs_dimensions[1]
        else:
            loops.append(For([Integer(0), lhs_matrix._width]))
            lhs_index = loops[0].get_iteration_variable()
            startIndex = loops[0].get_iteration_variable() * lhs_dimensions[1] * rhs_dimensions[1] * rhs_dimensions[0] + loops[0].get_iteration_variable() * rhs_dimensions[1]

        if type(rhs_matrix.get_type()) is TMatrix:
            loops.append(For([Integer(0), rhs_matrix._dimensions[0]]))
            loops.append(For([Integer(0), rhs_matrix._dimensions[1]]))
            rhs_index = loops[-2].get_iteration_variable() * rhs_dimensions[1] + loops[-1].get_iteration_variable()
            index = startIndex + loops[-1].get_iteration_variable() + loops[-2].get_iteration_variable() * lhs_dimensions[1] * rhs_dimensions[1]
        else:
            loops.append(For([Integer(0), rhs_matrix._width]))
            rhs_index = loops[-1].get_iteration_variable()
            index = startIndex + loops[-1].get_iteration_variable() + loops[-1].get_iteration_variable() * lhs_dimensions[1] * rhs_dimensions[1]

        lhs_subscript = Subscript(lhs_matrix.data_block, lhs_index)
        if type(lhs_matrix._internal) is Float or type(lhs_matrix._internal) is Integer:
            lhs_subscript = lhs_matrix._internal
        rhs_subscript = Subscript(rhs_matrix.data_block, rhs_index)
        if type(rhs_matrix._internal) is Float or type(rhs_matrix._internal) is Integer:
            rhs_subscript = rhs_matrix._internal

        loops[-1].add_statement(Assignment(Subscript(self._id, index), BinaryOperation("*", lhs_subscript, rhs_subscript)))

        for i in range(len(loops) - 1):                    
            loops[i].add_statement(loops[i + 1])
            loops[i].close_scope()
        loops[-1].close_scope()
        self._assignment = loops[0]

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_matrix_construction_from_kronecker_product(self, indent_level)

    def print_cpp(self, indent_level=0):
        return f"""{self._memory_allocation.print_cpp(indent_level)}
{self._initialisation.print_cpp(indent_level)}
{self._assignment.print_cpp(indent_level)}"""

    def print_omp(self, indent_level=0):
        return f"""{self._memory_allocation.print_omp(indent_level)}
{self._initialisation.print_omp(indent_level)}
{self._assignment.print_omp(indent_level)}"""

    def print_sycl(self, indent_level=0):
        pass

    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "KroneckerProduct"

    def get_type(self):
        return TMatrix()


class VectorIndex(Expression):
    def __init__(self, value: Expression, index: Expression):
        super().__init__()
        self._value = value
        self._index = index
    
    def get_type(self):
        return self._value.get_type()
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_vector_index(self, indent_level)
        
    def print_tree(self, indent_level=0):
        return f"""
VectorIndex: 
{self.id}
{self._value.print_tree(indent_level + 1)}
{self._index.print_tree(indent_level + 1)}"""
    

class Comparison(Expression):
    def __init__(self, operation, lhs, rhs):
        self._operation = operation
        self._lhs = lhs
        self._rhs = rhs

    def get_type(self):
        return self._lhs.get_type()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_comparison(self, indent_level)

    @staticmethod
    def get_mlir_comparison_op(operation, op_type_class):
        if op_type_class is TFloat:
            op_map = {
                "<": "arith.cmpf olt",
                ">": "arith.cmpf ogt",
                "<=": "arith.cmpf ole",
                ">=": "arith.cmpf oge", 
                "==": "arith.cmpf oeq",
                "!=": "arith.cmpf one"
            }
        else:  # TInteger or default
            op_map = {
                "<": "arith.cmpi slt",
                ">": "arith.cmpi sgt", 
                "<=": "arith.cmpi sle",
                ">=": "arith.cmpi sge",
                "==": "arith.cmpi eq",
                "!=": "arith.cmpi ne"
            }
        return op_map.get(operation, None)

    def print_tree(self, indent_level=0):
        return f"""
Comparison {self._operation}:
{self._lhs.print_tree(indent_level + 1)}
{self._rhs.print_tree(indent_level + 1)}"""

class BinaryOperation(Expression):
    def __init__(self, operation, lhs, rhs):
        super().__init__()
        self._operation = operation
        self._lhs = lhs
        self._rhs = rhs 

        if type(self._lhs) is BinaryOperation:
            if self._lhs._operation == "+":
                if self._lhs._rhs == Integer(0):
                    self._lhs = self._lhs._lhs
                elif self._lhs._lhs == Integer(0):
                    self._lhs = self._lhs._rhs
            elif self._lhs._operation == "-":
                if self._lhs._rhs == Integer(0):
                    self._lhs = self._lhs._lhs
            elif self._lhs._operation == "*":
                if self._lhs._rhs == Integer(1):
                    self._lhs = self._lhs._lhs
                elif self._lhs._lhs == Integer(1):
                    self._lhs = self._lhs._rhs

        if type(self._rhs) is BinaryOperation:
            if self._rhs._operation == "+":
                if self._rhs._rhs == Integer(0):
                    self._rhs = self._rhs._lhs
                elif self._rhs._lhs == Integer(0):
                    self._rhs = self._rhs._rhs
            elif self._rhs._operation == "-":
                if self._rhs._rhs == Integer(0):
                    self._rhs = self._rhs._lhs
            elif self._rhs._operation == "*":
                if self._rhs._rhs == Integer(1):
                    self._rhs = self._rhs._lhs
                elif self._rhs._lhs == Integer(1):
                    self._rhs = self._rhs._rhs

        if self._operation == "-" and type(self._rhs) is UnaryOperation and self._rhs._operation == "-":
            self._operation = "+"
            self._rhs = self._rhs._operand
        if self._operation == "+" and type(self._rhs) is UnaryOperation and self._rhs._operation == "-":
            self._operation = "-"
            self._rhs = self._rhs._operand

    def index(self, index):
        return BinaryOperation(self._operation, self._lhs.index(index), self._rhs.index(index))

    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        return BinaryOperation(self._operation, self._lhs.multidimensional_index(index, start_index, dimensions), self._rhs.multidimensional_index(index, start_index, dimensions))


    def __add__(self, rhs):
        return BinaryOperation("+", self, rhs)

    def __sub__(self, rhs):
        return BinaryOperation("-", self, rhs)
    
    def __mul__(self, rhs):
        return BinaryOperation("*", self, rhs)

    def __neg__(self):
        return UnaryOperation("-", self)

    def get_type(self):
        # For arithmetic operations, return the element type, not the pointer type
        op_type_class = TDataBlock.get_mlir_type_for_operation(self._lhs, self._rhs)
        return op_type_class()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_binary_operation(self, indent_level)

    @staticmethod
    def get_mlir_arithmetic_op(operation, op_type_class):
        if op_type_class is TFloat:
            op_map = {
                "*": "arith.mulf",
                "/": "arith.divf", 
                "+": "arith.addf",
                "-": "arith.subf"
            }
        else:  # TInteger, TLong, or default
            op_map = {
                "*": "arith.muli",
                "/": "arith.divi",
                "+": "arith.addi", 
                "-": "arith.subi"
            }
        return op_map.get(operation, None)
    
    @staticmethod
    def cast_operand(operand_mlir, operand, result_type, indent_level):
        indent = ' ' * indent_level * Node.spaces_per_tab
        operand_type = operand.get_type()
                
        if type(operand_type) is TDataBlock:
            operand_type = operand_type.get_element_type()
            
        if isinstance(operand_type, TInteger) and result_type == TFloat:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.sitofp {operand_mlir.strip(' ')} : i32 to f64\n")
            return new_op
        elif isinstance(operand_type, TLong) and result_type == TFloat:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.sitofp {operand_mlir.strip(' ')} : i64 to f64\n")
            return new_op
        elif isinstance(operand_type, TFloat) and result_type == TInteger:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.fptosi {operand_mlir.strip(' ')} : f64 to i32\n")
            return new_op
        elif isinstance(operand_type, TFloat) and result_type == TLong:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.fptosi {operand_mlir.strip(' ')} : f64 to i64\n")
            return new_op
        elif isinstance(operand_type, TInteger) and result_type == TLong:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.extsi {operand_mlir.strip(' ')} : i32 to i64\n")
            return new_op
        elif isinstance(operand_type, TLong) and result_type == TInteger:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.trunci {operand_mlir.strip(' ')} : i64 to i32\n")
            return new_op
        elif operand._index_type and result_type == TFloat:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.index_cast {operand_mlir.strip(' ')} : index to f64\n")
            return new_op
        elif operand._index_type and result_type == TInteger:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.index_cast {operand_mlir.strip(' ')} : index to i32\n")
            return new_op
        elif operand._index_type and result_type == TLong:
            new_op = f"%{Node.get_mlir_id()}"
            Node.context.get_block().mlir_append(indent + f"{new_op} = arith.index_cast {operand_mlir.strip(' ')} : index to i64\n")
            return new_op    
        return operand_mlir  # No casting needed, return original operand


    def get_mlir_op(self):
        op_type_class = TDataBlock.get_mlir_type_for_operation(self._lhs, self._rhs)
        return BinaryOperation.get_mlir_arithmetic_op(self._operation, op_type_class)    

    def print_tree(self, indent_level=0):
        return f"""
BinaryOperation {self._operation}:
{self._lhs.print_tree(indent_level + 1)}
{self._rhs.print_tree(indent_level + 1)}"""
    

class UnaryOperation(Expression):
    def __init__(self, operation, operand):
        super().__init__()
        self._operation = operation
        self._operand = operand
    
    def __add__(self, rhs):
        return BinaryOperation("+", self, rhs)

    def __sub__(self, rhs):
        return BinaryOperation("-", self, rhs)
    
    def __mul__(self, rhs):
        return BinaryOperation("*", self, rhs)

    def __neg__(self):
        if self._operation == "-":
            return self._operand
        else:
            return UnaryOperation("-", self._operand)

    def get_type(self):
        return self._operand.get_type()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_unary_operation(self, indent_level)
       
    def print_tree(self, indent_level=0):
        return f"""
UnaryOperation {self._operation}:
{self._operand.print_tree(indent_level + 1)}"""

class DataBlockIndex(Expression):
    def __init__(self, dataBlock, index):
        super().__init__()
        if type(dataBlock) is Name:
            self._dataBlock = Name.variables[dataBlock.id]
        else:
            self._dataBlock = dataBlock
        
        self._index = index
        self._iteration_range = self._dataBlock._iteration_range
        self._memory_range = self._dataBlock._memory_range
    
    def index(self, index):
        return VectorIndex(self._dataBlock.index(index), self._index)

    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        return VectorIndex(self._dataBlock.multidimensional_index(index, start_index, dimensions), self._index)

    def get_type(self):
        return TDataBlock(self._iteration_range, String("double"))

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_index(self, indent_level)

    def print_tree(self, indent_level=0):
        pass


class DataBlockComparison(Expression):
    def __init__(self, operation, lhs, rhs):
        if type(lhs) is Name and type(lhs.get_type()) is TDataBlock:
            self._lhs = Name.variables[lhs.id]
        else:
            self._lhs = lhs
        
        if type(rhs) is Name and type(rhs.get_type()) is TDataBlock:
            self._rhs = Name.variables[rhs.id]
        else:
            self._rhs = rhs
        
        self._operation = operation
        self.requires_memory_allocation = True
        self._offset = None

        if type(self._lhs.get_type()) is TDataBlock and type(self._rhs.get_type()) is TDataBlock:
            self._iteration_range = copy.deepcopy(self._lhs._iteration_range)
            self._memory_range = copy.deepcopy(self._lhs._memory_range)
            if type(self._rhs._iteration_range[0][1]) is not BinaryOperation and  type(self._iteration_range[0][1]) is not BinaryOperation and self._rhs._iteration_range[0][1] > self._iteration_range[0][1]:
                self._iteration_range[0][1] = self._rhs._iteration_range[0][1]
            elif type(self._iteration_range[0][1]) is not BinaryOperation and self._iteration_range[0][1] == Integer(1):
                self._iteration_range[0][1] = self._rhs._iteration_range[0][1]
            if type(self._rhs._memory_range[0]) is not BinaryOperation and  type(self._memory_range[0]) is not BinaryOperation and self._rhs._memory_range[0][1] > self._memory_range[0][1]:
                self._memory_range[0] = self._rhs._memory_range[0]
            elif type(self._memory_range[0]) is not BinaryOperation and self._memory_range[0] == Integer(1):
                self._memory_range[0] = self._rhs._memory_range[0]

        elif type(self._lhs.get_type()) is TDataBlock:
            self._iteration_range = copy.deepcopy(self._lhs._iteration_range)
            self._memory_range = copy.deepcopy(self._lhs._memory_range) 
        else:
            self._iteration_range = copy.deepcopy(self._rhs._iteration_range)
            self._memory_range = copy.deepcopy(self._rhs._memory_range)
    
    def index(self, index):
        return Comparison(self._operation, self._lhs.index(index), self._rhs.index(index))

    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        return Comparison(self._operation, self._lhs.multidimensional_index(index, start_index, dimensions), self._rhs.multidimensional_index(index, start_index, dimensions))

    def get_type(self):
        return TDataBlock(self._iteration_range, String("int"))

    def print_tree(self, indent_level=0):
        pass

class DataBlockBinaryOperation(Expression):
    def __init__(self, operation, lhs, rhs, useFunctionSyntax=False):
        super().__init__()
        if type(lhs) is Name and type(lhs.get_type()) is TDataBlock:
            self._lhs = Name.variables[lhs.id]
        else:
            self._lhs = lhs
        
        if type(rhs) is Name and type(rhs.get_type()) is TDataBlock:
            self._rhs = Name.variables[rhs.id]
        else:
            self._rhs = rhs
        
        self._operation = operation
        self.requires_memory_allocation = True
        self._useFunctionSyntax = useFunctionSyntax

        if type(self._lhs.get_type()) is TDataBlock and type(self._rhs.get_type()) is TDataBlock:
            self._iteration_range = copy.deepcopy(self._lhs._iteration_range)
            self._memory_range = copy.deepcopy(self._lhs._memory_range)
            if len(self._iteration_range) == 1:
                self._iteration_range = copy.deepcopy(self._rhs._iteration_range)
            elif type(self._rhs._iteration_range[0][1]) is not BinaryOperation and  type(self._iteration_range[0][1]) is not BinaryOperation and self._rhs._iteration_range[0][1] > self._iteration_range[0][1]:
                self._iteration_range[0][1] = self._rhs._iteration_range[0][1]
            elif type(self._iteration_range[0][1]) is not BinaryOperation and self._iteration_range[0][1] == Integer(1):
                self._iteration_range[0][1] = self._rhs._iteration_range[0][1]

            self._memory_range[0] = [Integer(0), self._iteration_range[0][1] - self._iteration_range[0][0]]

        elif type(self._lhs.get_type()) is TDataBlock:
            self._iteration_range = copy.deepcopy(self._lhs._iteration_range)
            self._memory_range = copy.deepcopy(self._lhs._memory_range) 
        else:
            self._iteration_range = copy.deepcopy(self._rhs._iteration_range)
            self._memory_range = copy.deepcopy(self._rhs._memory_range)

        if (type(self._iteration_range[0][0]) is Integer and type(self._iteration_range[0][1]) is Integer and (self._iteration_range[0][1] - self._iteration_range[0][0])._value == 1):
            self._iteration_range[0][0] = Integer(0)
            self._iteration_range[0][1] = Integer(1)

        self._offset = []
        for [x, y] in self._iteration_range:
            self._offset.append(x)

    
    def index(self, index):
        if self._useFunctionSyntax:
            return FunctionCall(self._operation, [self._lhs.index(index), self._rhs.index(index)])
        else:
            return BinaryOperation(self._operation, self._lhs.index(index), self._rhs.index(index))

    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        if self._useFunctionSyntax:
            return FunctionCall(self._operation, [self._lhs.multidimensional_index(index, start_index, dimensions), self._rhs.multidimensional_index(index, start_index, dimensions)])
        else:
            return BinaryOperation(self._operation, self._lhs.multidimensional_index(index, start_index, dimensions), self._rhs.multidimensional_index(index, start_index, dimensions))

    def get_type(self):
        lhs_is_double = False
        rhs_is_double = False
        if type(self._lhs.get_type()) is TDataBlock:
            lhs_is_double = (str(self._lhs.get_type().get_single()) == "double")
        else:
            lhs_is_double = (str(self._lhs.get_type()) == "double")

        if type(self._rhs.get_type()) is TDataBlock:
            rhs_is_double = (str(self._rhs.get_type().get_single()) == "double")
        else:
            rhs_is_double = (str(self._rhs.get_type()) == "double")

        if (lhs_is_double or rhs_is_double):
            return TDataBlock(self._iteration_range, String("double"))
        else:
            return TDataBlock(self._iteration_range, String("int"))

    def __sub__(self, rhs):
        return DataBlockBinaryOperation("-", self, rhs)
    
    def __add__(self, rhs):
        return DataBlockBinaryOperation("+", self, rhs)

    def print_tree(self, indent_level=0):
        pass


class DataBlockUnaryOperation(Expression):
    def __init__(self, operation, dataBlock):
        super().__init__()
        if type(dataBlock) is Name and type(dataBlock.get_type()) is TDataBlock:
            self._dataBlock = Name.variables[dataBlock.id]
        else:
            self._dataBlock = dataBlock
        
        self._operation = operation
        self.requires_memory_allocation = True
        self._offset = None

        self._iteration_range = copy.deepcopy(self._dataBlock._iteration_range)
        self._memory_range = copy.deepcopy(self._dataBlock._memory_range)
    
    def index(self, index):
        return UnaryOperation(self._operation, self._dataBlock.index(index))

    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        return UnaryOperation(self._operation, self._dataBlock.multidimensional_index(index, start_index, dimensions))

    def get_type(self):
        return TDataBlock(self._iteration_range, String("double"))

    def print_tree(self, indent_level=0):
        pass


class DataBlockUnaryMax:
    def __init__(self, dataBlock):
        if type(dataBlock) is Name:
            self._dataBlock = Name.variables[dataBlock.id]
        else:
            self._dataBlock = dataBlock

    def set_output_variable(self, outputVariable):
        self._outputVariable = Name.variables[outputVariable.id]
        self._loop = For(self._dataBlock._iteration_range[-1])
        self._loops = [self._loop]
        self._loop.add_statement(Assignment(self._outputVariable.multidimensional_index([self._loop.get_iteration_variable()]), Float(0.0)))
        for i in range(len(self._dataBlock._iteration_range) - 2, -1, -1):
            self._loops.append(For(self._dataBlock._iteration_range[i]))
            self._loops[-2].add_statement(self._loops[-1])
        index = []
        for loop in self._loops[::-1]:
            index.append(loop.get_iteration_variable())
        self._loops[-1].add_statement(Assignment(self._outputVariable.multidimensional_index(index), Max(self._outputVariable.multidimensional_index(index), self._dataBlock.multidimensional_index(index))))
        for loop in self._loops:
            loop.close_scope()
    
    def get_type(self):
        return TDataBlock(self._dataBlock._iteration_range, TFloat())

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_unary_max(self, indent_level)

    def print_tree(self, indent_level=0):
        return f"""DataBlockUnaryMax:
{self._dataBlock.print_tree(indent_level + 1)}"""
        

class DataBlockMax(DataBlockBinaryOperation):
    def __init__(self, lhs, rhs):
        super().__init__("max", lhs, rhs)

    def index(self, index):
        return Max(self._lhs.index(index), self._rhs.index(index))
        
    def multidimensional_index(self, index, start_index = 0, dimensions = []):
        return Max(self._lhs.multidimensional_index(index, start_index, dimensions), self._rhs.multidimensional_index(index, start_index, dimensions))

    def print_tree(self, indent_level=0):
        pass

# Types
class TCustom(Type):
    def __init__(self, type_name):
        self._type = type_name
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_custom_type(self, indent_level)

    def print_cpp(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + self._type

    def print_mlir(self, indent_level=0):
        mlir_type = Node.get_mlir_type(self._type)
        if mlir_type is not None:
            return ' ' * indent_level * Node.spaces_per_tab + mlir_type
        else:
            # Fallback to type_map for complex types
            type_entry = Node.type_map[f"{self._type}"]
            if isinstance(type_entry, str):
                return ' ' * indent_level * Node.spaces_per_tab + type_entry
            else:
                # Type object (e.g., TVoid, TFloat, etc.)
                return ' ' * indent_level * Node.spaces_per_tab + type_entry.print_mlir(0)
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TCustom: " + self._type
    
    def is_nested_memref(self):
        try:
            return isinstance(self._type, str) and ('xmemref<' in self._type or self._type.count('memref<') > 1)
        except Exception:
            return False


class TInteger(Type):
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_integer_type(self, indent_level)

    def print_cpp(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "int"
    
    def print_mlir(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "i32"
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TInteger"


class TLong(Type):
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_long_type(self, indent_level)

    """Type for 64-bit integers (long) used in MLIRCellData struct fields"""
    def print_cpp(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "long"

    def print_mlir(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "i64"
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TLong"


class TBoolean(Type):
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_boolean_type(self, indent_level)

    def print_cpp(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "bool"

    def print_mlir(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "i1"
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TBoolean"


class TFloat(Type):
    def __init__(self, reference = False):
        self._reference = reference

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_float_type(self, indent_level)

    def print_cpp(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "double" + ("&" if self._reference else "")
    
    def print_mlir(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "f64"
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TFloat"


class TString(Type):
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_string_type(self, indent_level)

    def print_cpp(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + "const char*"

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TString"

   
class TNestedMemref(Type):
    def __init__(self, element_type=TFloat()):
        super().__init__()
        self._element_type = element_type
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_nested_memref_type(self, indent_level)

    def print_cpp(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + f'double**'  # C++ representation

    def is_nested_memref(self):
        return True
    
    def get_element_type(self):
        return self._element_type


class TFunctionType(Type):
    def __init__(self, signature_string):
        super().__init__()
        self._signature = signature_string
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_function_type(self, indent_level)

    def print_cpp(self, indent_level=0):
        # C++ function pointers don't have direct equivalent
        return ' ' * indent_level * Node.spaces_per_tab + 'void*'

class TSimpleMemref(Type):
    def __init__(self, element_type=TFloat(), dimensions=1):
        super().__init__()
        self._element_type = element_type
        self._dimensions = dimensions
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_simple_memref_type(self, indent_level)

    def print_cpp(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + 'void*'
    
    def get_element_type(self):
        return self._element_type


class TDataBlock(Type):
    def __init__(self, dimensions, underlying_type):
        self._dimensions = dimensions
        self._underlying_type = underlying_type
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_type(self, indent_level)

    @staticmethod
    def needs_memref_load(operand):
        if isinstance(operand, VectorIndex):
            return False
        
        operand_type = operand.get_type()
        if isinstance(operand_type, TDataBlock):
            if (isinstance(operand_type._dimensions, int) and operand_type._dimensions == 1) or \
               (isinstance(operand_type._dimensions, list) and len(operand_type._dimensions) == 1):
                return True
            
            if isinstance(operand, Subscript) and operand.is_nested_subscript():
                return True
            
        return False
    
    @staticmethod
    def generate_memref_load(operand, operand_mlir, const_zero_id, indent_level):
        indent = ' ' * indent_level * Node.spaces_per_tab
        load_id = f"%{Node.get_mlir_id()}"
        operand_type = operand.get_type()
        
        if isinstance(operand_type, TDataBlock):
            # Handle nested array access (double** style arrays)
            if isinstance(operand, Subscript) and operand.is_nested_subscript():
                return operand_mlir
            
            element_type_class = operand_type.get_element_type()
            if element_type_class is not None:
                element_type = element_type_class().print_mlir(0)
            else:
                element_type = "f64"  # Default fallback

            #print(type(operand))
            #print(operand._index.id)
            load_str = indent + f"{load_id} = memref.load {operand_mlir}[{const_zero_id}] {{name = \"{operand_mlir.strip('%')}\"}} : memref<?x{element_type}>\n"
            Node.context.get_block().mlir_append(load_str)
            return load_id
            
        return operand_mlir
    
    @staticmethod
    def ensure_const_zero(const_zero_id, indent_level):
        if const_zero_id is None:
            const_zero_id = f"%{Node.get_mlir_id()}"
            indent = ' ' * indent_level * Node.spaces_per_tab
            Node.context.get_block().mlir_append(indent + f"{const_zero_id} = arith.constant 0 : index\n")
        return const_zero_id

    def get_single(self):
        return self._underlying_type
    
    def get_element_type(self):
        if isinstance(self._underlying_type, String):
            if self._underlying_type._value == "double":
                return TFloat
            elif self._underlying_type._value == "int":
                return TInteger
            elif self._underlying_type._value[0:17] == "tarch::la::Vector":
                return TVector
            else:
                return None
        else:
            return type(self._underlying_type)
    
    @staticmethod
    def get_mlir_type_for_operation(lhs, rhs):
        lhs_operand_type = lhs.get_type()
        if type(lhs_operand_type) is TDataBlock or type(lhs_operand_type) is TMatrix or type(lhs_operand_type) is TDiagonalMatrix:
            lhs_type = lhs_operand_type.get_element_type()
        else:
            lhs_type = type(lhs_operand_type)
            
        rhs_operand_type = rhs.get_type()
        if type(rhs_operand_type) is TDataBlock:
            rhs_type = rhs_operand_type.get_element_type()
        else:
            rhs_type = type(rhs_operand_type)
        
        # Use float if either operand is float
        if lhs_type is TFloat or rhs_type is TFloat:
            return TFloat
        elif lhs_type is TLong or rhs_type is TLong:
            return TLong
        elif lhs_type is TInteger and rhs_type is TInteger:
            return TInteger
        elif lhs_type is TBoolean and rhs_type is TBoolean:
            return TBoolean

        return None
    
    def get_array_element_info(self):
        element_type_class = self.get_element_type()
        
        # Check for vector types by examining the underlying type
        if isinstance(self._underlying_type, String):
            is_vector = "tarch::la::Vector" in self._underlying_type._value
        elif hasattr(self._underlying_type, '_type'):
            is_vector = "tarch::la::Vector" in self._underlying_type._type
        else:
            is_vector = "tarch::la::Vector" in self._underlying_type
        
        # Determine MLIR element type based on vector vs scalar
        if is_vector:
            mlir_element_type = "i64"  # Vectors stored as pointers (i64)
        else:
            mlir_element_type = element_type_class().print_mlir(0)
            
        return element_type_class or TFloat, is_vector, mlir_element_type

    
    def print_cpp(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + self._underlying_type.print_cpp() + "*"
    
    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TDataBlock"


def _is_nested_memref(type_map_entry):
    if isinstance(type_map_entry, str):
        # String: check for 'xmemref<' pattern
        return 'xmemref<' in type_map_entry
    elif hasattr(type_map_entry, 'is_nested_memref'):
        # Type object: call the method
        return type_map_entry.is_nested_memref()
    else:
        return False


class TMatrix(Type):
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_matrix_type(self, indent_level)

    def get_element_type(self):
        return TFloat
    
    def print_cpp(self, indent_level = 0):
        pass
    
    def print_omp(self, indent_level = 0):
        pass
    
    def print_sycl(self, indent_level = 0):
        pass
    
    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TMatrix"


class TDiagonalMatrix(Type):
    def __init__(self):
        self._underlying_type = TFloat()
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_type(TDataBlock([5], TFloat()), indent_level)

    def get_element_type(self):
        return TFloat

    def print_cpp(self, indent_level = 0):
        return "double*"
    
    def print_omp(self, indent_level = 0):
        pass
    
    def print_sycl(self, indent_level = 0):
        pass
    
    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TDiagonalMatrix"


class TVector(Type):
    def __init__(self, underlying_type=TFloat()):
        self._element_type = underlying_type
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_vector_type(self, indent_level)

    def print_cpp(self, indent_level = 0):
        pass
    
    def print_omp(self, indent_level = 0):
        pass
    
    def print_sycl(self, indent_level = 0):
        pass
    
    def print_mlir(self, indent_level=0):
        pass

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "TVector"


# Statements
class If(Statement):
    def __init__(self, boolean):
        super().__init__()
        self._boolean = boolean
        self._statements = []
    
    def add_statement(self, statement):
        if statement is not None:
            self._statements.append(statement)

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_if(self, indent_level) 

    def print_tree(self, indent_level=0):
        statement_prints = [statement.print_tree(indent_level + 1) for statement in self._statements]
        return ' ' * indent_level * Node.spaces_per_tab + f"""If: 
{os.linesep.join(statement_prints)}"""

class For(Statement):
    _inuse_iteration_variables = set()
    _iteration_variable_names = ['i', 'j', 'k', 'l', 'n', 'm', 'a', 'b', 'c', 'd']

    def __init__(self, iteration_range, iteration_variable_name = None, use_scheduler = False):
        super().__init__()
        self._iteration_range = iteration_range
        self._statements = []
        self._use_scheduler = False# use_scheduler
        
        if iteration_variable_name == None:
            for variable_name in For._iteration_variable_names:
                if variable_name not in For._inuse_iteration_variables:
                    self._iteration_variable = Name(variable_name, TInteger())
                    self._iteration_variable._index_type = True
                    For._inuse_iteration_variables.add(variable_name)
                    break
        else:
            self._iteration_variable = Name(iteration_variable_name, TInteger())
            For._inuse_iteration_variables.update({iteration_variable_name : self._iteration_variable})
        
    def get_interval_size(self):
        return self._iteration_range[1] - self._iteration_range[0]

    def get_iteration_variable(self):
        return self._iteration_variable

    def add_statement(self, statement):
        self._statements.append(statement)
    
    def close_scope(self):
        For._inuse_iteration_variables.remove(self._iteration_variable.id)
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_for(self, indent_level)

    def print_tree(self, indent_level = 0):
        statement_prints = [statement.print_tree(indent_level + 1) for statement in self._statements]
        return ' ' * indent_level * Node.spaces_per_tab + f"""For: 
{os.linesep.join(statement_prints)}"""
    

class Comment(Statement):
    def __init__(self, comment):
        super().__init__()
        self._comment = comment

    def get_type(self):
        return None

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_comment(self, indent_level)

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + "Comment"


class FunctionCall(Statement):
    def __init__(self, id, arguments, is_offloadable = False):
        self.id = id
        self._arguments = arguments
        self._is_offloadable = is_offloadable
    
    def add_argument(self, argument: Expression):
        self._arguments.append(argument)

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_function_call(self, indent_level)

    def print_tree(self, indent_level=0):
        argument_prints = [argument.print_tree() for argument in self._arguments]
        return ' ' * indent_level * Node.spaces_per_tab + f"""FunctionCall:
{' ' * (indent_level + 1) * Node.spaces_per_tab + ",".join(argument_prints)}"""


class Max(Statement):
    def __init__(self, lhs, rhs):
        super().__init__()
        self._lhs = lhs
        self._rhs = rhs
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_max(self, indent_level)

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""Max:
{self._lhs.print_tree(indent_level + 1)}
{self._rhs.print_tree(indent_level + 1)}"""


class MemoryAllocation(Statement):
    memoryAllocated = []
    allocated_types = {}  # Map from MLIR variable name to allocated memref type string

    def __init__(self,  name: Name, object_type: Type, dimensions, specify_type = True, add_to_stack=True):
        super().__init__()
        self._type = object_type
        if type(dimensions[0]) is list:
            self._dimensions = [dimension[1] for dimension in dimensions]
        else:
            self._dimensions = dimensions
        self._name = name
        self._specify_type = specify_type

        if add_to_stack == True:
            MemoryAllocation.memoryAllocated[-1].append(self)

        if len(self._dimensions) > 1:
            self._size = self._dimensions[-2]
            for i in range(len(self._dimensions) - 3, -1, -1):
                self._size = self._size * self._dimensions[i]

            self._loop = For([Integer(0), self._dimensions[-1]])
            self._loop.add_statement(MemoryAllocation(Subscript(self._name, self._loop.get_iteration_variable()), self._type, [self._size], False, False))
            self._loop.close_scope()
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_memory_allocation(self, indent_level)

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""MemoryAllocation:
{self._name.print_tree(indent_level + 1)}""" 


class MemoryDeallocation(Statement):
    def __init__(self, allocation: MemoryAllocation):
        super().__init__()
        self._dimensions = allocation._dimensions
        self._name = allocation._name
        self._allocated_type = None  # Will store the MLIR type string when known

        if len(self._dimensions) > 1:
            size = self._dimensions[-2]
            for i in range(len(self._dimensions) - 3, -1, -1):
                size = size * self._dimensions[i]

            self._loop = For([Integer(0), self._dimensions[-1]])
            self._loop.add_statement(MemoryDeallocation(MemoryAllocation(Subscript(self._name, self._loop.get_iteration_variable()), None, [size], False, False)))
            self._loop.close_scope()

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_memory_deallocation(self, indent_level)

    def print_tree(self, indent_level=0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""MemoryDeallocation:
{self._name.print_tree(indent_level + 1)}""" 
    

class Construction(Statement):
    def __init__(self, name: Name, expression: Expression):
        super().__init__()
        self._name = name
        self._name.on_left(True)
        self._expression = expression
        Name.variables.update({self._name.id: expression})
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_construction(self, indent_level)

    def print_tree(self, indent_level=0):
        return f"""
Construction: 
{self._name.print_tree(indent_level + 1)}
{self._expression.print_tree(indent_level + 1)}"""


class DataBlockConstructionFromExisting(Statement):
    def __init__(self, name: Name, dataBlock: DataBlock):
        super().__init__()
        self._name = name
        self._dataBlock = dataBlock
        self._dataBlock.id = name.id
        Name.variables.update({self._name.id: self._dataBlock})
        FunctionDefinition._syclDataToCopy.append(self)

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_construction_from_existing(self, indent_level)  

    def print_tree(self, indent_level=0):
        return f"""
DataBlockConstructionFromExisting: 
{self._name.print_tree(indent_level + 1)}
{self._dataBlock._internal.print_tree(indent_level + 1)}"""
    

class DataBlockConstructionFromOperation:
    def __init__(self, name: Name, dataBlockOperation):
        super().__init__()
        self._name = name
        self._dataBlock = DataBlock(dataBlockOperation._iteration_range, None, False, self._name.id, underlying_type=dataBlockOperation.get_type().get_single())
        Name.variables.update({self._name.id: self._dataBlock})
        if type(dataBlockOperation) is DataBlockBinaryOperation or type(dataBlockOperation) is DataBlockUnaryOperation or type(dataBlockOperation) is DataBlockMax or type(dataBlockOperation) is DataBlockComparison or type(dataBlockOperation._internal) is String:
            self._operation = dataBlockOperation
        else:
            self._operation = dataBlockOperation._internal

        self.memoryAllocation = MemoryAllocation(self._name, self._dataBlock.get_type().get_single(), self._dataBlock._memory_range)
        self.assignment = DataBlockAssignment(self._name, self._operation)
        
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_construction_from_operation(self, indent_level)

    def print_cpp(self, indent_level = 0):
        return f"""{self.memoryAllocation.print_cpp(indent_level)}
{self.assignment.print_cpp(indent_level)}"""
    
    def print_omp(self, indent_level = 0):
        return f"""{self.memoryAllocation.print_omp(indent_level)}
{self.assignment.print_omp(indent_level)}"""

    def print_sycl(self, indent_level = 0):
        return f"""{self.memoryAllocation.print_sycl(indent_level)}
{self.assignment.print_sycl(indent_level)}"""
    
    def print_mlir(self, indent_level = 0):
        return f"""{self.memoryAllocation.print_mlir(indent_level)}
{self.assignment.print_mlir(indent_level)}"""
    
    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""DataBlockConstructionFromOperation:
{self._name.print_tree(indent_level + 1)}
{self._dataBlock.print_tree(indent_level + 1)}"""


class DataBlockConstructionFromMatrixVectorProduct:
    def __init__(self, name: Name, matrix, vector):
        self._name = name
        if type(vector) is Name:
            self._vector = Name.variables[vector.id]
        else:
            self._vector = vector

        if type(matrix) is Name:
            self._matrix = Name.variables[matrix.id]
        else:
            self._matrix = matrix

        self._dataBlock = DataBlock(self._vector._iteration_range, None, False, self._name.id, underlying_type=vector.get_type().get_single())
        Name.variables.update({self._name.id: self._dataBlock})

        self.memoryAllocation = MemoryAllocation(self._name, self._dataBlock.get_type().get_single(), self._dataBlock._memory_range)

        element_width = self._vector._iteration_range[0][1] - self._vector._iteration_range[0][0]

        loops = []
        loops.append(For([Integer(0), self._dataBlock._iteration_range[-1][1] - self._dataBlock._iteration_range[-1][0]]))
        loops.append(For([Integer(0), self._matrix._dimensions[0]]))
        loops.append(For([Integer(0), self._dataBlock._iteration_range[0][1] - self._dataBlock._iteration_range[0][0]]))

        if Node.use_accelerator:
            size = Integer(1)
            for [a, b] in self._dataBlock._memory_range[0:-1]:
                size = size * (b - a)
            index = size * loops[0].get_iteration_variable() + (loops[1].get_iteration_variable() * element_width + loops[2].get_iteration_variable())
            loops[2].add_statement(Assignment(Subscript(self._dataBlock, index), Float(0.0)))
        else:
            loops[2].add_statement(Assignment(Subscript(Subscript(self._dataBlock, loops[0].get_iteration_variable()), loops[1].get_iteration_variable() * element_width + loops[2].get_iteration_variable()), Float(0.0)))
        
        loops.append(For([Integer(0), self._matrix._dimensions[1]]))

        if Node.use_accelerator:
            size = Integer(1)
            for [a, b] in self._dataBlock._memory_range[0:-1]:
                size = size * (b - a)
            index = size * loops[0].get_iteration_variable() + (loops[1].get_iteration_variable() * element_width + loops[2].get_iteration_variable())
            loops[3].add_statement(Assignment(Subscript(self._dataBlock, index), BinaryOperation("+", Subscript(self._dataBlock, index), BinaryOperation("*", matrix.index(loops[1].get_iteration_variable(), loops[3].get_iteration_variable()), Subscript(self._vector, loops[0].get_iteration_variable() * size + element_width * loops[3].get_iteration_variable() + loops[2].get_iteration_variable())))))
        else:
            loops[3].add_statement(Assignment(Subscript(Subscript(self._dataBlock, loops[0].get_iteration_variable()), loops[1].get_iteration_variable() * element_width + loops[2].get_iteration_variable()), BinaryOperation("+", Subscript(Subscript(self._dataBlock, loops[0].get_iteration_variable()), loops[1].get_iteration_variable() * element_width + loops[2].get_iteration_variable()), BinaryOperation("*", matrix.index(loops[1].get_iteration_variable(), loops[3].get_iteration_variable()), Subscript(Subscript(self._vector, loops[0].get_iteration_variable()), element_width * loops[3].get_iteration_variable() + loops[2].get_iteration_variable())))))
        

        
        loops[2].add_statement(loops[3])
        loops[1].add_statement(loops[2])
        loops[0].add_statement(loops[1])
        loops[3].close_scope()
        loops[2].close_scope()
        loops[1].close_scope()
        loops[0].close_scope()
        self.assignment = loops[0]
        
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_construction_from_matrix_vector_product(self, indent_level)

    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""DataBlockConstructionFromOperation:
{self._name.print_tree(indent_level + 1)}
{self._dataBlock.print_tree(indent_level + 1)}"""


class DataBlockConstructionFromDotProduct:
    def __init__(self, name: Name, matrix, vector):
        self._name = name
        if type(vector) is Name:
            self._vector = Name.variables[vector.id]
        else:
            self._vector = vector

        if type(matrix) is Name:
            self._matrix = Name.variables[matrix.id]
        else:
            self._matrix = matrix

        iteration_range = self._vector._iteration_range
        iteration_range[-1][1] = Integer(1)
        self._dataBlock = DataBlock(self._vector._iteration_range, None, False, self._name.id, underlying_type=vector.get_type().get_single())
        Name.variables.update({self._name.id: self._dataBlock})

        self.memoryAllocation = MemoryAllocation(self._name, self._dataBlock.get_type().get_single(), self._dataBlock._memory_range)

        loops = []
        index = []
        for [a, b] in self._dataBlock._iteration_range[-1::-1]:
            loops.append(For([Integer(0), b - a]))

        for loop in loops[::-1]:
            index.append(loop.get_iteration_variable())

        loops[-1].add_statement(Assignment(self._dataBlock.multidimensional_index(index), Float(0.0)))
        for i in range(len(loops) - 1):
            loops[i].add_statement(loops[i + 1])

        loops.append(For([Integer(0), self._matrix._width]))
        rhs_index = index.copy()
        rhs_index[-1] = loops[-1].get_iteration_variable()
        loops[-1].add_statement(Assignment(self._dataBlock.multidimensional_index(index), BinaryOperation("+", self._dataBlock.multidimensional_index(index), BinaryOperation("*", Subscript(self._matrix, rhs_index[-1]), self._vector.multidimensional_index(rhs_index)))))
        loops[-2].add_statement(loops[-1])

        for loop in loops:
            loop.close_scope()
        self.assignment = loops[0]
        
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_construction_from_dot_product(self, indent_level)

    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""DataBlockConstructionFromOperation:
{self._name.print_tree(indent_level + 1)}
{self._dataBlock.print_tree(indent_level + 1)}"""


class DataBlockConstructionFromFunction(Statement):
    def __init__(self, name: Name, dataBlock: DataBlock):
        super().__init__()
        self._name = name
        self._dataBlock = dataBlock
        self._dataBlock.id = name.id
        Name.variables.update({self._name.id: self._dataBlock})

        self.memoryAllocation = MemoryAllocation(self._name, self._dataBlock.get_type().get_single(), self._dataBlock._memory_range)
        self.initialisation = For(self._dataBlock._memory_range[-1])
        
        inner_loops = []
        for i in range(len(self._dataBlock._iteration_range) - 2, 0, -1):
            loop_bound = self._dataBlock._memory_range[i][1] - self._dataBlock._memory_range[i][0]
            inner_loop = For([Integer(0), loop_bound])
            inner_loops.append(inner_loop)
        if type(self._dataBlock._internal) is not FunctionCall:
            inner_loop = For(self._dataBlock._memory_range[0])
            inner_loops.append(inner_loop)

        index = []
        for i in range(len(inner_loops)):
            index.append(inner_loops[-(i + 1)].get_iteration_variable())
        index.append(self.initialisation.get_iteration_variable())
        
        output_offset = [Integer(0) for i in self._dataBlock._iteration_range[1:]]
        if type(self._dataBlock._internal._arguments[0]) is Index:
            input_offset = [0 for i in range(1, len(self._dataBlock._iteration_range))]
        else:
            input_offset = [self._dataBlock._iteration_range[i][0] - Name.variables[self._dataBlock._internal._arguments[0].id]._iteration_range[i][0] for i in range(1, len(self._dataBlock._iteration_range))]

        if type(self._dataBlock._internal) is FunctionCall:
            functionCall = FunctionCall(self._dataBlock._internal.id, [])
            functionCall._is_offloadable = self._dataBlock._internal._is_offloadable
            
            if type(self._dataBlock._internal._arguments[0]) is Index:
                functionCall.add_argument(Vector(index[0:-1], TInteger()))
            else:
                functionCall.add_argument(Reference(self._dataBlock._internal._arguments[0].multidimensional_index(index, 1)))

            for argument in self._dataBlock._internal._arguments[1:]:
                if type(argument.get_type()) is TDataBlock:
                    offset = [i[0] for i in Name.variables[argument.id]._iteration_range[1:]]
                    if len(Name.variables[argument.id]._iteration_range) == 1 or Name.variables[argument.id]._iteration_range[0][1] == Integer(1):
                        functionCall.add_argument(Name.variables[argument.id].multidimensional_index(index, 1))
                    else:
                        functionCall.add_argument(Reference(Name.variables[argument.id].multidimensional_index(index, 1)))
                else:
                    functionCall.add_argument(argument)
        
            functionCall.add_argument(Reference(self._dataBlock.multidimensional_index(index, 1)))
            if functionCall._is_offloadable:
                functionCall.add_argument(String("Solver::Offloadable::Yes"))
        else:
            functionCall = Assignment(self._dataBlock.multidimensional_index(index), self._dataBlock._internal)
        
        self.initialisation.add_statement(inner_loops[0])

        for i in range (0, len(inner_loops) - 1):
            inner_loops[i].add_statement(inner_loops[i + 1])
        inner_loops[-1].add_statement(functionCall)

        for i in range(len(inner_loops) - 1, -1, -1):
            inner_loops[i].close_scope()
        self.initialisation.close_scope()
        self._loops = inner_loops
        self._loops.insert(0, self.initialisation)

    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_construction_from_function(self, indent_level)

    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""DataBlockConstructionFromFunction:
{self._name.print_tree(indent_level + 1)}
{self._dataBlock.print_tree(indent_level + 1)}"""

class Assignment(Statement):
    def __init__(self, lhs: Expression, rhs: Expression):
        super().__init__()
        self._lhs = lhs
        self._lhs.on_left(True)
        self._rhs = rhs
        self._rhs.on_left(False) # Just to be sure...
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_assignment(self, indent_level)
    
    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""Assignment:
{self._lhs.print_tree(indent_level + 1)}
{self._rhs.print_tree(indent_level + 1)}"""
    

class DataBlockAssignment(Statement):
    def __init__(self, lhs: Expression, rhs: Expression):
        super().__init__()
        if type(lhs) is Name:
            self._lhs = Name.variables[lhs.id]
            self._lhs._id = lhs.id
        else:
            self._lhs = lhs
        
        if type(rhs) is Name:
            self._rhs = Name.variables[rhs.id]
        else:
            self._rhs = rhs

        self._lhs.on_left(True)    
        self._rhs.on_left(False)	# Just to be sure...

        self._loops = []
        index = []

        self._loops.append(For([Integer(0), self._lhs._iteration_range[-1][1] - self._lhs._iteration_range[-1][0]]))
        for i in range(len(self._lhs._memory_range) - 2, -1, -1):
            self._loops.append(For([Integer(0), self._lhs._iteration_range[i][1] - self._lhs._iteration_range[i][0]]))
       
        for i in range(len(self._lhs._memory_range) - 1, -1, -1):
            index.append(self._loops[i].get_iteration_variable())              

        if type(self._rhs.get_type()) is TDataBlock:
            rhs_offset = []
            for i in range(len(self._lhs._iteration_range)):
                lhs_start = self._lhs._iteration_range[i][0]
                rhs_start = self._rhs._iteration_range[i][0]
                if type(rhs_start.get_type()) is TDataBlock:
                    rhs_start = Subscript(rhs_start, lhs_start)
                rhs_offset.append(lhs_start - rhs_start)
        else:
            rhs_offset = [Integer(0) for i in self._lhs._iteration_range]

        lhs_offset = [Integer(0) for i in self._lhs._iteration_range]

        self._loops[-1].add_statement(Assignment(self._lhs.multidimensional_index(index, dimensions=self._lhs._memory_range), self._rhs.multidimensional_index(index, dimensions=self._lhs._memory_range)))
        for i in range(0, len(self._loops) - 1):
            self._loops[i].add_statement(self._loops[i + 1])

        for loop in self._loops[::-1]:
            loop.close_scope()
        self._loop = self._loops[0]
        self._numDimensions = len(self._loops)
    
    def print(self, printer: Printer, indent_level = 0):
        return printer.print_data_block_assignment(self, indent_level)
    
    def print_tree(self, indent_level = 0):
        return ' ' * indent_level * Node.spaces_per_tab + f"""DataBlockAssignment:
{self._lhs.print_tree(indent_level + 1)}
{self._rhs.print_tree(indent_level + 1)}"""
