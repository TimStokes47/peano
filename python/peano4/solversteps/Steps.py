# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org


class Steps(object):
  """
  Represents the total output generated from the Peano4 data model plus all
  the operations on it. This is basically a big collection.
  """
  
  def __init__(self,project):
    self.__project = project
    self.__steps = []
    
  def add_step(self,step):
    step.set_project(self.__project)
    self.__steps.append(step)
    
  def construct_output(self,output):
    for step in self.__steps:
      step.construct_output(output)
      
