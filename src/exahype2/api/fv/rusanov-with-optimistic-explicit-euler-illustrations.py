# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Generate the illustration of exahype2.api.fv.RusanovWithOptimisticExplicitEuler

Run from the repository root via

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
python3 src/exahype2/api/fv/rusanov-with-optimistic-explicit-euler-illustrations.py
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The script draws the reconstructed 2d patch of a single cell, i.e. a 10x10
patch with a halo of one, and colours its volumes by class: halo, shell
and patch interior. It writes the picture as
rusanov-with-optimistic-explicit-euler-volume-classes.png next to this
script. The PNG is referenced from the documentation of
RusanovWithOptimisticExplicitEuler.

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


def main():
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
    print("Wrote", output)


if __name__ == "__main__":
    main()
