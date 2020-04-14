# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.toolbox.blockstructured.ApplyFunctorOnPatch


class AMROnPatch(peano4.toolbox.blockstructured.ApplyFunctorOnPatch):
  """
  
   This is an extension of ApplyFunctorOnPatch which offers you an instance of 
   RefinementControl which is eventually fed back to Peano such that it triggers
   and realises the AMR.
   
   If you write a new solver, you will be asked to add action sets to the three
   fundamental steps of ExaHyPE2 (grid construction, time stepping, plotting).
   You can inject an instance of this class as action set.
  
  """
  def __init__(self,patch,functor_implementation,additional_includes):
    super(AMROnPatch,self).__init__(patch,functor_implementation,additional_includes)
    
  
  def get_body_of_getGridControlEvents(self):
    return "  return _refinementControl.getGridControlEvents();\n" 


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")
  

  def get_body_of_operation(self,operation_name):
    result = ""
    if operation_name==peano4.solversteps.ActionSet.OPERATION_BEGIN_TRAVERSAL:
      result = "_refinementControl.clear();"
    
    result += super(AMROnPatch,self).get_body_of_operation(operation_name)
    return result


  def get_static_initialisations(self,full_qualified_classname):
    return "::exahype2::RefinementControl " + full_qualified_classname + "::_refinementControl;"


  def get_attributes(self):
    return """
    static ::exahype2::RefinementControl  _refinementControl;
"""


  def get_includes(self):
    return """
#include <functional>
""" + self.additional_includes
  
