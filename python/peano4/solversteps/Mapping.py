# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from abc import abstractmethod
 

class Mapping:
  def __init__(self):
    pass


  @abstractmethod
  def get_constructor_body(self):
    return "// @todo Should be overwritten by mapping\n"


  def get_body_of_getGridControlEvents(self):
    return "return std::vector< peano4::grid::GridControlEvent >();\n" 


  @abstractmethod
  def get_body_of_operation(self,operation_name):
    return "// @todo Should be overwritten by mapping\n"


  @abstractmethod
  def get_mapping_name(self):
    """
     Returns a description (word) for the mapping which is also used as class name
     for the generated type. As a consequence, the result should be one word (if 
     possible) and uppercase. Also, every subclass should overwrite this routine.
    """
    return "Dummy"
