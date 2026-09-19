# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Compilers are simple functions 

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
def translate(graph: Graph, identifier: str) -> List["peano4.api.solversteps.Step"]:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

which take a task graph and yield a series of @ref peano4.api.solversteps.Step instances. 
In the Peano terminology, a @see peano4.api.solversteps.Step corresponds to an Observer: for every
mesh traversal, the traversal automaton is handed an observer that
distributes the grid events among the action sets tied to it.

There is one notable exception compared to the plain steps in 
@ref peano4.api.solversteps: Each step is also added an attribute
has_to_repeat which is a copy from Task's has_to_repeat. 
Consult the description of @ref peano4.api.taskgraph.Task for details on how to use this attribute to make a task repeat until a certain condition is met.

"""



from . import Sequential
