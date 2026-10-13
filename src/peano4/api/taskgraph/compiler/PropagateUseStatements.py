# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import Dict, List, Optional, Tuple

import peano4.api

from ..Task import Task


def propagate_use_statements(
    sequences: List[List["peano4.api.solversteps.Step"]],
    edges: Optional[List[Tuple[List["peano4.api.solversteps.Step"], List["peano4.api.solversteps.Step"]]]] = None,
) -> bool:
    """!

    Derive whichever further use_cell()/use_face()/use_vertex() declarations
    a set of compiled step sequences need so that every submodel they write
    is safely handed on to whoever reads it next.

    This is a postprocessing step that runs on the output of a task graph
    compiler (@see peano4.api.taskgraph.compiler), i.e. on the sequences of
    peano4.api.solversteps.Step instances (Observers, aka mesh traversals)
    a compiler such as peano4.api.taskgraph.compiler.Sequential.translate()
    or peano4.api.taskgraph.compiler.FuseSteps.translate() yields - one
    sequence per task graph. All distances below therefore count mesh
    traversals, not tasks: a compiler that fuses several tasks into one
    Step changes these distances, and only the compiled Steps know them.

    Every Step carries the Task.DataAccess qualifier of each submodel it
    declares in its cell_data_access/face_data_access/vertex_data_access
    (populated by the compiler). This function reads and amends these
    qualifiers, and it adds use_cell()/use_face()/use_vertex() declarations
    to the Steps where required.

    ## Upgrading a WRITE that immediately follows a READ_AND_WRITE

    Before any of the below runs, a narrower, separate pass first upgrades
    every WRITE declaration that sits immediately (distance 1, following
    edges the same way as everything else here) after a READ_AND_WRITE
    declaration of the same submodel into a READ_AND_WRITE of its own -
    @see _upgrade_writes_immediately_following_read_and_write() for why
    (in short: trusting the WRITE there would silently discard whatever
    the READ_AND_WRITE step explicitly asked to keep). This is logged via
    peano4.api.Logger().log() whenever it happens. A WRITE immediately
    after another plain WRITE is deliberately left alone by this - @see
    that function's own docstring for why that case is different.

    ## The rule

    Every use_cell()/use_face()/use_vertex() declaration carries a
    Task.DataAccess qualifier - READ_AND_DISCARD, WRITE, READ_AND_WRITE or
    TEMPORARY. Peano only relays a submodel from one Observer to the next
    through its stacks if *both* declare it, so a submodel a step WRITEs
    is lost unless some later step actually reads it, and every step in
    between has to relay it there. Precisely:

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
    - A TEMPORARY use is transparent: it is treated exactly like a step
      that does not mention the submodel at all (@see
      Task.DataAccess.TEMPORARY) - the search simply continues straight
      through it.

    That leaves READ_AND_DISCARD and READ_AND_WRITE as the only uses that
    actually stop the search and matter. For every WRITE or
    READ_AND_WRITE use at some step, let *distance* be how many steps
    separate it from the next such READ_AND_DISCARD/READ_AND_WRITE use of
    the same submodel (distance 1 means "the very next step"):

    - an odd distance (1, 3, 5, ...): always fine, whatever lies strictly
      in between (nothing, in the sense above - every step there is
      either genuinely silent or a WRITE/TEMPORARY, neither of which
      counts).
    - an even distance (2, 4, 6, ...): not admissible as it stands, but
      always fixable: insert a READ_AND_WRITE declaration of the submodel
      on the step exactly one before the next READ_AND_DISCARD/
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
    (idempotent).

    ### Worked examples (a single sequence of steps 0, 1, 2, 3, 4, ...)

    - Step N has a WRITE, step N+2 a READ_AND_DISCARD, nothing in between
      mentions the submodel: distance 2 (even) - not admissible as it
      stands. Fixed by inserting a READ_AND_WRITE declaration on step N+1.
    - Step N has a WRITE, step N+3 a READ_AND_DISCARD: distance 3 (odd) -
      already fine, nothing to do.
    - Step N has a WRITE, step N+4 a READ_AND_DISCARD: distance 4 (even) -
      not admissible. Fixed by inserting a READ_AND_WRITE declaration on
      step N+3 (one before the READ_AND_DISCARD) - not on step N+1:
      inserting right after step N would put the relay at distance 1 from
      the origin, and the *next* leg (relay to the eventual
      READ_AND_DISCARD) would then be distance 3 away, still fine, but the
      relay would sit doing nothing useful for two further steps;
      inserting right before the READ_AND_DISCARD instead keeps both legs
      minimal.
    - Step N has a WRITE, step N+1 WRITEs the same submodel again: N+1's
      WRITE supersedes N's - nothing to protect, nothing to do (even
      though the two are adjacent).

    ## Sequences, and how edges connect them

    Within one sequence, "the next step" is simply the next one in the
    list. Once a sequence's own steps run out, "the next step" continues
    into whichever sequence(s) edges declares reachable from it - a
    sequence can have several such successors (e.g. a time-stepping
    sequence that may be followed by either another time step or a plot,
    decided only at runtime), in which case the rule is checked, and fixed
    if need be, independently along *every* such continuation, since we
    cannot know which one will actually be taken. Two different
    continuations can legitimately reach the very same later step at
    distances of different parity (e.g. straight from setup into
    time-stepping versus via a plot detour first, if plotting happens to
    be due immediately) - both are checked and, if need be, independently
    fixed; a silent step is only ever revisited once per parity of how far
    away it was reached, which is all this rule ever cares about, so a
    cyclic edge structure (e.g. time-stepping and plotting following one
    another indefinitely) is still only walked a bounded number of times,
    not indefinitely. A sequence that is meant to repeat itself declares a
    (sequence, sequence) self-edge - the successor of its own last step is
    (among others, if it is also reachable from elsewhere) its own first
    step. A sequence with no outgoing edges at all simply ends there -
    whatever it last wrote and never got read is not this function's
    concern (it might be handed to a caller entirely outside the
    task-graph machinery, e.g. plotted from C++ code this Python layer does
    not see).

    sequences: [[peano4.api.solversteps.Step]]
      Every step sequence to consider, typically the outcome of one
      compiler call per task graph. A sequence has to be listed here to be
      analysed at all, whether or not it appears in edges (a sequence with
      no edges is simply analysed on its own). The steps are modified in
      place.

    edges: [([peano4.api.solversteps.Step], [peano4.api.solversteps.Step])]
      Declares which sequences may run immediately after which other ones -
      each (predecessor, successor) pair means "successor's first step may
      immediately follow predecessor's last step". Sequences are
      identified by identity, i.e. pass the very list objects that are
      also handed over through sequences. Defaults to none (every sequence
      analysed purely on its own).

    @return bool
      True if this call added or upgraded at least one declaration anywhere,
      False if everything was already admissible.

    """
    if edges is None:
        edges = []

    # Lists are not hashable, so we identify each sequence by its position
    # within sequences and translate the edges accordingly.
    def index_of_sequence(sequence) -> int:
        for index, candidate in enumerate(sequences):
            if candidate is sequence:
                return index
        raise ValueError("propagate_use_statements(): edge refers to a step sequence not passed through sequences")

    steps_of_sequence: Dict[int, List] = dict(enumerate(sequences))
    index_edges = [(index_of_sequence(predecessor), index_of_sequence(successor)) for predecessor, successor in edges]

    something_changed = False
    for attribute_name, access_attribute_name, use_method_name in (
        ("cell_data", "cell_data_access", "use_cell"),
        ("face_data", "face_data_access", "use_face"),
        ("vertex_data", "vertex_data_access", "use_vertex"),
    ):
        something_changed = (
            _upgrade_writes_immediately_following_read_and_write(
                steps_of_sequence, index_edges, attribute_name, access_attribute_name
            )
            or something_changed
        )
        something_changed = (
            _propagate_use_statements_for_one_kind(
                steps_of_sequence, index_edges, attribute_name, access_attribute_name, use_method_name
            )
            or something_changed
        )
    return something_changed


