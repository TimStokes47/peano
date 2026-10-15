# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 10_euler.ipynb

The script builds the tutorial's Euler setup for two volume sizes, runs it,
probes the solution at the origin and writes all pictures into the
directory of this script. The PDE terms are injected from Python, i.e. we
do not need any hand-written C++ file. Call it from anywhere:

     python3 tutorials/exahype2/illustrations/10_euler.py

All builds and outputs go into a temporary directory (or --work-directory).

"""
import argparse
import csv
import math
import os
import subprocess
import sys
import tempfile

Here = os.path.dirname(os.path.abspath(__file__))
PeanoRoot = os.path.abspath(os.path.join(Here, "..", "..", ".."))
sys.path.insert(0, os.path.join(PeanoRoot, "src"))

import matplotlib

matplotlib.use("Agg")
import matplotlib.patches
import matplotlib.pyplot as plt
import numpy as np

import peano4.api
import exahype2
import tarch.plotter.api.notebook


Flux = """
  const double irho  = 1.0 / Q[0];
  constexpr double gamma = 1.4;
  const double p = (gamma - 1.0) * (Q[3] - 0.5 * irho * (Q[1] * Q[1] + Q[2] * Q[2]));
  switch (normal) {
    case 0:
      F[0] = Q[1];
      F[1] = irho * Q[1] * Q[1] + p;
      F[2] = irho * Q[1] * Q[2];
      F[3] = irho * Q[1] * (Q[3] + p);
      break;
    case 1:
      F[0] = Q[2];
      F[1] = irho * Q[2] * Q[1];
      F[2] = irho * Q[2] * Q[2] + p;
      F[3] = irho * Q[2] * (Q[3] + p);
      break;
  }
"""

MaxEigenvalue = """
  const double irho  = 1.0 / Q[0];
  constexpr double gamma = 1.4;
  const double p = (gamma - 1.0) * (Q[3] - 0.5 * irho * (Q[1] * Q[1] + Q[2] * Q[2]));
  const double c = std::sqrt(gamma * p * irho);
  const double u = Q[1 + normal] * irho;
  lambdaMax[0] = std::max(std::abs(u + c), std::abs(u - c));
"""

InitPointWisely = """
  constexpr double gamma = 1.4;
  constexpr double p     = 1.0;
  constexpr double u     = 1.0;
  const double     rho   = 1.0 + 0.2 * std::sin(2.0 * M_PI * (x[0] + 0.1) / 0.2);
  Q[0] = rho;
  Q[1] = rho * u;
  Q[2] = 0.0;
  Q[3] = p / (gamma - 1) + 0.5 * rho * u * u;
"""

EndTime = 0.2
TimeInBetweenPlots = 0.005
PatchFile = "solution-EulerFV.peano-patch-file"


def exact_density(x, t):
    """! Density of the entropy wave advected with u=1 over [-0.1,0.1]"""
    return 1.0 + 0.2 * np.sin(2.0 * np.pi * (np.asarray(x) - t + 0.1) / 0.2)


def build_and_run(volume_size, directory):
    """! Generate, compile and run the tutorial setup within directory"""
    project = exahype2.api.Project(
        namespace=["tutorials", "exahype2", "euler"],
        project_name="Euler",
        directory=directory,
        executable="euler",
    )
    solver = exahype2.api.Solver("EulerFV", 4, 0)
    solver.add_user_solver_includes("#include <cmath>\n#include <algorithm>\n")
    solver.add_pde_term(exahype2.api.Flux(implementation=Flux, stateless=False))
    solver.add_pde_term(
        exahype2.api.MaxEigenvalue(implementation=MaxEigenvalue, stateless=False)
    )
    solver.add_pde_term(
        exahype2.api.InitPointWisely(implementation=InitPointWisely, stateless=False)
    )
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
    solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        solver, volume_size=volume_size
    )
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver, first_plot_time_stamp=0.0, time_in_between_plots=TimeInBetweenPlots
    )
    project.add_solver(solver)
    project.set_global_simulation_parameters(
        dimensions=2,
        offset=[-0.1, -0.1],
        size=[0.2, 0.2],
        min_end_time=EndTime,
        max_end_time=EndTime,
        periodic_BC=[True, True, True],
    )
    project.set_Peano4_installation(PeanoRoot, peano4.api.output.CompileMode.RELEASE)
    peano4_project = project.generate_Peano4_project()
    peano4_project.generate()
    subprocess.run(["make", "-j"], cwd=directory, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(["./euler"], cwd=directory, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(
        [
            sys.executable,
            os.path.join(PeanoRoot, "src", "tarch", "plotter", "api", "probe.py"),
            "--point",
            "0.0",
            "0.0",
            "-o",
            "probe.csv",
            "-j",
            str(os.cpu_count()),
            PatchFile,
        ],
        cwd=directory,
        check=True,
        stdout=subprocess.DEVNULL,
        env=dict(os.environ, PYTHONPATH=os.path.join(PeanoRoot, "src")),
    )


def read_probe(directory):
    """! Return arrays time, density from the probe's csv file"""
    with open(os.path.join(directory, "probe.csv")) as csv_file:
        rows = list(csv.DictReader(csv_file))
    return (
        np.array([float(row["time"]) for row in rows]),
        np.array([float(row["EulerFVQ_0"]) for row in rows]),
    )


