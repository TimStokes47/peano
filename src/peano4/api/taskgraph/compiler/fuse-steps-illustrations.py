# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Generate the illustrations of peano4.api.taskgraph.compiler.FuseSteps

Run from the repository root via

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PYTHONPATH=src python3 src/peano4/api/taskgraph/compiler/fuse-steps-illustrations.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The script builds a few small task graphs, compiles them with
peano4.api.taskgraph.compiler.FuseSteps.translate(), and writes one PNG per
example next to this script: the input task graph (as drawn by
peano4.api.taskgraph.Graph.visualise()) on the left, the compiled sequence
of mesh traversals (as drawn by
peano4.api.taskgraph.compiler.ReportCompiledStepSequence) on the right. The
PNGs are referenced from the documentation of FuseSteps.translate().

"""
import io
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from PIL import Image

from peano4.api.taskgraph import Graph, Task
from peano4.api.taskgraph.compiler import FuseSteps
from peano4.api.taskgraph.compiler.ReportCompiledStepSequence import (
    _visualise_compiled_step_sequence,
)


class _IllustrationActionSet:
    """Minimal stand-in for an action set: the illustrations only need a name"""

    def __init__(self, name):
        self.name = name
        self.descend_invocation_order = 0
        self.parallel = False

    def get_action_set_name(self):
        return self.name


class _IllustrationSubmodel:
    """Minimal stand-in for a submodel: the illustrations only need a name"""

    def __init__(self, name):
        self.name = name


def _task(name, entity, *action_set_names):
    task = Task(name=name, entity=entity)
    for action_set_name in action_set_names:
        task.add_action_set(_IllustrationActionSet(action_set_name))
    return task


def _render_current_figure() -> Image.Image:
    buffer = io.BytesIO()
    plt.gcf().savefig(buffer, format="png", dpi=110)
    plt.close(plt.gcf())
    buffer.seek(0)
    return Image.open(buffer).convert("RGB")


def _write_illustration(graph: Graph, title: str, file_name: str):
    graph.visualise(title=f"{title}: task graph")
    graph_image = _render_current_figure()

    steps = FuseSteps.translate(graph, "Example", None)
    _visualise_compiled_step_sequence(steps, f"{title}: fused mesh traversals")
    steps_image = _render_current_figure()

    height = max(graph_image.height, steps_image.height)
    result = Image.new("RGB", (graph_image.width + steps_image.width, height), "white")
    result.paste(graph_image, (0, 0))
    result.paste(steps_image, (graph_image.width, 0))
    result.save(os.path.join(os.path.dirname(os.path.abspath(__file__)), file_name))


def entity_order_example() -> Graph:
    graph = Graph()
    tasks = [
        _task("a", Task.GridEntity.VERTEX, "A"),
        _task("b", Task.GridEntity.FACE, "B"),
        _task("c", Task.GridEntity.CELL, "C"),
        _task("d", Task.GridEntity.VERTEX, "D"),
        _task("e", Task.GridEntity.CELL, "E"),
    ]
    for previous_task, task in zip(tasks, tasks[1:]):
        task.add_dependency(previous_task)
    for task in tasks:
        graph.add_task(task)
    return graph


def parallel_example() -> Graph:
    graph = Graph()
    vertex_task_1 = _task("v1", Task.GridEntity.VERTEX, "V1")
    vertex_task_2 = _task("v2", Task.GridEntity.VERTEX, "V2")
    cell_task_1 = _task("c1", Task.GridEntity.CELL, "C1")
    cell_task_2 = _task("c2", Task.GridEntity.CELL, "C2")
    final_task = _task("c3", Task.GridEntity.CELL, "C3a", "C3b")
    cell_task_1.add_dependency(vertex_task_1)
    cell_task_2.add_dependency(vertex_task_2)
    final_task.add_dependency(cell_task_1)
    final_task.add_dependency(cell_task_2)
    for task in (vertex_task_1, vertex_task_2, cell_task_1, cell_task_2, final_task):
        graph.add_task(task)
    return graph


def data_conflict_example() -> Graph:
    patch = _IllustrationSubmodel("Q")
    patch_with_halo = _IllustrationSubmodel("QWithHalo")
    face_data = _IllustrationSubmodel("QFace")

    graph = Graph()
    reconstruct = _task("Reconstruct", Task.GridEntity.CELL, "ReconstructPatchWithHalo")
    update = _task("Update cell", Task.GridEntity.CELL, "UpdateCell")
    project = _task("Project on faces", Task.GridEntity.CELL, "ProjectPatchOntoFaces")
    update.add_dependency(reconstruct)
    project.add_dependency(update)

    reconstruct.use_cell(patch, Task.DataAccess.READ_AND_WRITE)
    reconstruct.use_cell(patch_with_halo, Task.DataAccess.WRITE)
    reconstruct.use_face(face_data, Task.DataAccess.READ_AND_DISCARD)
    update.use_cell(patch_with_halo, Task.DataAccess.READ_AND_DISCARD)
    update.use_cell(patch, Task.DataAccess.WRITE)
    project.use_cell(patch, Task.DataAccess.READ_AND_WRITE)
    project.use_face(face_data, Task.DataAccess.WRITE)

    for task in (reconstruct, update, project):
        graph.add_task(task)
    return graph


def face_before_cell_example() -> Graph:
    patch = _IllustrationSubmodel("Q")
    patch_with_halo = _IllustrationSubmodel("QWithHalo")
    face_data = _IllustrationSubmodel("QFace")

    graph = Graph()
    boundary = _task("Apply BCs", Task.GridEntity.FACE, "ApplyBoundaryConditions")
    reconstruct = _task("Reconstruct", Task.GridEntity.CELL, "ReconstructPatchWithHalo")
    update = _task("Update cell", Task.GridEntity.CELL, "UpdateCell")
    reconstruct.add_dependency(boundary)
    update.add_dependency(reconstruct)

    boundary.use_face(face_data, Task.DataAccess.READ_AND_WRITE)
    reconstruct.use_cell(patch, Task.DataAccess.READ_AND_WRITE)
    reconstruct.use_cell(patch_with_halo, Task.DataAccess.WRITE)
    reconstruct.use_face(face_data, Task.DataAccess.READ_AND_DISCARD)
    update.use_cell(patch_with_halo, Task.DataAccess.READ_AND_DISCARD)
    update.use_cell(patch, Task.DataAccess.WRITE)

    for task in (boundary, reconstruct, update):
        graph.add_task(task)
    return graph


if __name__ == "__main__":
    _write_illustration(
        entity_order_example(), "Entity order", "fuse-steps-entity-order.png"
    )
    _write_illustration(parallel_example(), "Parallel tasks", "fuse-steps-parallel.png")
    _write_illustration(
        data_conflict_example(), "Conflicting face data", "fuse-steps-data-conflict.png"
    )
    _write_illustration(
        face_before_cell_example(),
        "Face task before cell task",
        "fuse-steps-face-before-cell.png",
    )
