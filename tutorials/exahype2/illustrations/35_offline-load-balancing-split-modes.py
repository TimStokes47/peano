# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the split-mode PNG of the tutorial 35_offline-load-balancing.ipynb

The picture contrasts Peano's two split modes. Its left column sketches how
a top-down and a bottom-up split hand over cells of a small one-dimensional
spacetree. Its right column shows what the two modes make out of the
tutorial's offline decomposition: The script computes the decomposition,
builds the tutorial's Euler setup twice, once with top-down and once with
bottom-up splits, runs both with four threads, and reads the subdomains from
the patch files. Call the script from anywhere:

     python3 tutorials/exahype2/illustrations/35_offline-load-balancing-split-modes.py

All builds and outputs go into a temporary directory (or --work-directory).
The script needs matplotlib, but no ParaView.

"""
import argparse
import importlib.util
import os
import re
import tempfile

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib.patches import Patch, Rectangle

Here = os.path.dirname(os.path.abspath(__file__))

# The file name of the script starts with a digit, i.e. we cannot import it
_specification = importlib.util.spec_from_file_location(
    "offline_load_balancing_illustrations",
    os.path.join(Here, "35_offline-load-balancing.py"),
)
offline_load_balancing = importlib.util.module_from_spec(_specification)
_specification.loader.exec_module(offline_load_balancing)
domain_decomposition = offline_load_balancing.domain_decomposition

import toolbox.loadbalancing.api.input.RegularGridGenerator as RegularGridGenerator
import toolbox.loadbalancing.api.output.Hardcoded as Hardcoded
import toolbox.loadbalancing.api.strategies.Decomposition as Decomposition
import toolbox.loadbalancing.api.strategies.SplitTrees as SplitTrees
import toolbox.loadbalancing.api.strategies.SFC as SFC

SplitModesPicture = os.path.join(Here, "35_offline-load-balancing-split-modes.png")

TreeColours = ["#4c72b0", "#dd8452", "#55a868", "#c44e52"]
CellsPerAxis = 27
DomainOffset = -0.1
DomainSize = 0.2


def offline_decomposition(modes):
    """! Arguments of the hardcoded load balancing with the given split modes"""
    tree = RegularGridGenerator.RegularGridGenerator(max_level=3, dimensions=2)
    SplitTrees.split_tree(
        tree,
        decomposition=Decomposition.equidistant_splitting(
            peano_tree=tree, number_of_subtrees=domain_decomposition.NumberOfThreads
        ),
    )
    destination_ranks = Hardcoded.assign_trees_to_ranks_round_Robin(
        number_of_subtrees=tree.number_of_subtrees, max_ranks=1
    )
    configuration, mode = Hardcoded.create_constructor_initialisation_string(
        tree, destination_ranks, modes=modes
    )
    return configuration


def read_subdomains(directory):
    """!

    Subdomain of each fine grid cell, read from the first snapshot's patch files

    The patch files hold one file per tree, but neither the file names nor the
    order in which the trees write their files tell us the tree number. We
    only know that tree 0 is the one tree which holds the cells along the
    periodic boundary. The result therefore holds 0 for tree 0 and 1, 2, ...
    for the other trees, numbered by the position of their first cell along
    the space-filling curve. These numbers are not necessarily Peano's tree
    numbers, as Peano runs through the curve forth and back in alternating
    mesh sweeps.

    """
    curve_position = np.zeros((CellsPerAxis, CellsPerAxis), dtype=int)
    for position in range(CellsPerAxis * CellsPerAxis):
        x, y = SFC.Peano_to_Cartesian_2d(position, CellsPerAxis)
        curve_position[y, x] = position

    cells_per_file = []
    for tree in range(domain_decomposition.NumberOfThreads):
        file_name = os.path.join(
            directory, "solution-EulerFV-{}.peano-patch-file".format(tree)
        )
        with open(file_name) as patch_file:
            text = patch_file.read()
        cells = []
        for match in re.finditer(r"begin patch\s+offset\s+(\S+)\s+(\S+)", text):
            x = int(
                round(
                    (float(match.group(1)) - DomainOffset) / DomainSize * CellsPerAxis
                )
            )
            y = int(
                round(
                    (float(match.group(2)) - DomainOffset) / DomainSize * CellsPerAxis
                )
            )
            cells.append((y, x))
        cells_per_file.append(cells)

    def holds_boundary_cell(cells):
        return any(min(y, x) == 0 or max(y, x) == CellsPerAxis - 1 for (y, x) in cells)

    def first_curve_position(cells):
        return min(curve_position[y, x] for (y, x) in cells)

    tree_zero = [cells for cells in cells_per_file if holds_boundary_cell(cells)]
    assert len(tree_zero) == 1, "expected exactly one tree along the periodic boundary"
    others = sorted(
        [cells for cells in cells_per_file if not holds_boundary_cell(cells)],
        key=first_curve_position,
    )

    owner = -np.ones((CellsPerAxis, CellsPerAxis), dtype=int)
    for subdomain, cells in enumerate(tree_zero + others):
        for y, x in cells:
            owner[y, x] = subdomain
    return owner


def read_cells_per_tree(directory):
    """! Cells per tree as Peano reports them in the last load balancing statistics of run.log"""
    with open(os.path.join(directory, "run.log")) as log_file:
        statistics = [line for line in log_file if "dumpStatistics" in line]
    return [int(count) for count in re.findall(r"#\d+:(\d+)/", statistics[-1])]


def draw_tree_sketch(axis, title, owners, caption):
    """!

    Draw a one-dimensional spacetree with three levels

    owners holds the owner per level, i.e. a list with 1, 3 and 9 entries.

    """
    for level, level_owners in enumerate(owners):
        width = 9 / len(level_owners)
        for i, owner in enumerate(level_owners):
            axis.add_patch(
                Rectangle(
                    (i * width, -level),
                    width,
                    0.7,
                    facecolor=TreeColours[owner],
                    edgecolor="black",
                    lw=0.8,
                )
            )
    axis.annotate(
        "",
        xy=(9.1, -2 + 0.35),
        xytext=(-0.1, -2 + 0.35),
        arrowprops=dict(arrowstyle="->", color="black", lw=0.8),
    )
    axis.set_xlim(-0.3, 9.3)
    axis.set_ylim(-2.9, 0.9)
    axis.axis("off")
    axis.set_title(title, fontsize=10)
    axis.text(4.5, -2.75, caption, ha="center", va="center", fontsize=8.5)


def draw_subdomains(axis, directory, title):
    """! Draw the subdomains of a run together with Peano's cell counts per tree"""
    owner = read_subdomains(directory)
    axis.imshow(
        owner,
        origin="lower",
        cmap=ListedColormap(TreeColours),
        vmin=0,
        vmax=3,
        extent=(0, CellsPerAxis, 0, CellsPerAxis),
    )
    for k in range(0, CellsPerAxis + 1, 3):
        width = 1.0 if k % 9 == 0 else 0.3
        axis.axhline(k, color="black", lw=width)
        axis.axvline(k, color="black", lw=width)
    counts = read_cells_per_tree(directory)
    axis.set_title(
        title + "\ncells on tree 0, 1, 2, 3: " + " / ".join(str(c) for c in counts),
        fontsize=10,
    )
    axis.set_xticks([])
    axis.set_yticks([])


