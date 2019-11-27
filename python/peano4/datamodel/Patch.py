# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import os


class Patch(object):
  def __init__(self, dim, no_of_unknowns, name):
    """
      dim should be a two-tuple or a three-tuple of integers, so a construction 
      could look like
      
        cell_data = peano4.datamodel.Patch( (6,6,6), "Fluid" )

    """
    self.dim       = dim
    self.no_of_unknowns = no_of_unknowns
    self.generator = PatchToDoubleArray(self)
    self.name      = name
    self.namespace = []


  def configure(self,namespace):
    """
    Typically called by model as soon as you add an object to it
    """
    self.namespace = namespace





class PatchToDoubleArray(object):
  """
  Very simple converter which maps the patch 1:1 onto a double array
  """
  def __init__(self,patch):
    self.data = patch
    
    
  def construct_output(self,output):
    """
      Pass in a version of output
    """
    output.makefile.add_cpp_file( self.data.namespace[-1] + "/" + self.data.name + ".cpp" )
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )    
    generated_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix+".h.template",
      templatefile_prefix+".cpp.template",
      self.data.name, 
      self.data.namespace,
      self.data.namespace[-1], 
      { 
        "CARDINALITY_2D": str(self.data.no_of_unknowns*self.data.dim[0]*self.data.dim[1]), 
        "CARDINALITY_3D": str(self.data.no_of_unknowns*self.data.dim[0]*self.data.dim[1]*self.data.dim[2]) 
      },
      True)
    output.add(generated_files)


