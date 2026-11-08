---
name: peano:couple-exahype-solvers
description: Take several solvers from ExaHyPE and couple them together, so they work as a unit.
---

#Couple ExaHyPE solvers

## Capture Intent

Start by understanding the user's intent.

1. Which solvers should be coupled?
2. Do you favour an additive or a multiplicative coupling?
3. What will the solver be called and where to place the resulting files?
4. Will the solvers exchange data only via volumetric coupling, or will they also globally synchronise with each other?

There is no preferred version of additive vs multiplicative.

## Preparatory steps

Create a new host class which derives from `exahype2.api.SolverHost`. It is what the user
adds to the project (`project.add_solver(host)`), and it brings its solvers along. 
The constructor creates one solver instance per solver to be coupled, and hands them over
to the superclass in the order in which they are realised. A solver can rely on the data
of its predecessors. Do **not** let the solvers call each other's realisation, and do not
add the individual solvers to the project yourself.

The host does not override `generate_solver_realisation()`. The superclass already does the
following, and the host only implements `couple()`:

1. Each solver is realised into task graphs of its own (`exahype2.api.PhaseGraphs`), and 
   each solver reports its own time stepping graph (text or visualisation, depending on the logger).
2. The superclass concatenates the graphs of all solvers into graphs of the host 
   (`peano4.api.taskgraph.Graph.append()`).
3. `couple()` adds tasks and dependencies between the chains of the solvers. 
4. The superclass reports the concatenated, coupled time stepping graph once more and 
   appends it to the graphs of the project.

If possible, display a rendering of each of these graphs to the user.

### Global tasks

Some tasks belong to the mesh rather than to a solver: the task that switches the load balancing
off, the last task which evaluates the adaptivity criterion and switches the load balancing on again,
and the mesh adaptation phase. They must exist once. Each solver's 
`generate_solver_realisation()` takes a flag `add_global_tasks`; the superclass passes True to
the solver in `global_tasks_solver` (default: the first one) and False to all others. 
If your host needs a different solver to own them, override the property. 
Ask the user which solver's adaptivity criterion should drive the mesh.
If a solver uses a custom numerical scheme, check that its `construct_output()` accepts and 
honours the flag.


## Volumetric coupling and data sets

If the coupling is volumetric, ask the user if auxiliary variables of one solver are
actually primary (evolved) variables of another one, or if solvers share auxiliary
variables accordingly. After that, change the data association accordingly.


## Algorithmic coupling

### Multiplicative solver coupling

`couple()` makes tasks of one solver wait for tasks of the other one. The graphs are already
concatenated, so there is nothing to append: look up the tasks to be tied together (ideally 
through tags that the solver's scheme puts on them - names only as a fallback, and then ask 
the solver's scheme for the names rather than hard-coding them), take over the dependencies 
of the consuming task, and make the consuming task depend on the producer. The first task of 
the second solver has to depend on the last task of the first solver, or, for 
a coupling per time step stage, each stage of the consumer on the matching stage of the 
producer. We create a chain of subtasks.
`exagrype.api.FOSOSolver` is a worked example. 

If possible, display a rendering of the arising task graph to the user.


### Additive solver coupling

The solvers' graphs run in parallel. In `couple()`, introduce a new KickOff task as the very 
first one, i.e. the first task of each solver's task graph depends on KickOff to have terminated.
`CCZ4WithEuler` in `benchmarks/exagrype/gauge-wave-with-Euler` is a worked example.

If possible, display a rendering of the arising task graph to the user.

Ask from the user if any of the mesh data are read and written by two solvers.
In this case, there could be a data race. If this is the case, add to KickOff
a temporary copy of the respective data field and add an action set which befills
this copy. One of the solvers has to redirect their access to the copy. After the
task graph has finished, the copy can be discarded, too.


### End of time step coupling (global coupling)

If two solvers furthermore have to synchronise each other globally, then we have 
to insert a WrapUp task after the whole task graph which waits for the last tasks
of all the created subtasks. Note that these might fork in their own right, i.e. we 
have to find all tasks that are not predecessor of any other task. 
`peano4.api.taskgraph.Graph.sink_tasks()` does that, for the host's concatenated graph.

The WrapUp tasks only does a global action in unprepare. The Abstract solver base
class that's generated get a new functor the says synchronise, and the user is told
to supply the missing information. 


### Support for Runge-Kutta time stepping

As we always want to support multi-step time stepping, too, we have to reread all
of the descriptions above re time steps as "subgraph per time step". This is only 
relevant for the additive scheme. It implies that WrapUp and KickOff might have to 
be inserted multiple times at the corresponding markers. Tasks at the stages are 
looked up in the concatenated graph in `couple()`.


## Optimise (additive coupling only)

If we do temporary copies, do a C++ source code analysis and try to find out which 
unknowns of the copied data are actually read. In principle, the developer could now
narrow down the auxiliary variables. This required a code rewrite. It is easier to 
only copy those unknowns which are actually used. The analysis needs a confirmation 
by the user if this is all correct. 

## Wrap up question

Analyse if both solvers use some input data from the other solver. If not, we work
with uni-directional coupling. Ask the user if they want to insert or append certain
coupling tasks which introduce bi-directional coupling between fields, or if some
key operations need alteration.

Often, the outcome (primary variables) of one task feed into the right-hand sides 
of coupled solvers. If this is the case and if we employ additive coupling, then
we might need to rerun the dependency analysis and introduce temporary copies.


