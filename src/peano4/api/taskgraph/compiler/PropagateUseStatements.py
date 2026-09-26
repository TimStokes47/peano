# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import Dict, List, Optional, Tuple

from ..Graph import Graph
from ..Task import Task


def propagate_use_statements(
    graphs: List[Graph],
    edges: Optional[List[Tuple[Graph, Graph]]] = None,
) -> bool:
    """!

    Derive whichever further use_cell()/use_face()/use_vertex() declarations
    a set of task graphs need so that every submodel they write is safely
    handed on to whoever reads it next - the consolidated, qualifier-aware
    successor to what used to be two separate functions (the old
    PropagateUseStatements, which only ever looked within one already-
    compiled Step sequence, and the old MakeTaskGraphsComposable, which only
    ever looked *across* graphs). Both jobs are really the same rule applied
    to a bigger or smaller "sequence of Observers"; this function applies it
    uniformly to graphs and to however they are declared composable with
    one another - @see edges below, which is also what replaces the old
    per-graph Graph.cyclic flag (a graph that repeats simply gets a
    (graph, graph) self-edge).

    ## The rule

    Every use_cell()/use_face()/use_vertex() declaration carries a
    Task.DataAccess qualifier - READ_AND_DISCARD, WRITE, READ_AND_WRITE or
    TEMPORARY. Peano only relays a submodel from one Observer to the next
    through its stacks if *both* declare it, so a submodel a task WRITEs
    is lost unless some later task actually reads it, and the compiler
    has to make sure every task in between relays it there. Precisely:

    Only a READ_AND_DISCARD or READ_AND_WRITE ever "picks up" a submodel a
    WRITE produced; a WRITE or READ_AND_WRITE is what obliges anything
    downstream in the first place (a plain READ_AND_DISCARD, or leaving
    the submodel alone entirely, imposes no obligation of its own). Two
    other qualifiers get special treatment when they turn up *downstream*
    of such an obligation:

    - A bare WRITE encountered downstream is a fresh, independent
      production - it does not read what came before, it simply
      supersedes it, so there is nothing left to protect on the way to
      it. The search stops there with no constraint at all (that WRITE is
      instead - separately - checked as an origin of its own, once the
      surrounding loop reaches it in turn).
    - A TEMPORARY use is transparent: it is treated exactly like a task
      that does not mention the submodel at all (@see
      Task.DataAccess.TEMPORARY) - the search simply continues straight
      through it.

    That leaves READ_AND_DISCARD and READ_AND_WRITE as the only uses that
    actually stop the search and matter. For every WRITE or
    READ_AND_WRITE use at some task, let *distance* be how many tasks
    separate it from the next such READ_AND_DISCARD/READ_AND_WRITE use of
    the same submodel (distance 1 means "the very next task"):

    - an odd distance (1, 3, 5, ...): always fine, whatever lies strictly
      in between (nothing, in the sense above - every task there is
      either genuinely silent or a WRITE/TEMPORARY, neither of which
      counts).
    - an even distance (2, 4, 6, ...): not admissible as it stands, but
      always fixable: insert a READ_AND_WRITE declaration of the submodel
      on the task exactly one before the next READ_AND_DISCARD/
      READ_AND_WRITE (an odd distance away from the origin, so the two
      legs it creates - origin-to-relay and relay-to-next-use - are each
      individually valid by the rule above). This is the *only* thing
      this function ever does to fix a violation.
    - no such READ_AND_DISCARD/READ_AND_WRITE reachable at all (every path
      from here either dead-ends or runs into a superseding WRITE first):
      nothing to do - there is nothing downstream to protect.

    The freshly-inserted READ_AND_WRITE is itself fed back through the very
    same check (it is, for this purpose, exactly as much of an origin as a
    plain WRITE), so a single call already iterates this to a fixed point
    - one call is enough, calling it again changes nothing further
    (idempotent), though nothing prevents a caller from invoking it
    repeatedly if that is more convenient while other things are still
    being built up around it.

    ### Worked examples (a single sequence of tasks 0, 1, 2, 3, 4, ...)

    - Task N has a WRITE, task N+2 a READ_AND_DISCARD, nothing in between
      mentions the submodel: distance 2 (even) - not admissible as it
      stands. Fixed by inserting a READ_AND_WRITE declaration on task N+1.
    - Task N has a WRITE, task N+3 a READ_AND_DISCARD: distance 3 (odd) -
      already fine, nothing to do.
    - Task N has a WRITE, task N+4 a READ_AND_DISCARD: distance 4 (even) -
      not admissible. Fixed by inserting a READ_AND_WRITE declaration on
      task N+3 (one before the READ_AND_DISCARD) - not on task N+1:
      inserting right after task N would put the relay at distance 1 from
      the origin, and the *next* leg (relay to the eventual
      READ_AND_DISCARD) would then be distance 3 away, still fine, but the
      relay would sit doing nothing useful for two further tasks;
      inserting right before the READ_AND_DISCARD instead keeps both legs
      minimal.
    - Task N has a WRITE, task N+1 WRITEs the same submodel again: N+1's
      WRITE supersedes N's - nothing to protect, nothing to do (even
      though the two are adjacent).

    ## Sequences, and how graphs and edges make one up

    Within one graph, "the next task" is simply the next one in
    Graph.ordered_tasks() - the same order
    peano4.api.taskgraph.compiler.Sequential.translate() compiles into
    Observers, so what this function derives here is exactly what that
    later compilation step will carry over. Once a graph's own tasks run
    out, "the next task" continues into whichever graph(s) edges declares
    reachable from it - a graph can have several such successors (e.g. a
    time-stepping graph that may be followed by either another time step
    or a plot, decided only at runtime), in which case the rule is checked,
    and fixed if need be, independently along *every* such continuation,
    since the compiler cannot know which one will actually be taken. Two
    different continuations can legitimately reach the very same later
    task at distances of different parity (e.g. straight from a setup
    graph into time-stepping versus via a plot detour first, if plotting
    happens to be due immediately) - both are checked and, if need be,
    independently fixed; a silent task is only ever revisited once per
    parity of how far away it was reached, which is all this rule ever
    cares about, so a cyclic edge structure (e.g. time-stepping and
    plotting following one another indefinitely) is still only walked a
    bounded number of times, not indefinitely. A graph that is meant to
    repeat itself declares a (graph, graph) edge -
    the successor of its own last task is (among others, if it is also
    reachable from elsewhere) its own first task; this is what replaces the
    old Graph.cyclic flag. A graph with no outgoing edges at all simply
    ends there - whatever it last wrote and never got read is not this
    function's concern (it might be handed to a caller entirely outside the
    task-graph machinery, e.g. plotted from C++ code this Python layer does
    not see).

    graphs: [peano4.api.taskgraph.Graph]
      Every graph to consider. A graph has to be listed here to be
      analysed at all, whether or not it appears in edges (a graph with no
      edges is simply analysed on its own, exactly like the old
      PropagateUseStatements used to for a single already-compiled Step
      sequence - except this now runs on the Graph's Tasks, before
      compilation). Modified in place.

    edges: [(peano4.api.taskgraph.Graph, peano4.api.taskgraph.Graph)]
      Declares which graphs may run immediately after which other ones -
      each (predecessor, successor) pair means "successor's first task may
      immediately follow predecessor's last task". Defaults to none (every
      graph analysed purely on its own).

    @return bool
      True if this call added at least one READ_AND_WRITE declaration anywhere,
      False if everything was already admissible.

    """
    if edges is None:
        edges = []

    something_changed = False
    for attribute_name, access_attribute_name, use_method_name in (
        ("cell_data", "cell_data_access", "use_cell"),
        ("face_data", "face_data_access", "use_face"),
        ("vertex_data", "vertex_data_access", "use_vertex"),
    ):
        something_changed = (
            _propagate_use_statements_for_one_kind(
                graphs, edges, attribute_name, access_attribute_name, use_method_name
            )
            or something_changed
        )
    return something_changed


