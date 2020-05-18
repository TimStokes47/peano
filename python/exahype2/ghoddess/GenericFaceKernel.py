# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet


class GenericFaceKernel(ActionSet):
  """

  @todo Docu  
  
  """
  
  
  def __init__(self,cell_data,predicate,implementation,additional_includes):
    """

  cell_data                Instance of peano4.datamodel.Patch. It is a 
                           degenerated patch of dimensions M x 1 x 1.
                           I need this one to get access to the right data.
  predicate                A function from the actual solver that decides 
                           whether I have to invoke the kernel. Omit the
                           brackets. Predicates have no arguments and are 
                           const.
  implementation           Plain C++ code.
  additional_includes      Further include statements that I need in the kernels.
  
  
  <h2> implementation </h2> 
  
  The implementation is a plain C/C++ 
  
  - If you want to use brackets in your implementation, please use double brackets {{ }} as 
    the template system otherwise gets confused.
  - The following C++ variables are defined:
  
    data
    
    It is a plain double pointer.
   
    """
    self.d = {}
      
    self.d[ "UNKNOWNS" ]               = str(cell_data.no_of_unknowns)
    self.d[ "DOFS" ]                   = str( int(cell_data.dim[0]) )
    self.d[ "FACE_ACCESSOR" ]          = "fineGridFace" + cell_data.name
    self.d[ "IMPLEMENTATION" ]         = implementation
    self.d[ "GUARD" ]                  = predicate
    
    self.additional_includes = additional_includes


  def get_constructor_body(self):
    return ""

  
  def get_destructor_body(self):
    return ""


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  __Template_TouchFaceFirstTime = """
  logTraceIn( "touchFaceFirstTime(...)" );

  if ({GUARD}) {{
    double* faceData = {FACE_ACCESSOR}.value;
    {IMPLEMENTATION}
  }}
  
  logTraceOut( "touchFaceFirstTime(...)" );
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME:
      result = self.__Template_TouchFaceFirstTime.format(**self.d)
      pass 
    return result


  def get_includes(self):
    return """
#include <functional>
""" + self.additional_includes
