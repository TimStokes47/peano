# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4

from .FiniteVolumeSolver import FiniteVolumeSolver
from .FiniteVolumeSolver import FiniteVolumeSolverType



class Project(object):
  """
   
  Represents an ExaHyPE2 project. An ExaHyPE2 project is a Peano4 
  project with a particular set of actions (algorithmic phases) 
  that you can choose from and with particular solver types. It 
  realises a builder mechanism, i.e. you build up your ExaHyPE2
  project and then you finally tell the project "give me the Peano4
  project". From hereon, you can use this Peano4 project to actually
  set up the Peano4 application.
  
  @see generate_Peano4_project()
  
  """
  def __init__(self, namespace, project_name, directory = "."):
    self._project = peano4.Project(namespace, project_name, directory)
    self._solvers = []
    
    
  def add_finite_volumes_solver(self,name, patch_size, unknowns, solver_type = FiniteVolumeSolverType.Rusanov):
    self._solvers.append( FiniteVolumeSolver( name, patch_size, unknowns, solver_type) )
    
  def generate_Peano4_project(self):
    return self._project

    