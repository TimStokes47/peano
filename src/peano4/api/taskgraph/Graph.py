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
        moves rightwards. Opens a blocking matplotlib window; close it to
        continue.
        """
        import matplotlib.pyplot as plt
        from matplotlib.patches import FancyArrowPatch

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
                        zorder=1,
                    )
                )

        for task, (x, y) in position_of_task.items():
            axes.scatter([x], [y], s=2000, color=entity_colour.get(task.entity, "tab:gray"), zorder=2)
            axes.annotate(
                f"{task.name}\n({task.entity.name})",
                (x, y),
                ha="center", va="center", zorder=3, fontsize=7,
            )

        axes.set_title(title)
        axes.set_xlabel("Dependency depth")
        axes.set_yticks([])
        axes.margins(0.2)
        plt.tight_layout()
        plt.show()
