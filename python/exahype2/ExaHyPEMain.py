# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4.runner.DefaultSequence
import os


class ExaHyPEMain(peano4.runner.DefaultSequence):
  def __init__(self,project):  
    super(ExaHyPEMain,self).__init__(project)
    self.overwrite = False  # Still not sure I should overwrite stuff

  def _get_header_file_template(self):
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )    
    return templatefile_prefix+".h.template"
    
  def _get_implementation_file_template(self):
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )    
    return templatefile_prefix+".cpp.template"

