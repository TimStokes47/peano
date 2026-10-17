# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org

from typing import List

from .Task import Task


class Graph:
    """!

    Holds a sequence of Task instances

    """

    def __init__(self):
        self.tasks: List[Task] = []

    def add_task(self, task: Task):
        self.tasks.append(task)

    def ordered_tasks(self) -> List[Task]:
        """!

        Linearise this graph's tasks into the order
        peano4.api.taskgraph.compiler.Sequential.translate() compiles them
        into: a straightforward recursive traversal of the dependency DAG
        (visit a task's dependencies before the task itself), which - since
        every dependency is visited before whatever depends on it - yields
        a valid topological order.

        This is the single source of truth for "the task after this one"
        that both translate() and
        peano4.api.taskgraph.compiler.PropagateUseStatements.propagate_use_statements()
        rely on; they must agree on the same order, since the latter's
        whole job is to reason about which task's Observer immediately
        follows which other one's once compiled.

        Repeated calls recompute the order from scratch (cheap - task
        graphs here are small) rather than caching it, so adding further
        tasks/dependencies after an earlier call is always reflected.

        """
        order: List[Task] = []
        visited = set()

        def visit(task: Task):
            if task not in visited:
                visited.add(task)
                for dependency in task.dependencies:
                    visit(dependency)
                order.append(task)

        for task in self.tasks:
            visit(task)
        return order

    def log_as_text(self, title: str = "Task graph"):
        """!
        Textual counterpart to visualise(): logs a plain-text rendering of
        this task graph through peano4.api.Logger(), for LoggerVerbosity.TEXT
        runs, where a matplotlib window is neither wanted nor - in a batch/
        non-interactive run - even possible.

        Renders the same information visualise() draws as a UML state
        diagram - every task's grid entity, its dependencies, its action
        sets (as "do / <name>" lines), the cell/face/vertex submodels it
        declares via use_cell()/use_face()/use_vertex() (each annotated
        with its Task.DataAccess qualifier) and whether it repeats - as one
        block of text per task, ordered root to sink by dependency depth
        exactly like visualise() lays its layers out top to bottom.
        """
        import peano4.api

        if not self.tasks:
            peano4.api.Logger().log(f"Task graph '{title}' is empty.")
            return

        # Depth of a task = 0 if it has no dependencies, else 1 + the
        # deepest of its dependencies. Recursive but memoised - task graphs
        # here are small and acyclic by construction. @see visualise(),
        # which computes the very same notion of depth to lay its layers
        # out top to bottom.
        depth_of_task = {}

        def depth(task: Task) -> int:
            if task not in depth_of_task:
                depth_of_task[task] = (
                    0 if not task.dependencies else 1 + max(depth(dependency) for dependency in task.dependencies)
                )
            return depth_of_task[task]

        ordered_tasks = sorted(self.tasks, key=depth)

        lines = [f"Task graph '{title}':"]
        for task in ordered_tasks:
            lines.append(f"- {task.name} «{task.entity.name.lower()}»")
            if task.dependencies:
                lines.append(
                    "    depends on: " + ", ".join(dependency.name for dependency in task.dependencies)
                )
            for action_set in task.action_sets:
                lines.append(f"    do / {action_set.get_action_set_name()}")
            for kind, submodels, access in (
                ("cell", task.cell_data, task.cell_data_access),
                ("face", task.face_data, task.face_data_access),
                ("vertex", task.vertex_data, task.vertex_data_access),
            ):
                if submodels:
                    lines.append(
                        f"    {kind} / "
                        + ", ".join(f"{submodel.name} [{access[submodel].name}]" for submodel in submodels)
                    )
            if task.has_to_repeat:
                lines.append(f"    repeats while: {task.has_to_repeat}")

        peano4.api.Logger().log("\n".join(lines))

    def visualise(self, title: str = "Task graph"):
        """!
        Render this task graph with matplotlib as a UML state diagram: one
        rounded-rectangle state per task, a solid transition arrow from
        every dependency to the task that depends on it. Tasks are laid
        out top to bottom by dependency depth (a task without dependencies
        sits at the top; everything that - directly or transitively -
        depends on it sits below it), so following the transitions always
        moves downwards, exactly like reading a state diagram's flow of
        control. The tasks of one layer sit side by side.

        A filled black circle - the UML initial pseudostate - transitions
        into every task that has no dependencies; every task nothing else
        depends on transitions into a bullseye - the UML final pseudostate.
        A task with has_to_repeat set (i.e. not None) is additionally drawn
        with a small self-transition loop right of its state, the standard UML
        notation for "this state can re-trigger itself", labelled with the
        task's has_to_repeat string itself.

        Every action set attached to a task (@see Task.add_action_set) is
        listed inside that task's own state box as a "do / <name>" line,
        below a separator - the same internal-activity compartment a UML
        state uses to list its entry/exit/do behaviours, as opposed to the
        transition arrows between states, which describe order, not
        ownership. The same compartment also lists the cell/face/vertex
        submodels the task declares via use_cell()/use_face()/use_vertex()
        as "cell / <name> [<Task.DataAccess>]" (and face/vertex
        counterparts) lines, one per kind that is actually used.

        Every state also carries a small coloured badge in its top-left
        corner indicating which kind of grid entity (vertex/face/cell/
        global) the task works on; a legend below the title spells out
        what each colour means, restricted to the entity kinds actually
        used in this particular graph.

        Opens a blocking matplotlib window; close it to continue.
        """
        import matplotlib.pyplot as plt
        from matplotlib.lines import Line2D
        from matplotlib.patches import Circle, FancyArrowPatch, FancyBboxPatch
        from matplotlib.path import Path

        if not self.tasks:
            print(f"Task graph '{title}' is empty - nothing to visualise.")
            return

        # Depth of a task = 0 if it has no dependencies, else 1 + the
        # deepest of its dependencies. Recursive but memoised - task graphs
        # here are small and acyclic by construction.
        depth_of_task = {}

        def depth(task: Task) -> int:
            if task not in depth_of_task:
                depth_of_task[task] = (
                    0 if not task.dependencies else 1 + max(depth(dependency) for dependency in task.dependencies)
                )
            return depth_of_task[task]

        layers = {}
        for task in self.tasks:
            layers.setdefault(depth(task), []).append(task)

        entity_colour = {
            Task.GridEntity.VERTEX: "tab:blue",
            Task.GridEntity.FACE: "tab:orange",
            Task.GridEntity.CELL: "tab:green",
        }

        # A state's box grows with its content: a fixed-height header
        # (task name + «entity» stereotype) plus, if the task owns action
        # sets, one "do / <name>" line per action set in a body
        # compartment below a separator - mirroring how a UML state lists
        # its internal entry/exit/do behaviours.
        HEADER_HEIGHT = 0.52
        ACTIVITY_LINE_HEIGHT = 0.22
        BODY_PADDING = 0.14
        MIN_WIDTH = 1.7
        CHAR_WIDTH = 0.10
        HORIZONTAL_GAP = 0.6
        VERTICAL_GAP = 0.9

        def activity_lines(task: Task):
            lines = [f"do / {action_set.get_action_set_name()}" for action_set in task.action_sets]
            for kind, submodels, access in (
                ("cell", task.cell_data, task.cell_data_access),
                ("face", task.face_data, task.face_data_access),
                ("vertex", task.vertex_data, task.vertex_data_access),
            ):
                if submodels:
                    lines.append(
                        f"{kind} / "
                        + ", ".join(f"{submodel.name} [{access[submodel].name}]" for submodel in submodels)
                    )
            return lines

        width_of_task, height_of_task = {}, {}
        for task in self.tasks:
            lines = activity_lines(task)
            longest_line = max([len(task.name)] + [len(line) for line in lines])
            width_of_task[task] = max(MIN_WIDTH, CHAR_WIDTH * longest_line + 0.6)
            height_of_task[task] = HEADER_HEIGHT + (
                BODY_PADDING + len(lines) * ACTIVITY_LINE_HEIGHT + 0.5 * BODY_PADDING if lines else 0
            )

        SELF_LOOP_WIDTH = 0.4
        SELF_LOOP_LABEL_CHAR_WIDTH = 0.065
        PSEUDOSTATE_OFFSET = 0.6

        # Horizontal space a task's self-transition loop and its label take
        # right of the task's state
        def self_loop_extent(task: Task) -> float:
            if not task.has_to_repeat:
                return 0.0
            return SELF_LOOP_WIDTH + 0.06 + SELF_LOOP_LABEL_CHAR_WIDTH * len(task.has_to_repeat)

        # Every layer forms one row. The rows are stacked top to bottom,
        # each as tall as its tallest state, and the states within a row
        # are placed side by side, centred around x=0. Within a row, the
        # states are sorted by the mean horizontal position of their
        # dependencies, so they sit below their predecessors and fewer
        # transitions cross.
        position_of_task = {}
        row_y = 0.0
        for layer in sorted(layers):
            tasks_in_layer = layers[layer]
            if layer > 0:
                tasks_in_layer = sorted(
                    tasks_in_layer,
                    key=lambda task: sum(position_of_task[dependency][0] for dependency in task.dependencies)
                    / len(task.dependencies),
                )
            row_height = max(height_of_task[task] for task in tasks_in_layer)
            row_width = sum(width_of_task[task] + self_loop_extent(task) for task in tasks_in_layer) + HORIZONTAL_GAP * (
                len(tasks_in_layer) - 1
            )
            x = -row_width / 2
            for task in tasks_in_layer:
                position_of_task[task] = (x + width_of_task[task] / 2, row_y - row_height / 2)
                x += width_of_task[task] + self_loop_extent(task) + HORIZONTAL_GAP
            row_y -= row_height + VERTICAL_GAP
        bottom_y = row_y + VERTICAL_GAP

        root_tasks = [task for task in self.tasks if not task.dependencies]
        sink_tasks = [
            task for task in self.tasks if not any(task in other.dependencies for other in self.tasks)
        ]

        # Text is sized in points, so - to guarantee it actually fits the
        # (content-dependent) box widths/heights computed above in data
        # units - the figure has to be built at a fixed, known number of
        # inches per data unit, rather than left to matplotlib's default
        # autoscaling, which would pick whatever scale happens to fit the
        # default figure size.
        INCH_PER_UNIT = 0.62
        PADDING = 0.5

        min_x = min(position_of_task[task][0] - width_of_task[task] / 2 for task in self.tasks)
        max_x = max(
            position_of_task[task][0] + width_of_task[task] / 2 + self_loop_extent(task) for task in self.tasks
        )
        min_y = bottom_y
        max_y = 0.0
        if root_tasks:
            max_y = PSEUDOSTATE_OFFSET + 0.09
        if sink_tasks:
            min_y = bottom_y - PSEUDOSTATE_OFFSET - 0.11
        min_x, max_x, min_y, max_y = min_x - PADDING, max_x + PADDING, min_y - PADDING, max_y + PADDING

        # Reserve a fixed number of inches around the axes for title/
        # legend/x-label chrome - fixed in inches, not a fraction of the
        # figure, so it does not eat into the data area disproportionately
        # on a small graph - and size the figure so the axes themselves
        # come out to exactly INCH_PER_UNIT inches per data unit in both
        # directions.
        LEGEND_HEIGHT = 0.26
        LEFT_CHROME, RIGHT_CHROME, TOP_CHROME, BOTTOM_CHROME = 0.35, 0.1, 0.4 + LEGEND_HEIGHT, 0.15
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

        # One rounded-rectangle "state" per task, with a small coloured
        # badge in the top-left corner indicating the grid entity it
        # works on, and - if it owns action sets - an internal-activity
        # compartment listing them as "do / <name>".
        state_patch = {}
        for task in self.tasks:
            x, y = position_of_task[task]
            w, h = width_of_task[task], height_of_task[task]
            lines = activity_lines(task)

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
            state_patch[task] = box

            axes.add_patch(
                Circle(
                    (x - w / 2 + 0.10, y + h / 2 - 0.10),
                    0.045,
                    facecolor=entity_colour.get(task.entity, "tab:gray"),
                    edgecolor="none",
                    zorder=3,
                )
            )

            header_center_y = y + h / 2 - HEADER_HEIGHT / 2 if lines else y
            axes.annotate(
                task.name,
                (x, header_center_y + 0.08),
                ha="center", va="center", zorder=3, fontsize=7.5, fontweight="bold",
            )
            axes.annotate(
                f"«{task.entity.name.lower()}»",
                (x, header_center_y - 0.13),
                ha="center", va="center", zorder=3, fontsize=6, style="italic", color="dimgray",
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

        # Transitions between states. patchA/patchB let matplotlib clip
        # the arrow to the actual (content-dependent) state boundary, so
        # this works regardless of how tall a particular box grew.
        for task in self.tasks:
            for dependency in task.dependencies:
                axes.add_patch(
                    FancyArrowPatch(
                        position_of_task[dependency],
                        position_of_task[task],
                        patchA=state_patch[dependency],
                        patchB=state_patch[task],
                        arrowstyle="-|>",
                        mutation_scale=13,
                        connectionstyle="arc3,rad=0.08",
                        color="black",
                        linewidth=1.1,
                        zorder=1,
                    )
                )

            if task.has_to_repeat:
                x, y = position_of_task[task]
                w, h = width_of_task[task], height_of_task[task]
                right_x = x + w / 2
                upper_y, lower_y = y + 0.18 * h, y - 0.18 * h
                path = Path(
                    [
                        (right_x, upper_y),
                        (right_x + SELF_LOOP_WIDTH, upper_y),
                        (right_x + SELF_LOOP_WIDTH, lower_y),
                        (right_x, lower_y),
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
                    task.has_to_repeat,
                    (right_x + SELF_LOOP_WIDTH + 0.06, y),
                    ha="left", va="center", zorder=3, fontsize=6, style="italic", color="crimson",
                )

        # UML initial pseudostate (filled circle) transitioning into every
        # task without dependencies, and UML final pseudostate (bullseye)
        # collecting every task nothing else depends on.
        if root_tasks:
            x = sum(position_of_task[task][0] for task in root_tasks) / len(root_tasks)
            initial_pos = (x, PSEUDOSTATE_OFFSET)
            initial_patch = Circle(initial_pos, 0.09, facecolor="black", edgecolor="black", zorder=3)
            axes.add_patch(initial_patch)
            for task in root_tasks:
                axes.add_patch(
                    FancyArrowPatch(
                        initial_pos,
                        position_of_task[task],
                        patchA=initial_patch,
                        patchB=state_patch[task],
                        arrowstyle="-|>",
                        mutation_scale=13,
                        color="black",
                        linewidth=1.1,
                        zorder=1,
                    )
                )

        if sink_tasks:
            x = sum(position_of_task[task][0] for task in sink_tasks) / len(sink_tasks)
            final_pos = (x, bottom_y - PSEUDOSTATE_OFFSET)
            final_outer = Circle(final_pos, 0.11, facecolor="white", edgecolor="black", linewidth=1.3, zorder=3)
            axes.add_patch(final_outer)
            axes.add_patch(Circle(final_pos, 0.05, facecolor="black", edgecolor="none", zorder=4))
            for task in sink_tasks:
                axes.add_patch(
                    FancyArrowPatch(
                        position_of_task[task],
                        final_pos,
                        patchA=state_patch[task],
                        patchB=final_outer,
                        arrowstyle="-|>",
                        mutation_scale=13,
                        color="black",
                        linewidth=1.1,
                        zorder=1,
                    )
                )

        # Title and, right below it, a legend spelling out what the
        # coloured badge on every state stands for - both placed as
        # figure-level (not axes-level) elements in the TOP_CHROME rows
        # reserved for them above, so they stay put regardless of the
        # axes' own data-driven size.
        title_height = TOP_CHROME - LEGEND_HEIGHT
        figure.suptitle(title, y=1.0 - 0.5 * title_height / figure_height, fontsize=10)

        used_entities = [entity for entity in Task.GridEntity if any(task.entity == entity for task in self.tasks)]
        legend_handles = [
            Line2D(
                [0], [0],
                marker="o", linestyle="none", markersize=6,
                markerfacecolor=entity_colour.get(entity, "tab:gray"), markeredgecolor="none",
                label=f"«{entity.name.lower()}»",
            )
            for entity in used_entities
        ]
        figure.legend(
            handles=legend_handles,
            loc="center",
            bbox_to_anchor=(0.5, 1.0 - (title_height + 0.5 * LEGEND_HEIGHT) / figure_height),
            ncol=len(legend_handles),
            frameon=False,
            fontsize=6.5,
            handletextpad=0.3,
            columnspacing=1.2,
        )

        axes.set_ylabel("Dependency depth")
        axes.set_xticks([])
        axes.set_yticks([])
        axes.set_aspect("equal")
        # Non-blocking: several related graphs (e.g. a project's Setup/
        # TimeStep/Plot task graphs) are typically visualised back to back,
        # and should all appear together rather than forcing the user to
        # close each one before the next is even drawn. @see
        # wait_for_visualisations(), which the caller invokes once after
        # such a group to actually pause for the user to look at all of
        # them together.
        plt.show(block=False)


def wait_for_visualisations():
    """!

    Block until the user closes every currently open matplotlib figure -
    the counterpart to Graph.visualise() and
    peano4.api.taskgraph.compiler.ReportCompiledStepSequence.report_compiled_step_sequence()'s
    own visualisation, both of which render with plt.show(block=False) so
    that a whole group of related figures (e.g. a project's Setup/
    TimeStep/Plot task graphs, or their compiled counterparts) can all
    appear together rather than one at a time. Call this once after such
    a group has been produced, to actually pause and let the user look at
    all of them side by side before execution continues.

    A no-op under any verbosity other than LoggerVerbosity.VISUALISATION,
    and also a no-op (does not even import matplotlib) if nothing is
    currently open - e.g. because the group in question turned out to be
    empty.

    """
    import peano4.api

    if not peano4.api.Logger().is_visualisation:
        return

    import matplotlib.pyplot as plt

    if plt.get_fignums():
        plt.show()
