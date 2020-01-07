# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.Mapping


class StepToMapping(object):
  def __init__(self,step):
    self.step = step
    
    
  def construct_output(self,output,mapping):
      """
        Construct one mapping
      """
      number_of_user_mappings = 0
      for x in self.step.mappings:
        if x.user_should_modify_template():
          number_of_user_mappings +=1

      mapping_name = ""
      if number_of_user_mappings<=1 and mapping.user_should_modify_template():
        mapping_name = self.step.name
      else:
        mapping_name = self.step.name + "2" + mapping.get_mapping_name() + str( self.step.mappings.index(mapping))
        
      subnamespace = ""
      if mapping.user_should_modify_template():
        subnamespace = "mappings" 
        print( "user has to modify class " + mapping_name + " in mappings directory manually ")
      else:
        subnamespace = "observers" 

      new_mapping = peano4.output.Mapping(
        mapping_name, self.step.project.namespace + [ subnamespace ], self.step.project.directory + "/" + subnamespace, mapping
      )
 
      for i in self.step.vertex_data:
        new_mapping.include_files.append( "vertexdata/" + i.name + ".h" )    
      for i in self.step.face_data:
        new_mapping.include_files.append( "facedata/" + i.name + ".h" )    
      for i in self.step.cell_data:
        new_mapping.include_files.append( "celldata/" + i.name + ".h" )    

      if len(self.step.vertex_data)>0:
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_PERSISTENT_VERTEX,  "void" ] + self.step.get_vertex_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_PERSISTENT_VERTEX, "void" ] + self.step.get_vertex_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_HANGING_VERTEX,     "void" ] + self.step.get_vertex_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_HANGING_VERTEX,    "void" ] + self.step.get_vertex_operations_signature() )

      if len(self.step.face_data)>0:
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_PERSISTENT_FACE, "void" ]  + self.step.get_face_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_PERSISTENT_FACE, "void" ] + self.step.get_face_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_HANGING_FACE, "void" ]     + self.step.get_face_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_HANGING_FACE, "void" ]    + self.step.get_face_operations_signature() )

      if len(self.step.cell_data)>0:
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_CREATE_CELL, "void" ]  + self.step.get_cell_operations_signature() )
        new_mapping.operations.append( [ peano4.solversteps.Mapping.OPERATION_DESTROY_CELL, "void" ] + self.step.get_cell_operations_signature() )

      output.artefacts.append( new_mapping )
      output.makefile.add_cpp_file( new_mapping.get_cpp_file_name() )
      
      return subnamespace + "::" + mapping_name
