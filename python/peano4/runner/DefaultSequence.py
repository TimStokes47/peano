# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import os


class DefaultSequence:
  def __init__(self,project):
    self.project = project
    
    
  def construct_output(self,output):
    """
      Pass in a version of output
    """
    output.makefile.add_cpp_file( "main.cpp" )
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )    
    generated_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix+".h.template",
      templatefile_prefix+".cpp.template",
      "main", 
      self.project.namespace,
      ".", 
      {},
      False)
    output.add(generated_files)
