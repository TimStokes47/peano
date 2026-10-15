# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import copy
import re
from typing import List

import dastgen2

import peano4.api
import peano4.api.datamodel
import peano4.api.solversteps

from ..Graph import Graph
from ..Task import Task
from . import FuseSteps
from . import Sequential
from .SpawnCellTaskActionSet import SpawnCellTaskActionSet
from .WaitForCellTaskActionSet import WaitForCellTaskActionSet


def translate(
    graph: Graph,
    identifier: str,
    peano_project: "peano4.api.Project",
    use_fusion: bool = False,
) -> List["peano4.api.solversteps.Step"]:
    """!

    Task graph compiler which splits tasks writing cell data into a producer spawning tasks and a consumer waiting for them (work in progress)

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
    WRITE or READ_AND_WRITE (@ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer._exclusively_writes_cell_data()). It may
    read face or vertex data. All other tasks are kept as they are. The
    two tasks are:

    1. The producer, named after the original task plus "Producer". It
       inherits all action sets and data declarations of the original
       task.
    2. The consumer, named after the original task plus "Consumer". It
       works on cells, too, and depends on the producer. It declares every
       cell submodel that the producer writes (WRITE or READ_AND_WRITE) as
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

    ## Spawning tasks

    If a peano_project is given, the producer spawns the touchCellFirstTime()
    events of its action sets as tasks, and the consumer waits for these
    tasks (@ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer._spawn_tasks()):

    - Every action set of the producer is wrapped into a
      @ref peano4.api.taskgraph.compiler.SpawnCellTaskActionSet.SpawnCellTaskActionSet.
      If the action set's touchCellFirstTime() has a non-empty body, the
      wrapper evaluates the guard. If it holds, the wrapper takes a new
      task number from the tarch, stores it in the cell's task label (see
      below), and spawns the body as a task with this number. The task
      depends on the task spawned by the previous action set of the same
      producer, so the bodies are executed in their original order.
    - The consumer is given a cousin
      @ref peano4.api.taskgraph.compiler.WaitForCellTaskActionSet.WaitForCellTaskActionSet
      per producer action set. If the cell's task label holds a valid task
      number, it waits for this task and resets the label to NoNumber=-1.
    - unprepareTraversal() and all touch...LastTime() events of the
      producer's action sets are moved into the consumer's cousins, i.e.
      they are invoked once the tasks have terminated.
    - The cousin's generated C++ class derives from the producer's
      generated class. The two hence share all static attributes, e.g.
      reductions evaluated by unprepareTraversal(). Non-static attributes
      are not preserved from the producer's sweep to the consumer's sweep.

    The spawned tasks outlive touchCellFirstTime(). The compiler therefore
    warns about constructs which are not safe within a task. Every warning
    has to be accepted or the generation is aborted
    (@ref peano4.api.Logger.Logger.warn_and_confirm()):

    - The task body accesses cell data which is held in a native array
      (@ref peano4.api.datamodel.PatchToDoubleArray) rather than via a
      smart pointer. The task copies the whole array, and writes to the copy
      are lost.
    - The task body accesses vertex, face or coarse grid data. The
      enumerators hold pointers into Peano's stacks, which are invalid
      once touchCellFirstTime() has terminated.
    - The task body or a moved event uses a non-static attribute of the
      action set. The task accesses an object which is destroyed after the
      producer's mesh traversal, and the moved events see a new object.
    - An action set of the producer has to be modified by the user
      (@ref peano4.api.solversteps.ActionSet.ActionSet.user_should_modify_template()).
      Such a producer is not altered, i.e. it spawns no tasks.

    ## Task labels

    The compiler adds a new cell data type <identifier>_TaskLabels to the
    project's data model. It is modelled via DaStGen2 and holds one integer
    per producer action set with a non-empty touchCellFirstTime(). The
    integer's name is the CamelCase version of the original task's name
    followed by ActionSet, the action set's index within the task and
    TaskNumber, e.g. EulerFVSmoothInitialConditionActionSet0TaskNumber
    (@ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer._task_number_attribute_name()).
    The producer declares the task labels as WRITE, the consumer as
    READ_AND_WRITE.

    @param graph: peano4.api.taskgraph.Graph
      The task graph to compile. Assumed to be acyclic.

    @param identifier: str
      @ref peano4.api.taskgraph.compiler.FuseSteps.translate()

    @param peano_project: peano4.api.Project
      Project the task labels are added to. If None, no task labels are
      added and the producers spawn no tasks.

    @param use_fusion: bool
      If True, hand the rewritten graph over to FuseSteps, otherwise to
      Sequential.

    @return [peano4.api.solversteps.Step]
      @ref peano4.api.taskgraph.compiler.FuseSteps.translate()

    """
    producer_consumer_graph, split_tasks = _split_cell_tasks(graph)

    if peano_project is not None and split_tasks:
        task_labels = peano4.api.datamodel.DaStGen2(f"{identifier}_TaskLabels")
        if _spawn_tasks(split_tasks, task_labels) > 0:
            peano_project.datamodel.add_cell(task_labels)

    non_fusable_task_pairs = [(producer, consumer) for _, producer, consumer in split_tasks]
    if use_fusion:
        return FuseSteps.translate(producer_consumer_graph, identifier, peano_project, non_fusable_task_pairs)
    else:
        return Sequential.translate(producer_consumer_graph, identifier, peano_project)


