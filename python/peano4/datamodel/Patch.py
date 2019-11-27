# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .PatchToDoubleArray import PatchToDoubleArray


class Patch(object):
  def __init__(self, dim, no_of_unknowns, name):
    """
      dim should be a two-tuple or a three-tuple of integers, so a construction 
      could look like
      
        cell_data = peano4.datamodel.Patch( (6,6,6), "Fluid" )

    """
    self.dim       = dim
    self.no_of_unknowns = no_of_unknowns
    self.generator = PatchToDoubleArray(self)
    self.name      = name
    self.namespace = []


  def configure(self,namespace):
    """
    Typically called by model as soon as you add an object to it
    """
    self.namespace = namespace





