# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4
import peano4.datamodel
import peano4.output.TemplatedHeaderFile
import peano4.output.TemplatedHeaderImplementationFilePair

from .FV import FV


class GenericRusanovFVFixedTimeStepSize( FV ):
  def __init__(self, name, patch_size, unknowns, time_step_size, flux=True, ncp=False):
    """
      Instantiate a generic FV scheme with an overlap of 1.
    """
    super(GenericRusanovFVFixedTimeStepSize,self).__init__(name, patch_size, 1, unknowns)

    self._time_step_size              = time_step_size
    
    if flux and not ncp:
      self.HandleCellTemplate = self.HandleCellTemplate_Flux
    #elif not flux and ncp:
    #  self.HandleCellTemplate = self.HandleCellTemplate_NCP
    elif flux and ncp:
      self.HandleCellTemplate = self.HandleCellTemplate_Flux_NCP
    else:
      print( "ERROR: Combination of PDE terms not supported" )
      
      
    self._fv_callbacks = []
    if flux:
      self._fv_callbacks.append( "flux" )
    if ncp:
      self._fv_callbacks.append( "ncp" )
    

  def get_user_includes(self):
    return """
#include "exahype2/fv/Rusanov.h"
"""    


  HandleBoundaryTemplate = """ 
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
"""


  CreateCellTemplate = """
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


  AMRTemplate = """
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
    _localRefinementControl.addCommand( marker.x(), marker.h(), refinementCriterion, {IS_GRID_CREATION} );
  }} 
"""


  HandleCellTemplate_Flux = """::exahype2::fv::applyRusanovToPatch_FaceLoops(
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


  HandleCellTemplate_Flux_NCP = """::exahype2::fv::applyRusanovToPatch_FaceLoops(
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
      double Q[],
      double gradQ[][Dimensions],
      double BgradQ[]
    ) -> void {{
      {SOLVER_INSTANCE}.nonconservativeProduct( Q, gradQ, BgradQ );
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


  def add_entries_to_text_replacement_dictionary(self,d):
    d[ "TIME_STEP_SIZE" ]  = self._time_step_size
    d[ "CALLBACKS" ]       = self._fv_callbacks