def _step_backward(sequence, index, steps_of_sequence, predecessors_of_sequence):
    """!

    The exact reverse of _step_forward(): yield every (sequence, index)
    that can immediately reach (sequence, index) - the previous step in the
    same sequence if there is one, or otherwise the last step of every
    sequence declared (via edges) to lead into this one.

    """
    if index > 0:
        yield (sequence, index - 1)
    else:
        for predecessor in predecessors_of_sequence.get(sequence, []):
            predecessor_steps = steps_of_sequence[predecessor]
            if predecessor_steps:
                yield (predecessor, len(predecessor_steps) - 1)


def _upgrade_writes_immediately_following_read_and_write(
    steps_of_sequence: Dict[int, List],
    edges: List[Tuple[int, int]],
    attribute_name: str,
    access_attribute_name: str,
) -> bool:
    """!

    A plain WRITE is normally treated as a fresh, independent production
    that supersedes whatever came before (@see _fix_forward_gap) - correct
    whenever a step genuinely does not care what the submodel held before
    it ran. But a WRITE immediately (distance 1) after a READ_AND_WRITE of
    the very same submodel is a different, narrower situation: the
    READ_AND_WRITE step explicitly asked for its result to be kept around,
    and the very next step then declares itself uninterested in reading it
    - one of these two declarations has to be wrong, and silently trusting
    the WRITE would mean the READ_AND_WRITE step's stack storage is
    populated (predicateForStore true) and then never loaded again
    (predicateForLoad false for a plain WRITE - @see
    peano4.api.taskgraph.compiler.DeriveLoadStoreComputeFlag), which is
    at best wasted storage and at worst - if the very next step's own
    computation actually does depend on that value, just under the wrong
    qualifier - silently wrong numerics.

    This is therefore auto-corrected rather than merely reported: every
    step whose own declaration of a submodel is WRITE, and that has at
    least one immediate predecessor (@see _step_backward - several, if the
    step is reachable from more than one sequence) that declares the same
    submodel READ_AND_WRITE, has its own declaration silently upgraded to
    READ_AND_WRITE, logged via peano4.api.Logger().log() so it is visible
    without changing behaviour otherwise. Run to a fixed point first,
    before _propagate_use_statements_for_one_kind()'s own relay-insertion
    pass: an upgraded declaration is exactly as valid an origin as one a
    task graph author wrote by hand, and doing this first means the relay
    pass never has to reason about a WRITE that is about to change.

    This is deliberately narrower than "any WRITE reachable from an
    earlier WRITE/READ_AND_WRITE": a WRITE immediately after another
    WRITE of the same submodel is left alone, since that is exactly the
    legitimate "recompute this fresh every time" pattern
    _fix_forward_gap() already has to support (@see there) - only a
    READ_AND_WRITE predecessor, which explicitly asked for its value to
    survive, triggers the upgrade.

    steps_of_sequence, edges: the sequences and edges handed over to
      propagate_use_statements(), each sequence identified by its index.

    attribute_name, access_attribute_name: which kind of submodel
      (cell/face/vertex) to analyse.

    @return bool
      True if this pass upgraded at least one declaration.

    """
    predecessors_of_sequence: Dict[int, List[int]] = {sequence: [] for sequence in steps_of_sequence}
    for predecessor, successor in edges:
        predecessors_of_sequence[successor].append(predecessor)

    something_changed = False
    changed_this_pass = True
    while changed_this_pass:
        changed_this_pass = False
        for sequence, steps in steps_of_sequence.items():
            for index, step in enumerate(steps):
                access = getattr(step, access_attribute_name)
                for submodel in list(getattr(step, attribute_name)):
                    if access[submodel] != Task.DataAccess.WRITE:
                        continue
                    for predecessor_sequence, predecessor_index in _step_backward(
                        sequence, index, steps_of_sequence, predecessors_of_sequence
                    ):
                        predecessor_step = steps_of_sequence[predecessor_sequence][predecessor_index]
                        predecessor_access = getattr(predecessor_step, access_attribute_name)
                        if predecessor_access.get(submodel) == Task.DataAccess.READ_AND_WRITE:
                            access[submodel] = Task.DataAccess.READ_AND_WRITE
                            peano4.api.Logger().log(
                                f"propagate_use_statements(): step '{step.name}' declared {attribute_name} "
                                f"submodel '{submodel.name}' as WRITE, but its predecessor step "
                                f"'{predecessor_step.name}' declares it READ_AND_WRITE - upgrading "
                                f"'{step.name}' to READ_AND_WRITE to avoid silently discarding it."
                            )
                            changed_this_pass = True
                            break
        something_changed = something_changed or changed_this_pass

    return something_changed