def density_along_x_axis(directory, snapshot):
    """! Return time stamp plus volume centres and densities along y=0"""
    time_stamp, patch_files = tarch.plotter.api.notebook.read_snapshots(
        os.path.join(directory, PatchFile)
    )[snapshot]
    x, rho = [], []
    for patch_file in patch_files:
        for offset, size, values in tarch.plotter.api.notebook.read_patches(patch_file):
            if offset[1] <= 0.0 < offset[1] + size[1]:
                dof = values.shape[0]
                h = size[0] / dof
                row = int((0.0 - offset[1]) / (size[1] / dof))
                for i in range(dof):
                    x.append(offset[0] + (i + 0.5) * h)
                    rho.append(values[row, i, 0])
    order = np.argsort(x)
    return time_stamp, np.array(x)[order], np.array(rho)[order]


def plot_mesh(directory):
    """! Initial density plus patch boundaries and the volumes of one patch"""
    file_name = os.path.join(directory, PatchFile)
    ax = tarch.plotter.api.notebook.plot_snapshot(file_name, 0, 0, label="density")
    _, patch_files = tarch.plotter.api.notebook.read_snapshots(file_name)[0]
    patches = [
        p for f in patch_files for p in tarch.plotter.api.notebook.read_patches(f)
    ]
    for offset, size, values in patches:
        ax.add_patch(
            matplotlib.patches.Rectangle(
                offset, size[0], size[1], fill=False, edgecolor="white", linewidth=1.2
            )
        )
    # highlight the volumes of one patch
    offset, size, values = min(patches, key=lambda p: (p[0][0] ** 2 + p[0][1] ** 2))
    dof = values.shape[0]
    for i in range(1, dof):
        ax.plot(
            [offset[0] + i * size[0] / dof] * 2,
            [offset[1], offset[1] + size[1]],
            color="red",
            linewidth=0.5,
        )
        ax.plot(
            [offset[0], offset[0] + size[0]],
            [offset[1] + i * size[1] / dof] * 2,
            color="red",
            linewidth=0.5,
        )
    ax.add_patch(
        matplotlib.patches.Rectangle(
            offset, size[0], size[1], fill=False, edgecolor="red", linewidth=1.5
        )
    )
    ax.set_title(
        "Initial density\n{0}x{0} patches, {1}x{1} volumes per patch".format(
            round(0.2 / size[0]), dof
        )
    )
    ax.figure.savefig(
        os.path.join(Here, "10_euler-mesh.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(ax.figure)


def plot_advection(directory):
    """! Density along y=0 at a few time stamps against the exact solution"""
    fig, ax = plt.subplots(figsize=(7, 4))
    x_exact = np.linspace(-0.1, 0.1, 400)
    snapshots = len(
        tarch.plotter.api.notebook.read_snapshots(os.path.join(directory, PatchFile))
    )
    for snapshot, colour in zip(
        [0, (snapshots - 1) // 4, (snapshots - 1) // 2], ["C0", "C1", "C2"]
    ):
        t, x, rho = density_along_x_axis(directory, snapshot)
        ax.plot(x_exact, exact_density(x_exact, t), color=colour, linewidth=1)
        ax.plot(x, rho, "o", color=colour, markersize=2.5, label="t={:.3f}".format(t))
    ax.set_xlabel("x")
    ax.set_ylabel("density")
    ax.set_title("Density along y=0: ExaHyPE (dots) vs exact solution (lines)")
    ax.legend()
    fig.savefig(
        os.path.join(Here, "10_euler-advection.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)


def plot_probe(directory):
    """! Probe at the origin over time against the exact sine"""
    t, rho = read_probe(directory)
    t_exact = np.linspace(0, EndTime, 400)
    fig, ax = plt.subplots(figsize=(7, 4))
    ax.plot(
        t_exact,
        exact_density(0.0, t_exact),
        "k-",
        linewidth=1,
        label="exact: 1+0.2 sin(10 pi t)",
    )
    ax.plot(t, rho, "o", markersize=3, label="probe.py at (0,0)")
    ax.set_xlabel("t")
    ax.set_ylabel("density")
    ax.set_title("Density at the origin over time")
    ax.legend()
    fig.savefig(os.path.join(Here, "10_euler-probe.png"), dpi=100, bbox_inches="tight")
    plt.close(fig)


def plot_convergence(directories):
    """! Probe error over time for several volume sizes"""
    fig, ax = plt.subplots(figsize=(7, 4))
    for volume_size, directory in directories:
        t, rho = read_probe(directory)
        error = np.abs(rho - exact_density(0.0, t))
        ax.plot(
            t,
            error,
            "o-",
            markersize=3,
            label="volume_size={}: max error {:.4f}".format(volume_size, error.max()),
        )
    ax.set_xlabel("t")
    ax.set_ylabel("|density - exact density| at (0,0)")
    ax.set_title("Error at the probe point for two mesh resolutions")
    ax.legend()
    fig.savefig(
        os.path.join(Here, "10_euler-convergence.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 10_euler.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(prefix="10_euler-")
    runs = []
    for volume_size in [0.005, 0.001]:
        directory = os.path.join(work_directory, "volume-size-{}".format(volume_size))
        os.makedirs(directory, exist_ok=True)
        print("build and run volume size {} in {}".format(volume_size, directory))
        build_and_run(volume_size, directory)
        runs.append((volume_size, directory))

    plot_mesh(runs[0][1])
    plot_advection(runs[0][1])
    plot_probe(runs[0][1])
    plot_convergence(runs)
    print("wrote PNGs into {}".format(Here))
