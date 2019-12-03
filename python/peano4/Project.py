# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org


import peano4.output
import peano4.datamodel
import peano4.solversteps
import peano4.runner


class Project (object):
  """ 
  Represents on Peano 4 project.
  
  namespace sequence of strings representing the (nested) namespace
  """
  
  def __init__(self, namespace, directory = "."):
    self.namespace = namespace
    self.directory = directory

    #
    # Create default output model, i.e. those parts that have to be there
    # always
    #
    self.output       = peano4.output.Output()    
    
    #
    # Empty model by default
    #
    self.datamodel    = peano4.datamodel.Model(namespace)
    
    self.solversteps  = peano4.solversteps.Steps(self)
    
    self.main         = peano4.runner.DefaultSequence(self) 
    
    self.is_generated = False
    self.is_built     = False

    
  def generate(self, overwrite=peano4.output.Overwrite.Default):
    """
    Generate all code.
    """
    self.is_generated = True

    self.datamodel.construct_output(self.output)

    self.solversteps.construct_output(self.output)
      
    self.main.construct_output(self.output)
    
    self.output.generate(overwrite, self.directory)

          
  def build(self):
    """
    Invokes the underlying make/C build mechanism on the project. 
    """
    self.is_built = True
    if not self.is_generated:
      self.generate();
    pass
  
  def run(self, args):
    """
    Runs the code
    """
    if not self.is_built:
      self.build()
    pass
  
      
      
      