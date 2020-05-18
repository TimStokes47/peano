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
  for (int i=0; i<2*Dimensions; i++) {{
    {SOLVER_INSTANCE}.projectOntoFace(
      cellData, faceData[i], marker.x(), marker.h(), i
    );
  }}
"""

  SolveRiemannTemplate = """
  {SOLVER_INSTANCE}.solveRiemannProblem(
    faceData, marker.x(), marker.h(), marker.normal(), {SOLVER_INSTANCE}.getMinTimeStamp()
  );
"""  

  SolveCellTemplate = """
  {SOLVER_INSTANCE}.solveCell(
    cellData, marker.x(), marker.h(), {SOLVER_INSTANCE}.getMinTimeStamp()
  );  
"""

  ProjectOntoCellTemplate = """
  for (int i=0; i<2*Dimensions; i++) {{
    {SOLVER_INSTANCE}.projectOntoCell(
      cellData, faceData[i], marker.x(), marker.h(), i
    );
  }}
"""  
     

  def add_entries_to_text_replacement_dictionary(self,d):
    d[ "TIME_STEP_SIZE" ] = self._time_step_size
    pass  
