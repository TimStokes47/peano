import peano4.api.solversteps
from peano4.api.taskgraph import Task
from peano4.api.taskgraph.compiler.PropagateUseStatements import propagate_use_statements

# propagate_use_statements() works on compiled step sequences, i.e. on lists
# of peano4.api.solversteps.Step. Below, a "graph" is such a list and its
# "tasks" are the Steps within it (one Step per task, as
# peano4.api.taskgraph.compiler.Sequential.translate() yields them).


READ_AND_DISCARD = Task.DataAccess.READ_AND_DISCARD
WRITE = Task.DataAccess.WRITE
READ_AND_WRITE = Task.DataAccess.READ_AND_WRITE
TEMPORARY = Task.DataAccess.TEMPORARY


class _FakeSubmodel:
    """
    Minimal stand-in for a peano4.api.datamodel.Patch: propagate_use_statements()
    only ever needs submodel identity, never anything Patch-specific.
    """

    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return self.name


def _chain_graph(name_prefix, length):
    """A step sequence of `length` steps. Returned twice: once as the
    sequence handed to propagate_use_statements(), once as the list of
    steps the tests inspect - both are the same list object."""
    steps = [
        peano4.api.solversteps.Step(name=f"{name_prefix}-{i}", add_user_defined_actions=False)
        for i in range(length)
    ]
    return steps, steps


def _use_cell(step, submodel, qualifier):
    step.use_cell(submodel)
    step.cell_data_access[submodel] = qualifier


def _use_face(step, submodel, qualifier):
    step.use_face(submodel)
    step.face_data_access[submodel] = qualifier


def _use_vertex(step, submodel, qualifier):
    step.use_vertex(submodel)
    step.vertex_data_access[submodel] = qualifier


def _access(task, attribute_name="cell_data_access"):
    return getattr(task, attribute_name)


# ---------------------------------------------------------------------------
# Single-graph, no edges: the "distance" rule itself.
# ---------------------------------------------------------------------------


def test_immediate_read_after_write_is_already_valid():
    """Distance 1, WRITE then READ_AND_DISCARD: nothing to do."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert tasks[1].cell_data.count(submodel) == 1  # not duplicated


def test_immediate_readwrite_after_write_is_already_valid():
    """Distance 1, WRITE then READ_AND_WRITE: also fine."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, READ_AND_WRITE)

    changed = propagate_use_statements([graph])

    assert changed is False


