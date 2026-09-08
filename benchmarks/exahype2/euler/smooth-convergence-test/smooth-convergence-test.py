import os, sys
import peano4.api
import exahype2

#import exahype2.fv.api.Solver as MySolver

project = exahype2.api.Project(namespace=["benchmarks", "exahype2", "euler", "smooth_convergence_test"], project_name="smooth-convergence-test", directory=".", executable="smooth-convergence-test")
#new_project = exahype2.api.Project(namespace=["benchmarks", "exahype2", "euler", "smooth_setup"], project_name="smooth-convergence-test", directory=".", executable="smooth-convergence-test")

solver_name = "EulerFVSmooth"
unknowns    = 4
auxiliary_variables = 0
mesh_size   = 0.2 / 9.0
patch_size  = 9
dimensions  = 2
verbosity   = peano4.api.LoggerVerbosity.VISUALISATION

thesolver = exahype2.api.Solver(
  solver_name,
  unknowns, auxiliary_variables,
)


thesolver.add_pde_term(
      exahype2.api.Flux(
          )
    )
thesolver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(thesolver)
thesolver.grid_construction = exahype2.api.fv.SetupSimulation(thesolver)
thesolver.plotter = exahype2.api.fv.PlotAsVTKFile(thesolver)

#  patch_size,
#  mesh_size,
#  mesh_size,
#  normalised_time_step_size=0.0001,
#  flux = exahype2.solvers.PDETerms.User_Defined_Implementation,
#  eigenvalues = exahype2.solvers.PDETerms.User_Defined_Implementation,



#oldsolver = exahype2.solvers.fv.musclhancock.GlobalFixedTimeStep(
#  solver_name,
#  patch_size,
#  unknowns, auxiliary_variables,
#  mesh_size,
#  mesh_size,
#  normalised_time_step_size=0.0001,
#  flux = exahype2.solvers.PDETerms.User_Defined_Implementation,
#  eigenvalues = exahype2.solvers.PDETerms.User_Defined_Implementation,
#)

project.add_solver(thesolver)

build_mode = peano4.api.output.CompileMode.Release
project.set_global_simulation_parameters(
  dimensions = dimensions,
  offset = [-0.1,-0.1],
  size = [0.2, 0.2],
  min_end_time = 0.01,
  max_end_time = 0.01,
  first_plot_time_stamp = 0.0,
  time_in_between_plots = 0.01,
  periodic_BC           = [False, False, False]
)

#project.set_load_balancing("toolbox::loadbalancing::strategies::RecursiveBipartition", "new ::exahype2::LoadBalancingConfiguration()")
project.set_Peano4_installation("../../../../", build_mode)
peano4_project = project.generate_Peano4_project(verbosity=verbosity)
#peano4_project.output.makefile.add_h_file("EulerFVSmooth.h")
#peano4_project.output.makefile.add_cpp_file("EulerFVSmooth.cpp")
peano4_project.generate()
#peano4_project.build(make_clean_first=True)
