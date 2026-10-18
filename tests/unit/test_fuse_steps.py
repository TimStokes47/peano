from peano4.api.taskgraph import Graph, Task
from peano4.api.taskgraph.compiler import FuseSteps
from peano4.api.solversteps import SynchroniseActionSets, get_task_groups


VERTEX = Task.GridEntity.VERTEX
FACE = Task.GridEntity.FACE
CELL = Task.GridEntity.CELL

READ_AND_DISCARD = Task.DataAccess.READ_AND_DISCARD
WRITE = Task.DataAccess.WRITE
READ_AND_WRITE = Task.DataAccess.READ_AND_WRITE
TEMPORARY = Task.DataAccess.TEMPORARY


class _FakeActionSet:
    def __init__(self, name, descend_invocation_order=0):
        self.name = name
        self.descend_invocation_order = descend_invocation_order
        self.parallel = False

    def get_action_set_name(self):
        return self.name


class _FakeSubmodel:
    def __init__(self, name):
        self.name = name


def _chain(*entities):
    """A chain of tasks t0 -> t1 -> ... with the given grid entities"""
    graph = Graph()
    tasks = [Task(name=f"t{i}", entity=entity) for i, entity in enumerate(entities)]
    for previous_task, task in zip(tasks, tasks[1:]):
        task.add_dependency(previous_task)
    for task in tasks:
        task.add_action_set(_FakeActionSet(task.name))
        graph.add_task(task)
    return graph, tasks


def _groups(step):
    """Names of the action sets per task group of the generated observer"""
    return [[action_set.name for action_set in group] for group in get_task_groups(step.action_sets)]


def _step_names(graph, non_fusable_task_pairs=None):
    return [step.name for step in FuseSteps.translate(graph, "S", None, non_fusable_task_pairs)]


def test_entity_order_fuses_vertex_face_cell_but_not_cell_vertex():
    graph, _ = _chain(VERTEX, FACE, CELL, VERTEX, CELL)
    assert _step_names(graph) == ["S_T0__T1__T2", "S_T3__T4"]


def test_repeating_task_is_never_fused():
    graph = Graph()
    refine = Task("refine", CELL, task_has_to_repeat="condition")
    initial_condition = Task("init", CELL)
    initial_condition.add_dependency(refine)
    graph.add_task(refine)
    graph.add_task(initial_condition)
    steps = FuseSteps.translate(graph, "S", None)
    assert [step.name for step in steps] == ["S_Refine", "S_Init"]
    assert steps[0].has_to_repeat == "condition"


def test_non_fusable_pair_starts_new_traversal():
    graph, tasks = _chain(CELL, CELL)
    assert _step_names(graph) == ["S_T0__T1"]
    assert _step_names(graph, [(tasks[1], tasks[0])]) == ["S_T0", "S_T1"]


def test_cell_data_do_not_constrain_fusion():
    graph, tasks = _chain(CELL, CELL)
    patch = _FakeSubmodel("Q")
    tasks[0].use_cell(patch, WRITE)
    tasks[1].use_cell(patch, READ_AND_WRITE)
    assert _step_names(graph) == ["S_T0__T1"]


def test_face_data_written_then_read_prevents_fusion():
    graph, tasks = _chain(CELL, CELL)
    face_data = _FakeSubmodel("F")
    tasks[0].use_face(face_data, WRITE)
    tasks[1].use_face(face_data, READ_AND_DISCARD)
    assert _step_names(graph) == ["S_T0", "S_T1"]


def test_face_data_read_then_written_prevents_fusion():
    """The reconstruction/projection pattern of a Finite Volume time step"""
    graph, tasks = _chain(CELL, CELL, CELL)
    face_data = _FakeSubmodel("F")
    tasks[0].use_face(face_data, READ_AND_DISCARD)
    tasks[2].use_face(face_data, WRITE)
    assert _step_names(graph) == ["S_T0__T1", "S_T2"]


def test_vertex_data_written_twice_prevents_fusion():
    graph, tasks = _chain(CELL, CELL)
    vertex_data = _FakeSubmodel("V")
    tasks[0].use_vertex(vertex_data, WRITE)
    tasks[1].use_vertex(vertex_data, WRITE)
    assert _step_names(graph) == ["S_T0", "S_T1"]


