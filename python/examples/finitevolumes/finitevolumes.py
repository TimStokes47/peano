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
import peano4



#
# Create a project and configure it to end up in a subnamespace (and thus
# subdirectory). 
#
project = peano4.Project( ["examples", "finitevolumes"], "." )

#
# The solver will be patch-based, i.e. we will have one patch per cell.
#
patch = peano4.datamodel.Patch( (4,4,4), 5, "Q" )
project.datamodel.add_cell(patch)

#
# Along the faces, we have the patch overlaps. As we use only a Rusanov flux, 
# one cell of overlap between adjacent patches is sufficient.
#
patch_overlap = peano4.datamodel.Patch( (1,4,4), 5, "Q" )
project.datamodel.add_face(patch_overlap)

#
# For each step that we wanna do, we define one solver step.
#
grid_printer = peano4.solversteps.Step( "PlotGrid" )
grid_printer.use_face(patch_overlap)
grid_printer.use_cell(patch)
grid_printer.add_mapping( peano4.solversteps.PlotGridInPeanoBlockFormat("grid-dump") )
project.solversteps.add_step(grid_printer)



#
# Peano's API does not know which settings to use on the present system. To 
# make it copy/clone the settings identified by ./configure, we ask it to 
# parse the generated configuration scripts. The makefile would also offer a
# routine to set the dimension. We take the default here.
#
project.output.makefile.parse_configure_script_outcome( "/home/tobias/git/Peano" )


#
# Standard triad of operations. You can skip the first two steps if you want as 
# the script then will automatically invoke the previous steps. The other way 
# round, it is always admissible to only generate stuff, e.g., but to build and
# run the project through a command line
#
project.generate(peano4.output.Overwrite.Default)
project.build()
project.run( ["myarguments"] )
