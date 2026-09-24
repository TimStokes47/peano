# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api.output.ActionSet


class StepToObserver(object):
  """!
  
  Take an action set and construct an observer from it.

  An action set is, as the name suggests, a set of actions. Each action
  set will have to be mapped onto one C++ action set, which is a where the 
  action behaviour is implemented: a selection of functions which react to 
  events such as "I have hit a face for the first time".
  
  An observer administers these action sets:
  
  - It holds a series of action sets dispatches the events to them.
  - It manages the data movements induced by the grid traversal automaton
    running through the mesh.
    
  Consequently, each action set among the solver steps yields exactly one
  observer, but potentially many action sets in the C++ implementation.
    
    
  ## Important links

  - @ref page_peano_action_sets    
  - @ref peano4.api.taskgraph


  """
  
  
  
  def __init__(self,step):
    self.step = step
    
    
  def construct_output(self,output,included_action_sets):
    """
      Construct one mapping
    """
    observer = peano4.api.output.Observer(
      self.step.name, self.step.project.namespace + [ "observers" ], self.step.project.subdirectory + "observers",
      included_action_sets,
      self.step.vertex_data,
      self.step.face_data,
      self.step.cell_data
    )
    output.artefacts.append( observer )
    output.makefile.add_cpp_file( observer.get_cpp_file_name(), generated=True )
