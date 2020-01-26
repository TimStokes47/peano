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


#
# Lets first clean up all plot files, so we don't get a mismatch
#
output_files = [ f for f in os.listdir(".") if f.endswith(".peano-patch-file") ]
for f in output_files:
  os.remove(f)


#
# Create a project and configure it to end up in a subnamespace (and thus
# subdirectory). 
#
project = peano4.Project( ["examples", "finitevolumes"], "." )


#
# The solver will be patch-based, i.e. we will have one patch per cell.
#
patch_size = 4
unknowns   = 5
patch = peano4.datamodel.Patch( (patch_size,patch_size,patch_size), unknowns, "Q" )
project.datamodel.add_cell(patch)


#
# Along the faces, we have the patch overlaps. As we use only a Rusanov flux, 
# one cell of overlap between adjacent patches is sufficient.
#
patch_overlap = peano4.datamodel.Patch( (2,patch_size,patch_size), unknowns, "Q" )
project.datamodel.add_face(patch_overlap)


#
# For each step that we wanna do, we define one solver step. In the first step that
# we use, we add one grid level to the mesh per step. We also initialise the blocks
# with the initial values.
#
create_grid = peano4.solversteps.Step( "CreateGrid" )
create_grid.use_face(patch_overlap)
create_grid.use_cell(patch)
create_grid.add_mapping( peano4.solversteps.PlotGridInPeanoBlockFormat("grid-dump", patch) )
create_grid.add_mapping( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(patch,patch_overlap) )
project.solversteps.add_step(create_grid)


#
# Next, we want to dump the final solution once. Luckily, quite a lot of 
# support for blockstructured grid is available within Peano's toolbox. So
# we use features from there. In the example above, we added code to the 
# step manually (the grid setup). This time, we don't want to add any 
# further code manually.
#that I can 
print_solution = peano4.solversteps.Step( "PlotSolution" )
print_solution.use_cell(patch)
print_solution.remove_all_mappings()
plotter = peano4.toolbox.blockstructured.PlotPatchesInPeanoBlockFormat("solution",patch,"Q")
print_solution.add_mapping( plotter )
project.solversteps.add_step(print_solution)


#
# We also need the actual solve. Basically, what we'll do is that we will run
# over the grid twice per time step: In the first step, we evaluate all the 
# Riemann problems. In the second step, we update the time. The Riemann solves
# assume that all the patch data is already projected onto the faces, i.e. the 
# faces hold all overlap data. As a result, they do not need the patch data. It
# is only the timestepping which needs both types of data: faces (that is all 
# overlaps) plus the actual cell data. 
#
solve_Riemann_problems = peano4.solversteps.Step( "SolveRiemannProblems" )
perform_time_step      = peano4.solversteps.Step( "PerformTimeStep" )
solve_Riemann_problems.use_face(patch_overlap)
perform_time_step.use_face(patch_overlap)
perform_time_step.use_cell(patch)
perform_time_step.add_mapping( peano4.toolbox.blockstructured.ProjectPatchOntoFaces(patch,patch_overlap) )
project.solversteps.add_step(solve_Riemann_problems)
project.solversteps.add_step(perform_time_step)


#
# Peano's API does not know which settings to use on the present system. To 
# make it copy/clone the settings identified by ./configure, we ask it to 
# parse the generated configuration scripts. The makefile would also offer a
# routine to set the dimension. We take the default here.
#
project.output.makefile.parse_configure_script_outcome( "/home/tobias/git/Peano" )
project.export_constant( patch_size, "PatchSize" )
project.export_constant( unknowns, "NumberOfUnknownsPerCell" )


#
# Standard triad of operations. You can skip the first two steps if you want as 
# the script then will automatically invoke the previous steps. The other way 
# round, it is always admissible to only generate stuff, e.g., but to build and
# run the project through a command line
#
project.generate(peano4.output.Overwrite.Default)
project.build()
project.run( ["myarguments"] )


#
# Dump grid into VTK
#
#convert = peano4.visualisation.Convert( "grid-dump" )
#convert.set_visualisation_tools_path( "/home/tobias/git/Peano/src/visualisation" )
#convert.extract_fine_grid()
#convert.convert_to_vtk()


convert = peano4.visualisation.Convert( "solution" )
convert.set_visualisation_tools_path( "/home/tobias/git/Peano/src/visualisation" )
convert.extract_fine_grid()
convert.convert_to_vtk()
