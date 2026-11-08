# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 44_localisation.ipynb

The script builds the tutorial's setup with the two Euler solvers EulerFV
and CoarseEuler, runs it, and writes two pictures into the directory of
this script: the density of both solvers over time at the centre of the
domain, and the density of both solvers side by side at t=0.1. The
implementation of EulerFV is taken from the tutorial's %%writefile cell.
Call the script from anywhere:

     python3 tutorials/exahype2/illustrations/44_localisation.py

All builds and outputs go into a temporary directory (or --work-directory).
The diagram 44_localisation.png is not created by this script.

"""
import argparse
import csv
import json
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

import peano4.api
import exahype2
import tarch.plotter.api.notebook

DomainSize = [0.2, 0.2]
PatchSize = 9
Solvers = ["EulerFV", "CoarseEuler"]


def euler_implementation():
    """! Body of the tutorial's cell which writes EulerFV.cpp"""
    with open(os.path.join(Here, "..", "44_localisation.ipynb")) as notebook_file:
        cells = json.load(notebook_file)["cells"]
    for cell in cells:
        source = "".join(cell["source"])
        if source.startswith("%%writefile 44_localisation/EulerFV.cpp"):
            return source.split("\n", 1)[1]
    raise Exception("tutorial has no cell writing EulerFV.cpp")


def add_plotter(solver):
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver, first_plot_time_stamp=0.0, time_in_between_plots=0.005
    )


def build_and_run(directory):
    """! Generate, compile and run the tutorial setup within directory"""
    project = exahype2.api.Project(
        namespace=["tutorials", "exahype2", "localisation"],
        project_name="Euler",
        directory=directory,
        executable="euler",
    )

    fine_solver = exahype2.api.Solver("EulerFV", 4)
    fine_solver.add_pde_term(exahype2.api.Flux(stateless=False))
    fine_solver.add_pde_term(exahype2.api.MaxEigenvalue(stateless=False))
    fine_solver.add_pde_term(exahype2.api.InitPointWisely(stateless=False))
    fine_solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(
        fine_solver, PatchSize
    )
    fine_solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        fine_solver, volume_size=0.001
    )
    add_plotter(fine_solver)

    coarse_solver = exahype2.api.Solver("CoarseEuler", 4)
    coarse_solver.add_user_solver_includes(
        '#include "repositories/SolverRepository.h"\n'
    )
    coarse_solver.add_pde_term(
        exahype2.api.Flux(
            implementation="repositories::instanceOfEulerFV.flux(Q, x, normal, F);",
            stateless=False,
        )
    )
    coarse_solver.add_pde_term(
        exahype2.api.MaxEigenvalue(
            implementation="repositories::instanceOfEulerFV.maxEigenvalue(Q, x, normal, lambdaMax);",
            stateless=False,
        )
    )
    coarse_solver.add_pde_term(
        exahype2.api.InitPointWisely(
            implementation="repositories::instanceOfEulerFV.initPointWisely(x, Q);",
            stateless=False,
        )
    )
    coarse_solver.grid_selector = peano4.api.toolbox.FixedResolutionGridSelector(
        2, DomainSize
    )
    coarse_solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(
        coarse_solver, PatchSize
    )
    coarse_solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        coarse_solver, volume_size=0.005
    )
    add_plotter(coarse_solver)

    project.add_solver(fine_solver)
    project.add_solver(coarse_solver)
    project.set_global_simulation_parameters(
        dimensions=2,
        offset=[-0.1, -0.1],
        size=DomainSize,
        min_end_time=0.2,
        max_end_time=0.2,
        periodic_BC=[True, True, True],
    )
    project.set_Peano4_installation(PeanoRoot, peano4.api.output.CompileMode.RELEASE)
    project.generate_Peano4_project().generate()
    with open(os.path.join(directory, "EulerFV.cpp"), "w") as implementation_file:
        implementation_file.write(euler_implementation())
    subprocess.run(["make", "-j"], cwd=directory, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(["./euler"], cwd=directory, check=True, stdout=subprocess.DEVNULL)


def probe(directory, name, point):
    """! Time stamps and density of solver name at point"""
    subprocess.run(
        [
            sys.executable,
            "-m",
            "tarch.plotter.api.probe",
            "--point",
            *[str(c) for c in point],
            "-o",
            f"probe-{name}.csv",
            f"solution-{name}.peano-patch-file",
        ],
        cwd=directory,
        env=dict(os.environ, PYTHONPATH=os.path.join(PeanoRoot, "src")),
        check=True,
        stdout=subprocess.DEVNULL,
    )
    with open(os.path.join(directory, f"probe-{name}.csv")) as csv_file:
        rows = list(csv.reader(csv_file))
    return [float(row[0]) for row in rows[1:]], [float(row[1]) for row in rows[1:]]


def closest_snapshot(file_name, time_stamp):
    snapshots = tarch.plotter.api.notebook.read_snapshots(file_name)
    return min(range(len(snapshots)), key=lambda i: abs(snapshots[i][0] - time_stamp))


def plot_probe(directory):
    fig, ax = plt.subplots(figsize=(7, 4))
    for name in Solvers:
        t, rho = probe(directory, name, (0.0, 0.0))
        ax.plot(t, rho, label=name)
    ax.set_xlabel("t")
    ax.set_ylabel("density at x=(0,0)")
    ax.legend()
    fig.savefig(
        os.path.join(Here, "44_localisation-probe.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)


def plot_comparison(directory):
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    for ax, name in zip(axes, Solvers):
        file_name = os.path.join(directory, f"solution-{name}.peano-patch-file")
        tarch.plotter.api.notebook.plot_snapshot(
            file_name,
            closest_snapshot(file_name, 0.1),
            0,
            clim=(0.8, 1.2),
            ax=ax,
            label=f"{name}: density",
        )
    fig.savefig(
        os.path.join(Here, "44_localisation-comparison.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 44_localisation.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(prefix="44_localisation-")
    print("build and run in {}".format(work_directory))
    build_and_run(work_directory)
    plot_probe(work_directory)
    plot_comparison(work_directory)
    print("wrote PNGs into {}".format(Here))
