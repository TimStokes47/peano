# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List

import peano4.api


def report_compiled_step_sequence(
    steps: List["peano4.api.solversteps.Step"], title: str = "Compiled task graph"
) -> None:
    """!

    Report the result of compiling a task graph into a linear sequence of
    Observers, through peano4.api.Logger() - the compiled-sequence
    counterpart to peano4.api.taskgraph.Graph.log_as_text()/visualise(),
    which report on the *pre*-compilation Graph/Task structure instead.

    Meant to be called right after a compiler (@see
    peano4.api.taskgraph.compiler.Sequential.translate) has derived its
    Observer sequence. Note that
    peano4.api.taskgraph.compiler.PropagateUseStatements.propagate_use_statements()
    runs as postprocessing after the compiler, i.e. any use_cell()/
    use_face()/use_vertex() declarations it adds later on are not part of
    this report.

    Each step's action sets are listed as an enumerated list in the order
    in which they are invoked ("<number>. do / <name>"). Action sets that
    run in parallel share the same number and are marked as
    "(in parallel)" - @ref peano4.api.taskgraph.compiler.ReportCompiledStepSequence._action_set_numbers() for where these numbers
    come from.

    Dispatches on peano4.api.Logger().verbosity exactly like
    Graph.log_as_text()/visualise() do: LoggerVerbosity.TEXT logs a plain
    listing of the sequence and, for each step, its action sets plus the
    cell/face/vertex submodels it uses;
    LoggerVerbosity.VISUALISATION draws a flow chart in the same UML state
    diagram style as Graph.visualise() - a left-to-right chain of states
    (one per step, in sequence order, since a compiled sequence has no
    dependency structure left to lay out - it already is the order),
    listing each step's action sets as enumerated "<number>. do / <name>"
    lines and the
    cell/face/vertex submodels it uses as "cell / <name> [<qualifier>]",
    "face / <name> [<qualifier>]", "vertex / <name> [<qualifier>]" lines -
    <qualifier> being the originating Task's Task.DataAccess
    (READ_AND_DISCARD, WRITE, READ_AND_WRITE or TEMPORARY), taken from the
    step's cell_data_access/face_data_access/vertex_data_access (@see
    peano4.api.taskgraph.compiler.Sequential.translate(), which populates
    them; a submodel missing from there - i.e. a Step built some other
    way than through that compiler - is simply listed without a
    qualifier) - self-loops for steps with has_to_repeat set, and
    initial/final pseudostates. Neither happens (this call is then a
    no-op) under any other verbosity.

    @param steps: [peano4.api.solversteps.Step]
      The compiled sequence to report on, exactly as the compiler is about
      to return it.

    @param title: str
      Used as-is as the text report's heading / the flow chart's title.

    """
    if peano4.api.Logger().is_visualisation:
        _visualise_compiled_step_sequence(steps, title)
    if peano4.api.Logger().is_text:
        _log_compiled_step_sequence_as_text(steps, title)


def _submodel_label(submodel, access) -> str:
    """!
    "<name>", or "<name> [<qualifier>]" if access (one of a Step's
    cell_data_access/face_data_access/vertex_data_access dicts) has a
    Task.DataAccess qualifier on file for this submodel.
    """
    qualifier = access.get(submodel)
    return f"{submodel.name} [{qualifier.name}]" if qualifier is not None else submodel.name


def _action_set_numbers(step) -> List[int]:
    """!
    One invocation number per action set of step, starting at 1. Action
    sets with the same number run in parallel.

    A compiler can hand over these numbers explicitly through an attribute
    action_set_numbers on the step (@ref
    peano4.api.taskgraph.compiler.FuseSteps). Otherwise, we derive them
    from the action sets' parallel flags (@ref
    peano4.api.solversteps.ActionSet): consecutive action sets which are
    all labelled as parallel share one number, every other action set gets
    a number of its own.
    """
    numbers = getattr(step, "action_set_numbers", None)
    if numbers is not None and len(numbers) == len(step.action_sets):
        return list(numbers)

    numbers = []
    number = 0
    for index, action_set in enumerate(step.action_sets):
        continues_parallel_group = index > 0 and action_set.parallel and step.action_sets[index - 1].parallel
        if not continues_parallel_group:
            number += 1
        numbers.append(number)
    return numbers


def _action_set_lines(step) -> List[str]:
    """!
    Enumerated list of the step's action sets in invocation order, i.e.
    "<number>. do / <name>". Action sets that run in parallel share the
    same number and are explicitly marked as running in parallel.
    """
    numbers = _action_set_numbers(step)
    lines = []
    for action_set, number in zip(step.action_sets, numbers):
        line = f"{number}. do / {action_set.get_action_set_name()}"
        if numbers.count(number) > 1:
            line += " (in parallel)"
        lines.append(line)
    return lines


