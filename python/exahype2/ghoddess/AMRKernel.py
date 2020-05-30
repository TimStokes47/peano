# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet


class AMRKernel(ActionSet):
  """

  @todo Docu  
  
  """
  
  
  def __init__(self,cell_data,predicate,implementation,additional_includes,called_by_grid_construction):
    """

  cell_data                Instance of peano4.datamodel.Patch. It is a 
                           degenerated patch of dimensions M x 1 x 1.
                           I need this one to get access to the right data.
  predicate                A function from the actual solver that decides 
                           whether I have to invoke the kernel. Add
                           brackets. Predicates typically have no arguments and are 
                           const. But you can also pass true, e.g.
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
    self.d[ "CELL_ACCESSOR" ]          = "fineGridCell" + cell_data.name
    self.d[ "IMPLEMENTATION" ]         = implementation
    self.d[ "GUARD" ]                  = predicate
    
    if called_by_grid_construction:
      self.d[ "CALLED_BY_GRID_CONSTRUCTION" ] = "true"
    else:
      self.d[ "CALLED_BY_GRID_CONSTRUCTION" ] = "false"
        
    self.additional_includes = additional_includes

  
  def get_body_of_getGridControlEvents(self):
    return """
  return _globalRefinementControl.getGridControlEvents();
""" 


  def get_constructor_body(self):
    return ""

  
  def get_destructor_body(self):
    return ""


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  __Template_TouchCellFirstTime = """
  logTraceIn( "touchCellFirstTime(...)" );

  if ({GUARD}) {{
    double* cellData = {CELL_ACCESSOR}.value;
    auto newCommand = {IMPLEMENTATION}
    _localRefinementControl.addCommand( marker.x(), marker.h(), newCommand, {CALLED_BY_GRID_CONSTRUCTION} );
  }}
  
  logTraceOut( "touchCellFirstTime(...)" );
"""


  def get_body_of_operation(self,operation_name):
    result = ""
    if operation_name==ActionSet.OPERATION_BEGIN_TRAVERSAL:
      result = """
  _localRefinementControl.clear();
  _globalRefinementControl.startToAccumulateLocally();
"""
    if operation_name==ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      result = self.__Template_TouchCellFirstTime.format(**self.d)
      pass 
    if operation_name==ActionSet.OPERATION_END_TRAVERSAL:
      result = """
  _globalRefinementControl.merge( _localRefinementControl );
"""
    return result


  def get_includes(self):
    return """
#include <functional>
""" + self.additional_includes


  def get_static_initialisations(self,full_qualified_classname):
    return "::exahype2::RefinementControl " + full_qualified_classname + "::_globalRefinementControl;"


  def get_attributes(self):
    return """
    static ::exahype2::RefinementControl  _globalRefinementControl;
    ::exahype2::RefinementControl         _localRefinementControl;
"""

  def get_includes(self):
    return """
#include "exahype2/RefinementControl.h"
#include "SolverRepository.h"
"""
