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

  
  def __get_default_includes(self):
    return """
#include "tarch/la/Vector.h" 

#include "peano4/utils/Globals.h"
#include "peano4/utils/Loop.h"

#include "SolverRepository.h"

#include "exahype2/PatchUtils.h"
#include "exahype2/fv/Rusanov.h"
"""


  def add_actions_to_create_grid(self, step):
    template = """
  {{ 
    int index = 0;
    dfor( volume, {NUMBER_OF_VOLUMES_PER_AXIS} ) {{
      {SOLVER_INSTANCE}.adjustSolution(
        fineGridCell{UNKNOWN_IDENTIFIER}.value + index,
        ::exahype2::getVolumeCentre( marker.x(), marker.h(), {NUMBER_OF_VOLUMES_PER_AXIS}, volume), 
        ::exahype2::getVolumeSize( marker.h(), {NUMBER_OF_VOLUMES_PER_AXIS} ),
        0.0
      );
      index += {NUMBER_OF_UNKNOWNS};
    }}
  }} 
  
"""

    d = {}
    self.__init_dictionary_with_default_parameters(d)

    step.add_action_set( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(self._patch,self._patch_overlap) )
    step.add_action_set( peano4.toolbox.blockstructured.ApplyFunctorOnPatch(self._patch,template.format(**d),self.__get_default_includes()) )
    pass
  
  
  def add_actions_to_plot_solution(self, step):
    step.add_action_set( peano4.toolbox.blockstructured.PlotPatchesInPeanoBlockFormat("solution" + self._name,self._patch, self._unknown_identifier()) )
    step.add_action_set( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(self._patch,self._patch_overlap) )
    pass
  
  
  def add_actions_to_perform_time_step(self, step):
    template = """
  ::exahype2::fv::applyRusanovToPatch(
    [&](
      double                                       Q[],
      const tarch::la::Vector<Dimensions,double>&  faceCentre,
      const tarch::la::Vector<Dimensions,double>&  volumeH,
      const tarch::la::Vector<Dimensions,double>&  t,
      const tarch::la::Vector<Dimensions,double>&  dt,
      int                                          normal,
      double                                       F[]
    ) -> void {{
      {SOLVER_INSTANCE}.flux( Q, faceCentre, volumeH, t, normal, F );
    }},
    [&](
      double                                       Q[],
      const tarch::la::Vector<Dimensions,double>&  faceCentre,
      const tarch::la::Vector<Dimensions,double>&  volumeH,
      const tarch::la::Vector<Dimensions,double>&  t,
      const tarch::la::Vector<Dimensions,double>&  dt,
      int                                          normal,
      double                                       lambdas[]
    ) -> void {{
      {SOLVER_INSTANCE}.eigenvalues( Q, faceCentre, volumeH, t, normal, lambdas );
    }},
    marker.x(),
    marker.h(),
    0.1, // t
    0.001, // dt
    {NUMBER_OF_VOLUMES_PER_AXIS},
    {HALO_SIZE},
    {NUMBER_OF_UNKNOWNS},
    reconstructedPatch,
    originalPatch
  );
"""

    d = {}
    self.__init_dictionary_with_default_parameters(d)

    step.add_action_set( peano4.toolbox.blockstructured.ReconstructPatchAndApplyFunctor(self._patch,self._patch_overlap,template.format(**d),self.__get_default_includes()) )
    step.add_action_set( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(self._patch,self._patch_overlap) )
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


  def __init_dictionary_with_default_parameters(self,d):
    d["NUMBER_OF_VOLUMES_PER_AXIS"] = self._patch.dim[0]
    d["HALO_SIZE"]                  = self._patch_overlap.dim[0]/2
    d["SOLVER_INSTANCE"]            = self.get_name_of_global_instance()
    d["UNKNOWN_IDENTIFIER"]         = self._unknown_identifier()
    d["NUMBER_OF_UNKNOWNS"]         = self._patch.no_of_unknowns
 
 
  #
  # Das muss hier raus! Gehoert in ein Action Set of itself
  #
  #
  def get_refinement_command(self):
    """
      Return an instance of ::exahype::RefinementCommand
      
      See get_initialisation_invocation for a description which variables you do have 
      in this block. Further to the arguments there, a variable refinementControl is 
      defined. Do not override this one, but update it via the and operator.

    """
    # @tood Das in eine exahype::geometry Toolbox raus anstelle in einen Funktor!
    
    template = """
  {{
    int index = 0;
    dfor( volume, {NUMBER_OF_VOLUMES_PER_AXIS} ) {{
      refinementCommand = refinementCommand and {SOLVER_INSTANCE}.refinementCriterion(
        fineGridCell{UNKNOWN_IDENTIFIER}.value + index,
        marker.x(),
        marker.h(),
        0.0 // @todo raus im AMR Kontext bzw von aussen kalibrieren
        // Solver muss im namen FixedTimeStep haben und dann nehmen wir 
        // den TimeStamp direkt aus dem Solver (und der muss natuerlich 
        // hochzaehlen. Alternativ koennen wir auch reduzieren. Waere 
        // noch schoener.
      );
      index += {NUMBER_OF_UNKNOWNS};
    }}
  }} 
  
"""

    d = {}
    self.__init_dictionary_with_default_parameters(d)

    return template.format(**d)


