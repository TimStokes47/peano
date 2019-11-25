# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org


import peano4.output


class Project (object):
  """ 
  Represents on Peano 4 project.
  """
  
  def __init__(self, namespace, directory = "."):
    self.namespace = namespace
    self.directory = directory

    #
    # Create default output model, i.e. those parts that have to be there
    # always
    #
    self.output    = peano4.output.Output()    
    self.is_generated = False
    self.is_built     = False
    pass
    
  def generate(self, overwrite=peano4.output.Overwrite.Default):
    """
    Generate all code.
    """
    self.is_generated = True
    self.output.generate(overwrite, self.directory)
    pass
          
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
  
      
      
      