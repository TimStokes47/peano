# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4
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
    #    self.patch = peano4.datamodel.Patch( (patch_size,patch_size,patch_size), unknowns, "Q" )
    #
    #self.datamodel.add_cell(self.patch)
    pass