def _propagate_use_statements_for_one_kind(
    steps_of_sequence: Dict[int, List],
    edges: List[Tuple[int, int]],
    attribute_name: str,
    access_attribute_name: str,
    use_method_name: str,
) -> bool:
    successors_of_sequence: Dict[int, List[int]] = {sequence: [] for sequence in steps_of_sequence}
    for predecessor, successor in edges:
        successors_of_sequence[predecessor].append(successor)

    something_changed = False
    changed_this_pass = True
    while changed_this_pass:
        changed_this_pass = False
        for sequence, steps in steps_of_sequence.items():
            for index, step in enumerate(steps):
                access = getattr(step, access_attribute_name)
                for submodel in list(getattr(step, attribute_name)):
                    if access[submodel] in (Task.DataAccess.WRITE, Task.DataAccess.READ_AND_WRITE):
                        if _fix_forward_gap(
                            sequence,
                            index,
                            submodel,
                            steps_of_sequence,
                            successors_of_sequence,
                            attribute_name,
                            access_attribute_name,
                            use_method_name,
                        ):
                            changed_this_pass = True
        something_changed = something_changed or changed_this_pass

    return something_changed


def _step_forward(sequence, index, steps_of_sequence, successors_of_sequence):
    """!

    Yield every (sequence, index) immediately reachable from (sequence,
    index) - the next step in the same sequence if there is one, or
    otherwise the first step of every declared successor sequence
    (several, if a sequence may be followed by more than one other).

    """
    steps = steps_of_sequence[sequence]
    if index + 1 < len(steps):
        yield (sequence, index + 1)
    else:
        for successor in successors_of_sequence.get(sequence, []):
            if steps_of_sequence[successor]:
                yield (successor, 0)


