# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Generate the illustrations of exahype2.api.fv.RusanovWithOptimisticExplicitEuler

Run from the repository root via

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PYTHONPATH=src:python python3 src/exahype2/api/fv/rusanov-with-optimistic-explicit-euler-illustrations.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The script writes two PNGs next to it, which are referenced from the
documentation of RusanovWithOptimisticExplicitEuler:

- rusanov-with-optimistic-explicit-euler-volume-classes.png draws the
  reconstructed 2d patch of a single cell, i.e. a 10x10 patch with a halo
  of one, and colours its volumes by class: halo, shell and patch interior.
- rusanov-with-optimistic-explicit-euler-time-step-task-graph.png shows
  the time stepping task graph as drawn by peano4.api.taskgraph.Graph.visualise().
  The script constructs it from the actual numerical scheme for a solver
  with a flux and a maximum eigenvalue, so the picture always matches the
  code. The kernels this requires are generated into a temporary directory.

"""
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Patch, Rectangle

PATCH_SIZE = 10
HALO = 1

COLOUR_HALO = "#fdae6b"
COLOUR_HALO_CORNER = "#e0e0e0"
COLOUR_SHELL = "#9ecae1"
COLOUR_PATCH_INTERIOR = "#a1d99b"
COLOUR_GRID = "#505050"


def _volume_class(i, j):
    """Class of volume (i,j) of the reconstructed patch, with 0<=i,j<PATCH_SIZE the patch itself"""
    outside_x = i < 0 or i >= PATCH_SIZE
    outside_y = j < 0 or j >= PATCH_SIZE
    if outside_x and outside_y:
        return "halo corner"
    if outside_x or outside_y:
        return "halo"
    if i < HALO or i >= PATCH_SIZE - HALO or j < HALO or j >= PATCH_SIZE - HALO:
        return "shell"
    return "patch interior"


def volume_classes():
    colours = {
        "halo": COLOUR_HALO,
        "halo corner": COLOUR_HALO_CORNER,
        "shell": COLOUR_SHELL,
        "patch interior": COLOUR_PATCH_INTERIOR,
    }
    figure, axes = plt.subplots(figsize=(6.5, 7.2))
    for i in range(-HALO, PATCH_SIZE + HALO):
        for j in range(-HALO, PATCH_SIZE + HALO):
            volume_class = _volume_class(i, j)
            axes.add_patch(
                Rectangle(
                    (i, j), 1, 1,
                    facecolor=colours[volume_class],
                    edgecolor=COLOUR_GRID,
                    linewidth=0.5,
                    hatch="////" if volume_class == "halo corner" else None,
                )
            )
    # Outlines of the reconstructed patch, of the patch, and of the patch interior
    for offset, linewidth in [(-HALO, 2.0), (0, 2.0), (HALO, 1.5)]:
        size = PATCH_SIZE - 2 * offset
        axes.add_patch(Rectangle((offset, offset), size, size, fill=False, edgecolor="black", linewidth=linewidth))

    interior_size = PATCH_SIZE - 2 * HALO
    axes.text(PATCH_SIZE / 2, PATCH_SIZE / 2, f"patch interior\n{interior_size}x{interior_size}", ha="center", va="center", fontsize=12)
    margin = HALO + 0.3
    axes.set_xlim(-margin, PATCH_SIZE + margin)
    axes.set_ylim(-margin, PATCH_SIZE + margin)
    axes.set_aspect("equal")
    axes.axis("off")
    total_size = PATCH_SIZE + 2 * HALO
    axes.set_title(
        f"Reconstructed patch, {total_size}x{total_size}:\n"
        f"{PATCH_SIZE}x{PATCH_SIZE} patch plus a halo of {HALO}",
        fontsize=12,
    )
    figure.legend(
        handles=[
            Patch(facecolor=COLOUR_HALO, edgecolor=COLOUR_GRID, label="halo"),
            Patch(facecolor=COLOUR_SHELL, edgecolor=COLOUR_GRID, label="shell"),
            Patch(facecolor=COLOUR_PATCH_INTERIOR, edgecolor=COLOUR_GRID, label="patch interior"),
            Patch(facecolor=COLOUR_HALO_CORNER, edgecolor=COLOUR_GRID, hatch="////", label="halo corner (not used by Rusanov)"),
        ],
        loc="lower center",
        ncol=2,
        frameon=False,
        fontsize=11,
    )
    figure.tight_layout(rect=(0, 0.1, 1, 1))
    output = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "rusanov-with-optimistic-explicit-euler-volume-classes.png"
    )
    figure.savefig(output, dpi=100, bbox_inches="tight")
    plt.close(figure)
    print("Wrote", output)


def time_step_task_graph():
    import tempfile

    import peano4.api
    import peano4.api.taskgraph
    import exahype2.api

    # The Python API writes a log file when the program terminates. Keep it
    # out of the working directory.
    peano4.api.Logger().file_name = os.path.join(
        tempfile.gettempdir(), "rusanov-with-optimistic-explicit-euler-illustrations-log.md"
    )

    with tempfile.TemporaryDirectory() as directory:
        peano_project = peano4.api.Project(["illustration"], "illustration", directory=directory)
        solver = exahype2.api.Solver("Euler", 4, 0)
        solver.add_pde_term(exahype2.api.Flux(stateless=False))
        solver.add_pde_term(exahype2.api.MaxEigenvalue(stateless=False))
        solver.numerical_scheme = exahype2.api.fv.RusanovWithOptimisticExplicitEuler(solver, PATCH_SIZE - 1)
        time_step = peano4.api.taskgraph.Graph()
        solver.numerical_scheme.construct_output(None, peano_project, time_step)

    time_step.visualise(title="Time stepping task graph of RusanovWithOptimisticExplicitEuler")
    output = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "rusanov-with-optimistic-explicit-euler-time-step-task-graph.png"
    )
    plt.gcf().savefig(output, dpi=90)
    plt.close(plt.gcf())
    print("Wrote", output)


if __name__ == "__main__":
    volume_classes()
    time_step_task_graph()
