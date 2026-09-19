# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

The task graph abstraction allows users to formalise their solver steps over small task graphs which are then lowered into a sequence of Peano mesh traversals. 

A Solver describes one algorithmic phase (setup, time stepping, plotting)
as a @ref peano4.api.taskgraph.Graph of @ref peano4.api.taskgraph.Task instances.
Each task defines computations over a mesh entity (vertex, face, cell, or global), subject to dependencies between the tasks.
A simple task graph reads as follows:

1. Run over all cells and compute A.
2. Run over all faces and compute B, meanwhile do a global calculation. Both steps require A to have finished.
3. Run over all vertices once B has completed.
4. Wait for the third step and the global calculation and run over all cells again.

As a UML-like state diagram, this graph looks as follows:

@image html task-graph-example.png


## Modelling this task graph

Creating such a task graph is simple with the present API.
Rather than exercising throught the example above, we refer to @ref exahype2.api.fv.SetupSimulationOverRegularGrid
which uses exactly the same API to create a task graph for the setup phase of a Finite Volume scheme over a regular, block-structured mesh:


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
@input exahype2/api/fv/SetupSimulationOverRegularGrid.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Note the nice feature at the end of the script: The API comes along
with a routine to visualise the arising task graph. This is the same
feature used for the example image above.

## Mapping the task graph onto Peano

The idea behind the task graph feature of Peano is that there are different
compilers which take the task graph and lower it onto grid traversals.
The term "lower" is taken from the xDSL compiler project and highlights that 
we go from more abstract definitions close to the actual implementation.

There are different compilers available, each producing a different type of 
a task graph implementation. You find them all in @ref peano4.api.taskgraph.compiler.
The simples one if @ref @ref peano4.api.taskgraph.compiler.Sequential, which 
maps the task graph onto a total order preservering the dependencies. Each task
of the rearranged total order (each step) is then mapped onto one mesh traversal.
This is the simplest way to execute a task graph, through obviously might not
be the most efficient one.

Besides the lowering onto Peano mesh traversals, the compilers also yield 
brief functions which we can call from the main() function later to step 
through all the mesh traversals which make up one task graph realisation.


## Graph properties


### Task dependencies

Each task can depend on an arbitrary number of predecessor tasks. There is 
no feature to model outgoing dependencies. They are implicitly given by other
tasks referencing predecessors.

The task graph does not have to have a unique root task. It can comprise
multiple ready tasks and can have multiple terminal tasks.

### Tasks and action sets

Each task can hold an arbitrary number of Peano 4 action sets (@ref peano4.api.solversteps.ActionSet).
Each action set can plug into all kinds of events. This seems to be a contradiction:
The task is tied to a particular mesh entity, while the action set can define actions for multiple mesh entities. 
The task association defines dependencies, the action set what is done.
If task A depends on B and B is tied to vertices, any operation over a cell in 
A will be executed after all operations over the cell's adjacent vertices in B 
have completed.
The action sets of A still might also do something on cells and faces as well, 
but this is not subject to the dependency handling.


@see @ref peano4.api.taskgraph.Task
@see @ref peano4.api.taskgraph.Graph
@see @ref peano4.api.taskgraph.compiler
@see @ref peano4.api.taskgraph.create_runner

"""

from .Task import Task
from .Graph import Graph
from .Runner import create_runner
from . import compiler
