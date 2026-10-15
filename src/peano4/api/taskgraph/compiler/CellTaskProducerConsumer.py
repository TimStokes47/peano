# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import copy
from typing import List

import dastgen2

import peano4.api
import peano4.api.datamodel
import peano4.api.solversteps

from ..Graph import Graph
from ..Task import Task
from . import FuseSteps
from . import Sequential


def translate(
    graph: Graph,
    identifier: str,
    peano_project: "peano4.api.Project",
    use_fusion: bool = False,
) -> List["peano4.api.solversteps.Step"]:
    """!

    Task graph compiler which splits tasks writing cell data into a producer and a consumer (work in progress)

    You can use this task graph in your ExaHyPE 2 project through

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    project.compiler = peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    This compiler adds a preamble which rewrites the task graph, and then
    hands the rewritten graph over to either
    @ref peano4.api.taskgraph.compiler.Sequential.translate() (default) or
    @ref peano4.api.taskgraph.compiler.FuseSteps.translate(). To use the
    latter, set the flag use_fusion:

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    import functools
    project.compiler = functools.partial(
      peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate,
      use_fusion=True,
    )
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    ## Preamble

    Every task which exclusively writes cell data is replaced by two tasks.
    A task exclusively writes cell data if it declares at least one cell
    submodel as WRITE or READ_AND_WRITE, but no face or vertex submodel as
    WRITE or READ_AND_WRITE (@ref _exclusively_writes_cell_data()). It may
    read face or vertex data. All other tasks are kept as they are. The
    two tasks are:

    1. The producer, named after the original task plus "Producer". It
       inherits all action sets and data declarations of the original
       task.
    2. The consumer, named after the original task plus "Consumer". It
       works on cells, too, and depends on the producer. For the time
       being, it holds no action sets. However, it declares every cell
       submodel that the producer writes (WRITE or READ_AND_WRITE) as
       READ_AND_WRITE.

    Every task that depended on the original task depends on the consumer
    instead. Producer and consumer are marked as non-fusable, i.e. if we
    use FuseSteps, it never puts them into the same mesh traversal. A
    consumer however might be fused with subsequent tasks. In an ExaHyPE 2
    setup, for example, the initial condition yields a producer and a
    consumer, and the latter is fused with the subsequent projection onto
    the faces.

    The preamble does not alter the graph passed in. It works on a copy of
    the graph's tasks.

    ## Task labels

    If the preamble identifies at least one producer-consumer pair and a
    peano_project is given, the compiler furthermore adds a new cell data
    type <identifier>_TaskLabels to the project's data model. It is
    modelled via DaStGen2 and holds one integer per producer-consumer pair.
    The integer's name is the CamelCase version of the original task's
    name followed by TaskNumber, e.g. EulerFVSmoothInitialConditionTaskNumber
    (@ref _task_number_attribute_name()). For the time being, no step uses
    these labels.

    graph: peano4.api.taskgraph.Graph
      The task graph to compile. Assumed to be acyclic.

    identifier: str
      @ref peano4.api.taskgraph.compiler.FuseSteps.translate()

    peano_project: peano4.api.Project
      Project the task labels are added to. If None, no task labels are
      added.

    use_fusion: bool
      If True, hand the rewritten graph over to FuseSteps, otherwise to
      Sequential.

    @return [peano4.api.solversteps.Step]
      @ref peano4.api.taskgraph.compiler.FuseSteps.translate()

    """
    producer_consumer_graph, non_fusable_task_pairs, split_tasks = _split_cell_tasks(graph)

    if peano_project is not None and split_tasks:
        peano_project.datamodel.add_cell(_create_task_labels(identifier, split_tasks))

    if use_fusion:
        return FuseSteps.translate(producer_consumer_graph, identifier, peano_project, non_fusable_task_pairs)
    else:
        return Sequential.translate(producer_consumer_graph, identifier, peano_project)


def _task_number_attribute_name(task: Task) -> str:
    """!

    CamelCase version of the task's name followed by TaskNumber. Words are
    separated by whitespace or underscores, and only the first letter of
    each word is capitalised, so existing camel case and acronyms are
    preserved. "EulerFVSmooth_Update cell" for example becomes
    EulerFVSmoothUpdateCellTaskNumber.

    """
    words = task.name.replace("_", " ").split()
    return "".join(word[:1].upper() + word[1:] for word in words) + "TaskNumber"


def _create_task_labels(identifier: str, split_tasks: List[Task]) -> "peano4.api.datamodel.DaStGen2":
    """!

    Create the DaStGen2 cell data type <identifier>_TaskLabels with one
    integer per split task. @ref translate()

    """
    task_labels = peano4.api.datamodel.DaStGen2(f"{identifier}_TaskLabels")
    for task in split_tasks:
        task_labels.data.add_attribute(dastgen2.attributes.Integer(name=_task_number_attribute_name(task)))
    return task_labels


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
    READ_AND_WRITE. @ref translate()

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

    Build the producer-consumer graph. @ref translate()

    @return (peano4.api.taskgraph.Graph, [(Task, Task)], [Task])
      The new graph, the list of (producer, consumer) pairs, which must not
      be fused, and the list of original tasks that have been split.

    """
    producer_of_task = {}
    consumer_of_task = {}
    for task in graph.tasks:
        producer_of_task[task] = _copy_task(task)
        if _exclusively_writes_cell_data(task):
            producer_of_task[task].name = f"{task.name} Producer"
            consumer = Task(name=f"{task.name} Consumer", entity=Task.GridEntity.CELL)
            for submodel in task.cell_data:
                if task.cell_data_access[submodel] in (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE):
                    consumer.use_cell(submodel, Task.DataAccess.READ_AND_WRITE)
            consumer_of_task[task] = consumer

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

    split_tasks = [task for task in graph.tasks if task in consumer_of_task]
    return result, non_fusable_task_pairs, split_tasks
