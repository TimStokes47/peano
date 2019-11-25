# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
class Patch(object):
  def __init__(self, dim):
    """
      dim should be a two-tuple or a three-tuple of integers, so a construction 
      could look like
      
        cell_data = peano4.datamodel.Patch( (6,6,6) )

    """
    self.dim = dim
    self.generator = PatchToDoubleArray(self)


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
    filename = "mytest"
    output.makefile.add_cpp_file( filename + ".cpp" )


