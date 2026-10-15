# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Interactive visualisation of Peano patch files within Jupyter notebooks

The routines here read 2d cell data from a Peano patch file (meta file plus
one file per snapshot) and render one unknown with matplotlib. Within a
notebook, show_solution() adds a time slider, a play button and a selector
for the unknown. plot_snapshot() is the non-interactive counterpart which
can also be used to write PNG files.

Only the default (equidistant) cell mapping in 2d is supported. Adaptive
meshes work, as every patch is drawn individually.

"""
import contextlib
import functools
import io
import os
import sys

import numpy as np
import matplotlib.pyplot as plt

from tarch.plotter.api.input.PatchFileParser import PatchFileParser
from tarch.plotter.api.output.Visualiser import Visualiser


def read_snapshots(file_name):
    """!

    Return list of (time stamp, list of patch files) from a patch meta file

    The included patch files are given relative to the meta file, so we
    can call the routine from any directory.

    """
    visualiser = Visualiser(file_name, start=0, end=sys.maxsize)
    with contextlib.redirect_stdout(io.StringIO()):
        visualiser.read_metadata()
    directory = os.path.dirname(file_name)
    return [
        (
            snapshot.timestamp,
            [os.path.join(directory, f) for f in snapshot.filenames],
        )
        for snapshot in visualiser.snapshots
    ]


@functools.lru_cache(maxsize=256)
def read_patches(patch_file_name):
    """!

    Return list of (offset, size, values) for all patches of one patch file

    values is a numpy array of shape (dof, dof, unknowns), i.e. we index it
    with values[y,x,unknown].

    """
    parser = PatchFileParser(patch_file_name, "", 0)
    parser.parse_file()
    result = []
    for attributes in parser.unknown_attributes.values():
        assert attributes.dimensions == 2, "Only 2d data is supported"
        assert attributes.is_data_associated_to_cell, "Only cell data is supported"
        for patch in attributes.cell_data:
            result.append(
                (
                    patch.offset,
                    patch.size,
                    patch.values.reshape(
                        attributes.dof, attributes.dof, attributes.unknowns
                    ),
                )
            )
    return result


def plot_snapshot(
    file_name, snapshot=-1, unknown=0, clim=None, ax=None, cmap="viridis", label=None
):
    """!

    Plot one unknown of one snapshot

    file_name: String
      Patch meta file as written by the Peano/ExaHyPE plotters.

    snapshot: Integer
      Index of snapshot. Negative values count from the end.

    unknown: Integer
      Index of unknown to plot.

    clim: (float,float) or None
      Colour range. If None, we use the range of the plotted data.

    Returns the matplotlib axes.

    """
    snapshots = read_snapshots(file_name)
    time_stamp, patch_files = snapshots[snapshot]

    if ax is None:
        _, ax = plt.subplots(figsize=(6, 5))

    patches = [p for f in patch_files for p in read_patches(f)]
    if clim is None:
        clim = (
            min(np.min(values[:, :, unknown]) for _, _, values in patches),
            max(np.max(values[:, :, unknown]) for _, _, values in patches),
        )

    for offset, size, values in patches:
        image = ax.imshow(
            values[:, :, unknown],
            origin="lower",
            extent=(offset[0], offset[0] + size[0], offset[1], offset[1] + size[1]),
            vmin=clim[0],
            vmax=clim[1],
            cmap=cmap,
            interpolation="nearest",
        )
    ax.set_xlim(
        min(o[0] for o, _, _ in patches), max(o[0] + s[0] for o, s, _ in patches)
    )
    ax.set_ylim(
        min(o[1] for o, _, _ in patches), max(o[1] + s[1] for o, s, _ in patches)
    )
    ax.set_aspect("equal")
    ax.locator_params(nbins=5)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    name = label if label is not None else "Q[{}]".format(unknown)
    ax.set_title("{} at t={:.4f}".format(name, time_stamp))
    ax.figure.colorbar(image, ax=ax)
    return ax


def show_solution(file_name, unknown=0, unknown_names=None, clim=None):
    """!

    Interactive viewer for a patch file within a Jupyter notebook

    Adds a slider over all snapshots, a play button and - if unknown_names
    is given - a dropdown to pick the unknown.

    unknown_names: List of strings or None
      Names of the unknowns in the order they are stored.

    clim: (float,float) or None
      Fixed colour range. If None, the colour range per unknown is taken
      from the first snapshot, so you see the solution evolve against a
      fixed scale.

    """
    import ipywidgets

    snapshots = read_snapshots(file_name)
    if unknown_names is None:
        unknown_names = ["Q[{}]".format(unknown)]
        unknown_indices = {unknown_names[0]: unknown}
    else:
        unknown_indices = {name: i for i, name in enumerate(unknown_names)}

    @functools.lru_cache(maxsize=None)
    def colour_range(index):
        if clim is not None:
            return clim
        patches = [p for f in snapshots[0][1] for p in read_patches(f)]
        low = min(np.min(values[:, :, index]) for _, _, values in patches)
        high = max(np.max(values[:, :, index]) for _, _, values in patches)
        if low == high:
            low, high = low - 0.5, high + 0.5
        return (low, high)

    def draw(snapshot, name):
        index = unknown_indices[name]
        plot_snapshot(file_name, snapshot, index, clim=colour_range(index), label=name)
        plt.show()

    slider = ipywidgets.IntSlider(
        value=0, min=0, max=len(snapshots) - 1, description="snapshot"
    )
    play = ipywidgets.Play(
        value=0, min=0, max=len(snapshots) - 1, interval=300, description="play"
    )
    ipywidgets.jslink((play, "value"), (slider, "value"))
    selector = ipywidgets.Dropdown(
        options=list(unknown_indices.keys()),
        value=[n for n, i in unknown_indices.items() if i == unknown][0],
        description="unknown",
    )
    output = ipywidgets.interactive_output(draw, {"snapshot": slider, "name": selector})
    return ipywidgets.VBox([ipywidgets.HBox([play, slider, selector]), output])