def _task_number_attribute_name(task: Task, action_set_index: int) -> str:
    """!

    CamelCase version of the task's name followed by ActionSet, the action
    set index and TaskNumber. Words are separated by whitespace or
    underscores, and only the first letter of each word is capitalised, so
    existing camel case and acronyms are preserved. "EulerFVSmooth_Update
    cell" with action set index 0 for example becomes
    EulerFVSmoothUpdateCellActionSet0TaskNumber.

    """
    words = task.name.replace("_", " ").split()
    return "".join(word[:1].upper() + word[1:] for word in words) + f"ActionSet{action_set_index}TaskNumber"


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
    READ_AND_WRITE. @ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate()

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

    Build the producer-consumer graph. @ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate()

    @return (peano4.api.taskgraph.Graph, [(Task, Task, Task)])
      The new graph and the list of (original task, producer, consumer)
      triples. Producer and consumer must not be fused.

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

    split_tasks = [(task, producer_of_task[task], consumer_of_task[task]) for task in graph.tasks if task in consumer_of_task]
    return result, split_tasks


def _non_static_attribute_names(attributes: str) -> List[str]:
    """!

    Names of the non-static attributes declared in the C++ snippet returned
    by an action set's get_attributes(). This is a heuristic: We split the
    snippet into statements, skip all statements which are static or
    contain a bracket (functions), and take the last identifier before an
    optional array size or initialisation.

    """
    attributes = re.sub(r"/\*.*?\*/", "", attributes, flags=re.S)
    attributes = re.sub(r"//[^\n]*", "", attributes)
    result = []
    for statement in attributes.split(";"):
        words = statement.split()
        if not words or "static" in words or "(" in statement:
            continue
        match = re.search(r"([A-Za-z_]\w*)\s*(\[[^\]]*\]\s*)*(=.*)?$", statement.strip(), re.S)
        if match:
            result.append(match.group(1))
    return result


