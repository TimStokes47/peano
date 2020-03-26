# This file is part of the DaStGen2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Attribute import Attribute


class String(Attribute):
  """ 
  Represents on DaStGen2 object, i.e. one data model.
  
  namespace Sequence of strings representing the (nested) namespace. Pass in 
    [ "examples", "algebraicmg" ] for example if you wanna write a solver that 
    is embedded into the namespace examples::algebraicmg.
    
  """
  def __init__(self, name, max_length=80):
    Attribute.__init__(self, name)
    self._max_length = max_length
    
  def get_methods(self):
    accessor_name = self._name.capitalize()
    return [ 
      ("get" + accessor_name + "() const", "std::string"),
      ("set" + accessor_name + "(const std::string& value)", "void")
    ]

  def get_plain_C_attributes(self):
    return [ 
      ("_" + self._name + "[" + str(self._max_length) + "]", "char" ),
      ("_" + self._name + "Length", "int" )
    ]

  def get_method_body(self,signature):
    if signature.startswith( "get" ):
      return """
  std::ostringstream result;
  for (int i=0; i<_""" + self._name + """Length; i++) {
    result << static_cast<char>(_""" + self._name + """[i]);
  }
  return result.str();
""" 
    elif signature.startswith( "set" ):
      return """
  _""" + self._name + """Length = value.length();
  for (int i=0; i<value.length(); i++) {
    _""" + self._name + """[i] = value.data()[i];
  }
    
"""
    else:
      assert(False)
    return ""
  
  def get_native_MPI_type(self):
    return [ ("MPI_CHAR", self._max_length), ("MPI_INT", 1) ]
  
  def get_to_string(self):
    """
    
      Return string representation of attribute.

    """
    return "_" + self._name