def test_face_data_read_twice_or_temporary_does_not_prevent_fusion():
    graph, tasks = _chain(CELL, CELL, CELL)
    face_data = _FakeSubmodel("F")
    tasks[0].use_face(face_data, READ_AND_DISCARD)
    tasks[1].use_face(face_data, READ_AND_DISCARD)
    tasks[2].use_face(face_data, TEMPORARY)
    assert _step_names(graph) == ["S_T0__T1__T2"]


def test_independent_tasks_form_one_parallel_node():
    graph = Graph()
    first, second = Task("a", VERTEX), Task("b", CELL)
    for task in (first, second):
        task.add_action_set(_FakeActionSet(task.name))
        graph.add_task(task)
    steps = FuseSteps.translate(graph, "S", None)
    assert [step.name for step in steps] == ["S_A__B"]
    assert _groups(steps[0]) == [["a", "b"]]
    assert all(action_set.parallel for action_set in steps[0].action_sets)


def test_independent_tasks_with_conflicting_data_are_not_parallel():
    graph = Graph()
    first, second = Task("a", CELL), Task("b", CELL)
    face_data = _FakeSubmodel("F")
    first.use_face(face_data, WRITE)
    second.use_face(face_data, READ_AND_DISCARD)
    for task in (first, second):
        task.add_action_set(_FakeActionSet(task.name))
        graph.add_task(task)
    steps = FuseSteps.translate(graph, "S", None)
    assert [step.name for step in steps] == ["S_A", "S_B"]
    assert not any(action_set.parallel for step in steps for action_set in step.action_sets)


def test_independent_tasks_with_conflicting_cell_data_are_fused_but_not_parallel():
    graph = Graph()
    first, second, third = Task("a", CELL), Task("b", CELL), Task("c", CELL)
    cell_data = _FakeSubmodel("Q")
    first.use_cell(cell_data, READ_AND_WRITE)
    second.use_cell(cell_data, READ_AND_WRITE)
    third.use_cell(cell_data, READ_AND_DISCARD)
    for task in (first, second, third):
        task.add_action_set(_FakeActionSet(task.name))
        graph.add_task(task)
    steps = FuseSteps.translate(graph, "S", None)
    assert [step.name for step in steps] == ["S_A__B__C"]
    assert _groups(steps[0]) == [["a"], ["b"], ["c"]]
    assert not any(action_set.parallel for action_set in steps[0].action_sets)


def test_independent_tasks_reading_the_same_cell_data_are_parallel():
    graph = Graph()
    first, second = Task("a", CELL), Task("b", CELL)
    cell_data = _FakeSubmodel("Q")
    first.use_cell(cell_data, READ_AND_DISCARD)
    second.use_cell(cell_data, READ_AND_DISCARD)
    for task in (first, second):
        task.add_action_set(_FakeActionSet(task.name))
        graph.add_task(task)
    steps = FuseSteps.translate(graph, "S", None)
    assert _groups(steps[0]) == [["a", "b"]]


def test_adjacent_parallel_nodes_are_separated_by_a_synchronisation_marker():
    graph = Graph()
    v1, v2, c1, c2 = Task("v1", VERTEX), Task("v2", VERTEX), Task("c1", CELL), Task("c2", CELL)
    c1.add_dependency(v1)
    c2.add_dependency(v2)
    for task in (v1, v2, c1, c2):
        task.add_action_set(_FakeActionSet(task.name))
        graph.add_task(task)
    steps = FuseSteps.translate(graph, "S", None)
    assert [step.name for step in steps] == ["S_V1__V2__C1__C2"]
    assert [type(action_set) for action_set in steps[0].action_sets].count(SynchroniseActionSets) == 1
    assert isinstance(steps[0].action_sets[2], SynchroniseActionSets)
    assert _groups(steps[0]) == [["v1", "v2"], ["c1", "c2"]]


def test_task_with_several_action_sets_is_never_part_of_a_parallel_node():
    graph = Graph()
    first, second = Task("a", CELL), Task("b", CELL)
    first.add_action_set(_FakeActionSet("a1"))
    first.add_action_set(_FakeActionSet("a2"))
    second.add_action_set(_FakeActionSet("b"))
    graph.add_task(first)
    graph.add_task(second)
    steps = FuseSteps.translate(graph, "S", None)
    assert [step.name for step in steps] == ["S_A__B"]
    assert not any(action_set.parallel for action_set in steps[0].action_sets)
    assert _groups(steps[0]) == [["a1"], ["a2"], ["b"]]
