# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .DaStGenToLegacyTool import DaStGenToLegacyTool
from .DoF                 import DoF


class DaStGen(DoF):
  def __init__(self, name):
    """
      dim should be a two-tuple or a three-tuple of integers, so a construction 
      could look like
      
        cell_data = peano4.datamodel.Patch( (6,6,6), "Fluid" )

    """
    super(DaStGen, self).__init__(name)
    self.generator = DaStGenToLegacyTool(self)
    self.attributes_double = []

  def add_double_scalar(self,name):
    self.attributes_double.append( name )
    
    