def _propagate_use_statements_for_one_kind(
    graphs: List[Graph],
    edges: List[Tuple[Graph, Graph]],
    attribute_name: str,
    access_attribute_name: str,
    use_method_name: str,
) -> bool:
    tasks_of_graph: Dict[Graph, List[Task]] = {graph: graph.ordered_tasks() for graph in graphs}

    successors_of_graph: Dict[Graph, List[Graph]] = {graph: [] for graph in graphs}
    for predecessor, successor in edges:
        successors_of_graph[predecessor].append(successor)

    something_changed = False
    changed_this_pass = True
    while changed_this_pass:
        changed_this_pass = False
        for graph in graphs:
            tasks = tasks_of_graph[graph]
            for index, task in enumerate(tasks):
                access = getattr(task, access_attribute_name)
                for submodel in list(getattr(task, attribute_name)):
                    if access[submodel] in (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE):
                        if _fix_forward_gap(
                            graph,
                            index,
                            submodel,
                            tasks_of_graph,
                            successors_of_graph,
                            attribute_name,
                            access_attribute_name,
                            use_method_name,
                        ):
                            changed_this_pass = True
        something_changed = something_changed or changed_this_pass

    return something_changed


def _step_forward(graph, index, tasks_of_graph, successors_of_graph):
    """!

    Yield every (graph, index) immediately reachable from (graph, index) -
    the next task in the same graph if there is one, or otherwise the
    first task of every declared successor graph (several, if a graph may
    be followed by more than one other).

    """
    tasks = tasks_of_graph[graph]
    if index + 1 < len(tasks):
        yield (graph, index + 1)
    else:
        for successor in successors_of_graph.get(graph, []):
            if tasks_of_graph[successor]:
                yield (successor, 0)


