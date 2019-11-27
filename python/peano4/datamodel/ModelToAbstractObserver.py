# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import os


class ModelToAbstractObserver(object):
  def __init__(self,model):
    self.model = model
    self.d     = {}
    #self.total_data_count = 0


  def __parse_data_declarations_in_model(self):
    self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     = ""
    self.d[ "DATA_CONTAINER_INCLUDES" ]      = ""
    self.d[ "DATA_CONTAINER_INSTANTIATION" ] = ""
    
    for i in self.model.cell_data:
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "typedef "
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.generator.get_stack_container()
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "  CellData" + str(self.model.cell_data.index(i)) + ";\n"
      self.d[ "DATA_CONTAINER_INCLUDES" ]      += i.generator.get_header_file_include()
      self.d[ "DATA_CONTAINER_INCLUDES" ]      += "\n"
      self.d[ "DATA_CONTAINER_INSTANTIATION" ] += "static std::map< DataKey, CellData" + str(self.model.cell_data.index(i)) + ">  _cellData" + str(self.model.cell_data.index(i)) + ";\n"

    for i in self.model.face_data:
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "typedef "
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.generator.get_stack_container()
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "  FaceData" + str(self.model.face_data.index(i)) + ";\n"
      self.d[ "DATA_CONTAINER_INCLUDES" ]      += i.generator.get_header_file_include()
      self.d[ "DATA_CONTAINER_INCLUDES" ]      += "\n"
      self.d[ "DATA_CONTAINER_INSTANTIATION" ] += "static std::map< DataKey, FaceData" + str(self.model.face_data.index(i)) + ">  _faceData" + str(self.model.face_data.index(i)) + ";\n"

    for i in self.model.vertex_data:
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "typedef "
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.generator.get_stack_container()
      self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "  VertexData" + str(self.model.vertex_data.index(i)) + ";\n"
      self.d[ "DATA_CONTAINER_INCLUDES" ]      += i.generator.get_header_file_include()
      self.d[ "DATA_CONTAINER_INCLUDES" ]      += "\n"
      self.d[ "DATA_CONTAINER_INSTANTIATION" ] += "static std::map< DataKey, VertexData" + str(self.model.vertex_data.index(i)) + ">  _vertexData" + str(self.model.vertex_data.index(i)) + ";\n"


  def construct_output(self,output):
    self.__parse_data_declarations_in_model()
      
    namespace  = self.model.namespace + [ "observers" ]
    class_name = "AbstractObserver"
    output.makefile.add_cpp_file( namespace[-1] + "/" + class_name + ".cpp" )
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
    generated_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix+".h.template",
      templatefile_prefix+".cpp.template",
      class_name, 
      namespace,
      namespace[-1], 
      self.d,
      True)
    output.add(generated_files)
    pass
