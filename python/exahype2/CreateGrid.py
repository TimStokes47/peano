# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4.solversteps.ActionSet

class CreateGrid(peano4.solversteps.ActionSet):
  def __init__(self):
    pass


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False

  def get_body_of_operation(self,operation_name):
    # An examples where we only do something for :
    if operation_name == peano4.solversteps.ActionSet.OPERATION_CREATE_CELL:
      return """
         
"""
    return "// Nothing to implement\n"
