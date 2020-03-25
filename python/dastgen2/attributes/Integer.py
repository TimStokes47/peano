# This file is part of the DaStGen2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Attribute import Attribute


class Integer(Attribute):
  """ 
  Represents on DaStGen2 object, i.e. one data model.
  
  namespace Sequence of strings representing the (nested) namespace. Pass in 
    [ "examples", "algebraicmg" ] for example if you wanna write a solver that 
    is embedded into the namespace examples::algebraicmg.
    
  """
  def __init__(self, name):
    Attribute.__init__(self, name)
    
  def get_methods(self):
    accessor_name = self._name.capitalize()
    return [ 
      ("get" + accessor_name + "() const", "int"),
      ("set" + accessor_name + "(int value)", "void")
    ]

  def get_plain_C_attributes(self):
    return [ ("_" + self._name, "int" ) ]

  def get_method_body(self,signature):
    if signature.startswith( "get" ):
      return "  return _" + self._name + ";\n"
    elif signature.startswith( "set" ):
      return "  _" + self._name + " = value;\n"
    else:
      assert(False)
    return ""
  
  def get_native_MPI_type(self):
    return ("MPI_INT", 1) 
  
  def get_to_string(self):
    """
    
      Return string representation of attribute.

    """
    return "_" + self._name

