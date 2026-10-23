# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 522_single-black-hole.ipynb

The script builds the single black hole setup of the tutorial, runs it,
extracts the lapse and the conformal factor along the x-axis with Peano's
PointProbe, and writes all pictures into the directory of this script. Call
it from anywhere:

     python3 tutorials/exahype2/exagrype/illustrations/522_single-black-hole.py

Peano has to be configured with --with-gsl, as the initial data stems from
TwoPunctures. All builds and outputs go into a temporary directory (or
--work-directory). If you pass a work directory, a run which has completed
before is reused, i.e. you can rerun the script to redraw the pictures
without recomputing.

"""
import argparse
import contextlib
import csv
import io
import os
import subprocess
import sys
import tempfile
import time

Here = os.path.dirname(os.path.abspath(__file__))
PeanoRoot = os.path.abspath(os.path.join(Here, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(PeanoRoot, "src"))

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

import peano4.api
import exahype2.api
import exagrype.api
import tarch.plotter.api.output


PatchFile = "solution-CCZ4.peano-patch-file"
Mass = 1.0
DomainOffset = -9.0
DomainSize = 18.0
CellsPerAxis = 9
PatchSize = 6
VolumesPerAxis = CellsPerAxis * PatchSize
TimeInBetweenPlots = 0.5

## y and z coordinate of the line along which we probe: the centre of the
## first volume next to the origin
LineOffset = DomainSize / VolumesPerAxis / 2.0


def psi(r):
    """! Conformal factor of the Schwarzschild black hole in isotropic coordinates"""
    return 1.0 + Mass / 2.0 / r


def build_and_run(directory, peano_installation, end_time):
    """!

    Generate, compile and run the tutorial setup within directory

    Returns the runtime of the executable in seconds. If the directory holds
    the result of a previous run, we do not rerun but return the stored
    runtime.

    """
    runtime_file = os.path.join(directory, "runtime.txt")
    if os.path.exists(runtime_file):
        with open(runtime_file) as f:
            return float(f.read())

    os.makedirs(directory, exist_ok=True)
    print(
        "build and run single black hole until t={} in {}".format(end_time, directory)
    )
    project = exahype2.api.Project(
        namespace=["tutorials", "exagrype", "singleblackhole"],
        project_name="SingleBlackHole",
        directory=directory,
        executable="single-black-hole",
    )
    solver = exagrype.api.CCZ4Solver(
        "CCZ4",
        parameters={"CCZ4bs": 1.0, "CCZ4sk": 1.0},
        initial_condition=exagrype.api.initialvalues.single_black_hole(mass=Mass),
    )
    solver.add_pde_term(
        exahype2.api.DirichletBoundaryConditions(
            implementation="::exagrype::initialvalues::singleBlackHole(Q, x, t, {});".format(
                Mass
            )
        )
    )
    solver.numerical_scheme = exahype2.api.fv.RusanovWithOptimisticExplicitEuler(
        solver, PatchSize
    )
    solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        solver, volume_size=1.01 * DomainSize / VolumesPerAxis
    )
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver,
        first_plot_time_stamp=0.0,
        time_in_between_plots=TimeInBetweenPlots,
        plotter_precision=10,
    )
    solver.plotter.add_plot_filter(
        offset=[DomainOffset, 0.1, 0.1], size=[DomainSize, 0.1, 0.1]
    )
    project.add_solver(solver)
    project.set_global_simulation_parameters(
        dimensions=3,
        offset=[DomainOffset] * 3,
        size=[DomainSize] * 3,
        min_end_time=end_time,
        max_end_time=end_time,
        periodic_BC=[False, False, False],
    )
    project.set_Peano4_installation(
        peano_installation, peano4.api.output.CompileMode.RELEASE
    )
    with contextlib.redirect_stdout(io.StringIO()):
        peano4_project = project.generate_Peano4_project(
            verbosity=peano4.api.LoggerVerbosity.TEXT
        )
        peano4_project.generate()

    subprocess.run(
        ["make", "-j"],
        cwd=directory,
        check=True,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    start = time.time()
    with open(os.path.join(directory, "run.log"), "w") as log:
        subprocess.run(["./single-black-hole"], cwd=directory, check=True, stdout=log)
    runtime = time.time() - start
    with open(runtime_file, "w") as f:
        f.write(str(runtime))
    return runtime


def probe(directory, point, output_file):
    """!

    Probe all snapshots at one point

    Returns the time stamps, the lapse and the conformal factor phi.

    """
    point_probe = tarch.plotter.api.output.PointProbe(
        file_name=os.path.join(directory, PatchFile),
        point=point,
        output_file=os.path.join(directory, output_file),
        start=0,
        end=sys.maxsize,
    )
    with contextlib.redirect_stdout(io.StringIO()):
        point_probe.display()
    with open(os.path.join(directory, output_file)) as csv_file:
        rows = list(csv.DictReader(csv_file))
    return (
        np.array([float(row["time"]) for row in rows]),
        np.array([float(row["lapse"]) for row in rows]),
        np.array([float(row["phi"]) for row in rows]),
    )


def profile(directory):
    """!

    Probe lapse and phi in the centre of each volume along the line y=z=LineOffset

    Returns the x coordinates, the time stamps and two 2d arrays
    lapse[snapshot,x] and phi[snapshot,x].

    """
    h = DomainSize / VolumesPerAxis
    x = DomainOffset + (np.arange(VolumesPerAxis) + 0.5) * h
    lapse, phi = [], []
    for i, x_i in enumerate(x):
        t, lapse_over_time, phi_over_time = probe(
            directory, [x_i, LineOffset, LineOffset], "profile-{}.csv".format(i)
        )
        lapse.append(lapse_over_time)
        phi.append(phi_over_time)
    return x, t, np.array(lapse).T, np.array(phi).T


def radius(x):
    return np.sqrt(np.asarray(x) ** 2 + 2.0 * LineOffset**2)


def plot_initial_data(x, lapse, phi):
    """! Initial lapse and conformal factor along the x-axis against the closed form"""
    h = DomainSize / VolumesPerAxis
    x_fine = np.linspace(DomainOffset, DomainOffset + DomainSize, 2000)

    fig, ax = plt.subplots(figsize=(8, 4))
    ax.plot(
        x_fine,
        1.0 / psi(radius(x_fine)),
        "-",
        color="tab:blue",
        linewidth=1,
        label=r"$1/\psi$",
    )
    ax.plot(
        x_fine,
        psi(radius(x_fine)) ** (-2),
        "-",
        color="tab:orange",
        linewidth=1,
        label=r"$\psi^{-2}$",
    )
    ax.plot(x, lapse[0], "o", color="tab:blue", markersize=3, label="lapse (ExaGRyPE)")
    ax.plot(x, phi[0], "s", color="tab:orange", markersize=3, label="phi (ExaGRyPE)")
    for cell in range(CellsPerAxis + 1):
        ax.axvline(
            DomainOffset + cell * PatchSize * h,
            color="tab:red",
            linewidth=0.5,
            linestyle="--",
        )
    ax.plot(
        [], [], color="tab:red", linewidth=0.5, linestyle="--", label="patch boundaries"
    )
    ax.set_xlabel("x (along y=z={:.3f})".format(LineOffset))
    ax.set_ylabel("initial value")
    ax.set_title("Initial data of a single black hole with M={}".format(Mass))
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0))
    fig.savefig(
        os.path.join(Here, "522_single-black-hole-initial-data.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_lapse_profile(x, t, lapse):
    """! Lapse along the x-axis for several snapshots"""
    fig, ax = plt.subplots(figsize=(8, 4))
    snapshots = sorted(set([0, len(t) // 4, len(t) // 2, 3 * len(t) // 4, len(t) - 1]))
    colours = plt.cm.viridis(np.linspace(0.0, 0.9, len(snapshots)))
    for snapshot, colour in zip(snapshots, colours):
        ax.plot(
            x,
            lapse[snapshot],
            "o-",
            color=colour,
            markersize=2,
            linewidth=1,
            label="t={:.2f}".format(t[snapshot]),
        )
    ax.set_xlabel("x (along y=z={:.3f})".format(LineOffset))
    ax.set_ylabel("lapse")
    ax.set_title("Lapse along the x-axis over time")
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0))
    fig.savefig(
        os.path.join(Here, "522_single-black-hole-lapse-profile.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_lapse_over_time(x, t, lapse):
    """! Lapse over time at a few distances from the puncture"""
    fig, ax = plt.subplots(figsize=(8, 4))
    for x_probe in [LineOffset, 1.0 + LineOffset, 3.0 + LineOffset, 8.0 + LineOffset]:
        i = np.argmin(np.abs(x - x_probe))
        ax.plot(
            t,
            lapse[:, i],
            "o-",
            markersize=2,
            linewidth=1,
            label="r={:.2f}".format(radius(x[i])),
        )
    ax.set_xlabel("t")
    ax.set_ylabel("lapse")
    ax.set_title("Lapse over time at several distances from the puncture")
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0))
    fig.savefig(
        os.path.join(Here, "522_single-black-hole-lapse-over-time.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 522_single-black-hole.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--peano-root",
        default=PeanoRoot,
        help="Configured (--with-gsl) and built Peano installation to link against (default: the repository holding this script)",
    )
    parser.add_argument(
        "--end-time", type=float, default=10.0, help="Simulated time span (default: 10)"
    )
    args = parser.parse_args()

    with contextlib.ExitStack() as stack:
        work_directory = args.work_directory or stack.enter_context(
            tempfile.TemporaryDirectory()
        )
        directory = os.path.join(os.path.abspath(work_directory), "run")
        runtime = build_and_run(
            directory, os.path.abspath(args.peano_root), args.end_time
        )
        print("runtime: {:.1f}s".format(runtime))

        x, t, lapse, phi = profile(directory)
        print("snapshots at t =", np.round(t, 3))
        for i in [
            VolumesPerAxis // 2,
            VolumesPerAxis // 2 + 3,
            VolumesPerAxis // 2 + 9,
            VolumesPerAxis - 1,
        ]:
            print(
                "r={:.3f}: lapse(t=0)={:.4f} (1/psi={:.4f}), lapse(t={:.2f})={:.4f}, phi(t=0)={:.4f} (psi^-2={:.4f})".format(
                    radius(x[i]),
                    lapse[0, i],
                    1.0 / psi(radius(x[i])),
                    t[-1],
                    lapse[-1, i],
                    phi[0, i],
                    psi(radius(x[i])) ** (-2),
                )
            )
        print("NaN in lapse:", np.isnan(lapse).any())

        plot_initial_data(x, lapse, phi)
        plot_lapse_profile(x, t, lapse)
        plot_lapse_over_time(x, t, lapse)
