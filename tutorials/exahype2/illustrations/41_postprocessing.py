# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 41_postprocessing.ipynb

The script builds the tutorial's shallow water setup over a flat sea floor
(the same one as 11_tsunami.py), runs it, converts the output into VTK files
with --split-unknowns, and plots the array hu of the snapshot at t=0.2 into
the directory of this script. Call it from anywhere:

     python3 tutorials/exahype2/illustrations/41_postprocessing.py

All builds and outputs go into a temporary directory (or --work-directory).

"""
import argparse
import importlib.util
import os
import subprocess
import sys
import tempfile

Here = os.path.dirname(os.path.abspath(__file__))
PeanoRoot = os.path.abspath(os.path.join(Here, "..", "..", ".."))
sys.path.insert(0, os.path.join(PeanoRoot, "src"))

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

import vtk
from vtk.util.numpy_support import vtk_to_numpy


def load_tsunami_illustrations():
    """! The module 11_tsunami.py, whose name is no valid Python identifier"""
    spec = importlib.util.spec_from_file_location(
        "tsunami_illustrations", os.path.join(Here, "11_tsunami.py")
    )
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def read_vtu(file_name):
    reader = vtk.vtkXMLUnstructuredGridReader()
    reader.SetFileName(file_name)
    reader.Update()
    return reader.GetOutput()


def plot_array(grid, name, ax):
    """! Plot the cell data array name of grid, as the tutorial does"""
    centres = vtk.vtkCellCenters()
    centres.SetInputData(grid)
    centres.Update()
    xyz = vtk_to_numpy(centres.GetOutput().GetPoints().GetData())
    values = vtk_to_numpy(grid.GetCellData().GetArray(name))
    image = ax.tripcolor(xyz[:, 0], xyz[:, 1], values, shading="gouraud")
    ax.set_aspect("equal")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_title(name)
    ax.figure.colorbar(image, ax=ax)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 41_postprocessing.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(
        prefix="41_postprocessing-"
    )
    print("build and run in {}".format(work_directory))
    load_tsunami_illustrations().build_and_run(work_directory)

    subprocess.run(
        [
            sys.executable,
            "-m",
            "tarch.plotter.api.render",
            "--split-unknowns",
            "solution-SWE.peano-patch-file",
        ],
        cwd=work_directory,
        env=dict(os.environ, PYTHONPATH=os.path.join(PeanoRoot, "src")),
        check=True,
        stdout=subprocess.DEVNULL,
    )

    fig, ax = plt.subplots(figsize=(6, 5))
    plot_array(
        read_vtu(os.path.join(work_directory, "solution-SWE.SWEQ-4.vtu")), "hu", ax
    )
    fig.savefig(
        os.path.join(Here, "41_postprocessing-hu.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)
    print("wrote PNG into {}".format(Here))
