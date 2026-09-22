# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List

import peano4.api.solversteps


def propagate_use_statements(steps: List["peano4.api.solversteps.Step"]) -> None:
    """!

    Postprocess a sequence of Observers so a cell/face/vertex submodel stays
    "in flight" across every step that has to hand it on to a later step.

    A peano4.api.solversteps.Step only receives a piece of mesh data
    (fineGridCell..., fineGridFace..., fineGridVertices...) as an argument of
    its generated operations if the step itself declares that it uses that
    submodel, via use_cell()/use_face()/use_vertex(). Peano moves mesh data
    from one grid sweep (Observer) to the next through stacks: a submodel
    that a given step does not declare as used is not read from the input
    stream and not written to the output stream of that step, i.e. it is not
    relayed onwards. Consequently, if some step N0 needs a submodel that was
    last (or first) produced/needed at an earlier or later step, every step
    in between has to declare that same submodel as used too, purely to
    relay it through its stacks, even if the step's own action sets never
    touch it.

    This function derives and adds exactly those additional, purely
    relaying, use declarations. It is meant to run once a compiler (@see
    peano4.api.taskgraph.compiler.Sequential, and other compilers to come)
    has already built the full sequence of Steps and populated each Step's
    own use_cell()/use_face()/use_vertex() with whatever its originating
    task(s) explicitly declared - i.e. it is a postprocessing pass over an
    already-derived sequence of Observers, not part of deriving that
    sequence.

    ## The two propagation rules

    Let a submodel be "used" at step i if steps[i] has that submodel in its
    cell_data/face_data/vertex_data (either originally, or because this
    algorithm added it). The sequence steps[0], steps[1], ..., steps[-1] is
    then enriched, per submodel, according to two rules:

    (a) If a submodel is used in the *last* step of the sequence, it also
        has to be used in the *first* step. This is what allows a submodel
        to survive the wrap-around from one run of the sequence into the
        next, for sequences that represent a phase that repeats (e.g. one
        time step, run over and over).

    (b) If a submodel is used in step N, it also has to be used in steps
        N+1, N+3, N+5, ... (i.e. every later step reachable from N through
        an odd number of steps).

    Both rules are applied repeatedly - including to the steps a previous
    application just added a submodel to - until a full pass over every
    submodel changes nothing anymore, i.e. until both constraints hold
    everywhere simultaneously. (Rule (b), reapplied to a step it just
    switched on, alone already forces a submodel to be used at every single
    later step; rule (a) is what feeds that propagation back around to the
    front of the sequence.)

    steps: [peano4.api.solversteps.Step]
      The full, ordered sequence of Observers to enrich, exactly as a
      compiler is about to return it. Modified in place.

    """
    if len(steps) < 2:
        return

    for attribute_name, use_method_name in (
        ("cell_data", "use_cell"),
        ("face_data", "use_face"),
        ("vertex_data", "use_vertex"),
    ):
        _propagate_use_statements_for_one_kind(steps, attribute_name, use_method_name)


def _propagate_use_statements_for_one_kind(
    steps: List["peano4.api.solversteps.Step"],
    attribute_name: str,
    use_method_name: str,
) -> None:
    number_of_steps = len(steps)

    used_steps_of_submodel = {}
    for step_index, step in enumerate(steps):
        for submodel in getattr(step, attribute_name):
            used_steps_of_submodel.setdefault(submodel, set()).add(step_index)

    something_changed = True
    while something_changed:
        something_changed = False
        for used_steps in used_steps_of_submodel.values():
            # Rule (a): last step used implies first step used.
            if (number_of_steps - 1) in used_steps and 0 not in used_steps:
                used_steps.add(0)
                something_changed = True

            # Rule (b): step N used implies steps N+1, N+3, N+5, ... used.
            for step_index in list(used_steps):
                offset = 1
                while step_index + offset < number_of_steps:
                    if (step_index + offset) not in used_steps:
                        used_steps.add(step_index + offset)
                        something_changed = True
                    offset += 2

    for submodel, used_steps in used_steps_of_submodel.items():
        for step_index in sorted(used_steps):
            step = steps[step_index]
            if submodel not in getattr(step, attribute_name):
                getattr(step, use_method_name)(submodel)
