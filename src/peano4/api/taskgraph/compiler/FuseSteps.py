# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List, Optional, Tuple

import peano4.api
import peano4.api.solversteps

from ..Graph import Graph
from ..Task import Task
from .ReportCompiledStepSequence import report_compiled_step_sequence


def translate(
    graph: Graph,
    identifier: str,
    peano_project: "peano4.api.Project",
    non_fusable_task_pairs: Optional[List[Tuple[Task, Task]]] = None,
) -> List["peano4.api.solversteps.Step"]:
    """!

    Task graph compiler which fuses tasks into as few mesh traversals as possible

    You can use this task graph compiler in your ExaHyPE 2 project through

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    project.compiler = peano4.api.taskgraph.compiler.FuseSteps.translate
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    Where peano4.api.taskgraph.compiler.Sequential.translate() maps every
    task onto a mesh traversal (Observer) of its own, this compiler packs
    several tasks into one traversal whenever this is safe.

    ## When can two tasks be fused?

    Two tasks end up in the same traversal if and only if all of the
    following constraints hold:

    1. **Entity order.** Each grid sweep first does something with the
       vertices, then with the faces, then with the cells (@ref
       peano4.api.taskgraph.Task.Task.GridEntity, whose values follow exactly this order). A task that
       works on a face can therefore be followed by a task working on a face
       or a cell within the same traversal, but a task working on a vertex
       has to go into the next traversal. In general, a node can be appended
       to the traversal built up so far if and only if the lowest grid
       entity it works on is not lower than the highest grid entity the
       traversal works on so far. Tasks of one dependency layer (see
       below) are independent of each other, so this constraint does not
       apply among them.
    2. **No conflicting face or vertex data.** A face or a vertex is shared
       between several cells. If one task of a traversal writes a face or
       vertex submodel (WRITE or READ_AND_WRITE) while another task of the
       same traversal reads or writes the very same submodel, a cell might
       see the submodel before or after a neighbouring cell has already
       altered it, depending on the traversal order. Such tasks are never
       fused (@ref peano4.api.taskgraph.compiler.FuseSteps._have_conflicting_data()). Cell data are not shared
       between cells and hence do not constrain the fusion. TEMPORARY uses
       are ignored. This constraint is conservative: it relies only on the
       Task.DataAccess qualifiers and does not know that a task might for
       example write the face data only when it touches a face for the
       last time.
    3. **No repetition.** Tasks which have to repeat (Task.has_to_repeat)
       are never fused. They always yield a traversal of their own.
    4. **No annotation.** The caller can annotate the fusion through
       non_fusable_task_pairs: two tasks forming such a pair never end up
       in the same traversal.

<br /><br />
<div style="background-color: #fcc ; padding: 10px; border: 1px solid green;"> 
A task does not know if its action sets plug into touch-first or touch-last
events. That is, a task A1 might use touchVertexFirstTime() and touchVertexLastTime(),
while its successor task B1 only uses touchCellFirstTime(). In this case, the 
tasks will not be fused if A1 writes vertex data that B1 reads. Conversely, let A2 only plug into 
touchVertexFirstTime(), while B2 sticks to touchCellFirstTime(). Now, we could fuse.
But this option is hidden from the task compiler.
</div>


    ## Algorithm

    1. Split the graph into layers by dependency depth (a task without
       dependencies has depth 0, every other task 1 + the deepest of its
       dependencies - the same notion of depth Graph.visualise() uses).
       Tasks within one layer can never depend on one another, i.e. they
       can run in parallel. Within a layer, tasks keep the order in which
       Graph.ordered_tasks() lists them.
    2. The tasks of one layer are combined into intermediate nodes. A
       repeating task forms a node of its own, placed in front of the
       layer's remaining nodes. The remaining tasks are added one by one to
       the layer's current node unless they have conflicting data with it
       (constraint 2) or form a non-fusable pair with one of its tasks
       (constraint 4). In this case, they start a new node. A node might
       work on several grid entities. If a node holds more than one task,
       every action set of these tasks is labelled as parallel (@ref
       peano4.api.solversteps.ActionSet.ActionSet.parallel), i.e. they may be invoked
       concurrently.
    3. The resulting sequence of nodes is fused greedily into traversals:
       a node is appended to the current traversal if constraints 1 to 4
       hold for the node and all tasks already in the traversal. Otherwise,
       it starts a new traversal. The fusion never reorders nodes and never
       looks further ahead.

    ## Examples

    The illustrations below are generated by
    src/peano4/api/taskgraph/compiler/fuse-steps-illustrations.py. Each
    shows the input task graph on top and the compiled sequence of
    traversals underneath.

    ### Entity order

    A chain of tasks working on a vertex, a face, a cell, a vertex and a
    cell. The vertex, face and cell tasks are fused into one traversal.
    The second vertex task has to start a new traversal, which also takes
    the subsequent cell task.

    @image html fuse-steps-entity-order.png

    ### Parallel tasks

    Two independent vertex tasks form one layer and hence one parallel
    node. They are fused with the two independent cell tasks of the
    subsequent layer. Action sets with the same number run in parallel.

    @image html fuse-steps-parallel.png

    ### Conflicting face data

    The time step of an ExaHyPE 2 Finite Volume solver: the reconstruction
    reads the face data that the projection eventually writes. Therefore,
    the projection cannot be fused with the reconstruction and the update,
    and starts a traversal of its own.

    @image html fuse-steps-data-conflict.png

    ## Constructing the fused step

    A fused step's name concatenates the names of its tasks, prefixed with
    identifier. Its action sets are the tasks' action sets one after the
    other, so the evaluation order of the tasks is preserved. We do not use
    peano4.api.solversteps.Step.add_action_set() for this, as it reorders
    the action sets according to their descend_invocation_order: a
    reconstruction with a higher descend_invocation_order would then
    overtake the update of the subsequent task. The descend_invocation_order
    is applied (stably) only among the action sets of one task. The
    resulting invocation order, including which action sets run in
    parallel, is recorded in the step's action_set_numbers - @ref
    peano4.api.taskgraph.compiler.FuseSteps._create_step().

    A submodel used by several tasks of a fused step is declared once on
    the step. Its qualifier is merged from the tasks' qualifiers: TEMPORARY
    uses are ignored, the step loads the submodel if the first remaining
    use reads it, and stores it if the last remaining use writes it -
    @ref peano4.api.taskgraph.compiler.FuseSteps._merge_data_access().

    propagate_use_statements() is not invoked here. It is a postprocessing
    step called outside of the translation.

    @param graph: peano4.api.taskgraph.Graph
      The task graph to compile. Assumed to be acyclic.

    @param identifier: str
      Identifies which of the surrounding project's algorithmic phases this
      graph belongs to (e.g. "SetupSimulation", "TimeStep", "Plot") and
      prefixes every generated Observer's name.

    @param peano_project: peano4.api.Project
      Not used by this compiler. @ref peano4.api.taskgraph.compiler

    @param non_fusable_task_pairs: [(peano4.api.taskgraph.Task, peano4.api.taskgraph.Task)]
      Pairs of tasks of graph which must not end up in the same traversal.
      The order within a pair does not matter. Defaults to none.

    @return [peano4.api.solversteps.Step]
      One Observer per fused group of tasks, in an order that respects
      every dependency.

    """
    non_fusable = set()
    for first_task, second_task in non_fusable_task_pairs or []:
        non_fusable.add((first_task, second_task))
        non_fusable.add((second_task, first_task))

    def can_be_combined(tasks: List[Task], further_tasks: List[Task]) -> bool:
        """Constraints 2 and 4 between two lists of tasks"""
        return not _have_conflicting_data(tasks, further_tasks) and not any(
            (task, further_task) in non_fusable for task in tasks for further_task in further_tasks
        )

    order = graph.ordered_tasks()

    depth_of_task = {}

    def depth(task: Task) -> int:
        if task not in depth_of_task:
            depth_of_task[task] = (
                0 if not task.dependencies else 1 + max(depth(dependency) for dependency in task.dependencies)
            )
        return depth_of_task[task]

    layers = {}
    for task in order:
        layers.setdefault(depth(task), []).append(task)

    # Step 1 and 2: nodes (lists of tasks) per layer
    nodes = []
    for layer in sorted(layers.keys()):
        tasks_in_layer = layers[layer]
        for task in tasks_in_layer:
            if task.has_to_repeat:
                nodes.append([task])
        nodes_of_layer = []
        for task in tasks_in_layer:
            if task.has_to_repeat:
                continue
            if nodes_of_layer and can_be_combined(nodes_of_layer[-1], [task]):
                nodes_of_layer[-1].append(task)
            else:
                nodes_of_layer.append([task])
        for node in nodes_of_layer:
            if len(node) > 1:
                for task in node:
                    for action_set in task.action_sets:
                        action_set.parallel = True
        nodes += nodes_of_layer

    # Step 3: greedy fusion along the sequence of nodes
    groups = []
    current_group = None
    highest_entity_of_current_group = None
    for node in nodes:
        lowest_entity = min(task.entity.value for task in node)
        highest_entity = max(task.entity.value for task in node)
        is_repeating = len(node) == 1 and node[0].has_to_repeat
        if (
            not is_repeating
            and current_group is not None
            and lowest_entity >= highest_entity_of_current_group
            and can_be_combined([task for group_node in current_group for task in group_node], node)
        ):
            current_group.append(node)
            highest_entity_of_current_group = max(highest_entity_of_current_group, highest_entity)
        else:
            current_group = [node]
            highest_entity_of_current_group = highest_entity
            groups.append(current_group)
        if is_repeating:
            current_group = None

    steps = [_create_step(group, identifier) for group in groups]

    peano4.api.Logger().new_section(f"Compile task graph '{identifier}'")
    report_compiled_step_sequence(steps, title=f"Compiled task graph for {identifier}")

    return steps