def render(top_down_directory, bottom_up_directory, picture=SplitModesPicture):
    figure, axes = plt.subplots(
        2, 2, figsize=(9.5, 8.2), gridspec_kw={"width_ratios": [1.25, 1]}
    )
    draw_tree_sketch(
        axes[0][0],
        "Top-down: tree 0 hands 4 cells to tree 1",
        [[0], [1, 1, 0], [1, 1, 1, 1, 1, 1, 0, 0, 0]],
        "Whole refined cells move while cells are still owed:\n"
        "the second coarse cell moves although only 1 cell is owed,\n"
        "so tree 1 gets 6 instead of 4 fine cells",
    )
    draw_tree_sketch(
        axes[1][0],
        "Bottom-up: tree 0 hands 4 cells to tree 1",
        [[0], [1, 0, 0], [1, 1, 1, 1, 0, 0, 0, 0, 0]],
        "Fine cells move one by one along the curve, and a coarse\n"
        "cell follows once all of its children have moved:\n"
        "tree 1 gets exactly 4 fine cells",
    )
    draw_subdomains(axes[0][1], top_down_directory, "Tutorial run, top-down")
    draw_subdomains(axes[1][1], bottom_up_directory, "Tutorial run, bottom-up")
    figure.legend(
        handles=[
            Patch(color=TreeColours[0], label="tree 0"),
            Patch(color=TreeColours[1], label=""),
            Patch(color=TreeColours[2], label=""),
            Patch(
                color=TreeColours[3],
                label="trees split off from tree 0 (tree 1 in the sketches)",
            ),
        ],
        loc="lower center",
        ncol=4,
        frameon=False,
        fontsize=9,
        columnspacing=0.4,
    )
    plt.tight_layout(rect=(0, 0.04, 1, 1))
    plt.savefig(picture, dpi=130)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the split-mode PNG of the tutorial 35_offline-load-balancing.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--peano-root",
        default=domain_decomposition.PeanoRoot,
        help="Peano installation with the compiled libraries (default: repository holding this script)",
    )
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(
        prefix="35_offline-load-balancing-split-modes-"
    )
    print("build and run in {}".format(work_directory))
    directories = {}
    for modes in [Hardcoded.TopDown, Hardcoded.BottomUp]:
        directories[modes] = os.path.join(work_directory, modes)
        os.makedirs(directories[modes], exist_ok=True)
        domain_decomposition.build_and_run(
            directories[modes],
            os.path.abspath(args.peano_root),
            tutorial="35_offline-load-balancing",
            namespace="offlineloadbalancing",
            load_balancer="toolbox::loadbalancing::strategies::Hardcoded",
            load_balancer_arguments=offline_decomposition(modes),
        )
    render(directories[Hardcoded.TopDown], directories[Hardcoded.BottomUp])
    print("wrote {}".format(SplitModesPicture))
