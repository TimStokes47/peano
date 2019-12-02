# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import peano4.output.MappingInterface
#import peano4.output.Mapp


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
      result += ["vertex" + i.name,"vertexdata::" + i.name + "&"]
    return result

  def __get_face_operations_signature(self):
    result  = self.__get_spatial_attributes_of_mapping_signature()
    result += ["normal", "int"]
    for i in self.vertex_data:
      result += ["vertices" + i.name, "peano4::datamanagement::VertexEnumerator<vertexdata::" + i.name + ">&" ]
    for i in self.face_data:
      result += ["face" + i.name,"facedata::" + i.name + "&"]
    return result

  def __get_cell_operations_signature(self):
    result  = self.__get_spatial_attributes_of_mapping_signature()
    for i in self.vertex_data:
      result += ["vertices" + i.name, "peano4::datamanagement::VertexEnumerator<vertexdata::" + i.name + ">&" ]
    for i in self.face_data:
      result += ["faces" + i.name, "peano4::datamanagement::FaceEnumerator<facedata::" + i.name + ">&" ]
    for i in self.cell_data:
      result += ["cell" + i.name,"facedata::" + i.name + "&"]
    return result


  def construct_output(self,output):
    """
    Each solver step basically gives us two big files/classes: A class which 
    handles the actual data movements, i.e. is an implementation of the tree
    visitor pattern over the Peano4 tree. The other type is an interface which 
    provides plugin points into the created data transitions from a user's 
    perspective.
    """
    interface = peano4.output.MappingInterface(
      self.name, self.project.namespace+ [ "mappings" ],self.project.directory + "/mappings",False
    )
 
    for i in self.vertex_data:
      interface.include_files.append( "vertexdata/" + i.name + ".h" )    
    for i in self.face_data:
      interface.include_files.append( "facedata/" + i.name + ".h" )    
    for i in self.cell_data:
      interface.include_files.append( "celldata/" + i.name + ".h" )    

    if len(self.vertex_data)>0:
      interface.operations.append( [ "createPersistentVertex", "void" ] + self.__get_vertex_operations_signature() )
      interface.operations.append( [ "destroyPersistentVertex", "void" ] + self.__get_vertex_operations_signature() )
      interface.operations.append( [ "createHangingVertex", "void" ] + self.__get_vertex_operations_signature() )
      interface.operations.append( [ "destroyHangingVertex", "void" ] + self.__get_vertex_operations_signature() )

    if len(self.face_data)>0:
      interface.operations.append( [ "createPersistentFace", "void" ] + self.__get_face_operations_signature() )
      interface.operations.append( [ "destroyPersistentFace", "void" ] + self.__get_face_operations_signature() )
      interface.operations.append( [ "createHangingFace", "void" ] + self.__get_face_operations_signature() )
      interface.operations.append( [ "destroyHangingFace", "void" ] + self.__get_face_operations_signature() )

    if len(self.cell_data)>0:
      interface.operations.append( [ "createCell", "void" ] + self.__get_cell_operations_signature() )
      interface.operations.append( [ "destroyCell", "void" ] + self.__get_cell_operations_signature() )

    output.artefacts.append( interface )

    observer = peano4.output.Observer(
      self.name, self.project.namespace+ [ "observers" ],self.project.directory + "/observers"
    )
    output.artefacts.append( observer )
    output.makefile.add_cpp_file( observer.get_cpp_file_name() )

    # @todo Kein virtual in die generierten Signaturen
    