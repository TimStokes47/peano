"""

 A very simple example which demonstrates how to configure a patch-based
 Finite Volume solver in Peano4. The code relies on snippets from ExaHyPE2.
 However, it relies only on ExaHyPE's C/FORTRAN compute kernels, i.e. the 
 sophisticated build environment of this H2020 project is not used. The 
 solver simulates the simple Euler equations.

"""


#
# We import Peano4 as project. If this step fails, ensure that your environment
# variable PYTHONPATH points to Peano4's python directory.
#
import os
import peano4
import peano4.datamodel
import peano4.solversteps
import peano4.output
import peano4.visualisation
import peano4.toolbox.blockstructured


import exahype2


#
# Lets first clean up all plot files, so we don't get a mismatch
#
output_files = [ f for f in os.listdir(".") if f.endswith(".peano-patch-file") or f.endswith(".vtu") ]
for f in output_files:
  os.remove(f)


#
# Create a project and configure it to end up in a subnamespace (and thus
# subdirectory). 
#
project = exahype2.Project( ["examples", "exahype2", "euler"], "finitevolumes", "." )


#
# Add the Finite Volumes solver
#
patch_size     = 25
unknowns       = 5
time_step_size = 0.0001
project.add_solver(  exahype2.solvers.GenericRusanovFVFixedTimeStepSize("Euler", patch_size, unknowns, time_step_size) )


dimensions = 2
#build_mode = peano4.output.CompileMode.Release
build_mode = peano4.output.CompileMode.Asserts


#
# Lets configure some global parameters
#
if dimensions==2:
  project.set_global_simulation_parameters(
    dimensions,  [0.0,0.0],  [1.0,1.0],
    0.1,           # end time
    0.0, 0.02      # snapshots
  )
else:
  project.set_global_simulation_parameters(
    dimensions, [0.0,0.0,0.0], [1.0,1.0,1.0],
    0.1,           # end time
    0.0, 0.02      # snapshots
  )


#
# So here's the parallel stuff. This is new compared to the serial
# prototype we did start off with.
#
project.set_load_balancing( "toolbox::loadbalancing::RecursiveSubdivision" )



peano4_project = project.generate_Peano4_project()
peano4_project.output.makefile.parse_configure_script_outcome( "../../../.." )
peano4_project.output.makefile.add_library( project.get_core_library(build_mode), "../../../../src/exahype2" )
peano4_project.output.makefile.add_library( "ToolboxLoadBalancing" + project.get_library_postfix(build_mode), "../../../../src/toolbox/loadbalancing" )
peano4_project.output.makefile.set_mode(build_mode)
peano4_project.build()
#
# Use this variant if you build without MPI
#
#success = peano4_project.run( [] )
success = peano4_project.run( [], ["mpirun", "-n", "1"] )



if success:
  #
  # Command line version of these steps:
  #
  # ../../../../src/visualisation/convert apply-filter solutionEuler.peano-patch-file EulerQ . extract-fine-grid finegrid
  # ../../../../src/visualisation/convert convert-file solutionEuler.peano-patch-file finegrid . vtu
  #
  convert = peano4.visualisation.Convert( "solutionEuler", True )
  convert.set_visualisation_tools_path( "../../../../src/visualisation" )
  convert.plot_domain_decomposition()
  convert.extract_fine_grid()
  convert.convert_to_vtk()

