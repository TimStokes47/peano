# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import os


class DefaultSequence:
  def __init__(self,project):
    self.project = project
    
    
  def construct_output(self,output,main_name):
    """
      Pass in a version of output
      
      main_name  Is the name of you main file. By default, this might be 
         main.cpp, but you might want to have different main files for 
         different experiments. Please do not give the file an extension.
    """
    output.makefile.add_cpp_file( main_name + ".cpp" )
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )    
    generated_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix+".h.template",
      templatefile_prefix+".cpp.template",
      main_name, 
      self.project.namespace,
      ".", 
      {},
      False)
    output.add(generated_files)
