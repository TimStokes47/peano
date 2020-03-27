# This file is part of the DaStGen2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import dastgen2.attributes.IntegerArray


class Peano4IntegerArray(dastgen2.attributes.IntegerArray):
  def __init__(self, name, cardinality):
    dastgen2.attributes.IntegerArray.__init__(self, name, cardinality)
    self._cardinality = cardinality
    
  def get_methods(self,_full_qualified_class_name):
    accessor_name = self._name[:1].title() + self._name[1:]
    return [ 
      ("get" + accessor_name + "() const", "tarch::la::Vector<" + self._cardinality + ",int>"),
      ("set" + accessor_name + "(const tarch::la::Vector<" + self._cardinality + ",int>& value)", "void"),
      ("get" + accessor_name + "(int index) const", "int"),
      ("set" + accessor_name + "(int index, int value)", "void"),
    ]

  def get_plain_C_attributes(self):
    return [ ("_" + self._name, "tarch::la::Vector<" + self._cardinality + ",int>" ) ]

  def get_first_plain_C_attribute(self):
    return [ ("_" + self._name + ".data()[0]", "int" ) ]

  def get_method_body(self,signature):
    if signature.startswith( "get" ) and  "index" in signature:
      return "  return _" + self._name + "(index);\n"
    elif signature.startswith( "set" ) and  "index" in signature:
      return "  _" + self._name + "(index) = value;\n"
    elif signature.startswith( "get" ):
      return "  return _" + self._name + ";\n"
    elif signature.startswith( "set" ):
      return "  _" + self._name + " = value;\n"
    else:
      assert(False)
    return ""
  
  def get_native_MPI_type(self):
    return [ ("MPI_INT", self._cardinality ) ]
      
  
  def get_to_string(self):
    """
    
      Return string representation of attribute.

    """
    return "_" + self._name

