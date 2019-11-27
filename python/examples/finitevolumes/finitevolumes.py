"""

 A very simple example which demonstrates how to configure a patch-based
 Finite Volume solver in Peano4. The code relies on snippets from ExaHyPE2.
 However, it relies only on ExaHyPE's C/FORTRAN compute kernels, i.e. the 
 sophisticated build environment of this H2020 project is not used.

"""


# We import Peano4 as project. If this step fails, ensure that your environment
# variable PYTHONPATH points to Peano4's python directory.
import peano4



# Create a project and configure it to end up in a subnamespace (and thus
# subdirectory). 
#
project = peano4.Project( ["examples", "finitevolumes"], "." )


