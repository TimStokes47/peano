# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class ReconstructPatchWithHalo(ActionSet):
  """!

  Reconstruct a cell patch together with its halo into a separate,
  already-declared reconstructed_patch cell submodel.

  This class assumes that you have an NxNxN patch within your block. It also
  assumes that you have a 2MxNxN patch on your faces, M<N. The code
  interprets these face-associated data as overlap with the patch, hooks
  into touchCellFirstTime, and gathers the cell patch and its face-associated
  overlaps into reconstructed_patch - a real, separately-declared cell
  submodel of shape (N+2M)x(N+2M)x(N+2M) (@see ProjectPatchOntoFaces for the
  counterpart that fills the face-associated overlap patches in the first
  place).

  reconstructed_patch is not a scratch array this class allocates and frees
  itself - it is a proper Peano-managed cell submodel handed in directly by
  the caller (declared on the surrounding Task/Step the same way patch and
  patch_overlap are), so this class never has to choose where or how to
  allocate memory for it: it simply writes into reconstructed_patch's own
  accessor.

  @see ReconstructPatchAndApplyFunctor, which extends this with an
  additional functor invoked once the reconstruction is done.

  """

  def __init__(self,
               patch,
               patch_overlap,
               reconstructed_patch,
               guard="true",
               add_assertions_to_halo_exchange = True,
               additional_includes = ""
               ):
    """!

  patch: peano4.api.datamodel.Patch
    Patch which is to be used

  patch_overlap: peano4.api.datamodel.Patch
    Consult remark above about how the dimensions of this overlap
    patch have to match

  reconstructed_patch: peano4.api.datamodel.Patch
    Where to write the reconstructed patch-plus-halo to - has to already be
    declared (use_cell()) on the surrounding Task/Step, just like patch and
    patch_overlap, and its dimensions have to be patch's plus twice
    patch_overlap's overlap width, per axis.

  additional_includes: str
    Further includes this action set's generated code needs - e.g. a
    solver's own default and user includes.

  ## Data validation

  I use quite a lot of validitiy checks for the copied data via comparison to its self. This
  way, I can at least spot nans. I use the dictionary entries ASSERTION_WITH_X_ARGUMENTS to
  realise this, and they are, by default, set to Peano's assertion macros. You can redefine
  them.

    """
    super(ReconstructPatchWithHalo,self).__init__(descend_invocation_order=1,parallel=False)

    if patch_overlap.dim[0] % 2 != 0:
      print( "Error: Patch associated to face has to have even number of cells. Otherwise, it is not a symmetric overlap." )
      assert( patch_overlap.dim[0] % 2 == 0 )
    if patch.dim[0] != patch.dim[1]:
      print( "Error: Can only handle square patches." )
      assert( patch.dim[0] == patch.dim[1] )
    if patch_overlap.dim[1] != patch.dim[0]:
      print( "Error: Patch of overlap and patch of cell have to match" )
      assert( patch_overlap.dim[1] == patch.dim[0] )
    if reconstructed_patch.dim[0] != patch.dim[0] + patch_overlap.dim[0]:
      print( "Error: reconstructed_patch has to be patch's plus patch_overlap's full width per axis." )
      assert( reconstructed_patch.dim[0] == patch.dim[0] + patch_overlap.dim[0] )

    self.guard          = guard
    self.no_of_unknowns = int(patch.no_of_unknowns)
    self.dofs_per_axis  = int(patch.dim[0])
    self.total_overlap  = int(patch_overlap.dim[0])
    self.overlap_name   = patch_overlap.name
    self.patch_name     = patch.name
    self.reconstructed_patch_name = reconstructed_patch.name

    self.add_assertions_to_halo_exchange = add_assertions_to_halo_exchange
    self.additional_includes = additional_includes


  def _add_action_set_entries_to_dictionary(self,d):
    """!

    Befill dictionary

    If you inherit from this class and specialise this routine, you can add
    more entries to the dictionary. This is important if you change the
    actual compute kernels (templates) and use futher unknowns in there. If
    you specialise the function, i.e. overwrite it in a subclass, please
    ensure that you use the superclass still.

    """
    d[ "GUARD" ]              = self.guard

    assert isinstance(self.no_of_unknowns,int)
    assert isinstance(self.dofs_per_axis,int)
    assert isinstance(self.total_overlap,int)

    d[ "UNKNOWNS" ]           = str(self.no_of_unknowns)
    d[ "DOFS_PER_AXIS" ]      = str(self.dofs_per_axis)
    d[ "OVERLAP" ]            = str(int(self.total_overlap/2))
    d[ "NUMBER_OF_DOUBLE_VALUES_IN_ORIGINAL_PATCH_2D" ]      = str(self.no_of_unknowns * self.dofs_per_axis * self.dofs_per_axis)
    d[ "NUMBER_OF_DOUBLE_VALUES_IN_ORIGINAL_PATCH_3D" ]      = str(self.no_of_unknowns * self.dofs_per_axis * self.dofs_per_axis * self.dofs_per_axis)
    d[ "NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_2D" ] = str(self.no_of_unknowns * (self.total_overlap + self.dofs_per_axis) * (self.total_overlap + self.dofs_per_axis))
    d[ "NUMBER_OF_DOUBLE_VALUES_IN_RECONSTRUCTED_PATCH_3D" ] = str(self.no_of_unknowns * (self.total_overlap + self.dofs_per_axis) * (self.total_overlap + self.dofs_per_axis) * (self.total_overlap + self.dofs_per_axis))
    d[ "FACES_ACCESSOR" ]     = "fineGridFaces"  + self.overlap_name
    d[ "CELL_ACCESSOR" ]      = "fineGridCell" + self.patch_name
    d[ "RECONSTRUCTED_PATCH_ACCESSOR" ] = "fineGridCell" + self.reconstructed_patch_name

    d[ "ASSERTION_WITH_1_ARGUMENTS" ] = "assertion1"
    d[ "ASSERTION_WITH_2_ARGUMENTS" ] = "assertion2"
    d[ "ASSERTION_WITH_3_ARGUMENTS" ] = "assertion3"
    d[ "ASSERTION_WITH_4_ARGUMENTS" ] = "assertion4"
    d[ "ASSERTION_WITH_5_ARGUMENTS" ] = "assertion5"
    d[ "ASSERTION_WITH_6_ARGUMENTS" ] = "assertion6"
    d[ "ASSERTION_WITH_7_ARGUMENTS" ] = "assertion7"

    if self.add_assertions_to_halo_exchange:
      d[ "ASSERTION_PREFIX_FOR_HALO" ] = "false"
    else:
      d[ "ASSERTION_PREFIX_FOR_HALO" ] = "true"


  def get_constructor_body(self):
    return """  _treeNumber = treeNumber;
"""


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  _Template_TouchCellFirstTime_Preamble = """
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

  if ({GUARD}) {{
    logTraceInWith1Argument( "touchCellFirstTime(...)", marker.toString() );

    double* oldQWithHalo = {RECONSTRUCTED_PATCH_ACCESSOR}.value;
"""


  _Template_TouchCellFirstTime_Fill_Patch = """
    //
    // Loop over original patch (k) and copy stuff over.
    //
    dfor(sourceCell,{DOFS_PER_AXIS}) {{
      tarch::la::Vector<Dimensions,int> destinationCell = sourceCell + tarch::la::Vector<Dimensions,int>({OVERLAP});
      int sourceCellSerialised       = peano4::utils::dLinearised(sourceCell,{DOFS_PER_AXIS});
      int destinationCellSerialised  = peano4::utils::dLinearised(destinationCell,{DOFS_PER_AXIS} + 2*{OVERLAP});
      for (int j=0; j<{UNKNOWNS}; j++) {{
        oldQWithHalo[destinationCellSerialised*{UNKNOWNS}+j] = {CELL_ACCESSOR}.value[ sourceCellSerialised*{UNKNOWNS}+j ];
        {ASSERTION_WITH_4_ARGUMENTS}( oldQWithHalo[destinationCellSerialised*{UNKNOWNS}+j]==oldQWithHalo[destinationCellSerialised*{UNKNOWNS}+j], sourceCell, j, _treeNumber, marker.toString() );
      }}
    }}
"""


  _Template_TouchCellFirstTime_Fill_Halos = """
    //
    // Bring in the auxiliary patches, i.e. befill halo
    //
    for(int d=0; d<Dimensions; d++) {{
      logTraceInWith1Argument( "touchCellFirstTime(...)::loopOverFace", d );
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
            oldQWithHalo[ destinationCellSerialised*{UNKNOWNS}+j ] = {FACES_ACCESSOR}(d).value[ sourceCellSerialised*{UNKNOWNS}+j ];
            {ASSERTION_WITH_7_ARGUMENTS}(
              {ASSERTION_PREFIX_FOR_HALO} or
              oldQWithHalo[ destinationCellSerialised*{UNKNOWNS}+j ]==oldQWithHalo[ destinationCellSerialised*{UNKNOWNS}+j ],
              sourceCell, destinationCell, j, d, marker.toString(), _treeNumber, marker.toString()
            );
          }}

          destinationCell(d) = i+{DOFS_PER_AXIS}+{OVERLAP};
          sourceCell(d)      = i+{OVERLAP};

          destinationCellSerialised   = peano4::utils::dLinearised(destinationCell,{DOFS_PER_AXIS} + 2*{OVERLAP});
          sourceCellSerialised        = serialisePatchIndex(sourceCell,d);
          for (int j=0; j<{UNKNOWNS}; j++) {{
            oldQWithHalo[ destinationCellSerialised*{UNKNOWNS}+j ] = {FACES_ACCESSOR}(d+Dimensions).value[ sourceCellSerialised*{UNKNOWNS}+j ];
            {ASSERTION_WITH_7_ARGUMENTS}(
              {ASSERTION_PREFIX_FOR_HALO} or
              oldQWithHalo[ destinationCellSerialised*{UNKNOWNS}+j ]==oldQWithHalo[ destinationCellSerialised*{UNKNOWNS}+j ],
              sourceCell, destinationCell, j, d, marker.toString(), _treeNumber, marker.toString()
            );
          }}
        }}
      }}
      logTraceOut( "touchCellFirstTime(...)::loopOverFace" );
    }}
"""


  _Template_TouchCellFirstTime_Epilogue = """
    logTraceOut( "touchCellFirstTime(...)" );
  }}
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      d = {}
      self._add_action_set_entries_to_dictionary(d)
      result  = self._Template_TouchCellFirstTime_Preamble.format(**d)
      result += self._Template_TouchCellFirstTime_Fill_Patch.format(**d)
      result += self._Template_TouchCellFirstTime_Fill_Halos.format(**d)
      result += self._Template_TouchCellFirstTime_Epilogue.format(**d)
      pass
    return result


  def get_attributes(self):
    return """int  _treeNumber;
"""


  def get_includes(self):
    return """
#include <functional>
#include "peano4/utils/Loop.h"
#include "tarch/multicore/Core.h"
""" + self.additional_includes
