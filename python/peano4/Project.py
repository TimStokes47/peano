# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org


import peano4.output
import peano4.datamodel
import peano4.solversteps
import peano4.runner


import subprocess
import sys


class Project (object):
  """ 
  Represents on Peano 4 project.
  
  namespace sequence of strings representing the (nested) namespace
  """
  
  def __init__(self, namespace, directory = "."):
    if sys.version_info.major < 3:
      print( "Warning: should be invoked through python3, i.e. with newer Python version" )

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
    
    self.constants  = peano4.output.Constants(self)


  def export_constant( self, value, name ):
    self.constants.export_constant( value, name )

    
  def generate(self, overwrite=peano4.output.Overwrite.Default):
    """
    Generate all code.
    """
    print( "generate all code ..." )
    self.is_generated = True

    self.datamodel.construct_output(self.output)

    self.solversteps.construct_output(self.output)
      
    self.main.construct_output(self.output)
    
    self.output.generate(overwrite, self.directory)

    self.constants.generate(overwrite, self.directory)

    print( "generation complete" )

          
  def build(self, additional_libraries = []):
    """
    Invokes the underlying make/C build mechanism on the project. 
    We invoke the make command via a subprocess. That's it.
    """
    self.is_built = True
    if not self.is_generated:
      self.generate();

    print( "clean up project ..." )
    try:
      subprocess.check_call(["make", "clean"])
      print( "clean complete" )
    except Exception as e:
      print( "clean failed (" + str(e) + ") - continue anyway" )

    if self.is_built:
      print( "start to compile ..." )
      try:
        subprocess.check_call(["make", "-j"])
        print( "compile complete" )
      except Exception as e:
        print( "compile was not successful: " + str(e) )
        self.is_built = False
    else:
      print( "can not build as code generation has not been successful" )
  
  def run(self, args):
    """
    Runs the code. args should be a list of strings or the empty list.
    """
    if not self.is_built:
      self.build()
    if self.is_built:
      print( "run application ..." )
      try:
        subprocess.call(["./peano4"] + args)
        print( "run complete" )
      except Exception as e:
        print( "run of application was not successful: " + str(e) )
    else:
      print( "can not run as code compilation has not been successful" )
  
      
      
      