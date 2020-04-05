# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4
import peano4.datamodel
import peano4.output.TemplatedHeaderFile
import peano4.output.TemplatedHeaderImplementationFilePair

from enum import Enum



class FiniteVolumeSolverType(Enum):
  Rusanov = 0
  


class FiniteVolumeSolver():
  """ 
  Represents on Peano 4 project.
  
  namespace Sequence of strings representing the (nested) namespace. Pass in 
    ["examples", "exahype2", "finitevolumes"] for example.
    
  """
  def __init__(self, name, patch_size, unknowns, solver_type = FiniteVolumeSolverType.Rusanov):
    self._name  = name
    self._patch = peano4.datamodel.Patch( (patch_size,patch_size,patch_size), unknowns, self._unknown_identifier() )
    if solver_type==FiniteVolumeSolverType.Rusanov:
      self._patch_overlap = peano4.datamodel.Patch( (2,patch_size,patch_size), unknowns, self._unknown_identifier() )
    else:
      print( "Error: unknown FV solver type" )
    pass
  
  
  def _unknown_identifier(self):
    return self._name+"Q"
  

  def get_name_of_global_instance(self):
    return "InstanceOf" + self._name

  
  def add_to_Peano4_datamodel( self, datamodel ):
    """
    
      Add all required data to the Peano4 project's datamodel 
      so it is properly built up
      
    """
    datamodel.add_cell(self._patch)
    datamodel.add_face(self._patch_overlap)
 
 
  def add_data_movements_to_Peano4_solver_step(self, step):
    """
     
      Inform Peano4 step which data are to be moved around
    
    """
    step.use_cell(self._patch)
    step.use_face(self._patch_overlap)


  def add_actions_to_create_grid(self, step):
    pass
  
  
  def add_actions_to_plot_solution(self, step):
    step.add_action_set( peano4.toolbox.blockstructured.PlotPatchesInPeanoBlockFormat("solution" + self._name,self._patch, self._unknown_identifier()) )
    pass
  
  
  def add_actions_to_perform_time_step(self, step):
    pass
  
  
  def add_implementation_files_to_project(self,namespace,output):
    """
    
      makefile is an instance of peano4.output.Makefile, so you can 
      add your files to this instance.
      
    """

    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
    
    abstractHeaderDictionary = {
      "NUMBER_OF_UNKNOWNS" : self._patch.no_of_unknowns
    }
    implementationDictionary = {
      "NUMBER_OF_UNKNOWNS" : self._patch.no_of_unknowns
    }
        
    generated_abstract_header_file = peano4.output.TemplatedHeaderFile(
      templatefile_prefix + "AbstractRusanov.template.h",
      "Abstract" + self._name, 
      namespace,
      ".", 
      abstractHeaderDictionary,
      True)
    generated_solver_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix + "Rusanov.template.h",
      templatefile_prefix + "Rusanov.template.cpp",
      self._name, 
      namespace,
      ".", 
      implementationDictionary,
      False)

    output.add( generated_abstract_header_file )
    output.add( generated_solver_files )
    output.makefile.add_cpp_file( self._name + ".cpp" )