def _log_compiled_step_sequence_as_text(steps, title: str) -> None:
    if not steps:
        peano4.api.Logger().log(f"Compiled task graph '{title}' is empty.")
        return

    lines = [f"Compiled task graph '{title}':"]
    for step in steps:
        lines.append(f"- {step.name}")
        for line in _action_set_lines(step):
            lines.append(f"    {line}")
        for kind, submodels, access in (
            ("cell", step.cell_data, step.cell_data_access),
            ("face", step.face_data, step.face_data_access),
            ("vertex", step.vertex_data, step.vertex_data_access),
        ):
            if submodels:
                lines.append(
                    f"    {kind} / " + ", ".join(_submodel_label(submodel, access) for submodel in submodels)
                )
        if step.has_to_repeat:
            lines.append(f"    repeats while: {step.has_to_repeat}")

    peano4.api.Logger().log("\n".join(lines))


def _visualise_compiled_step_sequence(steps, title: str) -> None:
    """!
    Simplified sibling of peano4.api.taskgraph.Graph.visualise(): the same
    UML-state-diagram look (rounded-rectangle states, an internal-activity
    compartment listing "do / <name>" per action set plus "cell / <name>",
    "face / <name>", "vertex / <name>" per kind of mesh data the step uses,
    self-loops for repeating steps, initial/final pseudostates), but laid
    out as a single left-to-right chain following steps' list order, since a
    compiled sequence - unlike a Graph - has no dependency structure left to
    derive layers from; it already is the order.
    """
    import matplotlib.pyplot as plt
    from matplotlib.patches import Circle, FancyArrowPatch, FancyBboxPatch
    from matplotlib.path import Path

    if not steps:
        print(f"Compiled task graph '{title}' is empty - nothing to visualise.")
        return

    HEADER_HEIGHT = 0.52
    ACTIVITY_LINE_HEIGHT = 0.22
    BODY_PADDING = 0.14
    MIN_WIDTH = 1.7
    CHAR_WIDTH = 0.10
    HORIZONTAL_GAP = 1.3
    SELF_LOOP_HEIGHT = 0.4
    SELF_LOOP_LABEL_HEIGHT = 0.22
    INCH_PER_UNIT = 0.62
    PADDING = 0.5

    def activity_lines(step):
        lines = _action_set_lines(step)
        for kind, submodels, access in (
            ("cell", step.cell_data, step.cell_data_access),
            ("face", step.face_data, step.face_data_access),
            ("vertex", step.vertex_data, step.vertex_data_access),
        ):
            if submodels:
                lines.append(f"{kind} / " + ", ".join(_submodel_label(submodel, access) for submodel in submodels))
        return lines

    width_of_step, height_of_step = {}, {}
    for step in steps:
        lines = activity_lines(step)
        longest_line = max([len(step.name)] + [len(line) for line in lines])
        width_of_step[step] = max(MIN_WIDTH, CHAR_WIDTH * longest_line + 0.6)
        height_of_step[step] = HEADER_HEIGHT + (
            BODY_PADDING + len(lines) * ACTIVITY_LINE_HEIGHT + 0.5 * BODY_PADDING if lines else 0
        )

    x_step = max(width_of_step.values()) + HORIZONTAL_GAP
    position_of_step = {step: (i * x_step, 0.0) for i, step in enumerate(steps)}
    PSEUDOSTATE_OFFSET = x_step * 0.55

    max_height = max(height_of_step.values())
    max_repeat_extra = (
        SELF_LOOP_HEIGHT + SELF_LOOP_LABEL_HEIGHT if any(step.has_to_repeat for step in steps) else 0.0
    )

    min_x = -PSEUDOSTATE_OFFSET - 0.09 - PADDING
    max_x = (len(steps) - 1) * x_step + PSEUDOSTATE_OFFSET + 0.11 + PADDING
    min_y = -max_height / 2 - PADDING
    max_y = max_height / 2 + max_repeat_extra + PADDING

    LEFT_CHROME, RIGHT_CHROME, TOP_CHROME, BOTTOM_CHROME = 0.1, 0.1, 0.4, 0.5
    data_width_inches = (max_x - min_x) * INCH_PER_UNIT
    data_height_inches = (max_y - min_y) * INCH_PER_UNIT
    figure_width = data_width_inches + LEFT_CHROME + RIGHT_CHROME
    figure_height = data_height_inches + TOP_CHROME + BOTTOM_CHROME

    figure = plt.figure(figsize=(figure_width, figure_height))
    axes = figure.add_axes(
        (
            LEFT_CHROME / figure_width,
            BOTTOM_CHROME / figure_height,
            data_width_inches / figure_width,
            data_height_inches / figure_height,
        )
    )
    axes.set_xlim(min_x, max_x)
    axes.set_ylim(min_y, max_y)

    state_patch = {}
    for step in steps:
        x, y = position_of_step[step]
        w, h = width_of_step[step], height_of_step[step]
        lines = activity_lines(step)

        box = FancyBboxPatch(
            (x - w / 2, y - h / 2),
            w,
            h,
            boxstyle="round,pad=0,rounding_size=0.09",
            linewidth=1.3,
            edgecolor="black",
            facecolor="white",
            zorder=2,
        )
        axes.add_patch(box)
        state_patch[step] = box

        header_center_y = y + h / 2 - HEADER_HEIGHT / 2 if lines else y
        axes.annotate(
            step.name,
            (x, header_center_y),
            ha="center", va="center", zorder=3, fontsize=7.5, fontweight="bold",
        )

        if lines:
            separator_y = y + h / 2 - HEADER_HEIGHT
            axes.plot(
                [x - w / 2, x + w / 2], [separator_y, separator_y],
                color="black", linewidth=0.8, zorder=3,
            )
            for index, line in enumerate(lines):
                line_y = separator_y - BODY_PADDING - index * ACTIVITY_LINE_HEIGHT - 0.08
                axes.annotate(
                    line,
                    (x - w / 2 + 0.12, line_y),
                    ha="left", va="center", zorder=3, fontsize=6.5, color="dimgray",
                )

    for i in range(len(steps) - 1):
        axes.add_patch(
            FancyArrowPatch(
                position_of_step[steps[i]],
                position_of_step[steps[i + 1]],
                patchA=state_patch[steps[i]],
                patchB=state_patch[steps[i + 1]],
                arrowstyle="-|>",
                mutation_scale=13,
                color="black",
                linewidth=1.1,
                zorder=1,
            )
        )

    for step in steps:
        if step.has_to_repeat:
            x, y = position_of_step[step]
            w, h = width_of_step[step], height_of_step[step]
            top_y = y + h / 2
            left_x, right_x = x - 0.18 * w, x + 0.18 * w
            path = Path(
                [
                    (left_x, top_y),
                    (left_x, top_y + SELF_LOOP_HEIGHT),
                    (right_x, top_y + SELF_LOOP_HEIGHT),
                    (right_x, top_y),
                ],
                [Path.MOVETO, Path.CURVE4, Path.CURVE4, Path.CURVE4],
            )
            axes.add_patch(
                FancyArrowPatch(
                    path=path,
                    arrowstyle="-|>",
                    mutation_scale=12,
                    color="crimson",
                    linewidth=1.1,
                    zorder=2.5,
                )
            )
            axes.annotate(
                step.has_to_repeat,
                (x, top_y + SELF_LOOP_HEIGHT + 0.06),
                ha="center", va="bottom", zorder=3, fontsize=6, style="italic", color="crimson",
            )

    first_pos = position_of_step[steps[0]]
    initial_pos = (-PSEUDOSTATE_OFFSET, first_pos[1])
    initial_patch = Circle(initial_pos, 0.09, facecolor="black", edgecolor="black", zorder=3)
    axes.add_patch(initial_patch)
    axes.add_patch(
        FancyArrowPatch(
            initial_pos, first_pos,
            patchA=initial_patch, patchB=state_patch[steps[0]],
            arrowstyle="-|>", mutation_scale=13, color="black", linewidth=1.1, zorder=1,
        )
    )

    last_pos = position_of_step[steps[-1]]
    final_pos = (last_pos[0] + PSEUDOSTATE_OFFSET, last_pos[1])
    final_outer = Circle(final_pos, 0.11, facecolor="white", edgecolor="black", linewidth=1.3, zorder=3)
    axes.add_patch(final_outer)
    axes.add_patch(Circle(final_pos, 0.05, facecolor="black", edgecolor="none", zorder=4))
    axes.add_patch(
        FancyArrowPatch(
            last_pos, final_pos,
            patchA=state_patch[steps[-1]], patchB=final_outer,
            arrowstyle="-|>", mutation_scale=13, color="black", linewidth=1.1, zorder=1,
        )
    )

    figure.suptitle(title, y=1.0 - 0.5 * TOP_CHROME / figure_height, fontsize=10)
    axes.set_xticks([])
    axes.set_yticks([])
    axes.set_aspect("equal")
    # Non-blocking - @see peano4.api.taskgraph.Graph.wait_for_visualisations(),
    # which the caller invokes once after a whole group of compiled
    # sequences (e.g. SetupSimulation/TimeStep/Plot) has been reported, to
    # let them all appear side by side rather than one at a time.
    plt.show(block=False)
