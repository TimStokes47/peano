# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4
import peano4.datamodel
import peano4.output.TemplatedHeaderFile
import peano4.output.TemplatedHeaderImplementationFilePair

import exahype2.grid.AMROnPatch

from enum import Enum



class FiniteVolumeSolverType(Enum):
  Rusanov = 0
  


class FiniteVolumeSolver():
  """ 
  A finite volume solver with fixed time step sizes that works on patch-based
  AMR with a halo layer of one.
  
  namespace Sequence of strings representing the (nested) namespace. Pass in 
    ["examples", "exahype2", "finitevolumes"] for example.
    
  """
  def __init__(self, name, patch_size, unknowns, time_step_size, solver_type = FiniteVolumeSolverType.Rusanov):
    self._time_step_size = time_step_size
    self._name  = name
    self._patch = peano4.datamodel.Patch( (patch_size,patch_size,patch_size), unknowns, self._unknown_identifier() )
    if solver_type==FiniteVolumeSolverType.Rusanov:
      self._patch_overlap     = peano4.datamodel.Patch( (2,patch_size,patch_size), unknowns, self._unknown_identifier() )
      self._patch_overlap_new = peano4.datamodel.Patch( (2,patch_size,patch_size), unknowns, self._unknown_identifier() + "Old" )
    else:
      print( "Error: unknown FV solver type" )
    self._patch_overlap.generator.set_merge_method_definition( peano4.toolbox.blockstructured.get_face_overlap_merge_implementation(self._patch_overlap) )
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
    datamodel.add_face(self._patch_overlap_new)
 
 
  def add_use_data_statements_to_Peano4_solver_step(self, step):
    """
     
      Inform Peano4 step which data are to be moved around via the 
      use_cell and use_face commands.
    
    """
    step.use_cell(self._patch)
    step.use_face(self._patch_overlap)
    step.use_face(self._patch_overlap_new)

  
  def __get_default_includes(self):
    return """
#include "tarch/la/Vector.h" 

#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"

#include "SolverRepository.h"

#include "exahype2/PatchUtils.h"
#include "exahype2/fv/Rusanov.h"
#include "exahype2/fv/BoundaryConditions.h"
"""


  _CreateCellTemplate = """
  {{ 
    int index = 0;
    dfor( volume, {NUMBER_OF_VOLUMES_PER_AXIS} ) {{
      {SOLVER_INSTANCE}.adjustSolution(
        fineGridCell{UNKNOWN_IDENTIFIER}.value + index,
        ::exahype2::getVolumeCentre( marker.x(), marker.h(), {NUMBER_OF_VOLUMES_PER_AXIS}, volume), 
        ::exahype2::getVolumeSize( marker.h(), {NUMBER_OF_VOLUMES_PER_AXIS} ),
        {SOLVER_INSTANCE}.getMinTimeStamp()
      );
      index += {NUMBER_OF_UNKNOWNS};
    }}
  }} 
  
"""

  _AMRTemplate = """
  {{ 
    ::exahype2::RefinementCommand refinementCriterion = ::exahype2::getDefaultRefinementCommand();
    int index = 0;
    dfor( volume, {NUMBER_OF_VOLUMES_PER_AXIS} ) {{
      refinementCriterion = refinementCriterion and {SOLVER_INSTANCE}.refinementCriterion(
        fineGridCell{UNKNOWN_IDENTIFIER}.value + index,
        ::exahype2::getVolumeCentre( marker.x(), marker.h(), {NUMBER_OF_VOLUMES_PER_AXIS}, volume), 
        ::exahype2::getVolumeSize( marker.h(), {NUMBER_OF_VOLUMES_PER_AXIS} ),
        {SOLVER_INSTANCE}.getMinTimeStamp()
      );
      index += {NUMBER_OF_UNKNOWNS};
    }}
    _refinementControl.addCommand( marker.x(), marker.h(), refinementCriterion, {IS_GRID_CREATION} );
  }} 
  
"""

  def add_actions_to_create_grid(self, step):
    d = {}
    self.__init_dictionary_with_default_parameters(d)
    d["IS_GRID_CREATION"] = "true"
    
    step.add_action_set( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(self._patch,self._patch_overlap) )
    step.add_action_set( peano4.toolbox.blockstructured.ApplyFunctorOnPatch(self._patch,self._CreateCellTemplate.format(**d),self.__get_default_includes()) )
    step.add_action_set( exahype2.grid.AMROnPatch(self._patch,self._AMRTemplate.format(**d),  self.__get_default_includes()) )
    pass
  
  
  def add_actions_to_plot_solution(self, step):
    d = {}
    self.__init_dictionary_with_default_parameters(d)
    
    step.add_action_set( peano4.toolbox.blockstructured.PlotPatchesInPeanoBlockFormat("solution" + self._name,self._patch, self._unknown_identifier()) )
    step.add_action_set( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(self._patch,self._patch_overlap) )
    step.add_action_set( peano4.toolbox.blockstructured.ApplyFunctorOnPatch(self._patch,self._CreateCellTemplate.format(**d),self.__get_default_includes()) )
    pass
  
  
  def add_actions_to_perform_time_step(self, step):
    touchFaceFirstTimeTemplate = """
  if (fineGridFaceLabel.getBoundary()) {{
    ::exahype2::fv::applyBoundaryConditions(
      [&](
        double                                       Qinside[],
        double                                       Qoutside[],
        const tarch::la::Vector<Dimensions,double>&  faceCentre,
        const tarch::la::Vector<Dimensions,double>&  volumeH,
        double                                       t,
        double                                       dt,
        int                                          normal
      ) -> void {{
        {SOLVER_INSTANCE}.boundaryConditions( Qinside, Qoutside, faceCentre, volumeH, t, normal );
      }},
      marker.x(),
      marker.h(),
      {SOLVER_INSTANCE}.getMinTimeStamp(),
      {TIME_STEP_SIZE},
      {NUMBER_OF_VOLUMES_PER_AXIS},
      {NUMBER_OF_UNKNOWNS},
      marker.getSelectedFaceNumber(),
      fineGridFace{UNKNOWN_IDENTIFIER}.value
    );
  }}
"""
    
    
    patchFunctorTemplate = """
  //::exahype2::fv::applyRusanovToPatch(
  ::exahype2::fv::applyRusanovToPatch_FaceLoops2d(
    [&](
      double                                       Q[],
      const tarch::la::Vector<Dimensions,double>&  faceCentre,
      const tarch::la::Vector<Dimensions,double>&  volumeH,
      double                                       t,
      double                                       dt,
      int                                          normal,
      double                                       F[]
    ) -> void {{
      {SOLVER_INSTANCE}.flux( Q, faceCentre, volumeH, t, normal, F );
    }},
    [&](
      double                                       Q[],
      const tarch::la::Vector<Dimensions,double>&  faceCentre,
      const tarch::la::Vector<Dimensions,double>&  volumeH,
      double                                       t,
      double                                       dt,
      int                                          normal,
      double                                       lambdas[]
    ) -> void {{
      {SOLVER_INSTANCE}.eigenvalues( Q, faceCentre, volumeH, t, normal, lambdas );
    }},
    marker.x(),
    marker.h(),
    {SOLVER_INSTANCE}.getMinTimeStamp(),
    {TIME_STEP_SIZE}, 
    {NUMBER_OF_VOLUMES_PER_AXIS},
    {NUMBER_OF_UNKNOWNS},
    reconstructedPatch,
    originalPatch
  );
"""

    d = {}
    self.__init_dictionary_with_default_parameters(d)
    d["IS_GRID_CREATION"] = "false"

    step.add_action_set( peano4.toolbox.blockstructured.ReconstructPatchAndApplyFunctor(
      self._patch,
      self._patch_overlap,
      patchFunctorTemplate.format(**d),
      touchFaceFirstTimeTemplate.format(**d),
      self.__get_default_includes()
    ))
    step.add_action_set( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(self._patch,self._patch_overlap_new) )
    step.add_action_set( peano4.toolbox.blockstructured.ApplyFunctorOnPatch(self._patch,self._CreateCellTemplate.format(**d),self.__get_default_includes()) )
    step.add_action_set( exahype2.grid.AMROnPatch(self._patch,self._AMRTemplate.format(**d),  self.__get_default_includes()) )
    step.add_action_set( peano4.toolbox.blockstructured.BackupPatchOverlap(
      self._patch_overlap_new,
      self._patch_overlap,
      False
    ))
    pass
  

  
  def add_implementation_files_to_project(self,namespace,output):
    """
    
      makefile is an instance of peano4.output.Makefile, so you can 
      add your files to this instance.
      
    """

    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
    
    abstractHeaderDictionary = {
      "NUMBER_OF_UNKNOWNS" : self._patch.no_of_unknowns,
      "TIME_STEP_SIZE":      self._time_step_size
    }
    implementationDictionary = {
      "NUMBER_OF_UNKNOWNS" : self._patch.no_of_unknowns
    }
        
    generated_abstract_header_file = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix + "AbstractRusanov.template.h",
      templatefile_prefix + "AbstractRusanov.template.cpp",
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
    output.makefile.add_cpp_file( "Abstract" + self._name + ".cpp" )
    output.makefile.add_cpp_file( self._name + ".cpp" )


  def __init_dictionary_with_default_parameters(self,d):
    d["TIME_STEP_SIZE"]             = self._time_step_size
    d["NUMBER_OF_VOLUMES_PER_AXIS"] = self._patch.dim[0]
    d["HALO_SIZE"]                  = self._patch_overlap.dim[0]/2
    d["SOLVER_INSTANCE"]            = self.get_name_of_global_instance()
    d["UNKNOWN_IDENTIFIER"]         = self._unknown_identifier()
    d["NUMBER_OF_UNKNOWNS"]         = self._patch.no_of_unknowns
    if self._patch_overlap.dim[0]/2!=1:
      print( "ERROR: Finite Volume solver currently supports only a halo size of 1")
 
