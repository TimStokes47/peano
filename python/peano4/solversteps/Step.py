# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import peano4.output.Mapping
import peano4.solversteps.UserMapping


class Step:
  """
    Most generic version of a solver step. A solverstep describes what Peano4 
    should do when it runs over the grid. As users have to specify which pieces
    of data from the data model are used by the solver step, it can handle all
    the user data stack flow.
  """
  def __init__(self,name):
    self.name        = name
    self.cell_data   = []
    self.face_data   = []
    self.vertex_data = []
    self.mappings    = [ peano4.solversteps.UserMapping() ]


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

  def __get_vertex_operations_signature(self):
    result = self.__get_spatial_attributes_of_mapping_signature()
    for i in self.vertex_data:
      result += ["vertex" + i.name,i.get_full_qualified_type() + "&"]
    return result

  def __get_face_operations_signature(self):
    result  = self.__get_spatial_attributes_of_mapping_signature()
    result += ["normal", "int"]
    for i in self.vertex_data:
      result += ["vertices" + i.name, "peano4::datamanagement::VertexEnumerator<" + i.get_full_qualified_type() + ">&" ]
    for i in self.face_data:
      result += ["face" + i.name,i.get_full_qualified_type() + "&"]
    return result

  def __get_cell_operations_signature(self):
    result  = self.__get_spatial_attributes_of_mapping_signature()
    for i in self.vertex_data:
      result += ["vertices" + i.name, "peano4::datamanagement::VertexEnumerator<" + i.get_full_qualified_type() + ">&" ]
    for i in self.face_data:
      result += ["faces" + i.name, "peano4::datamanagement::FaceEnumerator<" + i.get_full_qualified_type() + ">&" ]
    for i in self.cell_data:
      result += ["cell" + i.name,i.get_full_qualified_type() + "&"]
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
    number_of_user_mappings = 0
    for mapping in self.mappings:
      if mapping.user_should_modify_template():
        number_of_user_mappings +=1
    #print( "number of user-defined mappings for this step: " + str(number_of_user_mappings) )
    
    for mapping in self.mappings:
      mapping_name = ""
      if number_of_user_mappings<=1 and mapping.user_should_modify_template():
        mapping_name = self.name
      else:
        mapping_name = self.name + "2" + mapping.get_mapping_name() + str( self.mappings.index(mapping))
        
      subnamespace = ""
      if mapping.user_should_modify_template():
        subnamespace = "mappings" 
        print( "user has to modify class " + mapping_name + " in mappings directory manually ")
      else:
        subnamespace = "observers" 
        
      mapping = peano4.output.Mapping(
        mapping_name, self.project.namespace + [ subnamespace ], self.project.directory + "/" + subnamespace, mapping
      )
      included_mappings.append( subnamespace + "::" + mapping_name )
 
      for i in self.vertex_data:
        mapping.include_files.append( "vertexdata/" + i.name + ".h" )    
      for i in self.face_data:
        mapping.include_files.append( "facedata/" + i.name + ".h" )    
      for i in self.cell_data:
        mapping.include_files.append( "celldata/" + i.name + ".h" )    

      if len(self.vertex_data)>0:
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_PERSISTENT_VERTEX,  "void" ] + self.__get_vertex_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_PERSISTENT_VERTEX, "void" ] + self.__get_vertex_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_HANGING_VERTEX,     "void" ] + self.__get_vertex_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_HANGING_VERTEX,    "void" ] + self.__get_vertex_operations_signature() )

      if len(self.face_data)>0:
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_PERSISTENT_FACE, "void" ] + self.__get_face_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_PERSISTENT_FACE, "void" ] + self.__get_face_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_HANGING_FACE, "void" ] + self.__get_face_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_HANGING_FACE, "void" ] + self.__get_face_operations_signature() )

      if len(self.cell_data)>0:
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_CELL, "void" ] + self.__get_cell_operations_signature() )
        mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_CELL, "void" ] + self.__get_cell_operations_signature() )

      output.artefacts.append( mapping )
      output.makefile.add_cpp_file( mapping.get_cpp_file_name() )

    observer = peano4.output.Observer(
      self.name, self.project.namespace+ [ "observers" ],self.project.directory + "/observers",included_mappings
    )
    output.artefacts.append( observer )
    output.makefile.add_cpp_file( observer.get_cpp_file_name() )
    

    