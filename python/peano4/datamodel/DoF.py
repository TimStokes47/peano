# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from enum import Enum


class DoFAssociation(Enum):
  Undef  = 0
  Vertex = 1
  Face   = 2
  Cell   = 3
  
  
  
class DoF(object):
  def __init__(self, name):
    self.assocation = DoFAssociation.Undef
    self.name      = name
    self.namespace = []

  
  
  def configure(self,namespace,assocation):
    """
    Typically called by model as soon as you add an object to it
    """
    self.namespace = [x for x in namespace]
    self.assocation = assocation
    
    if assocation==DoFAssociation.Vertex:
      self.namespace += [ "vertexdata" ]
    elif assocation==DoFAssociation.Cell:
      self.namespace += [ "celldata" ]
    elif assocation==DoFAssociation.Face:
      self.namespace += [ "facedata" ]
    else:
      assert False
    #print( "added a new DoF: " + str(self.namespace) )
    

  def get_full_qualified_type(self):
    result = ""
    for i in self.namespace:
      result += i
      result += "::"
    result += self.name
    return result
    

  def get_logical_type_name(self):
    """
      What should the data type be called within the data repository,
      or within action sets. We add a prefix name here.
    """
    result = ""
    if self.assocation==DoFAssociation.Vertex:
      result += "VertexData"
    elif self.assocation==DoFAssociation.Cell:
      result += "CellData"
    elif self.assocation==DoFAssociation.Face:
      result += "FaceData"
    else:
      assert False
    result += self.name
    return result

    
  #def has_enumerator(self):
  #  if self.assocation==DoFAssociation.Vertex:
  #    return True
  #  elif self.assocation==DoFAssociation.Face:
  #    return True
  #  else:
  #    return False


  def get_enumeration_type(self):
    """
      What should the data type be called within the data repository.
    """
    result = ""
    if self.assocation==DoFAssociation.Vertex:
      result += "peano4::datamanagement::VertexEnumerator<"
    elif self.assocation==DoFAssociation.Face:
      result += "peano4::datamanagement::FaceEnumerator<"
    #elif self.assocation==DoFAssociation.Cell:
    #  result += "peano4::datamanagement::CellWrapper<"
    else:
      assert False
    result += self.get_full_qualified_type()
    result += ">"
    return result

