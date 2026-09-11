# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

The task graph abstraction sitting between a Solver's algorithmic
description and the generated C++ that actually runs it.

A Solver describes one algorithmic phase (setup, time stepping, plotting)
as a Graph of Task instances: each Task is one mesh sweep over a single
kind of grid entity (Task.GridEntity.VERTEX/FACE/CELL/GLOBAL), and
Task.add_dependency() records which other tasks have to have completed
before a given task may run.

A compiler (@see peano4.api.taskgraph.compiler) then lowers such a Graph
into a concrete, ordered sequence of peano4.api.solversteps.Step objects -
one per Task, in an order that respects every dependency - which is what
actually drives Peano's code generation (each Step corresponds to one
generated Observer/mesh traversal).

create_runner() finally wraps one phase's compiled sequence of Steps into a
generated C++ class (a peano4::RunMultipleSteps subclass) whose run()
traverses the spacetree set once per step, in the order the compiler chose.

@see peano4.api.taskgraph.Task
@see peano4.api.taskgraph.Graph
@see peano4.api.taskgraph.compiler
@see peano4.api.taskgraph.create_runner

"""

from .Task import Task
from .Graph import Graph
from .Runner import create_runner
from . import compiler
