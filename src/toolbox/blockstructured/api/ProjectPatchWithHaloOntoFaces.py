# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class ProjectPatchWithHaloOntoFaces(ActionSet):
  """!

  Write a patch with halo onto the overlaps of all of its faces

  Counterpart of ReconstructPatchWithHalo: That action set gathers a cell's
  NxNxN patch and the 2MxNxN overlaps of its faces into one patch with halo
  of (N+2M)x(N+2M)x(N+2M) entries. This action set takes such a patch with
  halo and writes it back onto the face overlaps. Different to
  ProjectPatchOntoFaces, it writes both halves of each overlap: the half
  inside the cell from the patch, and the half outside of the cell from the
  halo. Afterwards, the overlaps hold exactly the data the patch with halo
  has been reconstructed from. The corners of the halo are not used.

  The action set is used to restore the face data from a checkpoint
  (@ref exahype2.api.blockstructured.LoadCheckpoint.LoadCheckpoint): Along the
  domain boundary, the outer half of each overlap holds the ghost data. A
  projection from the patch alone cannot restore it.

  Faces between two cells receive their data from both cells. If the patches
  with halo are consistent, as they are in a checkpoint, both cells write the
  same values.

  """

  def __init__(self,
               patch_with_halo,
               patch_overlap,
               guard="true",
               additional_includes=""):
    """!

    @param patch_with_halo: peano4.api.datamodel.Patch
      Cell data of (N+2M)^d entries which is projected.

    @param patch_overlap: peano4.api.datamodel.Patch
      Face data of 2M x N^(d-1) entries which is written.

    @param guard: str
      C++ predicate. The action set only projects cells for which it holds.

    @param additional_includes: str
      Further includes the generated code needs, e.g. for the guard.

    """
    super(ProjectPatchWithHaloOntoFaces, self).__init__(descend_invocation_order=1, parallel=False)

    if patch_overlap.dim[0] % 2 != 0:
      raise Exception("Patch associated to face has to have even number of cells. Otherwise, it is not a symmetric overlap.")
    if patch_with_halo.dim[0] != patch_overlap.dim[1] + patch_overlap.dim[0]:
      raise Exception("patch_with_halo has to hold the face overlap's tangential extent plus its full width per axis.")

    self.guard               = guard
    self.unknowns            = int(patch_with_halo.no_of_unknowns)
    self.dofs_per_axis       = int(patch_overlap.dim[1])
    self.overlap             = int(patch_overlap.dim[0] // 2)
    self.patch_with_halo     = patch_with_halo.name
    self.patch_overlap       = patch_overlap.name
    self.additional_includes = additional_includes

  def get_constructor_body(self):
    return ""

  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")

  def user_should_modify_template(self):
    return False

  def get_body_of_operation(self, operation_name):
    if operation_name != ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      return "\n"
    return f"""
  logTraceInWith1Argument( "touchCellFirstTime(...)", marker.toString() );

  constexpr int Unknowns            = {self.unknowns};
  constexpr int DoFsPerAxis         = {self.dofs_per_axis};
  constexpr int Overlap             = {self.overlap};
  constexpr int EntriesPerAxisHalo  = DoFsPerAxis + 2 * Overlap;

  // Index of an entry within a face overlap with 2*Overlap entries along the
  // normal and DoFsPerAxis entries along all other axes
  auto serialiseOverlapIndex = [](const tarch::la::Vector<Dimensions,int>& overlapCell, int normal) {{
    int base   = 1;
    int result = 0;
    for (int d=0; d<Dimensions; d++) {{
      result += overlapCell(d) * base;
      base   *= (d==normal) ? 2*Overlap : DoFsPerAxis;
    }}
    return result;
  }};

  const double* patchWithHalo = fineGridCell{self.patch_with_halo}.value;

  for (int d=0; d<Dimensions; d++) {{
    dfore(k,DoFsPerAxis,d,0) {{
      for (int j=0; j<2*Overlap; j++) {{
        tarch::la::Vector<Dimensions,int> overlapCell = k;
        overlapCell(d) = j;
        const int overlapCellSerialised = serialiseOverlapIndex(overlapCell,d);

        // Left/bottom face: entries 0 to 2*Overlap-1 along d of the patch with halo
        tarch::la::Vector<Dimensions,int> patchCell = k + tarch::la::Vector<Dimensions,int>(Overlap);
        patchCell(d) = j;
        int patchCellSerialised = peano4::utils::dLinearised(patchCell,EntriesPerAxisHalo);
        for (int unknown=0; unknown<Unknowns; unknown++) {{
          fineGridFaces{self.patch_overlap}(d).value[overlapCellSerialised*Unknowns+unknown] = patchWithHalo[patchCellSerialised*Unknowns+unknown];
        }}

        // Right/top face: entries DoFsPerAxis to DoFsPerAxis+2*Overlap-1 along d
        patchCell(d) = DoFsPerAxis + j;
        patchCellSerialised = peano4::utils::dLinearised(patchCell,EntriesPerAxisHalo);
        for (int unknown=0; unknown<Unknowns; unknown++) {{
          fineGridFaces{self.patch_overlap}(d+Dimensions).value[overlapCellSerialised*Unknowns+unknown] = patchWithHalo[patchCellSerialised*Unknowns+unknown];
        }}
      }}
    }}
  }}

  logTraceOut( "touchCellFirstTime(...)" );
"""

  def get_guard_of_operation(self, operation_name):
    if operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      return self.guard
    return "true"

  def get_attributes(self):
    return ""

  def get_includes(self):
    return """
#include "peano4/utils/Loop.h"
""" + self.additional_includes