def _find_next_consecutive_uses(
    origin_sequence, origin_index, submodel, attribute_name, access_attribute_name, steps_of_sequence, successors_of_sequence
):
    """!

    Breadth-first search forward from (origin_sequence, origin_index),
    stopping each path the moment it reaches a step that declares a
    READ_AND_DISCARD, WRITE or READ_AND_WRITE use of submodel (a TEMPORARY
    declaration is transparent to this search - @see
    Task.DataAccess.TEMPORARY - so a path continues straight through it,
    exactly as if it were silent). Returns one (step, distance,
    one_before_step) tuple per such path (several if the search branches
    across more than one successor sequence) - one_before_step is the step
    exactly one before the found one *along that path* (possibly in a
    different, preceding sequence), i.e. where a READ_AND_WRITE relay has
    to go if distance turns out to be even.

    """
    results = []
    # Frontier entries: (sequence, index, distance, previous_step). Distance
    # counts steps away from the origin; the origin itself is distance 0
    # and is never itself reported as a result.
    frontier = [(origin_sequence, origin_index, 0, None)]
    # Whether a gap is admissible depends only on the *parity* of its
    # distance, never its exact length, so a silent (or TEMPORARY-only)
    # node only ever needs to be expanded once per parity of how far it
    # was reached - but it does need both: which sequence runs next (e.g.
    # Plot immediately after Setup, or TimeStep immediately after Setup)
    # is a runtime decision we cannot see, and different such choices can
    # genuinely reach the same later step at distances of different parity
    # (e.g. straight from Setup into TimeStep versus via a Plot detour
    # first). Deduplicating on the plain (sequence, index) alone would
    # silently drop whichever parity is not reached first, hiding a real
    # violation reachable via the other path.
    visited = set()

    while frontier:
        sequence, index, distance, previous_step = frontier.pop(0)
        for next_sequence, next_index in _step_forward(sequence, index, steps_of_sequence, successors_of_sequence):
            next_distance = distance + 1
            next_step = steps_of_sequence[next_sequence][next_index]
            access = getattr(next_step, access_attribute_name)
            if submodel in access and access[submodel] != Task.DataAccess.TEMPORARY:
                results.append((next_step, next_distance, previous_step))
                # This path stops here - do not look past the first real use.
            else:
                state = (next_sequence, next_index, next_distance % 2)
                if state not in visited:
                    visited.add(state)
                    frontier.append((next_sequence, next_index, next_distance, next_step))

    return results


def _fix_forward_gap(
    origin_sequence,
    origin_index,
    submodel,
    steps_of_sequence,
    successors_of_sequence,
    attribute_name,
    access_attribute_name,
    use_method_name,
) -> bool:
    changed = False
    for next_step, distance, previous_step in _find_next_consecutive_uses(
        origin_sequence, origin_index, submodel, attribute_name, access_attribute_name, steps_of_sequence, successors_of_sequence
    ):
        qualifier = getattr(next_step, access_attribute_name)[submodel]
        if qualifier == Task.DataAccess.WRITE:
            # A bare WRITE is a fresh, independent production of the
            # submodel - it does not read what the origin wrote, it simply
            # supersedes it. There is therefore nothing to protect on the
            # way there: this path ends here with no constraint at all
            # (next_step's own forward gap is - separately - checked once
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
            # previous_step always exists here: distance >= 2 means at
            # least one step lies strictly in between the origin and
            # next_step, and that is exactly what previous_step is.
            # A TEMPORARY declaration on previous_step is upgraded, as a
            # TEMPORARY use does not relay the submodel.
            if getattr(previous_step, access_attribute_name).get(submodel) is None:
                getattr(previous_step, use_method_name)(submodel)
            if getattr(previous_step, access_attribute_name).get(submodel) != Task.DataAccess.READ_AND_WRITE:
                getattr(previous_step, access_attribute_name)[submodel] = Task.DataAccess.READ_AND_WRITE
                changed = True

    return changed
