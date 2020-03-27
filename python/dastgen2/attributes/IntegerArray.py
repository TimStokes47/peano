# This file is part of the DaStGen2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Integer import Integer


class IntegerArray(Integer):
  def __init__(self, name, cardinality):
    Integer.__init__(self, name)
    self._cardinality = cardinality
    
  def get_methods(self,_full_qualified_class_name):
    accessor_name = self._name[:1].title() + self._name[1:]
    return [ 
      ("get" + accessor_name + "() const", "const int*"),
      ("set" + accessor_name + "(int value[" + self._cardinality + "])", "void"),
      ("get" + accessor_name + "(int index) const", "int"),
      ("set" + accessor_name + "(int index, int)", "void"),
    ]

  def get_plain_C_attributes(self):
    return [ ("_" + self._name + "[" + self._cardinality + "]", "double" ) ]

  def get_first_plain_C_attribute(self):
    return [ ("_" + self._name + "[0]", "int" ) ]

  def get_method_body(self,signature):
    if signature.startswith( "get" ) and  "index" in signature:
      return "  return _" + self._name + "[index];\n"
    elif signature.startswith( "set" ) and  "index" in signature:
      return "  _" + self._name + "[index] = value;\n"
    elif signature.startswith( "get" ):
      return "  return _" + self._name + ";\n"
    elif signature.startswith( "set" ):
      return "  std::copy_n(value, " + self._cardinality + ", _" + self._name + " );\n"
    else:
      assert(False)
    return ""
  
  def get_native_MPI_type(self):
    return [ ("MPI_INT", self._cardinality ) ]
      
  
  def get_to_string(self):
    """
    
      Return string representation of attribute.

    """
    return "_" + self._name + "[0] << \",...\""

