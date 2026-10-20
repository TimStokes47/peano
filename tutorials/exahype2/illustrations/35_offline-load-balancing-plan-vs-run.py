# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the plan-vs-run PNG of the tutorial 35_offline-load-balancing.ipynb

The script computes the offline decomposition of the tutorial's regular mesh
along Peano's space-filling curve, keeps the cells along the periodic
boundary on tree 0, and hands out all splits bottom-up. It then builds the
tutorial's Euler setup with this hardcoded load balancing, runs it with four
threads, and draws the offline plan next to the subdomains that Peano
actually builds. Cells which end up in a different subdomain than planned
are crossed out. Call the script from anywhere:

     python3 tutorials/exahype2/illustrations/35_offline-load-balancing-plan-vs-run.py

All builds and outputs go into a temporary directory (or --work-directory).
The script needs matplotlib, but no ParaView.

"""
import argparse
import importlib.util
import os
import tempfile

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from matplotlib.patches import Patch

Here = os.path.dirname(os.path.abspath(__file__))

# The file names of the scripts start with a digit, i.e. we cannot import them
_specification = importlib.util.spec_from_file_location(
    "split_modes_illustrations",
    os.path.join(Here, "35_offline-load-balancing-split-modes.py"),
)
split_modes = importlib.util.module_from_spec(_specification)
_specification.loader.exec_module(split_modes)
domain_decomposition = split_modes.domain_decomposition

import toolbox.loadbalancing.api.input.RegularGridGenerator as RegularGridGenerator
import toolbox.loadbalancing.api.output.Hardcoded as Hardcoded
import toolbox.loadbalancing.api.strategies.Decomposition as Decomposition
import toolbox.loadbalancing.api.strategies.SFC as SFC
import toolbox.loadbalancing.api.strategies.SplitTrees as SplitTrees

PlanVsRunPicture = os.path.join(Here, "35_offline-load-balancing-plan-vs-run.png")

CellsPerAxis = split_modes.CellsPerAxis
TreeColours = split_modes.TreeColours


def offline_plan():
    """! Split tree and Hardcoded arguments of the tutorial's offline decomposition"""
    tree = RegularGridGenerator.RegularGridGenerator(max_level=3, dimensions=2)
    SplitTrees.split_tree(
        tree,
        decomposition=Decomposition.equidistant_splitting(
            peano_tree=tree, number_of_subtrees=domain_decomposition.NumberOfThreads
        ),
        periodic_BC=[True, True],
    )
    destination_ranks = Hardcoded.assign_trees_to_ranks_round_Robin(
        number_of_subtrees=tree.number_of_subtrees, max_ranks=1
    )
    configuration, mode = Hardcoded.create_constructor_initialisation_string(
        tree, destination_ranks, modes=Hardcoded.BottomUp
    )
    owner = np.zeros((CellsPerAxis, CellsPerAxis), dtype=int)
    for x in range(CellsPerAxis):
        for y in range(CellsPerAxis):
            owner[y, x] = tree._tree_arrays[2][x][y][1]
    return owner, configuration


def match_subdomains(planned, built):
    """!

    Number the subdomains of a run like the planned subdomains

    The run does not tell us Peano's tree numbers (see
    split_modes.read_subdomains()). We therefore give each subdomain of the
    run the number of the planned subdomain with which it overlaps most.

    """
    result = -np.ones_like(built)
    for subdomain in range(domain_decomposition.NumberOfThreads):
        mask = built == subdomain
        overlaps = [
            int((planned[mask] == tree).sum())
            for tree in range(domain_decomposition.NumberOfThreads)
        ]
        result[mask] = int(np.argmax(overlaps))
    return result


def draw(axis, owner, title, show_curve=False, crosses=None):
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
    if show_curve:
        curve = [
            SFC.Peano_to_Cartesian_2d(i, CellsPerAxis)
            for i in range(CellsPerAxis * CellsPerAxis)
        ]
        axis.plot(
            [x + 0.5 for x, y in curve],
            [y + 0.5 for x, y in curve],
            color="white",
            lw=0.7,
        )
    if crosses is not None:
        ys, xs = np.nonzero(crosses)
        axis.scatter(xs + 0.5, ys + 0.5, marker="x", color="black", s=14, lw=0.9)
    counts = [
        int((owner == tree).sum())
        for tree in range(domain_decomposition.NumberOfThreads)
    ]
    axis.set_title(
        title + "\ncells per subdomain: " + " / ".join(str(c) for c in counts),
        fontsize=10,
    )
    axis.set_xticks([])
    axis.set_yticks([])


def render(run_directory, picture=PlanVsRunPicture):
    planned, configuration = offline_plan()
    built = match_subdomains(planned, split_modes.read_subdomains(run_directory))
    differences = planned != built

    figure, axes = plt.subplots(1, 2, figsize=(9.0, 5.6))
    draw(axes[0], planned, "Offline plan along Peano's curve", show_curve=True)
    draw(
        axes[1],
        built,
        "Run with bottom-up splits: {} cell(s) differ".format(int(differences.sum())),
        crosses=differences,
    )
    figure.legend(
        handles=[Patch(color=TreeColours[0], label="tree 0 (keeps the periodic frame)")]
        + [Patch(color=c, label="") for c in TreeColours[1:-1]]
        + [Patch(color=TreeColours[-1], label="trees split off from tree 0")],
        loc="lower center",
        ncol=4,
        frameon=False,
        fontsize=9,
        columnspacing=0.4,
    )
    plt.tight_layout(rect=(0, 0.06, 1, 0.97))
    plt.savefig(picture, dpi=130, bbox_inches="tight")
    return int(differences.sum())


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the plan-vs-run PNG of the tutorial 35_offline-load-balancing.ipynb"
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
        prefix="35_offline-load-balancing-plan-vs-run-"
    )
    print("build and run in {}".format(work_directory))
    planned, configuration = offline_plan()
    domain_decomposition.build_and_run(
        work_directory,
        os.path.abspath(args.peano_root),
        tutorial="35_offline-load-balancing",
        namespace="offlineloadbalancing",
        load_balancer="toolbox::loadbalancing::strategies::Hardcoded",
        load_balancer_arguments=configuration,
    )
    differences = render(work_directory)
    print("wrote {} ({} cell(s) differ)".format(PlanVsRunPicture, differences))
