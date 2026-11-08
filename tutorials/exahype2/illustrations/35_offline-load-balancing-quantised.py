# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the quantised-decomposition PNG of the tutorial 35_offline-load-balancing.ipynb

The script computes two offline decompositions of the tutorial's regular
mesh along Peano's space-filling curve, the equidistant one and the quantised
one, keeps the cells along the periodic boundary on tree 0, and draws them next
to each other. It does not build or run anything. Call it from anywhere:

     python3 tutorials/exahype2/illustrations/35_offline-load-balancing-quantised.py

The script needs matplotlib, but no ParaView.

"""
import importlib.util
import os

import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

Here = os.path.dirname(os.path.abspath(__file__))

# The file names of the scripts start with a digit, i.e. we cannot import them
_specification = importlib.util.spec_from_file_location(
    "plan_vs_run_illustrations",
    os.path.join(Here, "35_offline-load-balancing-plan-vs-run.py"),
)
plan_vs_run = importlib.util.module_from_spec(_specification)
_specification.loader.exec_module(plan_vs_run)

import toolbox.loadbalancing.api.input.RegularGridGenerator as RegularGridGenerator
import toolbox.loadbalancing.api.strategies.Decomposition as Decomposition
import toolbox.loadbalancing.api.strategies.SplitTrees as SplitTrees

QuantisedPicture = os.path.join(Here, "35_offline-load-balancing-quantised.png")
NumberOfSubtrees = plan_vs_run.domain_decomposition.NumberOfThreads
CellsPerAxis = plan_vs_run.CellsPerAxis


def owner_of_cells(quantised):
    """!
    Owner of each fine grid cell after the offline decomposition

    @param quantised: Bool
      Quantised instead of equidistant chunk sizes
    """
    tree = RegularGridGenerator.RegularGridGenerator(max_level=3, dimensions=2)
    if quantised:
        decomposition = Decomposition.quantised_splitting(
            peano_tree=tree, number_of_subtrees=NumberOfSubtrees, dimension=2
        )
    else:
        decomposition = Decomposition.equidistant_splitting(
            peano_tree=tree, number_of_subtrees=NumberOfSubtrees
        )
    SplitTrees.split_tree(tree, decomposition=decomposition, periodic_BC=[True, True])
    owner = np.zeros((CellsPerAxis, CellsPerAxis), dtype=int)
    for x in range(CellsPerAxis):
        for y in range(CellsPerAxis):
            owner[y, x] = tree._tree_arrays[2][x][y][1]
    return owner


if __name__ == "__main__":
    figure, axes = plt.subplots(1, 2, figsize=(9.0, 5.2))
    plan_vs_run.draw(axes[0], owner_of_cells(False), "Equidistant decomposition")
    plan_vs_run.draw(axes[1], owner_of_cells(True), "Quantised decomposition")
    plt.tight_layout()
    plt.savefig(QuantisedPicture, dpi=130, bbox_inches="tight")
    print("wrote {}".format(QuantisedPicture))
