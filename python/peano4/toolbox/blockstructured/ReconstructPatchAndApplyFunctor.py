# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet


class ReconstructPatchAndApplyFunctor(ActionSet):
  """
  This class assumes that you have NxNxN patch within your block. It also assumes that 
  you have an 2MxNxN patch on your faces. M<N. The code interprets these face-associated
  data as overlap with the patch, hooks into touchCellLastTime, and projects the cell's
  patch data onto the overlap (simple copy). See ProjectPatchOntoFaces for details. 
  
  If the face patches are auxiliary patches holding copies from the cell patches, then 
  we can reconstruct N+2M x N+2M x N+2M patches per cell whenever we hit a cell: We 
  create a new temporary array, and copy data from both the cell patch and the faces
  into this array (gather operation). After that, we can launch the passed functor
  giving it access to the temporary, large array plus the original patch data. 
  
  """
  
  
  def __init__(self,patch,patch_overlap,functor_implementation):
    """

  patch          Instance of peano4.datamodel.Patch
  patch_overlap  Instance of peano4.datamodel.Patch. Consult remark above about how the
                 dimensions of this overlap patch have to match. 
  functor_implementation Plain C++ code
  
  
  <h2> functor_implementation </h2> 
  
  The functor implementation is a plain C/C++ 
  
  - If you want to use brackets in your implementation, please use double brackets {{ }} as 
    the template system otherwise gets confused.
  - The following C++ variables are defined:
  
    reconstructedPatch 
    originalPatch
    
    Both are plain double pointers.
  
   
    """
    self.d = {}
    if patch_overlap.dim[0] % 2 != 0:
      print( "Error: Patch associated to face has to have even number of cells. Otherwise, it is not a symmetric overlap." )
      assert( patch_overlap.dim[0] % 2 == 0 )
    if patch.dim[0] != patch.dim[1]:
      print( "Error: Can only handle square patches." )
      assert( patch.dim[0] == patch.dim[1] )
    if patch_overlap.dim[1] != patch.dim[0]:
      print( "Error: Patch of overlap and patch of cell have to match" )
      assert( patch_overlap.dim[1] == patch.dim[0] )
      
    self.d[ "UNKNOWNS" ]           = str(patch.no_of_unknowns)
    self.d[ "DOFS_PER_AXIS" ]      = str(patch.dim[0])
    self.d[ "OVERLAP" ]            = str(patch_overlap.dim[0]/2)
    self.d[ "NUMBER_OF_DOUBLE_VALUES_IN_ORIGINAL_PATCH_2D" ] = str(patch.no_of_unknowns * patch.dim[0] * patch.dim[0])
    self.d[ "NUMBER_OF_DOUBLE_VALUES_IN_ORIGINAL_PATCH_3D" ] = str(patch.no_of_unknowns * patch.dim[0] * patch.dim[0])
    self.d[ "NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_2D" ] = str(patch.no_of_unknowns * (patch_overlap.dim[0] + patch.dim[0]) * (patch_overlap.dim[0] + patch.dim[0]))
    self.d[ "NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_3D" ] = str(patch.no_of_unknowns * (patch_overlap.dim[0] + patch.dim[0]) * (patch_overlap.dim[0] + patch.dim[0]) * (patch_overlap.dim[0] + patch.dim[0]))
    self.d[ "FACES_ACCESSOR" ]     = "fineGridFaces"  + patch_overlap.name
    self.d[ "CELL_ACCESSOR" ]      = "fineGridCell" + patch.name
    
    self.d[ "FUNCTOR_IMPLEMENTATION" ]      = functor_implementation


  def get_constructor_body(self):
    return ""

  
  def get_destructor_body(self):
    return ""


  def get_body_of_getGridControlEvents(self):
    return "  return std::vector< peano4::grid::GridControlEvent >();\n" 


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  __Template_TouchCellFirstTime = """
  auto serialisePatchIndex = [](tarch::la::Vector<Dimensions,int> overlapCell, int normal) {{
    int base   = 1;
    int result = 0;
    for (int d=0; d<Dimensions; d++) {{
      result += overlapCell(d) * base;
      if (d==normal) {{
        base *= {OVERLAP}*2;
      }}
      else {{
        base *= {DOFS_PER_AXIS};
      }}
    }}
    return result;
  }};

  #if Dimensions==2
  double reconstructedPatch[{NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_2D}];
  #elif Dimensions==3
  double reconstructedPatch[{NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_3D}];
  #endif

  //
  // Loop over original patch (k) and copy stuff over.
  //
  dfor(sourceCell,{DOFS_PER_AXIS}) {{
    tarch::la::Vector<Dimensions,int> destinationCell = sourceCell + tarch::la::Vector<Dimensions,int>({OVERLAP});
    int sourceCellSerialised       = peano4::utils::dLinearised(sourceCell,{DOFS_PER_AXIS});
    int destinationCellSerialised  = peano4::utils::dLinearised(destinationCell,{DOFS_PER_AXIS} + 2*{OVERLAP});
    for (int j=0; j<{UNKNOWNS}; j++) {{
      reconstructedPatch[destinationCellSerialised*{UNKNOWNS}+j] = {CELL_ACCESSOR}.value[ sourceCellSerialised*{UNKNOWNS}+j ];
      assertion2( reconstructedPatch[destinationCellSerialised*{UNKNOWNS}+j]==reconstructedPatch[destinationCellSerialised*{UNKNOWNS}+j], sourceCell, j );
    }}
  }}
  
  //
  // Bring in the auxiliary patches, i.e. befill halo
  //
  for(int d=0; d<Dimensions; d++) {{
    //
    // d-loop over all dimensions except d. The vector k's entry d is set
    // to 0. We start with the left/bottom face, i.e. the one closer to the 
    // coordinate system's origin.
    //
    dfore(k,{DOFS_PER_AXIS},d,0) {{
      for (int i=0; i<{OVERLAP}; i++) {{
        tarch::la::Vector<Dimensions,int> destinationCell = k + tarch::la::Vector<Dimensions,int>({OVERLAP});
        tarch::la::Vector<Dimensions,int> sourceCell      = k;
        destinationCell(d) = i;
        sourceCell(d)      = i;
        
        int destinationCellSerialised   = peano4::utils::dLinearised(destinationCell,{DOFS_PER_AXIS} + 2*{OVERLAP});
        int sourceCellSerialised        = serialisePatchIndex(sourceCell,d);

        for (int j=0; j<{UNKNOWNS}; j++) {{
          reconstructedPatch[ destinationCellSerialised*{UNKNOWNS}+j ] = {FACES_ACCESSOR}(d+Dimensions).value[ sourceCellSerialised*{UNKNOWNS}+j ];
          assertion( reconstructedPatch[ destinationCellSerialised*{UNKNOWNS}+j ]==reconstructedPatch[ destinationCellSerialised*{UNKNOWNS}+j ] );
        }}

        destinationCell(d) = i+{DOFS_PER_AXIS}+{OVERLAP};
        sourceCell(d)      = i+{OVERLAP};

        destinationCellSerialised   = peano4::utils::dLinearised(destinationCell,{DOFS_PER_AXIS} + 2*{OVERLAP});
        sourceCellSerialised        = serialisePatchIndex(sourceCell,d);
        for (int j=0; j<{UNKNOWNS}; j++) {{
          reconstructedPatch[ destinationCellSerialised*{UNKNOWNS}+j ] = {FACES_ACCESSOR}(d+Dimensions).value[ sourceCellSerialised*{UNKNOWNS}+j ];
          assertion( reconstructedPatch[ destinationCellSerialised*{UNKNOWNS}+j ]==reconstructedPatch[ destinationCellSerialised*{UNKNOWNS}+j ] );
        }}
      }}
    }}
  }}

  #if Dimensions==2
  auto f = [&]( double reconstructedPatch[{NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_2D}], double originalPatch[{NUMBER_OF_DOUBLE_VALUES_IN_ORIGINAL_PATCH_2D}], const tarch::la::Vector<Dimensions,double>& centre, double dx ) -> void {{
  #elif Dimensions==3
  auto f = [&]( double reconstructedPatch[{NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_3D}], double originalPatch[{NUMBER_OF_DOUBLE_VALUES_IN_ORIGINAL_PATCH_3D}], const tarch::la::Vector<Dimensions,double>& centre, double dx ) -> void {{
  #endif
{FUNCTOR_IMPLEMENTATION}
  }};

  f( reconstructedPatch, {CELL_ACCESSOR}.value, marker.x().data(), marker.h()(0)/{DOFS_PER_AXIS} );
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      result = self.__Template_TouchCellFirstTime.format(**self.d)
      pass 
    return result


  def get_attributes(self):
    return ""


  def get_includes(self):
    return """
#include <functional>
#include "peano4/utils/Loop.h"
"""
