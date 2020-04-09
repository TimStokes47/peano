# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet



class SetLabels(ActionSet):
  def __init__(self):
    """
      maximum_cell_width
    """
    pass
    #self.d = {}
    #self.d[ "H_MAX" ]     = str(maximum_cell_width)


  def get_constructor_body(self):
    return ""

    
  def get_destructor_body(self):
    return ""


  #def get_body_of_getGridControlEvents(self):
  #  return self.__Template_GridControlEvents.format(**self.d)


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_CREATE_PERSISTENT_FACE or operation_name==ActionSet.OPERATION_CREATE_HANGING_FACE:
      result += """
  tarch::la::Vector<Dimensions, double> offset(DomainOffset);
  tarch::la::Vector<Dimensions, double> size(DomainSize);
  bool isBoundary = false;
  for (int d=0; d<Dimensions; d++) {{
    isBoundary |= tarch::la::equals( marker.x()(d), offset(d) );
    isBoundary |= tarch::la::equals( marker.x()(d), offset(d) + size(d) );
  }}
"""

    return result


  def get_attributes(self):
    return ""


  def get_includes(self):
    return """
#include "Constants.h"
"""    

