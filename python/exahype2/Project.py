# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4
import peano4.output.TemplatedHeaderImplementationFilePair


import os

import exahype2.grid
import exahype2.solvers
#from .FiniteVolumeSolver import FiniteVolumeSolver
#from .FiniteVolumeSolver import FiniteVolumeSolverType



class Project(object):
  """
   
  Represents an ExaHyPE2 project. An ExaHyPE2 project is a Peano4 
  project with a particular set of actions (algorithmic phases) 
  that you can choose from and with particular solver types. It 
  realises a builder mechanism, i.e. you build up your ExaHyPE2
  project and then you finally tell the project "give me the Peano4
  project". From hereon, you can use this Peano4 project to actually
  set up the Peano4 application.
  
  The project will have a marker per cell that encodes stuff alike
  a boundary marker. But it is also used to coordinate different 
  solver types.
  
  @see generate_Peano4_project()
  
  """
  def __init__(self, namespace, project_name, directory = "."):
    self._project = peano4.Project(namespace, project_name, directory)
    self._solvers = []
    self._domain_offset = [0.0, 0.0, 0.0]
    self._domain_size   = [1.0, 1.0, 1.0]
    self._dimensions    = 2
    self._terminal_time = 1.0
    self._first_plot_time_stamp = 0.0
    self._time_in_between_plots = 0.1

    
    
  def add_finite_volumes_solver(self,name, patch_size, unknowns, solver_type = exahype2.solvers.FiniteVolumeSolverType.Rusanov):
    self._solvers.append( exahype2.solvers.FiniteVolumeSolver( name, patch_size, unknowns, solver_type) )


  def __export_constants(self):
    offset_string = "{" + str(self._domain_offset[0])
    size_string   = "{" + str(self._domain_size[0])
    for i in range(1,self._dimensions):
      offset_string += ","
      size_string   += ","
      offset_string += str(self._domain_offset[i])
      size_string   += str(self._domain_size[i])
    offset_string += "}"
    size_string   += "}"
    self._project.constants.export( "DomainOffset", offset_string )
    self._project.constants.export( "DomainSize", size_string )
    self._project.constants.export( "TerminalTime", str(self._terminal_time) )
    self._project.constants.export( "FirstPlotTimeStamp", str(self._first_plot_time_stamp) )
    self._project.constants.export( "TimeInBetweenPlots", str(self._time_in_between_plots) )


  def __configure_makefile(self):
    self._project.output.makefile.set_dimension(self._dimensions)
    

  def set_global_simulation_parameters(self,dimensions,offset,size,terminal_time,first_plot_time_stamp,time_in_between_plots):
    """
    
      offset and size should be lists with dimensions double entries.
      
      time_in_between_plots Set to zero if you don't want to have any plots
      
    """
    self._domain_offset = offset
    self._domain_size   = size
    self._dimensions    = dimensions
    self._terminal_time = terminal_time
    self._first_plot_time_stamp = first_plot_time_stamp
    self._time_in_between_plots = time_in_between_plots
    
    
  def __generate_solver_repository(self):
    solverRepositoryDictionary = {
      "SOLVER_INCLUDES" : "",
      "SOLVER_DECLARATIONS" : "",
      "SOLVER_DEFINITIONS" : "",
      "SEQUENCE_OF_GET_MAX_TIME_STEP_SIZE_CALLS": "0.0",
      "SEQUENCE_OF_GET_MIN_TIME_STEP_SIZE_CALLS": "std::numeric_limits<double>::max()",
      "SEQUENCE_OF_GET_MAX_TIME_STAMP_CALLS": "0.0",
      "SEQUENCE_OF_GET_MIN_TIME_STAMP_CALLS": "std::numeric_limits<double>::max()",
      "SEQUENCE_OF_START_TIME_STEP_CALLS": "",
      "SEQUENCE_OF_FINISH_TIME_STEP_CALLS": "",
    }

    for solver in self._solvers:
      solverRepositoryDictionary[ "SOLVER_INCLUDES" ]     += """#include "../""" + solver._name + """.h" \n"""
      solverRepositoryDictionary[ "SOLVER_DECLARATIONS" ] += "  extern " + solver._name + "  " + solver.get_name_of_global_instance() + ";\n"
      solverRepositoryDictionary[ "SOLVER_DEFINITIONS" ]  += solver._name + "  " + solver.get_name_of_global_instance() + ";\n"
      solverRepositoryDictionary[ "SEQUENCE_OF_GET_MAX_TIME_STEP_SIZE_CALLS" ]  += "," + solver.get_name_of_global_instance() + ".getMaxTimeStepSize()"
      solverRepositoryDictionary[ "SEQUENCE_OF_GET_MIN_TIME_STEP_SIZE_CALLS" ]  += "," + solver.get_name_of_global_instance() + ".getMinTimeStepSize()"
      solverRepositoryDictionary[ "SEQUENCE_OF_GET_MAX_TIME_STAMP_CALLS" ]      += "," + solver.get_name_of_global_instance() + ".getMaxTimeStamp()"
      solverRepositoryDictionary[ "SEQUENCE_OF_GET_MIN_TIME_STAMP_CALLS" ]      += "," + solver.get_name_of_global_instance() + ".getMinTimeStamp()"
      solverRepositoryDictionary[ "SEQUENCE_OF_START_TIME_STEP_CALLS" ]         += solver.get_name_of_global_instance() + ".startTimeStep(minTimeStamp, maxTimeStamp, minTimeStepSize, maxTimeStepSize); "
      solverRepositoryDictionary[ "SEQUENCE_OF_FINISH_TIME_STEP_CALLS" ]        += solver.get_name_of_global_instance() + ".finishTimeStep(); "


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
    self.__export_constants()
    self.__configure_makefile()
    
    create_grid       = peano4.solversteps.Step( "CreateGrid", False )
    plot_solution     = peano4.solversteps.Step( "PlotSolution", False )
    perform_time_step = peano4.solversteps.Step( "TimeStep", False )
    
    self._project.solversteps.add_step(create_grid)
    self._project.solversteps.add_step(plot_solution)
    self._project.solversteps.add_step(perform_time_step)

    
    for solver in self._solvers:
      solver.add_to_Peano4_datamodel( self._project.datamodel )
      
      solver.add_use_data_statements_to_Peano4_solver_step( create_grid )
      solver.add_use_data_statements_to_Peano4_solver_step( plot_solution )
      solver.add_use_data_statements_to_Peano4_solver_step( perform_time_step )
      
      solver.add_actions_to_create_grid( create_grid )
      solver.add_actions_to_plot_solution( plot_solution )
      solver.add_actions_to_perform_time_step( perform_time_step )
      
      solver.add_implementation_files_to_project( self._project.namespace, self._project.output )

    self.__generate_solver_repository();

    face_label = exahype2.grid.create_face_label()  
    self._project.datamodel.add_face(face_label)
    create_grid.use_face(face_label)
    plot_solution.use_face(face_label)
    perform_time_step.use_face(face_label)
    
    set_labels_action_set = exahype2.grid.SetLabels()
    create_grid.add_action_set( set_labels_action_set )
    plot_solution.add_action_set( set_labels_action_set )
    perform_time_step.add_action_set( set_labels_action_set )
    
    self._project.main = exahype2.ExaHyPEMain(self._project)

    return self._project

    