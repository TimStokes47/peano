# This file is part of the ExaHyPE project and follow-up activities. 
# For conditions of distribution and use, please see the copyright 
# notice at www.peano-framework.org
import peano4
from enum import Enum



class CompileMode(Enum):
  Debug = 0
  Trace = 1
  Asserts = 2
  Release = 3
  


class FiniteVolumeSolver(peano4.Project):
  """ 
  Represents on Peano 4 project.
  
  namespace Sequence of strings representing the (nested) namespace. Pass in 
    ["examples", "exahype2", "finitevolumes"] for example.
    
  """
  def __init__(self, namespace, project_name, patch_size, solver_type, directory = "."):
    super( FiniteVolumeSolver, self ).__init__(namespace, project_name, directory)