def _find_next_consecutive_uses(
    origin_graph, origin_index, submodel, attribute_name, access_attribute_name, tasks_of_graph, successors_of_graph
):
    """!

    Breadth-first search forward from (origin_graph, origin_index),
    stopping each path the moment it reaches a task that declares a
    READ_AND_DISCARD, WRITE or READ_AND_WRITE use of submodel (a TEMPORARY
    declaration is transparent to this search - @see
    Task.DataAccess.TEMPORARY - so a path continues straight through it,
    exactly as if it were silent). Returns one (task, distance,
    one_before_task) tuple per such path (several if the search branches
    across more than one successor graph) - one_before_task is the task
    exactly one step before the found one *along that path* (possibly in
    a different, preceding graph), i.e. where a READ_AND_WRITE relay has
    to go if distance turns out to be even.

    """
    results = []
    # Frontier entries: (graph, index, distance, previous_task). Distance
    # counts steps away from the origin; the origin itself is distance 0
    # and is never itself reported as a result.
    frontier = [(origin_graph, origin_index, 0, None)]
    # Whether a gap is admissible depends only on the *parity* of its
    # distance, never its exact length, so a silent (or TEMPORARY-only)
    # node only ever needs to be expanded once per parity of how far it
    # was reached - but it does need both: which graph the compiler ends
    # up choosing to run next (e.g. Plot immediately after Setup, or
    # TimeStep immediately after Setup) is a runtime decision this
    # compiler cannot see, and different such choices can genuinely reach
    # the same later task at distances of different parity (e.g. straight
    # from Setup into TimeStep versus via a Plot detour first).
    # Deduplicating on the plain (graph, index) alone would silently drop
    # whichever parity is not reached first, hiding a real violation
    # reachable via the other path.
    visited = set()

    while frontier:
        graph, index, distance, previous_task = frontier.pop(0)
        for next_graph, next_index in _step_forward(graph, index, tasks_of_graph, successors_of_graph):
            next_distance = distance + 1
            next_task = tasks_of_graph[next_graph][next_index]
            access = getattr(next_task, access_attribute_name)
            if submodel in access and access[submodel] != Task.DataAccess.TEMPORARY:
                results.append((next_task, next_distance, previous_task))
                # This path stops here - do not look past the first real use.
            else:
                state = (next_graph, next_index, next_distance % 2)
                if state not in visited:
                    visited.add(state)
                    frontier.append((next_graph, next_index, next_distance, next_task))

    return results


def _fix_forward_gap(
    origin_graph,
    origin_index,
    submodel,
    tasks_of_graph,
    successors_of_graph,
    attribute_name,
    access_attribute_name,
    use_method_name,
) -> bool:
    changed = False
    for next_task, distance, previous_task in _find_next_consecutive_uses(
        origin_graph, origin_index, submodel, attribute_name, access_attribute_name, tasks_of_graph, successors_of_graph
    ):
        qualifier = getattr(next_task, access_attribute_name)[submodel]
        if qualifier == Task.DataAccess.WRITE:
            # A bare WRITE is a fresh, independent production of the
            # submodel - it does not read what the origin wrote, it simply
            # supersedes it. There is therefore nothing to protect on the
            # way there: this path ends here with no constraint at all
            # (next_task's own forward gap is - separately - checked once
            # the surrounding loop reaches it as an origin in its own
            # right, since it is itself a WRITE).
            continue
        # Only a READ_AND_DISCARD or READ_AND_WRITE reaches this point
        # (TEMPORARY is transparent to _find_next_consecutive_uses and
        # never appears here). Any odd distance - including 1, the
        # smallest one - is then automatically fine: an even distance is
        # not, but always fixable by inserting a READ_AND_WRITE relay one
        # step before it.
        if distance % 2 == 0:
            # previous_task always exists here: distance >= 2 means at
            # least one task lies strictly in between the origin and
            # next_task, and that is exactly what previous_task is.
            if submodel not in getattr(previous_task, attribute_name):
                getattr(previous_task, use_method_name)(submodel, Task.DataAccess.READ_AND_WRITE)
                changed = True

    return changed
