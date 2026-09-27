from peano4.api.taskgraph import Graph, Task
from peano4.api.taskgraph.compiler.PropagateUseStatements import propagate_use_statements


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


def _make_task(name):
    return Task(name=name, entity=Task.GridEntity.CELL)


def _chain_graph(name_prefix, length):
    """A simple linear chain of `length` tasks, all added to one Graph."""
    graph = Graph()
    tasks = [_make_task(f"{name_prefix}-{i}") for i in range(length)]
    for i in range(1, length):
        tasks[i].add_dependency(tasks[i - 1])
    for task in tasks:
        graph.add_task(task)
    return graph, tasks


def _access(task, attribute_name="cell_data_access"):
    return getattr(task, attribute_name)


# ---------------------------------------------------------------------------
# Single-graph, no edges: the "distance" rule itself.
# ---------------------------------------------------------------------------


def test_immediate_read_after_write_is_already_valid():
    """Distance 1, WRITE then READ_AND_DISCARD: nothing to do."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, WRITE)
    tasks[1].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert tasks[1].cell_data.count(submodel) == 1  # not duplicated


def test_immediate_readwrite_after_write_is_already_valid():
    """Distance 1, WRITE then READ_AND_WRITE: also fine."""
    graph, tasks = _chain_graph("A", 2)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, WRITE)
    tasks[1].use_cell(submodel, READ_AND_WRITE)

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
    tasks[0].use_cell(submodel, WRITE)
    tasks[1].use_cell(submodel, WRITE)

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
    tasks[0].use_cell(submodel, WRITE)
    tasks[1].use_cell(submodel, WRITE)
    tasks[2].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False


def test_distance_two_inserts_readwrite_in_between():
    """Worked example 1: task N writes, task N+2 reads, nothing in between
    mentions the submodel (distance 2, even) - fixed by inserting a
    READ_AND_WRITE declaration on task N+1."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, WRITE)
    tasks[2].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert submodel in tasks[1].cell_data
    assert _access(tasks[1])[submodel] == READ_AND_WRITE


def test_distance_three_needs_no_change():
    """Worked example 2: task N writes, task N+3 reads (distance 3, odd) -
    already fine as it stands."""
    graph, tasks = _chain_graph("A", 4)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, WRITE)
    tasks[3].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert submodel not in tasks[1].cell_data
    assert submodel not in tasks[2].cell_data


def test_distance_four_inserts_readwrite_immediately_before_the_read():
    """Worked example 3: task N writes, task N+4 reads (distance 4, even) -
    fixed by inserting a READ_AND_WRITE declaration on task N+3, not N+1."""
    graph, tasks = _chain_graph("A", 5)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, WRITE)
    tasks[4].use_cell(submodel, READ_AND_DISCARD)

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
    tasks[0].use_cell(submodel, WRITE)
    tasks[1].use_cell(submodel, TEMPORARY)
    tasks[3].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert submodel not in tasks[2].cell_data


def test_read_origin_triggers_nothing():
    """A READ_AND_DISCARD never obliges anything downstream - even an
    immediately following task that does not mention the submodel at all
    is fine."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert submodel not in tasks[1].cell_data
    assert submodel not in tasks[2].cell_data


def test_temporary_origin_triggers_nothing():
    """Same for TEMPORARY."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, TEMPORARY)

    changed = propagate_use_statements([graph])

    assert changed is False


def test_readwrite_origin_is_treated_like_write():
    """READ_AND_WRITE both reads and writes, so - like a plain WRITE - it
    still obliges whatever comes next (distance 2 here, needs a relay)."""
    graph, tasks = _chain_graph("A", 3)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, READ_AND_WRITE)
    tasks[2].use_cell(submodel, READ_AND_DISCARD)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert _access(tasks[1])[submodel] == READ_AND_WRITE


def test_is_idempotent():
    """A second call once everything is already admissible must change
    nothing and report False."""
    graph, tasks = _chain_graph("A", 5)
    submodel = _FakeSubmodel("Q")
    tasks[0].use_cell(submodel, WRITE)
    tasks[4].use_cell(submodel, READ_AND_DISCARD)

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
    tasks[0].use_cell(cell_submodel, WRITE)
    tasks[2].use_cell(cell_submodel, READ_AND_DISCARD)
    tasks[0].use_face(face_submodel, WRITE)
    tasks[2].use_face(face_submodel, READ_AND_DISCARD)
    tasks[0].use_vertex(vertex_submodel, WRITE)
    tasks[2].use_vertex(vertex_submodel, READ_AND_DISCARD)

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
    tasks[0].use_cell(submodel, WRITE)
    tasks[2].use_cell(submodel, READ_AND_DISCARD)

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
    tasks_a[0].use_cell(submodel, WRITE)
    tasks_b[0].use_cell(submodel, READ_AND_DISCARD)

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
    tasks[0].use_cell(submodel, READ_AND_DISCARD)
    tasks[2].use_cell(submodel, WRITE)

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
    tasks[1].use_cell(submodel, READ_AND_DISCARD)
    tasks[2].use_cell(submodel, WRITE)

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
    tasks[1].use_cell(submodel, WRITE)

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
    setup_tasks[0].use_cell(submodel, WRITE)
    time_step_tasks[1].use_cell(submodel, READ_AND_DISCARD)  # distance 2 from setup
    plot_tasks[0].use_cell(submodel, READ_AND_DISCARD)  # distance 1 from setup

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
    time_step_tasks[0].use_cell(submodel, WRITE)
    plot_tasks[0].use_cell(submodel, READ_AND_DISCARD)

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
    tasks[0].use_cell(submodel, READ_AND_WRITE)
    tasks[1].use_cell(submodel, WRITE)

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
    tasks[0].use_cell(submodel, WRITE)
    tasks[1].use_cell(submodel, WRITE)

    changed = propagate_use_statements([graph])

    assert changed is False
    assert _access(tasks[1])[submodel] == WRITE


def test_write_immediately_after_readwrite_across_an_edge_is_upgraded():
    """The same upgrade, but with the READ_AND_WRITE the last task of one
    graph and the WRITE the first task of a successor graph."""
    graph_a, tasks_a = _chain_graph("A", 1)
    graph_b, tasks_b = _chain_graph("B", 1)
    submodel = _FakeSubmodel("Q")
    tasks_a[0].use_cell(submodel, READ_AND_WRITE)
    tasks_b[0].use_cell(submodel, WRITE)

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
    tasks[0].use_cell(submodel, WRITE)

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
    tasks[0].use_cell(submodel, READ_AND_WRITE)
    tasks[1].use_cell(submodel, WRITE)
    tasks[2].use_cell(submodel, READ_AND_WRITE)

    changed = propagate_use_statements([graph])

    assert changed is True
    assert _access(tasks[1])[submodel] == READ_AND_WRITE
    assert tasks[0].cell_data.count(submodel) == 1  # unchanged, no relay inserted
    assert tasks[2].cell_data.count(submodel) == 1  # unchanged, no relay inserted