def test_immediate_write_after_write_supersedes_needs_no_relay():
    """Distance 1, WRITE then WRITE again: the second WRITE is a fresh,
    independent production (e.g. ProjectPatchOntoFaces recomputing face
    data from scratch every time step) - it does not need to read what
    the first WRITE produced, so there is nothing to protect and nothing
    to do, even though the two are immediately adjacent."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, WRITE)

    changed = propagate_use_statements([graph])

    assert changed is False


def test_write_then_write_then_read_finds_the_real_read_past_the_superseding_write():
    """A WRITE at N is immediately followed by another WRITE at N+1 - N's
    own search stops right there with nothing to do (superseded). N+1 is
    itself a WRITE, though, so it is independently checked too, on its
    own terms: N+1 to the READ_AND_DISCARD at N+2 is distance 1 (odd),
    already fine."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, WRITE)
    _use_cell(tasks[2], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False


def test_distance_two_inserts_readwrite_in_between():
    """Worked example 1: task N writes, task N+2 reads, nothing in between
    mentions the submodel (distance 2, even) - fixed by inserting a
    READ_AND_WRITE declaration on task N+1."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[2], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert submodel in tasks[1].cell_data
    assert _access(tasks[1])[submodel] == READ_AND_WRITE


def test_distance_three_needs_no_change():
    """Worked example 2: task N writes, task N+3 reads (distance 3, odd) -
    already fine as it stands."""
    graph, tasks = _chain_graph("A", 4)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[3], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert submodel not in tasks[1].cell_data
    assert submodel not in tasks[2].cell_data


def test_distance_four_inserts_readwrite_immediately_before_the_read():
    """Worked example 3: task N writes, task N+4 reads (distance 4, even) -
    fixed by inserting a READ_AND_WRITE declaration on task N+3, not N+1."""
    graph, tasks = _chain_graph("A", 5)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[4], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert submodel not in tasks[1].cell_data
    assert submodel not in tasks[2].cell_data
    assert submodel in tasks[3].cell_data
    assert _access(tasks[3])[submodel] == READ_AND_WRITE


def test_temporary_declaration_is_transparent_to_the_search():
    """A TEMPORARY use in between is treated exactly like a task that does
    not mention the submodel at all: task N writes, task N+1 already has
    a (user-declared) TEMPORARY use of the same submodel, task N+3 reads -
    the distance that matters is still N to N+3 (distance 3, odd, fine),
    not N to N+1 (which would wrongly look like an immediate non-read)."""
    graph, tasks = _chain_graph("A", 4)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, TEMPORARY)
    _use_cell(tasks[3], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert submodel not in tasks[2].cell_data


def test_read_origin_triggers_nothing():
    """A READ_AND_DISCARD never obliges anything downstream - even an
    immediately following task that does not mention the submodel at all
    is fine."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert submodel not in tasks[1].cell_data
    assert submodel not in tasks[2].cell_data


def test_temporary_origin_triggers_nothing():
    """Same for TEMPORARY."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, TEMPORARY)

    changed = propagate_use_statements([graph])

    assert changed is False


def test_readwrite_origin_is_treated_like_write():
    """READ_AND_WRITE both reads and writes, so - like a plain WRITE - it
    still obliges whatever comes next (distance 2 here, needs a relay)."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, READ_AND_WRITE)
    _use_cell(tasks[2], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert _access(tasks[1])[submodel] == READ_AND_WRITE


def test_is_idempotent():
    """A second call once everything is already admissible must change
    nothing and report False."""
    graph, tasks = _chain_graph("A", 5)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[4], submodel, READ_AND_DISCARD)

    first_changed = propagate_use_statements([graph])
    state_after_first = [list(task.cell_data) for task in tasks]
    second_changed = propagate_use_statements([graph])
    state_after_second = [list(task.cell_data) for task in tasks]

    assert first_changed is True
    assert second_changed is False
    assert state_after_first == state_after_second


def test_cell_face_and_vertex_are_propagated_independently():
    graph, tasks = _chain_graph("A", 3)
    cell_submodel = _FakeSubmodel("Cell")
    face_submodel = _FakeSubmodel("Face")
    vertex_submodel = _FakeSubmodel("Vertex")
    _use_cell(tasks[0], cell_submodel, WRITE)
    _use_cell(tasks[2], cell_submodel, READ_AND_DISCARD)
    _use_face(tasks[0], face_submodel, WRITE)
    _use_face(tasks[2], face_submodel, READ_AND_DISCARD)
    _use_vertex(tasks[0], vertex_submodel, WRITE)
    _use_vertex(tasks[2], vertex_submodel, READ_AND_DISCARD)

    propagate_use_statements([graph])

    assert tasks[1].cell_data == [cell_submodel]
    assert tasks[1].face_data == [face_submodel]
    assert tasks[1].vertex_data == [vertex_submodel]


def test_graph_with_no_edges_is_analysed_on_its_own():
    """A graph does not need any edge at all to be analysed - it is simply
    treated as a self-contained sequence, exactly like the old
    PropagateUseStatements used to for a single already-compiled Step
    sequence."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[2], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])  # no edges at all

    assert changed is True
    assert submodel in tasks[1].cell_data


# ---------------------------------------------------------------------------
# Cross-graph edges, including self-edges (replacing the old Graph.cyclic).
# ---------------------------------------------------------------------------


def test_gap_across_a_cross_graph_edge_is_fixed_in_the_right_graph():
    """A's last task writes; B's first task is at distance 2 across the
    (A, B) edge (one silent task in between, in A) - the relay has to be
    inserted into A (the task exactly one before B's first task), not B."""
    graph_a, tasks_a = _chain_graph("A", 2)
    graph_b, tasks_b = _chain_graph("B", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks_a[0], submodel, WRITE)
    _use_cell(tasks_b[0], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph_a, graph_b], edges=[(graph_a, graph_b)])

    assert changed is True
    assert submodel in tasks_a[1].cell_data
    assert _access(tasks_a[1])[submodel] == READ_AND_WRITE
    assert tasks_b[0].cell_data.count(submodel) == 1  # unchanged, still just the original read


def test_self_edge_propagates_across_the_wrap_around():
    """A (graph, graph) self-edge is what replaces the old Graph.cyclic
    flag: a graph's last task writing and its own first task reading, at
    distance 1 across the wrap-around, is already valid."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, READ_AND_DISCARD)
    _use_cell(tasks[2], submodel, WRITE)

    changed = propagate_use_statements([graph], edges=[(graph, graph)])

    assert changed is False  # distance from tasks[2] to tasks[0] (wrap) is 1, and tasks[0] reads


def test_self_edge_wrap_around_gap_is_fixed():
    """Same wrap-around, but with a genuine gap needing a relay: the last
    task writes, the graph's own first task is silent, and the real reader
    is the *second* task - distance 2 via the wrap (last -> first (silent,
    distance 1) -> second (read, distance 2)), matching worked example 1
    but reached through the self-edge instead of a plain successor."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[1], submodel, READ_AND_DISCARD)
    _use_cell(tasks[2], submodel, WRITE)

    changed = propagate_use_statements([graph], edges=[(graph, graph)])

    assert changed is True
    assert submodel in tasks[0].cell_data
    assert _access(tasks[0])[submodel] == READ_AND_WRITE


def test_without_the_self_edge_the_wrap_around_is_not_considered():
    """Without a (graph, graph) edge, a graph's own last task is a dead
    end - nothing wraps back to its first task, so a write at the last
    task with no further use is simply left alone."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[1], submodel, WRITE)

    changed = propagate_use_statements([graph])  # no self-edge

    assert changed is False
    assert submodel not in tasks[0].cell_data


def test_branching_successor_graphs_are_each_fixed_independently():
    """Setup -> TimeStep and Setup -> Plot: a submodel Setup last writes has
    to reach a real reader down *either* branch, and each branch is fixed
    on its own terms (here: TimeStep's reader sits at distance 2, needing a
    relay; Plot's sits at distance 1, already valid)."""
    setup, setup_tasks = _chain_graph("Setup", 1)
    time_step, time_step_tasks = _chain_graph("TimeStep", 2)
    plot, plot_tasks = _chain_graph("Plot", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(setup_tasks[0], submodel, WRITE)
    _use_cell(time_step_tasks[1], submodel, READ_AND_DISCARD)  # distance 2 from setup
    _use_cell(plot_tasks[0], submodel, READ_AND_DISCARD)  # distance 1 from setup

    changed = propagate_use_statements(
        [setup, time_step, plot],
        edges=[(setup, time_step), (setup, plot)],
    )

    assert changed is True
    assert submodel in time_step_tasks[0].cell_data
    assert _access(time_step_tasks[0])[submodel] == READ_AND_WRITE
    assert plot_tasks[0].cell_data.count(submodel) == 1


def test_time_step_and_plot_composed_in_both_directions():
    """TimeStep and Plot may follow one another in either order - a
    submodel TimeStep's last task writes and only Plot's first task reads,
    at distance 1, is already valid down that path. (Self-edges are
    deliberately left out here - a single-task graph whose only task both
    writes and self-repeats would conflict with itself immediately, which
    is a separate scenario covered by the self-edge tests above.)"""
    time_step, time_step_tasks = _chain_graph("TimeStep", 1)
    plot, plot_tasks = _chain_graph("Plot", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(time_step_tasks[0], submodel, WRITE)
    _use_cell(plot_tasks[0], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements(
        [time_step, plot],
        edges=[(time_step, plot), (plot, time_step)],
    )

    assert changed is False


# ---------------------------------------------------------------------------
# Upgrading a WRITE that immediately follows a READ_AND_WRITE.
# ---------------------------------------------------------------------------


def test_write_immediately_after_readwrite_is_upgraded():
    """A READ_AND_WRITE explicitly asks for its value to be kept around -
    a WRITE immediately after it would otherwise silently discard that
    value (predicateForLoad false for a plain WRITE), so it gets upgraded
    to READ_AND_WRITE instead of being left as a superseding WRITE."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, READ_AND_WRITE)
    _use_cell(tasks[1], submodel, WRITE)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert _access(tasks[1])[submodel] == READ_AND_WRITE
    assert tasks[1].cell_data.count(submodel) == 1  # upgraded in place, not duplicated


def test_write_immediately_after_write_is_not_upgraded():
    """Contrast with the above: a WRITE immediately after another plain
    WRITE is the legitimate "recompute this fresh every time" pattern and
    is deliberately left alone - only a READ_AND_WRITE predecessor (which
    explicitly asked to survive) triggers the upgrade."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, WRITE)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert _access(tasks[1])[submodel] == WRITE


def test_write_immediately_after_readwrite_across_an_edge_is_upgraded():
    """The same upgrade, but with the READ_AND_WRITE the last task of one
    graph and the WRITE the first task of a successor graph."""
    graph_a, tasks_a = _chain_graph("A", 1)
    graph_b, tasks_b = _chain_graph("B", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks_a[0], submodel, READ_AND_WRITE)
    _use_cell(tasks_b[0], submodel, WRITE)

    changed = propagate_use_statements([graph_a, graph_b], edges=[(graph_a, graph_b)])

    assert changed is True
    assert _access(tasks_b[0])[submodel] == READ_AND_WRITE


def test_write_immediately_after_write_via_self_edge_is_not_upgraded():
    """The wrap-around counterpart of test_write_immediately_after_write_is_not_upgraded():
    a single-task graph whose one task WRITEs and self-repeats (e.g.
    ProjectPatchOntoFaces recomputing face data fresh every time step) must
    not get upgraded just because it is - via the self-edge - immediately
    "after" its own previous WRITE."""
    graph, tasks = _chain_graph("A", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)

    changed = propagate_use_statements([graph], edges=[(graph, graph)])

    assert changed is False
    assert _access(tasks[0])[submodel] == WRITE


def test_upgrade_runs_before_relay_insertion_so_no_relay_is_needed():
    """Mirrors exahype2.api.fv.RusanovWithExplicitEuler's own task graph:
    task 0 READ_AND_WRITEs, task 1 WRITEs (upgraded to READ_AND_WRITE by
    this pass), task 2 READ_AND_WRITEs again - once task 1 is upgraded,
    every remaining gap is distance 1 (odd), so no relay is ever needed."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, READ_AND_WRITE)
    _use_cell(tasks[1], submodel, WRITE)
    _use_cell(tasks[2], submodel, READ_AND_WRITE)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert _access(tasks[1])[submodel] == READ_AND_WRITE
    assert tasks[0].cell_data.count(submodel) == 1  # unchanged, no relay inserted
    assert tasks[2].cell_data.count(submodel) == 1  # unchanged, no relay inserted


def test_relay_upgrades_a_temporary_declaration():
    """A TEMPORARY use does not relay the submodel through the stacks. If
    the relay has to go onto a step that already declares the submodel
    TEMPORARY, that declaration is upgraded to READ_AND_WRITE."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, TEMPORARY)
    _use_cell(tasks[2], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert tasks[1].cell_data.count(submodel) == 1
    assert _access(tasks[1])[submodel] == READ_AND_WRITE


# ---------------------------------------------------------------------------
# Distances count compiled mesh traversals, not tasks.
# ---------------------------------------------------------------------------


def test_distances_count_fused_steps_not_tasks():
    """Four tasks (cell WRITE, vertex, cell, vertex READ_AND_DISCARD) are
    three tasks apart, which would be fine. FuseSteps however compiles them
    into the traversals [A], [B, C], [D], i.e. the read is two traversals
    after the write, and a relay is required on the fused step [B, C]."""
    from peano4.api.taskgraph import Graph
    from peano4.api.taskgraph.compiler import FuseSteps

    submodel = _FakeSubmodel("Q")
    graph = Graph()
    task_a = Task("A", Task.GridEntity.CELL)
    task_b = Task("B", Task.GridEntity.VERTEX)
    task_c = Task("C", Task.GridEntity.CELL)
    task_d = Task("D", Task.GridEntity.VERTEX)
    for task in (task_a, task_b, task_c, task_d):
        graph.add_task(task)
    task_b.add_dependency(task_a)
    task_c.add_dependency(task_b)
    task_d.add_dependency(task_c)
    task_a.use_cell(submodel, WRITE)
    task_d.use_cell(submodel, READ_AND_DISCARD)

    steps = FuseSteps.translate(graph, "Test", None)
    assert [step.name for step in steps] == ["Test_A", "Test_B__C", "Test_D"]

    changed = propagate_use_statements([steps])

    assert changed is True
    assert _access(steps[1])[submodel] == READ_AND_WRITE


# ---------------------------------------------------------------------------
# Submodels that are read before they are ever written.
# ---------------------------------------------------------------------------


def test_read_before_any_write_adds_write_to_last_step_of_entry_sequence():
    """Setup never touches Q, the time step reads it first: the last setup
    step creates it."""
    setup, setup_steps = _chain_graph("Setup", 2)
    time_step, time_step_steps = _chain_graph("TimeStep", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(time_step_steps[0], submodel, READ_AND_WRITE)
    _use_cell(time_step_steps[1], submodel, WRITE)

    changed = propagate_use_statements(
        [setup, time_step],
        edges=[(setup, time_step), (time_step, time_step)],
    )

    assert changed is True
    assert _access(setup_steps[1])[submodel] == WRITE
    assert submodel not in _access(setup_steps[0])


def test_read_before_any_write_via_plot_detour_also_relays_through_plot():
    """Setup may be followed by a plot first, which does not use Q: the
    WRITE goes into setup, and the plot relays Q to the time step."""
    setup, setup_steps = _chain_graph("Setup", 1)
    time_step, time_step_steps = _chain_graph("TimeStep", 2)
    plot, plot_steps = _chain_graph("Plot", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(time_step_steps[0], submodel, READ_AND_WRITE)
    _use_cell(time_step_steps[1], submodel, READ_AND_WRITE)

    propagate_use_statements(
        [setup, time_step, plot],
        edges=[
            (setup, time_step),
            (setup, plot),
            (time_step, plot),
            (plot, time_step),
            (time_step, time_step),
            (plot, plot),
        ],
    )

    assert _access(setup_steps[0])[submodel] == WRITE
    assert _access(plot_steps[0])[submodel] == READ_AND_WRITE


def test_read_before_write_within_entry_sequence_is_not_changed():
    """There is no earlier sequence to create Q in: nothing changes."""
    setup, setup_steps = _chain_graph("Setup", 2)
    submodel = _FakeSubmodel("Q")
    _use_cell(setup_steps[0], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([setup])

    assert changed is False
    assert submodel not in _access(setup_steps[1])


def test_write_before_read_needs_no_creation():
    """Setup writes Q, so nothing has to be created."""
    setup, setup_steps = _chain_graph("Setup", 2)
    time_step, time_step_steps = _chain_graph("TimeStep", 1)
    submodel = _FakeSubmodel("Q")
    _use_cell(setup_steps[0], submodel, WRITE)
    _use_cell(time_step_steps[0], submodel, READ_AND_WRITE)

    propagate_use_statements(
        [setup, time_step],
        edges=[(setup, time_step), (time_step, time_step)],
    )

    assert submodel not in _access(setup_steps[1]) or _access(setup_steps[1])[submodel] == READ_AND_WRITE


# ---------------------------------------------------------------------------
# READ_AND_DISCARD followed by another read.
# ---------------------------------------------------------------------------


def test_discard_followed_by_read_is_upgraded():
    """Two sibling consumers in subsequent steps: the first must keep Q."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, READ_AND_DISCARD)
    _use_cell(tasks[2], submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert _access(tasks[1])[submodel] == READ_AND_WRITE
    assert _access(tasks[2])[submodel] == READ_AND_DISCARD


def test_discard_followed_by_write_is_not_upgraded():
    """The next use produces Q afresh: discarding is fine."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, READ_AND_DISCARD)
    _use_cell(tasks[2], submodel, WRITE)

    propagate_use_statements([graph])

    assert _access(tasks[1])[submodel] == READ_AND_DISCARD


def test_discard_followed_by_read_at_even_distance_gets_a_relay():
    """The upgraded declaration is an origin like any other: a reader two
    steps later gets a relay in between."""
    graph, tasks = _chain_graph("A", 4)
    submodel = _FakeSubmodel("Q")
    _use_cell(tasks[0], submodel, WRITE)
    _use_cell(tasks[1], submodel, READ_AND_DISCARD)
    _use_cell(tasks[3], submodel, READ_AND_DISCARD)

    propagate_use_statements([graph])

    assert _access(tasks[1])[submodel] == READ_AND_WRITE
    assert _access(tasks[2])[submodel] == READ_AND_WRITE
