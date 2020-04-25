# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet



def get_face_overlap_merge_implementation(patch_overlap):
  d = {
    "OVERLAP": patch_overlap.dim[0]/2,
    "DOFS_PER_AXIS": patch_overlap.dim[1]
  }
  
  
  template = """
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
  
  dfore(i,{DOFS_PER_AXIS},marker.normalAxis(),0) {{
    for (int j=0; j<{OVERLAP}; j++) {{
    }}
    // marker.getSelectedFaceNumber()<Dimensions ? 0 : 1
  }}
"""

  return template.format(**d)
