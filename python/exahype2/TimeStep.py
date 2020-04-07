# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4.solversteps.ActionSet

class TimeStep(peano4.solversteps.ActionSet):
  def __init__(self,solvers):
    self._solvers = solvers


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  def get_includes(self):
    result = """
#include "SolverRepository.h"
#include "exahype2/RefinementControl.h"
"""
    for solver in self._solvers:
      result += solver.add_includes()
      
    return result


  def get_body_of_operation(self,operation_name):
    #
    # ===========================
    # Touch cell the first time
    # ===========================
    #
    if operation_name == peano4.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      result = ""
      for solver in self._solvers:
        result += solver.get_time_step_invocation()
        
      return result
    return "// Nothing to implement\n"


  def get_body_of_getGridControlEvents(self):
    return """
  return std::vector< peano4::grid::GridControlEvent >();
""" 


  def get_attributes(self):
    """
     We augment the mapping with an array of grid events and then befill this
     array througout the traversal.
    """
    return ""
