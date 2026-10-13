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

    Task graph compiler which splits every cell task into a producer and a consumer (work in progress)

    You can use this task graph in your ExaHyPE 2 project through

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    project.compiler = peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This compiler is a strict extension of
    @ref peano4.api.taskgraph.compiler.FuseSteps.translate(): it adds a
    preamble which rewrites the task graph, and then hands the rewritten
    graph over to the fusion compiler.

    ## Preamble

    Every task working on a cell (Task.GridEntity.CELL) is replaced by two
    tasks:

    1. The producer. For the time being, this is the original task, i.e.
       it holds the original action sets and data declarations.
    2. The consumer, named after the original task plus "Consumer". It
       works on cells, too, depends on the producer and, for the time
       being, holds neither action sets nor data declarations.

    Every task that depended on the original task depends on the consumer
    instead. Producer and consumer are marked as non-fusable, i.e.
    FuseSteps never puts them into the same mesh traversal. A consumer
    however might be fused with subsequent tasks.

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
        if task.entity == Task.GridEntity.CELL:
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