def _have_conflicting_data(tasks: List[Task], further_tasks: List[Task]) -> bool:
    """!

    Check whether two sets of tasks can not share one mesh traversal due to
    their face or vertex data

    True if one of the tasks writes a face or vertex submodel (WRITE or
    READ_AND_WRITE) which one of the further_tasks reads or writes
    (READ_AND_DISCARD, WRITE or READ_AND_WRITE), or vice versa. Cell data
    and TEMPORARY uses are ignored. @ref peano4.api.taskgraph.compiler.FuseSteps.translate()

    """
    writing_accesses = (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE)
    using_accesses = (Task.DataAccess.READ_AND_DISCARD, Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE)

    def submodels(task_list, accesses):
        return {
            (kind, submodel)
            for task in task_list
            for kind, access_of_submodel in (("face", task.face_data_access), ("vertex", task.vertex_data_access))
            for submodel, access in access_of_submodel.items()
            if access in accesses
        }

    return bool(
        submodels(tasks, writing_accesses) & submodels(further_tasks, using_accesses)
        or submodels(further_tasks, writing_accesses) & submodels(tasks, using_accesses)
    )


def _task_name(task: Task) -> str:
    """!

    Turn every whitespace-separated word into its own name segment,
    capitalising only its first letter - a blind .title() would also
    lower-case the rest of the word, which mangles existing camel case and
    acronyms (e.g. "EulerFVSmooth" would become "Eulerfvsmooth").

    """
    return "_".join(word[:1].upper() + word[1:] for word in task.name.split())


