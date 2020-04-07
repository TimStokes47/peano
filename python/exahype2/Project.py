# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4
import peano4.output.TemplatedHeaderImplementationFilePair

from .CreateGrid         import CreateGrid
from .TimeStep           import TimeStep

from .FiniteVolumeSolver import FiniteVolumeSolver
from .FiniteVolumeSolver import FiniteVolumeSolverType

import os



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

  def __generate_solver_repository(self):
    solverRepositoryDictionary = {
      "SOLVER_INCLUDES" : "",
      "SOLVER_DECLARATIONS" : "",
      "SOLVER_DEFINITIONS" : ""
    }

    for solver in self._solvers:
      solverRepositoryDictionary[ "SOLVER_INCLUDES" ]     += """#include "../""" + solver._name + """.h" \n"""
      solverRepositoryDictionary[ "SOLVER_DECLARATIONS" ] += "  extern " + solver._name + "  " + solver.get_name_of_global_instance() + ";\n"
      solverRepositoryDictionary[ "SOLVER_DEFINITIONS" ]  += solver._name + "  " + solver.get_name_of_global_instance() + ";\n"


    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
    generated_solver_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix + "SolverRepository.template.h",
      templatefile_prefix + "SolverRepository.template.cpp",
      "SolverRepository", 
      self._project.namespace + ["observers"],
      "observers", 
      solverRepositoryDictionary,
      True)

    self._project.output.add( generated_solver_files )
    self._project.output.makefile.add_cpp_file( "observers/SolverRepository.cpp" )
    
    
  def generate_Peano4_project(self):
    """
    
     Build the Peano4 project, i.e. all the action sets et al that you require
     to run this ExaHyPE2 application. 
     
     
     You will have to add the ExaHyPE2 library later on manually. I would like 
     to do this here, but most codes first want to parse the config file before
     they add the library. And, obviously, you still have to make the choice 
     which variant of the library (release, trace, debug, ...) you want to use.

     So most codes invoke something alike
     
     
     on the result of this routine.     
     
    """
    create_grid       = peano4.solversteps.Step( "CreateGrid", False )
    plot_solution     = peano4.solversteps.Step( "PlotSolution", False )
    perform_time_step = peano4.solversteps.Step( "TimeStep", False )
    
    self._project.solversteps.add_step(create_grid)
    self._project.solversteps.add_step(plot_solution)
    self._project.solversteps.add_step(perform_time_step)

    for solver in self._solvers:
      solver.add_to_Peano4_datamodel( self._project.datamodel )
      solver.add_data_movements_to_Peano4_solver_step( create_grid )
      solver.add_data_movements_to_Peano4_solver_step( plot_solution )
      solver.add_data_movements_to_Peano4_solver_step( perform_time_step )
      solver.add_actions_to_create_grid( create_grid )
      solver.add_actions_to_plot_solution( plot_solution )
      solver.add_actions_to_perform_time_step( perform_time_step )
      solver.add_implementation_files_to_project( self._project.namespace, self._project.output )

    self.__generate_solver_repository();

    return self._project

    