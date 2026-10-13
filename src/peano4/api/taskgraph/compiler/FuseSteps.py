# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List

import peano4.api
import peano4.api.solversteps

from ..Graph import Graph
from ..Task import Task
from .ReportCompiledStepSequence import report_compiled_step_sequence


def translate(graph: Graph, identifier: str) -> List["peano4.api.solversteps.Step"]:
    """!

    Task graph compiler which fuses tasks into as few mesh traversals as possible

    Where peano4.api.taskgraph.compiler.Sequential.translate() maps every
    task onto a mesh traversal (Observer) of its own, this compiler packs
    several tasks into one traversal whenever the order in which a
    traversal visits the mesh entities permits it.

    ## The fusion constraint

    Each grid sweep first does something with the vertices, then with the
    faces, then with the cells (@see Task.GridEntity, whose values follow
    exactly this order). A task that works on a face can therefore be
    followed by a task working on a face or a cell within the same
    traversal, but a task working on a vertex has to go into the next
    traversal. In general, a node can be appended to the traversal built
    up so far if and only if the lowest grid entity it works on is not
    lower than the highest grid entity the traversal works on so far.

    ## Algorithm

    1. Split the graph into layers by dependency depth (a task without
       dependencies has depth 0, every other task 1 + the deepest of its
       dependencies - the same notion of depth Graph.visualise() uses).
       Tasks within one layer can never depend on one another, i.e. they
       can run in parallel. Within a layer, tasks keep the order in which
       Graph.ordered_tasks() lists them.
    2. All tasks of one layer are first combined into one intermediate
       node. Such a node might work on several grid entities. If it holds
       more than one task, every action set of these tasks is labelled as
       parallel (@see peano4.api.solversteps.ActionSet.parallel), i.e.
       they may be invoked concurrently.
    3. The resulting sequence of nodes is fused greedily into traversals
       subject to the fusion constraint above.

    Tasks which have to repeat (Task.has_to_repeat) are never fused: they
    are removed from their layer before step 2 and always yield a
    traversal of their own, placed right before whatever the remaining
    tasks of their layer yield.

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
    parallel, is recorded in the step's action_set_numbers - @see
    _create_step().

    A submodel used by several tasks of a fused step is declared once on
    the step. Its qualifier is merged from the tasks' qualifiers: TEMPORARY
    uses are ignored, the step loads the submodel if the first remaining
    use reads it, and stores it if the last remaining use writes it -
    @see _merge_data_access().

    propagate_use_statements() is not invoked here. It is a postprocessing
    step called outside of the translation.

    graph: peano4.api.taskgraph.Graph
      The task graph to compile. Assumed to be acyclic.

    identifier: str
      Identifies which of the surrounding project's algorithmic phases this
      graph belongs to (e.g. "SetupSimulation", "TimeStep", "Plot") and
      prefixes every generated Observer's name.

    @return [peano4.api.solversteps.Step]
      One Observer per fused group of tasks, in an order that respects
      every dependency.

    """
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

    # Step 1 and 2: one node (list of tasks) per layer, plus one node per
    # repeating task
    nodes = []
    for layer in sorted(layers.keys()):
        tasks_in_layer = layers[layer]
        for task in tasks_in_layer:
            if task.has_to_repeat:
                nodes.append([task])
        parallel_tasks = [task for task in tasks_in_layer if not task.has_to_repeat]
        if len(parallel_tasks) > 1:
            for task in parallel_tasks:
                for action_set in task.action_sets:
                    action_set.parallel = True
        if parallel_tasks:
            nodes.append(parallel_tasks)

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

    Create one step from a group of fused nodes. @see translate()

    Every node is a list of tasks: either a single task, or several tasks
    from one layer of the task graph which can run in parallel.

    Besides the action sets, the step is also given an attribute
    action_set_numbers which holds one invocation number per action set
    (starting at 1). Action sets that are invoked one after the other have
    increasing numbers, whereas all action sets of one parallel node share
    the same number. This attribute is purely used for reporting (@see
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
