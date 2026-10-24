# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Generate the illustrations of exahype2.api.rkfd4.CentralDifferencesWithRungeKutta

Run from the repository root via

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
PYTHONPATH=src python3 src/exahype2/api/rkfd4/central-differences-with-runge-kutta-illustrations.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The script writes two PNGs next to it, which are referenced from the
documentation of CentralDifferencesWithRungeKutta:

- central-differences-with-runge-kutta-stencil.png draws the reconstructed
  2d patch of a single cell, i.e. a 9x9 patch with a halo of three, and the
  stencil of one grid cell next to the patch boundary: the fourth-order
  central differences reach two grid cells along each axis, the
  Kreiss-Oliger dissipation three.
- central-differences-with-runge-kutta-time-step-task-graph.png shows the
  time stepping task graph of a second-order Runge-Kutta scheme as drawn by
  peano4.api.taskgraph.Graph.visualise(). The script constructs it from the
  actual numerical scheme for a solver with a flux, a maximum eigenvalue and
  Dirichlet boundary conditions, so the picture always matches the code. The
  kernels this requires are generated into a temporary directory.

"""
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.patches import Patch, Rectangle

PATCH_SIZE = 9
HALO = 3

COLOUR_HALO = "#fdae6b"
COLOUR_HALO_CORNER = "#e0e0e0"
COLOUR_PATCH = "#9ecae1"
COLOUR_DIFFERENCES = "#31a354"
COLOUR_DISSIPATION = "#a1d99b"
COLOUR_GRID_CELL = "#de2d26"
COLOUR_GRID = "#505050"


def stencil():
    figure, axes = plt.subplots(figsize=(6.5, 7.4))
    for i in range(-HALO, PATCH_SIZE + HALO):
        for j in range(-HALO, PATCH_SIZE + HALO):
            outside_x = i < 0 or i >= PATCH_SIZE
            outside_y = j < 0 or j >= PATCH_SIZE
            if outside_x and outside_y:
                colour, hatch = COLOUR_HALO_CORNER, "////"
            elif outside_x or outside_y:
                colour, hatch = COLOUR_HALO, None
            else:
                colour, hatch = COLOUR_PATCH, None
            axes.add_patch(
                Rectangle(
                    (i, j),
                    1,
                    1,
                    facecolor=colour,
                    edgecolor=COLOUR_GRID,
                    linewidth=0.5,
                    hatch=hatch,
                )
            )

    # Stencil of the grid cell (1,4): two neighbours per direction for the
    # central differences, the third one for the dissipation only
    centre = (1, 4)
    for distance in range(1, HALO + 1):
        colour = COLOUR_DIFFERENCES if distance <= 2 else COLOUR_DISSIPATION
        for dx, dy in [(distance, 0), (-distance, 0), (0, distance), (0, -distance)]:
            axes.add_patch(
                Rectangle(
                    (centre[0] + dx + 0.15, centre[1] + dy + 0.15),
                    0.7,
                    0.7,
                    facecolor=colour,
                    edgecolor="black",
                    linewidth=0.8,
                )
            )
    axes.add_patch(
        Rectangle(
            (centre[0] + 0.15, centre[1] + 0.15),
            0.7,
            0.7,
            facecolor=COLOUR_GRID_CELL,
            edgecolor="black",
            linewidth=0.8,
        )
    )

    for offset, linewidth in [(-HALO, 2.0), (0, 2.0)]:
        size = PATCH_SIZE - 2 * offset
        axes.add_patch(
            Rectangle(
                (offset, offset),
                size,
                size,
                fill=False,
                edgecolor="black",
                linewidth=linewidth,
            )
        )

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
            Patch(facecolor=COLOUR_PATCH, edgecolor=COLOUR_GRID, label="patch"),
            Patch(facecolor=COLOUR_HALO, edgecolor=COLOUR_GRID, label="halo"),
            Patch(
                facecolor=COLOUR_HALO_CORNER,
                edgecolor=COLOUR_GRID,
                hatch="////",
                label="halo corner (not used)",
            ),
            Patch(
                facecolor=COLOUR_GRID_CELL,
                edgecolor="black",
                label="grid cell to update",
            ),
            Patch(
                facecolor=COLOUR_DIFFERENCES,
                edgecolor="black",
                label="fourth-order central differences",
            ),
            Patch(
                facecolor=COLOUR_DISSIPATION,
                edgecolor="black",
                label="Kreiss-Oliger dissipation only",
            ),
        ],
        loc="lower center",
        ncol=2,
        frameon=False,
        fontsize=10,
    )
    figure.tight_layout(rect=(0, 0.13, 1, 1))
    output = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "central-differences-with-runge-kutta-stencil.png",
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
        tempfile.gettempdir(),
        "central-differences-with-runge-kutta-illustrations-log.md",
    )

    with tempfile.TemporaryDirectory() as directory:
        peano_project = peano4.api.Project(
            ["illustration"], "illustration", directory=directory
        )
        solver = exahype2.api.Solver("Euler", 4, 0)
        solver.add_pde_term(exahype2.api.Flux(stateless=False))
        solver.add_pde_term(exahype2.api.MaxEigenvalue(stateless=False))
        solver.add_pde_term(exahype2.api.DirichletBoundaryConditions(stateless=False))
        solver.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
            solver, patch_size=PATCH_SIZE, rk_order=2
        )
        # Non-periodic boundaries, so the graph shows the boundary tasks
        exahype2_project = exahype2.api.Project(
            ["illustration"], "illustration", "illustration", directory=directory
        )
        exahype2_project.set_global_simulation_parameters(
            dimensions=2,
            offset=[0.0, 0.0],
            size=[1.0, 1.0],
            min_end_time=1.0,
            periodic_BC=[False, False],
        )
        time_step = peano4.api.taskgraph.Graph()
        solver.numerical_scheme.construct_output(
            exahype2_project, peano_project, time_step
        )

    time_step.visualise(
        title="Time stepping task graph of CentralDifferencesWithRungeKutta (rk_order=2)"
    )
    output = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "central-differences-with-runge-kutta-time-step-task-graph.png",
    )
    plt.gcf().savefig(output, dpi=90)
    plt.close(plt.gcf())
    print("Wrote", output)


if __name__ == "__main__":
    stencil()
    time_step_task_graph()