def _merge_data_access(accesses: List["Task.DataAccess"]) -> "Task.DataAccess":
    """!

    Merge the qualifiers several fused tasks attach to one submodel into one

    TEMPORARY uses are ignored. The fused step has to load the submodel if
    the first remaining use reads it (READ_AND_DISCARD or READ_AND_WRITE),
    and it has to store the submodel if the last remaining use writes it
    (WRITE or READ_AND_WRITE):

    | load | store | merged qualifier |
    |------|-------|------------------|
    | yes  | yes   | READ_AND_WRITE   |
    | yes  | no    | READ_AND_DISCARD |
    | no   | yes   | WRITE            |
    | no   | no    | TEMPORARY        |

    A submodel produced by one fused task and consumed by a later one
    (WRITE followed by READ_AND_DISCARD) hence becomes TEMPORARY. For a
    single access, the merged qualifier equals the access itself.

    """
    relevant_accesses = [access for access in accesses if access != Task.DataAccess.TEMPORARY]
    if not relevant_accesses:
        return Task.DataAccess.TEMPORARY
    load = relevant_accesses[0] in (Task.DataAccess.READ_AND_DISCARD, Task.DataAccess.READ_AND_WRITE)
    store = relevant_accesses[-1] in (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE)
    if load and store:
        return Task.DataAccess.READ_AND_WRITE
    if load:
        return Task.DataAccess.READ_AND_DISCARD
    if store:
        return Task.DataAccess.WRITE
    return Task.DataAccess.TEMPORARY


def _create_step(nodes: List[List[Task]], identifier: str) -> "peano4.api.solversteps.Step":
    """!

    Create one step from a group of fused nodes. @ref peano4.api.taskgraph.compiler.FuseSteps.translate()

    Every node is a list of tasks: either a single task, or several tasks
    from one layer of the task graph which can run in parallel.

    Besides the action sets, the step is also given an attribute
    action_set_numbers which holds one invocation number per action set
    (starting at 1). Action sets that are invoked one after the other have
    increasing numbers, whereas all action sets of one parallel node share
    the same number. This attribute is purely used for reporting (@ref
    peano4.api.taskgraph.compiler.ReportCompiledStepSequence).

    """
    tasks = [task for node in nodes for task in node]
    step = peano4.api.solversteps.Step(
        name=f"{identifier}_" + "__".join(_task_name(task) for task in tasks),
        add_user_defined_actions=False,
    )
    # Repeating tasks are never fused, i.e. a group with a repeating task
    # holds exactly this one task
    step.has_to_repeat = tasks[0].has_to_repeat if len(tasks) == 1 else None

    for data_attribute, access_attribute, step_access, use in (
        ("cell_data", "cell_data_access", step.cell_data_access, step.use_cell),
        ("face_data", "face_data_access", step.face_data_access, step.use_face),
        ("vertex_data", "vertex_data_access", step.vertex_data_access, step.use_vertex),
    ):
        accesses_of_submodel = {}
        for task in tasks:
            for submodel in getattr(task, data_attribute):
                accesses_of_submodel.setdefault(submodel, []).append(getattr(task, access_attribute)[submodel])
        for submodel, accesses in accesses_of_submodel.items():
            use(submodel)
            step_access[submodel] = _merge_data_access(accesses)

    step.action_set_numbers = []
    number = 0
    for node in nodes:
        is_parallel = len(node) > 1
        if is_parallel:
            number += 1
        for task in node:
            for action_set in sorted(task.action_sets, key=lambda action_set: action_set.descend_invocation_order):
                if not is_parallel:
                    number += 1
                step.action_sets.append(action_set)
                step.action_set_numbers.append(number)

    return step
