# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

import enum
from typing import List, Optional


class Task:
    """!

    One task within a peano4.api.taskgraph.Graph

    Every task works on exactly one kind of mesh entity - a vertex, a face
    or a cell - which has to be specified through entity when the task is
    constructed.
    
    ## Task dependencies

    Every task also holds dependencies, i.e. other Task instances that have
    to be completed before this task can run. This sequence is empty by
    default; use add_dependency() to add further tasks to it.

    ## Task action sets

    Every task furthermore holds an arbitrary number of Peano 4 action sets
    (peano4.api.solversteps.ActionSet), i.e. the actual grid-sweep logic the
    task realises once it is compiled into an Observer. This sequence is
    empty by default; use add_action_set() to add further action sets to it.
    Their order matters - see peano4.api.solversteps.Step.add_action_set().

    ## Task data usage

    A task's action sets are handed mesh data (fineGridCell..., fineGridFace...,
    fineGridVertices...) as arguments once they are compiled into an Observer -
    but only for the vertex/face/cell submodels the surrounding
    peano4.api.solversteps.Step was told about via its own use_cell()/
    use_face()/use_vertex(). An action set cannot make that declaration
    itself (several tasks, and hence several Steps, can share the same
    action set), so every task has to declare which submodels it needs
    through use_cell()/use_face()/use_vertex() below. The task graph
    compiler then forwards these declarations to the Step(s) it compiles
    this task into.

    ## Task repetitions
    
    Finally, a task can be marked as repeating via task_has_to_repeat. By
    default (None), a task runs once. Passing a non-None string works if 
    and only if it is a valid boolean C++ expression. Peano will evaluate
    it on the global runner after the task has finished and then decide if
    to rerun this step. An all time classic is the following expression
    
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    not peano4::grid::isGridStationary( peano4::parallel::SpacetreeSet::getInstance().getGridStatistics() )
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    which means this task will repeat until the mesh becomes stationary.

    ## Task processing

    The tasks will later on be taken by the graph compiler which maps them
    onto a sequence of @ref peano4.api.solversteps.Step instances. How many
    of these logical steps (grid traversals) are required depends on the 
    structure of the task graph but also the type of compiler chosen. 
    Contraray, each action set that is added to a task will remain to be a 
    real @ref peano4.api.solversteps.ActionSet, and subsequently be translated
    into exactly one @ref peano4.api.output.ActionSet, i.e. one pair of header
    and implementation file called MyFancyObserverOfAGridTraversal2ThisActionSet.
    
    ## Things to read to get the full picture:    

    - @ref peano4.api.taskgraph This is the overall description of the task 
           graph API and should be the starting point for understanding how to use it.
           It notably describes some design decisions re dependencies, grid
           entities and action sets.
    - @ref peano_action_sets    
    - @ref peano4.api.taskgraph

    """

    class GridEntity(enum.Enum):
        """! 
        
        Which kind of mesh entity a Task works on. 
        
        - GLOBAL implies that the task either has only a global semantics
          such as one global reduction, or that it touches each and every
          entity in the mesh, i.e. vertices, cells and faces.
          
        """

        VERTEX = 1
        FACE = 2
        CELL = 3
        GLOBAL = 4

    def __init__(
        self,
        name: str,
        entity: "Task.GridEntity",
        task_has_to_repeat: Optional[str] = None,
    ):
        self.name = name
        self.entity = entity
        self.dependencies: List["Task"] = []
        self.action_sets: List["peano4.api.solversteps.ActionSet"] = []
        self.cell_data: List["peano4.api.datamodel.DoF"] = []
        self.face_data: List["peano4.api.datamodel.DoF"] = []
        self.vertex_data: List["peano4.api.datamodel.DoF"] = []
        self.has_to_repeat = task_has_to_repeat

    def add_dependency(self, task: "Task"):
        self.dependencies.append(task)

    def use_cell(self, submodel: "peano4.api.datamodel.DoF"):
        """!

        Declare that this task's action sets need read/write access to a
        cell submodel (e.g. a peano4.api.datamodel.Patch registered via
        peano_project.datamodel.add_cell()). Forwarded to the
        peano4.api.solversteps.Step this task is compiled into, i.e. to its
        own use_cell(), so the generated Observer passes fineGridCell<name>
        into every touch-cell/create-cell/... operation.

        """
        self.cell_data.append(submodel)

    def use_face(self, submodel: "peano4.api.datamodel.DoF"):
        """!

        Declare that this task's action sets need read/write access to a
        face submodel. @see use_cell() for the general mechanism.

        """
        self.face_data.append(submodel)

    def use_vertex(self, submodel: "peano4.api.datamodel.DoF"):
        """!

        Declare that this task's action sets need read/write access to a
        vertex submodel. @see use_cell() for the general mechanism.

        """
        self.vertex_data.append(submodel)

    def add_action_set(self, action_set: "peano4.api.solversteps.ActionSet"):
        """!
        
        A task can hold an arbitrary number of Peano 4 action sets. Eventually
        the overarching task graph will be mapped onto a sequence of steps
        (observers) and those steps then grab those action sets from the tasks and execute them. 
        
        It is important to note that the action set passed in can define 
        actions for multiple grid entities, while the task is specified over 
        a particular mesh entity. This seems to be contradiction but is
        actually explained in @ref peano4.api.taskgraph.
        
        """
        self.action_sets.append(action_set)
