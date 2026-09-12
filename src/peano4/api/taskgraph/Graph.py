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

    def visualise(self, title: str = "Task graph"):
        """!
        Render this task graph with matplotlib: one node per task, an arrow
        from every dependency to the task that depends on it. Tasks are laid
        out left to right by dependency depth (a task without dependencies
        sits on the left; everything that - directly or transitively -
        depends on it sits to its right), so following the arrows always
        moves rightwards. A task with may_repeat set is additionally drawn
        with a small self-loop to indicate it can run again on itself.

        Every action set attached to a task (@see Task.add_action_set) is
        drawn as a small leaf dangling below-right of that task's own node,
        connected to it by a plain (arrowless) line - the same "task owns
        these action sets" containment relationship an old file system
        browser would draw as files nested under a folder, as opposed to
        the dependency arrows between tasks, which describe order, not
        ownership.

        Opens a blocking matplotlib window; close it to continue.
        """
        import numpy as np
        import matplotlib.pyplot as plt
        from matplotlib.patches import FancyArrowPatch
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
            Task.GridEntity.GLOBAL: "tab:red",
        }

        position_of_task = {}
        for layer, tasks_in_layer in layers.items():
            for row, task in enumerate(tasks_in_layer):
                position_of_task[task] = (layer, -row)

        figure, axes = plt.subplots()

        # Node markers are drawn with this area (in points^2, matplotlib's
        # scatter unit); shrink every arrow by the marker's radius so the
        # arrowhead lands next to the node instead of underneath it.
        node_marker_size = 2000
        node_radius_in_points = (node_marker_size / 3.14159265) ** 0.5

        for task in self.tasks:
            for dependency in task.dependencies:
                axes.add_patch(
                    FancyArrowPatch(
                        position_of_task[dependency],
                        position_of_task[task],
                        arrowstyle="-|>",
                        mutation_scale=15,
                        connectionstyle="arc3,rad=0.08",
                        color="black",
                        shrinkA=node_radius_in_points,
                        shrinkB=node_radius_in_points,
                        zorder=1,
                    )
                )

            if task.may_repeat:
                x, y = position_of_task[task]
                a, b = 0.07, 0.07  # semi-major, semi-minor axes
                loop_centre_x, loop_centre_y = x, y

                theta = np.linspace(0, np.pi * 1.85, 100)
                arrow_x, arrow_y = a * np.cos(theta) + loop_centre_x, b * np.sin(theta) + loop_centre_y

                verts = list(zip(arrow_x, arrow_y))
                codes = [Path.MOVETO] + [Path.LINETO] * (len(verts) - 1)
                path = Path(verts, codes)

                axes.add_patch(
                    FancyArrowPatch(
                        path=path,
                        arrowstyle="-|>",
                        mutation_scale=15,
                        color="crimson",
                        zorder=2.5,
                    )
                )

        # Action sets dangle below-right of the task that owns them, one
        # step further down (and slightly further right) per action set -
        # the same staircase an old file system browser draws for files
        # nested under a folder.
        action_set_marker_size = node_marker_size / 6
        action_set_radius_in_points = (action_set_marker_size / 3.14159265) ** 0.5
        action_set_row_height = 0.28

        for task, (x, y) in position_of_task.items():
            axes.scatter([x], [y], s=node_marker_size, color=entity_colour.get(task.entity, "tab:gray"), zorder=2)
            axes.annotate(
                f"{task.name}\n({task.entity.name})",
                (x, y),
                ha="center", va="center", zorder=3, fontsize=7,
            )

            for index, action_set in enumerate(task.action_sets):
                leaf_x = x + 0.22
                leaf_y = y - action_set_row_height * (index + 1)

                axes.add_patch(
                    FancyArrowPatch(
                        (x, y),
                        (leaf_x, leaf_y),
                        arrowstyle="-",
                        linestyle=(0, (2, 2)),
                        linewidth=1,
                        color="gray",
                        shrinkA=node_radius_in_points,
                        shrinkB=action_set_radius_in_points,
                        zorder=1,
                    )
                )
                axes.scatter(
                    [leaf_x], [leaf_y],
                    s=action_set_marker_size,
                    color="whitesmoke", edgecolors="gray",
                    zorder=2,
                )
                axes.annotate(
                    action_set.get_action_set_name(),
                    (leaf_x, leaf_y),
                    xytext=(8, 0), textcoords="offset points",
                    ha="left", va="center", zorder=3, fontsize=6, color="dimgray",
                )

        axes.set_title(title)
        axes.set_xlabel("Dependency depth")
        axes.set_yticks([])
        axes.margins(0.2)
        plt.tight_layout()
        plt.show()
