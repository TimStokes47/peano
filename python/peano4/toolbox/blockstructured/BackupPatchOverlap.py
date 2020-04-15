# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet



class BackupPatchOverlap(ActionSet):
  """
  
  This class plugs into the first access to a face and copies the data over
  
  patch_overlap_in
  patch_overlap_out
  
  """
  
  
  def __init__(self,patch_overlap_in,patch_overlap_out):
    self.d = {}
    self.d[ "UNKNOWNS" ]           = str(patch_overlap_in.no_of_unknowns)
    self.d[ "DOFS_PER_AXIS" ]      = str(patch_overlap_in.dim[1])
    self.d[ "OVERLAP" ]            = str(patch_overlap_in.dim[0]/2)
    self.d[ "FACES_ACCESSOR_IN" ]  = "fineGridFace"  + patch_overlap_in.name
    self.d[ "FACES_ACCESSOR_OUT" ] = "fineGridFace"  + patch_overlap_out.name


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


  __Template_TouchFaceFirstTime = """
    int counter = 0;
    dfore(k,{DOFS_PER_AXIS},0,0) {{
      for (int i=0; i<{OVERLAP}*2; i++) {{
        for (int j=0; j<{UNKNOWNS}; j++) {{
          {FACES_ACCESSOR_OUT}.value[counter] = 
            {FACES_ACCESSOR_IN}.value[counter];
          counter++;
        }}
      }}
    }}
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME:
      result = self.__Template_TouchFaceFirstTime.format(**self.d)
      pass 
    return result


  def get_attributes(self):
    return ""


  def get_includes(self):
    return """
#include "peano4/utils/Loop.h"
"""
