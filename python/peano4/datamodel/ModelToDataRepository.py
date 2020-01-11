# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import os


class ModelToDataRepository(object):
  def __init__(self,model):
    self.model = model
    self.d     = {}


  def __build_up_dictionary_for_one_data_set(self,i):
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "     typedef "
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.get_full_qualified_type()
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "  " 
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.get_repository_type_name()
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += ";\n  " 

    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "     typedef "
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.generator.get_stack_container()
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "  " 
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += i.get_logical_type_name()
    #self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     += "Stack;\n  " 
    
    self.d[ "DATA_CONTAINER_INCLUDES" ]      += i.generator.get_header_file_include()
    self.d[ "DATA_CONTAINER_INCLUDES" ]      += "\n"
    
    self.d[ "DATA_CONTAINER_DECLARATION" ]   += "static std::map< DataKey, " + i.generator.get_stack_container() + ">  _" + i.get_logical_type_name() + "Stack;\n"
    self.d[ "DATA_CONTAINER_INSTANTIATION" ] += "std::map< " + self.d[ "FULL_QUALIFIED_CLASS_NAME" ] + "::DataKey, " + i.generator.get_stack_container()  + ">   " + self.d[ "FULL_QUALIFIED_CLASS_NAME" ] + "::_" + i.get_logical_type_name() + "Stack;\n"

      
  def __parse_data_declarations_in_model(self):
    self.d[ "DATA_CONTAINER_TYPE_DEFS" ]     = ""
    self.d[ "DATA_CONTAINER_INCLUDES" ]      = ""
    self.d[ "DATA_CONTAINER_DECLARATION" ]   = ""
    self.d[ "DATA_CONTAINER_INSTANTIATION" ] = ""

    self.d[ "FULL_QUALIFIED_CLASS_NAME" ] = ""
    for i in self.__get_full_namespace():
      self.d[ "FULL_QUALIFIED_CLASS_NAME" ] += i
      self.d[ "FULL_QUALIFIED_CLASS_NAME" ] += "::"
    self.d[ "FULL_QUALIFIED_CLASS_NAME" ] += self.__get_class_name()
    
    for i in self.model.cell_data:
      self.__build_up_dictionary_for_one_data_set(i)

    for i in self.model.face_data:
      self.__build_up_dictionary_for_one_data_set(i)

    for i in self.model.vertex_data:
      self.__build_up_dictionary_for_one_data_set(i)

  def __get_full_namespace(self):
    return self.model.namespace + [ "observers" ]
      
  def __get_class_name(self):
    return "DataRepository"


  def construct_output(self,output):
    self.__parse_data_declarations_in_model()
      
    namespace  = self.__get_full_namespace()
    class_name = self.__get_class_name()
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
