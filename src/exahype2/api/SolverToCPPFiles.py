# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.output.Jinja2TemplatedHeaderImplementationFilePair

import dastgen2

import os
import sys

from .Solver import Solver


class SolverToCPPFiles(Solver.SolverGenerator):
    """!

    Default implementation that takes a solver and maps it onto two C++ classes:
    An abstract base class and a class for the user to implement it.

    """
    def __init__(self, solver: Solver):
        super().__init__(solver)


    def construct_output(self,
                         exahype2_project,
                         verbose):
        """!

        Dump the solver into actual C++ classes.

        """
        class_name = "Abstract" + self._solver.name
        templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
        if verbose:
            print( "Map class {} onto C++ files using {}".format(self._solver.name, templatefile_prefix) )
#      output.makefile.add_cpp_file( subdirectory + "repositories" + "/" + class_name + ".cpp", generated=True )
        #local_d = self.d.copy()
        
        generated_files = peano4.api.output.TemplatedHeaderImplementationFilePair(
            templatefile_prefix+".Abstract.template.h",
            templatefile_prefix+".Abstract.template.cpp",
            class_name, 
            exahype2_project.namespace,
            exahype2_project.subdirectory,
            {}, #local_d,
            default_overwrite=True)
        exahype2_project.peano4_project.output.add(generated_files)

