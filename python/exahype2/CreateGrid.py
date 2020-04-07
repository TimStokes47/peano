# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4.solversteps.ActionSet

class CreateGrid(peano4.solversteps.ActionSet):
  def __init__(self,solvers):
    self._solvers = solvers


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  def get_includes(self):
    return """
#include "peano4/utils/Loop.h"
#include "SolverRepository.h"
#include "exahype2/PatchUtils.h"
#include "exahype2/RefinementControl.h"
"""


#
# @todo Ich sollte die Aspekte hier aufgliedern, d.h. Initialisation ist was anderes als 
# AMR. Es macht keinen Sinn, alles in einem Mapping zu haben.
#

  def get_body_of_operation(self,operation_name):
    # An examples where we only do something for :
    if operation_name == peano4.solversteps.ActionSet.OPERATION_CREATE_CELL:
      result = """

"""      
      
      for solver in self._solvers:
        result += solver.get_initialisation_invocation()
        
      result += """
  ::exahype2::RefinementCommand  refinementCommand = ::exahype2::RefinementCommand::Coarsen;
"""

      for solver in self._solvers:
        result += solver.get_refinement_command()
     
      result += """
  _refinementControl.addCommand( marker.x(), marker.h(), refinementCommand, true ); 
"""
      return result
    if operation_name == peano4.solversteps.ActionSet.OPERATION_BEGIN_TRAVERSAL:
      return """
  _refinementControl.clear();   
"""
    return "// Nothing to implement\n"


  def get_body_of_getGridControlEvents(self):
    return """
  return _refinementControl.getGridControlEvents();
""" 


  def get_attributes(self):
    """
     We augment the mapping with an array of grid events and then befill this
     array througout the traversal.
    """
    return "::exahype2::RefinementControl  _refinementControl;"