def _warn_about_unsafe_task(task: Task, action_set_index: int, action_set: "peano4.api.solversteps.ActionSet", task_body: str):
    """!

    Issue the warnings about task bodies and moved events which are not
    safe. @ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate()

    """
    logger = peano4.api.Logger()
    description = f"action set {action_set_index} ({action_set.get_action_set_name()}) of task {task.name}"

    for submodel in task.cell_data:
        if type(submodel.generator) is peano4.api.datamodel.PatchToDoubleArray and f"fineGridCell{submodel.name}" in task_body:
            logger.warn_and_confirm(
                f"The touchCellFirstTime() task of {description} accesses cell data {submodel.name}, which is held in a native "
                f"array. The task works on a copy of the array, i.e. all writes are lost. Use a generator with a smart pointer, "
                f"e.g. peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer."
            )

    for accessor in ("fineGridVertices", "fineGridFaces", "coarseGridVertices", "coarseGridFaces", "coarseGridCell"):
        if accessor in task_body:
            logger.warn_and_confirm(
                f"The touchCellFirstTime() task of {description} accesses {accessor}. The enumerators hold pointers into Peano's "
                f"stacks, which are invalid once touchCellFirstTime() has terminated."
            )

    moved_events = "".join(
        action_set.get_implementation_of_operation(operation_name) for operation_name in SpawnCellTaskActionSet.MovedOperations
    )
    for attribute_name in _non_static_attribute_names(action_set.get_attributes()):
        if re.search(rf"\b{attribute_name}\b", task_body):
            logger.warn_and_confirm(
                f"The touchCellFirstTime() task of {description} uses the non-static attribute {attribute_name}. The task accesses "
                f"an action set object which is destroyed after the producer's mesh traversal."
            )
        if re.search(rf"\b{attribute_name}\b", moved_events):
            logger.warn_and_confirm(
                f"The touch...LastTime() events of {description} use the non-static attribute {attribute_name}. These events are "
                f"moved into the consumer, which works with a new action set object, i.e. the attribute's value is not preserved."
            )


def _spawn_tasks(split_tasks, task_labels: "peano4.api.datamodel.DaStGen2"):
    """!

    Make the producers spawn tasks and the consumers wait for them, and add
    one task number per spawning action set to the task labels.
    @ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate()

    @param split_tasks: [(Task, Task, Task)]
      Triples of original task, producer and consumer.

    @param task_labels: peano4.api.datamodel.DaStGen2
      The task labels. This routine adds the attributes.

    @return int
      Number of attributes added to the task labels.

    """
    logger = peano4.api.Logger()
    number_of_task_numbers = 0
    for task, producer, consumer in split_tasks:
        user_action_sets = [action_set for action_set in task.action_sets if action_set.user_should_modify_template()]
        if user_action_sets:
            logger.warn_and_confirm(
                f"Task {task.name} holds action sets which the user has to modify ("
                + ", ".join(action_set.get_action_set_name() for action_set in user_action_sets)
                + "). The task is split into producer and consumer, but spawns no tasks."
            )
            continue

        producer_action_sets = []
        previous_task_number_attribute_name = None
        moves_events = False
        for action_set_index, action_set in enumerate(task.action_sets):
            task_body = action_set.get_body_of_operation(peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME)
            task_number_attribute_name = None
            if task_body.strip() != "":
                _warn_about_unsafe_task(task, action_set_index, action_set, task_body)
                task_number_attribute_name = _task_number_attribute_name(task, action_set_index)
                task_labels.data.add_attribute(dastgen2.attributes.Integer(name=task_number_attribute_name))
                number_of_task_numbers += 1
            else:
                _warn_about_unsafe_task(task, action_set_index, action_set, "")
            producer_action_sets.append(
                SpawnCellTaskActionSet(
                    action_set,
                    task_labels.name,
                    task_number_attribute_name,
                    previous_task_number_attribute_name,
                )
            )
            if task_number_attribute_name is not None:
                previous_task_number_attribute_name = task_number_attribute_name
            moves_events = moves_events or any(
                action_set.get_implementation_of_operation(operation_name).strip() != ""
                for operation_name in SpawnCellTaskActionSet.MovedOperations
            )

        producer.action_sets = producer_action_sets
        consumer.action_sets = [WaitForCellTaskActionSet(producer_action_set) for producer_action_set in producer_action_sets]

        if previous_task_number_attribute_name is not None:
            producer.use_cell(task_labels, Task.DataAccess.WRITE)
            consumer.use_cell(task_labels, Task.DataAccess.READ_AND_WRITE)

        # The moved touch...LastTime() events might read all the producer's
        # data, so the consumer has to declare it as well
        if moves_events:
            for submodel in task.cell_data:
                if submodel not in consumer.cell_data:
                    consumer.use_cell(submodel, Task.DataAccess.READ_AND_DISCARD)
            for submodel in task.face_data:
                consumer.use_face(submodel, Task.DataAccess.READ_AND_DISCARD)
            for submodel in task.vertex_data:
                consumer.use_vertex(submodel, Task.DataAccess.READ_AND_DISCARD)

    return number_of_task_numbers
