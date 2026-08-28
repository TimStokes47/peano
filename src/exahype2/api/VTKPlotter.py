# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from exahype2.plotter.api import Plotter


class VTKPlotter(Plotter):
    """!

    Default realisation of exahype2.plotter.api.Plotter. Writes the solver's
    data out as VTK files.

    """

    def __init__(self, solver):
        super().__init__(solver)

    def construct_output(self, output, verbose):
        """!
        @todo Not implemented yet
        """
        pass
