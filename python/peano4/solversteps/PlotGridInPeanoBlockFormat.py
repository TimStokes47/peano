# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Mapping import Mapping


# @todo getter einfuehren, so dass man sieht was gen Observer geht und was net
# @todo Konstanten fuer ops

class PlotGridInPeanoBlockFormat(Mapping):
  def __init__(self):
    pass


  def get_constructor_body(self):
    return "// @todo Please implement\n"


  def get_body_of_getGridControlEvents(self):
    return "// @todo Please implement\nreturn std::vector< peano4::grid::GridControlEvent >();\n" 


  def get_body_of_operation(self,operation_name):
    return "// @todo Please implement\n"


  def get_mapping_name(self):
    return "PlotGridInPeanoBlockFormat"

  def user_should_modify_template(self):
    return False
