# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.solversteps.UserMapping

from peano4.solversteps.StepToMapping import StepToMapping
from peano4.solversteps.StepToObserver import StepToObserver


# 
# @todo Aufteilen in mehrere Files mit ToXXXXX wie im Model.
#

class Step:
  """
    Most generic version of a solver step. A solverstep describes what Peano4 
    should do when it runs over the grid. As users have to specify which pieces
    of data from the data model are used by the solver step, it can handle all
    the user data stack flow.
  """
  def __init__(self,name,add_user_defined_mapping = True):
    """
     By default, the step generates at least one mapping for the user. 
     However, you can disable this behaviour by passing False to
     add_user_defined_mapping. Alternatively, use remove_all_mappings()
     on the step.
    """
    self.name        = name
    self.cell_data   = []
    self.face_data   = []
    self.vertex_data = []
    if add_user_defined_mapping:
      self.mappings    = [ peano4.solversteps.UserMapping() ]
    else:
      self.mappings    = []
    self.mapping_generator   = StepToMapping(self)
    self.observer_generator  = StepToObserver(self)


  def remove_all_mappings(self):
    """
     Each step holds a set of mappings. They describe what the step actually
     should do whenever it loads a vertex, runs into a cell, and so forth. By
     default, the step holds one user-defined mapping (UserMapping) which means
     that a stub is generated where users can insert their functionality. You
     can remove this one (or any other one added so far) with this routine.
    """
    self.mappings = []

  
  def add_mapping(self,mapping):
    """
     Each step holds a set of mappings. They describe what the step actually
     should do whenever it loads a vertex, runs into a cell, and so forth. By
     default, the step holds one user-defined mapping (UserMapping) which means
     that a stub is generated where users can insert their functionality. It is 
     important in which order you add your mappings: The whole tree traversal 
     is a top-down/depth-first tree traversal. So all the enter, create, ...
     operations of a mapping are invoked exactly in the order you add them to 
     the step. All the delete, leave, ... operations are invokved in reversed
     order.
    """
    self.mappings.append(mapping)

    
  def set_project(self,project):
    """
     You don't have to call this. It is automatically called once you add a step 
     to the project's steps.
    """
    self.project     = project
    
  def use_cell(self,submodel):
    self.cell_data.append(submodel)

  def use_face(self,submodel):
    self.face_data.append(submodel)

  def use_vertex(self,submodel):
    self.vertex_data.append(submodel)

  def __get_spatial_attributes_of_mapping_signature(self):
    return ["center", "const tarch::la::Vector<Dimensions,double>&", "h", "const tarch::la::Vector<Dimensions,double>&" ]

  def get_vertex_operations_signature(self):
    result = self.__get_spatial_attributes_of_mapping_signature()
    for i in self.vertex_data:
      result += ["fineGridVertex" + i.name,i.get_full_qualified_type() + "&"]
    for i in self.vertex_data:
      result += ["coarseGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["coarseGridFaces" + i.name, i.get_enumeration_type() + "" ]
    for i in self.cell_data:
      result += ["coarseGridCell" + i.name,i.get_enumeration_type() + ""]
    result += [ "marker", "peano4::datamanagement::VertexMarker" ]
    return result

  def get_face_operations_signature(self):
    result  = self.__get_spatial_attributes_of_mapping_signature()
    result += ["normal", "const tarch::la::Vector<Dimensions,double>&"]
    for i in self.vertex_data:
      result += ["fineGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["fineGridFace" + i.name,i.get_full_qualified_type() + "&"]
    for i in self.vertex_data:
      result += ["coarseGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["coarseGridFaces" + i.name, i.get_enumeration_type() + "" ]
    for i in self.cell_data:
      result += ["coarseGridCell" + i.name,i.get_enumeration_type() + ""]
    return result


  def get_cell_operations_signature(self):
    result  = self.__get_spatial_attributes_of_mapping_signature()
    for i in self.vertex_data:
      result += ["fineGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["fineGridFaces" + i.name, i.get_enumeration_type() + "" ]
    for i in self.cell_data:
      result += ["fineGridCell" + i.name,i.get_full_qualified_type() + "&"]
    for i in self.vertex_data:
      result += ["coarseGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["coarseGridFaces" + i.name, i.get_enumeration_type() + "" ]
    for i in self.cell_data:
      result += ["coarseGridCell" + i.name,i.get_enumeration_type() + ""]
    return result
      

  def get_touch_cell_signature(self):
    result  = []
    for i in self.vertex_data:
      result += ["fineGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["fineGridFaces" + i.name, i.get_enumeration_type() + "" ]
    for i in self.cell_data:
      result += ["fineGridCell" + i.name,i.get_enumeration_type() + ""]
      
    if len(self.cell_data)==0:
      result += ["fineGridCell","peano4::datamanagement::CellWrapper<void>"]
      
    for i in self.vertex_data:
      result += ["coarseGridVertices" + i.name, i.get_enumeration_type() + "" ]
    for i in self.face_data:
      result += ["coarseGridFaces" + i.name, i.get_enumeration_type() + "" ]
    for i in self.cell_data:
      result += ["coarseGridCell" + i.name,i.get_enumeration_type() + ""]

    if len(self.cell_data)==0:
      result += ["coarseGridCell","peano4::datamanagement::CellWrapper<void>"]
      
    return result
      

  def construct_output(self,output):
    """
    Each solver step basically gives us two big files/classes: A class which 
    handles the actual data movements, i.e. is an implementation of the tree
    visitor pattern over the Peano4 tree. The other type is an interface which 
    provides plugin points into the created data transitions from a user's 
    perspective.
    """
    included_mappings = []
    for mapping in self.mappings:
      full_qualified_mapping_name = self.mapping_generator.construct_output(output,mapping)
      included_mappings.append( full_qualified_mapping_name )
    
    self.observer_generator.construct_output(output, included_mappings)
    

    