# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from dastgen2 import *


class MPI(object):
  """ 
  
  Represents Peano's MPI aspect injected into a DaStGen model. 
  It mainly ensures that we include the right headers.
    
  """
  def __init__(self):
    pass


  def set_model(self,data_model):
    self._data_model = data_model

  def get_include(self):
    return """
#include "tarch/mpi/Rank.h"
"""
  
  def get_attributes(self):
    return ""
  
  def get_method_declarations(self,full_qualified_name):
    return ""


  def get_implementation(self,full_qualified_name):
    return ""    




