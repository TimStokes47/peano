# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.ActionSet


class StepToActionSet(object):
  def __init__(self,step):
    self.step = step
    
    
  def construct_output(self,output,mapping):
      """
        Construct one mapping
      """
      number_of_user_action_sets = 0
      for x in self.step.action_sets:
        if x.user_should_modify_template():
          number_of_user_action_sets +=1

      action_set_name = ""
      if number_of_user_action_sets<=1 and mapping.user_should_modify_template():
        action_set_name = self.step.name
      else:
        action_set_name = self.step.name + "2" + mapping.get_action_set_name() + str( self.step.action_sets.index(mapping))
        
      subnamespace = ""
      if mapping.user_should_modify_template():
        subnamespace = "actions" 
        print( "user has to modify class " + action_set_name + " in actions directory manually ")
      else:
        subnamespace = "observers" 

      new_action_set = peano4.output.ActionSet(
        action_set_name, self.step.project.namespace + [ subnamespace ], self.step.project.directory + "/" + subnamespace, mapping
      )
 
      for i in self.step.vertex_data:
        new_action_set.include_files.append( "vertexdata/" + i.name + ".h" )    
      for i in self.step.face_data:
        new_action_set.include_files.append( "facedata/" + i.name + ".h" )    
      for i in self.step.cell_data:
        new_action_set.include_files.append( "celldata/" + i.name + ".h" )    

      if len(self.step.vertex_data)>0:
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_CREATE_PERSISTENT_VERTEX,  "void" ] + self.step.get_vertex_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_DESTROY_PERSISTENT_VERTEX, "void" ] + self.step.get_vertex_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_CREATE_HANGING_VERTEX,     "void" ] + self.step.get_vertex_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_DESTROY_HANGING_VERTEX,    "void" ] + self.step.get_vertex_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_TOUCH_VERTEX_FIRST_TIME,   "void" ] + self.step.get_vertex_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_TOUCH_VERTEX_LAST_TIME,    "void" ] + self.step.get_vertex_operations_signature() )

      if len(self.step.face_data)>0:
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_CREATE_PERSISTENT_FACE,  "void" ] + self.step.get_face_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_DESTROY_PERSISTENT_FACE, "void" ] + self.step.get_face_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_CREATE_HANGING_FACE,     "void" ] + self.step.get_face_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_DESTROY_HANGING_FACE,    "void" ] + self.step.get_face_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME,   "void" ] + self.step.get_face_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_TOUCH_FACE_LAST_TIME,    "void" ] + self.step.get_face_operations_signature() )

      if len(self.step.cell_data)>0:
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_CREATE_CELL,           "void" ] + self.step.get_cell_operations_signature() )
        new_action_set.operations.append( [ peano4.solversteps.ActionSet.OPERATION_DESTROY_CELL,          "void" ] + self.step.get_cell_operations_signature() )

      new_action_set.operations.append( 
        [ peano4.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME, "void" ] + 
        self.step.get_touch_cell_signature()
      )
      new_action_set.operations.append( 
        [ peano4.solversteps.ActionSet.OPERATION_TOUCH_CELL_LAST_TIME, "void" ] + 
        self.step.get_touch_cell_signature()
      )

      output.artefacts.append( new_action_set )
      output.makefile.add_cpp_file( new_action_set.get_cpp_file_name() )
      
      return subnamespace + "::" + action_set_name
