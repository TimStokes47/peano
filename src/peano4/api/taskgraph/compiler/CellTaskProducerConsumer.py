# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import copy
from typing import List

import peano4.api
import peano4.api.solversteps

from ..Graph import Graph
from ..Task import Task
from . import FuseSteps


def translate(graph: Graph, identifier: str) -> List["peano4.api.solversteps.Step"]:
    """!

    Task graph compiler which splits tasks writing cell data into a producer and a consumer (work in progress)

    You can use this task graph in your ExaHyPE 2 project through

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    project.compiler = peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This compiler is a strict extension of
    @ref peano4.api.taskgraph.compiler.FuseSteps.translate(): it adds a
    preamble which rewrites the task graph, and then hands the rewritten
    graph over to the fusion compiler.

    ## Preamble

    Every task which exclusively writes cell data is replaced by two tasks.
    A task exclusively writes cell data if it declares at least one cell
    submodel as WRITE or READ_AND_WRITE, but no face or vertex submodel as
    WRITE or READ_AND_WRITE (@see _exclusively_writes_cell_data()). It may
    read face or vertex data. All other tasks are kept as they are. The
    two tasks are:

    1. The producer, named after the original task plus "Producer". It
       inherits all action sets and data declarations of the original
       task.
    2. The consumer, named after the original task plus "Consumer". It
       works on cells, too, depends on the producer and, for the time
       being, holds neither action sets nor data declarations.

    Every task that depended on the original task depends on the consumer
    instead. Producer and consumer are marked as non-fusable, i.e.
    FuseSteps never puts them into the same mesh traversal. A consumer
    however might be fused with subsequent tasks. In an ExaHyPE 2 setup,
    for example, the initial condition yields a producer and a consumer,
    and the latter is fused with the subsequent projection onto the faces.

    The preamble does not alter the graph passed in. It works on a copy of
    the graph's tasks.

    graph: peano4.api.taskgraph.Graph
      The task graph to compile. Assumed to be acyclic.

    identifier: str
      @see peano4.api.taskgraph.compiler.FuseSteps.translate()

    @return [peano4.api.solversteps.Step]
      @see peano4.api.taskgraph.compiler.FuseSteps.translate()

    """
    producer_consumer_graph, non_fusable_task_pairs = _split_cell_tasks(graph)
    return FuseSteps.translate(producer_consumer_graph, identifier, non_fusable_task_pairs)


def _copy_task(task: Task) -> Task:
    """!

    Shallow copy of a task with its own lists and dictionaries, so we can
    alter the copy's dependencies without touching the original graph. The
    action sets and the submodels themselves are shared.

    """
    result = copy.copy(task)
    result.dependencies = []
    result.action_sets = list(task.action_sets)
    result.cell_data = list(task.cell_data)
    result.face_data = list(task.face_data)
    result.vertex_data = list(task.vertex_data)
    result.cell_data_access = dict(task.cell_data_access)
    result.face_data_access = dict(task.face_data_access)
    result.vertex_data_access = dict(task.vertex_data_access)
    return result


def _exclusively_writes_cell_data(task: Task) -> bool:
    """!

    True if task declares at least one cell submodel as WRITE or
    READ_AND_WRITE, and no face or vertex submodel as WRITE or
    READ_AND_WRITE. @see translate()

    """
    writing_accesses = (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE)
    writes_cell_data = any(access in writing_accesses for access in task.cell_data_access.values())
    writes_other_data = any(
        access in writing_accesses
        for access in list(task.face_data_access.values()) + list(task.vertex_data_access.values())
    )
    return writes_cell_data and not writes_other_data


def _split_cell_tasks(graph: Graph):
    """!

    Build the producer-consumer graph. @see translate()

    @return (peano4.api.taskgraph.Graph, [(Task, Task)])
      The new graph and the list of (producer, consumer) pairs, which must
      not be fused.

    """
    producer_of_task = {}
    consumer_of_task = {}
    for task in graph.tasks:
        producer_of_task[task] = _copy_task(task)
        if _exclusively_writes_cell_data(task):
            producer_of_task[task].name = f"{task.name} Producer"
            consumer_of_task[task] = Task(name=f"{task.name} Consumer", entity=Task.GridEntity.CELL)

    result = Graph()
    non_fusable_task_pairs = []
    for task in graph.tasks:
        producer = producer_of_task[task]
        for dependency in task.dependencies:
            # Depend on whatever finishes the dependency: its consumer if
            # it has been split, otherwise the (copied) task itself
            producer.add_dependency(consumer_of_task.get(dependency, producer_of_task[dependency]))
        result.add_task(producer)
        if task in consumer_of_task:
            consumer = consumer_of_task[task]
            consumer.add_dependency(producer)
            result.add_task(consumer)
            non_fusable_task_pairs.append((producer, consumer))

    return result, non_fusable_task_pairs
