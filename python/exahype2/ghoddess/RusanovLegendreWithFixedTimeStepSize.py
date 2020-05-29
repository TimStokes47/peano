# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import os

import peano4
import peano4.datamodel
import peano4.output.TemplatedHeaderFile
import peano4.output.TemplatedHeaderImplementationFilePair


import exahype2.grid.AMROnPatch


from .Solver import Solver


class RusanovLegendreWithFixedTimeStepSize(Solver):
  """ 
  
  
   
  """
  def __init__(self,name, order, unknowns, dimensions, time_step_size):
    super( RusanovLegendreWithFixedTimeStepSize, self ).__init__(name, order, unknowns, dimensions)
    self._time_step_size = time_step_size

  #
  # We have a couple of predefined macros here. See Solver superclass.
  #
  AdjustCellTemplate = """
  {SOLVER_INSTANCE}.adjustSolution(
    cellData, marker.x(), marker.h(), {SOLVER_INSTANCE}.getMinTimeStamp()
  );  
"""


  #AMRTemplate = """
  #  {SOLVER_INSTANCE}.refinementCriterion(
  #    cellData,
  #    marker.x(), 
  #    marker.h(), 
  #    {SOLVER_INSTANCE}.getMinTimeStamp()
  #  )
  #);
  #"""

  BoundaryTemplate = """
  {SOLVER_INSTANCE}.boundaryConditions(
    faceData, marker.x(), marker.h(), {SOLVER_INSTANCE}.getMinTimeStamp(), marker.outerNormal()
  );
"""

  ProjectOntoFaceTemplate = """
  // face 0 is the left face (see guidebook)
  // entries are enumerated in z-order
  faceData[0][1] = cellData[3];  // let the 3 be the left bottom unknown in upper triangle
  faceData[0][3] = cellData[4];  // let the 4 the left top unknown in upper triangle
  // face 1 is the bottom face (they are enumerated along their normals)
  faceData[1][2] = cellData[0];  // bottom left unknown within lower triange
  faceData[1][3] = cellData[1];
  // and then I leave it to you. If you wanna have the ownership of the enumeration you have
  // to implement this yourself ;-p
"""


  SolveCellTemplate = """
  {SOLVER_INSTANCE}.solveCell(
    cellData, 
    faceData[0], faceData[1], faceData[2], faceData[3],
    marker.x(), marker.h(), {SOLVER_INSTANCE}.getMinTimeStamp()
  );  
"""
     

  def add_entries_to_text_replacement_dictionary(self,d):
    d[ "TIME_STEP_SIZE" ] = self._time_step_size
    pass  
