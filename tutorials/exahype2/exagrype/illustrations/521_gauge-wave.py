# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 521_gauge-wave.ipynb

The script builds the gauge wave setup of the tutorial for several volume
and patch sizes, runs each configuration over one period of the wave,
extracts the lapse along the x-axis with Peano's PointProbe, and writes all
pictures into the directory of this script. It also prints the convergence
table which the tutorial discusses. Call it from anywhere:

     python3 tutorials/exahype2/exagrype/illustrations/521_gauge-wave.py

All builds and outputs go into a temporary directory (or --work-directory).
If you pass a work directory, runs which have completed before are reused,
i.e. you can rerun the script to redraw the pictures without recomputing.
The whole study takes around two hours on an eight-core workstation, most of
it spent on the three finest runs.

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


EndTime = 1.0
PatchFile = "solution-CCZ4.peano-patch-file"
Amplitude = 0.1

## Maximum volume sizes handed to the grid construction, indexed by the
## number of volumes per axis that they yield. Peano refines every cell whose
## edge length is greater than or equal to the maximum volume size times the
## patch size, i.e. the resulting volume size h is strictly smaller than the
## maximum volume size. We therefore pass in values slightly bigger than
## the volume sizes 1/9, 1/27 and 1/81 that we want to obtain.
MaxVolumeSizes = {9: 0.12, 27: 0.04, 81: 0.013}

## (volumes per axis, patch size) pairs of the convergence study. Peano
## always refines the root cell at least once, i.e. we need at least three
## patches per axis.
Configurations = [
    (9, 3),
    (27, 3),
    (81, 3),
    (27, 9),
    (81, 9),
    (81, 27),
]

## The configuration of the tutorial itself
Reference = (27, 9)


def exact_lapse(x, t):
    """! Lapse of the gauge wave, i.e. sqrt(H) with H = 1 - A sin(2 pi (x-t))"""
    return np.sqrt(1.0 - Amplitude * np.sin(2.0 * np.pi * (np.asarray(x) - t)))


def build_and_run(volumes_per_axis, patch_size, directory, peano_installation):
    """!

    Generate, compile and run the tutorial setup within directory

    Returns the runtime of the executable in seconds. If the directory holds
    the result of a previous run, we do not rerun but return the stored
    runtime. peano_installation is the configured and built Peano root
    directory which the generated Makefile links against.

    """
    runtime_file = os.path.join(directory, "runtime.txt")
    if os.path.exists(runtime_file):
        with open(runtime_file) as f:
            return float(f.read())

    os.makedirs(directory, exist_ok=True)
    print(
        "build and run {} volumes per axis with patch size {} in {}".format(
            volumes_per_axis, patch_size, directory
        )
    )
    project = exahype2.api.Project(
        namespace=["tutorials", "exagrype", "gaugewave"],
        project_name="GaugeWave",
        directory=directory,
        executable="gauge-wave",
    )
    solver = exagrype.api.CCZ4Solver(
        "CCZ4", initial_condition=exagrype.api.initialvalues.gauge_wave()
    )
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(
        solver, patch_size
    )
    solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        solver, volume_size=MaxVolumeSizes[volumes_per_axis]
    )
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver,
        first_plot_time_stamp=0.0,
        time_in_between_plots=0.05,
        plotter_precision=10,
    )
    solver.plotter.add_plot_filter([0.0, 0.49, 0.49], [1.0, 0.02, 0.02])
    project.add_solver(solver)
    project.set_global_simulation_parameters(
        dimensions=3,
        offset=[0.0, 0.0, 0.0],
        size=[1.0, 1.0, 1.0],
        min_end_time=EndTime,
        max_end_time=EndTime,
        periodic_BC=[True, True, True],
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
        subprocess.run(["./gauge-wave"], cwd=directory, check=True, stdout=log)
    runtime = time.time() - start
    with open(runtime_file, "w") as f:
        f.write(str(runtime))
    return runtime


def probe(directory, point, output_file):
    """!

    Probe all snapshots at one point and return (time stamps, lapse)

    This is the Python counterpart of src/tarch/plotter/api/probe.py.

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
    )


def wave_profile(directory, volumes_per_axis):
    """!

    Probe the lapse in the centre of each volume along the line y=z=0.5

    Returns the x coordinates, the time stamps of all snapshots and a 2d
    array lapse[snapshot,x].

    """
    x = (np.arange(volumes_per_axis) + 0.5) / volumes_per_axis
    lapse = []
    for i, x_i in enumerate(x):
        t, lapse_over_time = probe(
            directory, [x_i, 0.5, 0.5], "profile-{}.csv".format(i)
        )
        lapse.append(lapse_over_time)
    return x, t, np.array(lapse).T


def plot_mesh(directory, volumes_per_axis, patch_size):
    """! Initial lapse along the x-axis with volumes and patches"""
    x, t, lapse = wave_profile(directory, volumes_per_axis)
    x_fine = np.linspace(0.0, 1.0, 400)
    h = 1.0 / volumes_per_axis

    fig, ax = plt.subplots(figsize=(8, 4))
    ax.plot(x_fine, exact_lapse(x_fine, 0.0), "k-", linewidth=1, label="exact lapse")
    ax.step(
        np.append(x - 0.5 * h, 1.0),
        np.append(lapse[0], lapse[0][-1]),
        where="post",
        color="tab:blue",
        label="volume averages",
    )
    for patch in range(volumes_per_axis // patch_size + 1):
        ax.axvline(patch * patch_size * h, color="tab:red", linewidth=1, linestyle="--")
    ax.plot(
        [], [], color="tab:red", linewidth=1, linestyle="--", label="patch boundaries"
    )
    ax.set_xlabel("x (along y=z=0.5)")
    ax.set_ylabel("lapse")
    ax.set_title(
        "Initial lapse: {} volumes per axis, patch size {}".format(
            volumes_per_axis, patch_size
        )
    )
    ax.legend(loc="lower right")
    fig.savefig(
        os.path.join(Here, "521_gauge-wave-mesh.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)


def plot_profile(directory, volumes_per_axis):
    """! Lapse along the x-axis for three snapshots"""
    x, t, lapse = wave_profile(directory, volumes_per_axis)
    x_fine = np.linspace(0.0, 1.0, 400)

    fig, ax = plt.subplots(figsize=(8, 4))
    for snapshot, colour in zip(
        [0, len(t) // 2, len(t) - 1], ["tab:blue", "tab:orange", "tab:green"]
    ):
        ax.plot(
            x_fine, exact_lapse(x_fine, t[snapshot]), "-", color=colour, linewidth=1
        )
        ax.plot(
            x,
            lapse[snapshot],
            "o",
            color=colour,
            markersize=3,
            label="t={:.2f}".format(t[snapshot]),
        )
    ax.set_xlabel("x (along y=z=0.5)")
    ax.set_ylabel("lapse")
    ax.set_title("Lapse profile (lines: exact, dots: ExaGRyPE)")
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0))
    fig.savefig(
        os.path.join(Here, "521_gauge-wave-profile.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)


def plot_probe(directory):
    """! Lapse at the centre of the domain over time"""
    t, lapse = probe(directory, [0.5, 0.5, 0.5], "probe.csv")
    t_fine = np.linspace(0.0, EndTime, 400)

    fig, ax = plt.subplots(figsize=(8, 4))
    ax.plot(t_fine, exact_lapse(0.5, t_fine), "k-", linewidth=1, label="exact")
    ax.plot(t, lapse, "o", markersize=3, label="PointProbe at (0.5,0.5,0.5)")
    ax.set_xlabel("t")
    ax.set_ylabel("lapse")
    ax.set_title("Lapse at the centre of the domain over time")
    ax.legend(loc="lower right")
    fig.savefig(
        os.path.join(Here, "521_gauge-wave-probe.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(fig)


def errors_over_time(directory, volumes_per_axis):
    """!

    Maximum and L2 error of the lapse along the x-axis for all snapshots

    Returns the time stamps plus the two errors per snapshot.

    """
    x, t, lapse = wave_profile(directory, volumes_per_axis)
    error = lapse - exact_lapse(x[np.newaxis, :], t[:, np.newaxis])
    return t, np.max(np.abs(error), axis=1), np.sqrt(np.mean(error**2, axis=1))


def plot_convergence(results):
    """!

    Maximum error over volume size for several points in time

    The lines show patch size 3 at several time stamps. The markers for patch
    sizes 9 and 27 are drawn for the final time stamp only.

    """
    fig, ax = plt.subplots(figsize=(7, 5))
    times = [0.05, 0.2, 0.5, 1.0]
    colours = ["tab:blue", "tab:orange", "tab:green", "tab:red"]
    for time_stamp, colour in zip(times, colours):
        data = sorted(
            (volumes, values) for (volumes, p), values in results.items() if p == 3
        )
        h = np.array([1.0 / volumes for volumes, _ in data])
        error = np.array(
            [
                values["max errors"][np.argmin(np.abs(values["t"] - time_stamp))]
                for _, values in data
            ]
        )
        ax.loglog(h, error, "o-", color=colour, label="t={}".format(time_stamp))
    for patch_size, marker in [(9, "s"), (27, "^")]:
        data = [
            (1.0 / volumes, values["max errors"][-1])
            for (volumes, p), values in results.items()
            if p == patch_size
        ]
        ax.loglog(
            [h for h, _ in data],
            [e for _, e in data],
            marker,
            color="tab:red",
            markerfacecolor="none",
            markersize=10,
            label="t=1.0, patch size {}".format(patch_size),
        )
    h = np.array([1.0 / 81, 1.0 / 9])
    reference = results[(81, 3)]["max errors"][1]
    ax.loglog(h, reference * h / h[0], "k--", linewidth=1, label="first order")
    ax.set_xlabel("volume size h = 1 / (volumes per axis)")
    ax.set_ylabel("max error of the lapse")
    ax.set_title("Convergence of the Finite Volume scheme (lines: patch size 3)")
    ax.legend(loc="upper left", bbox_to_anchor=(1.01, 1.0))
    fig.savefig(
        os.path.join(Here, "521_gauge-wave-convergence.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def order(coarse_error, fine_error):
    """! Estimated convergence order from two meshes whose volume sizes differ by a factor of three"""
    return np.log(coarse_error / fine_error) / np.log(3.0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 521_gauge-wave.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--peano-root",
        default=PeanoRoot,
        help="Configured and built Peano installation to link against (default: the repository holding this script)",
    )
    args = parser.parse_args()

    with contextlib.ExitStack() as stack:
        work_directory = args.work_directory or stack.enter_context(
            tempfile.TemporaryDirectory()
        )
        work_directory = os.path.abspath(work_directory)

        def directory_of(volumes_per_axis, patch_size):
            return os.path.join(
                work_directory, "h{}-p{}".format(volumes_per_axis, patch_size)
            )

        results = {}
        for volumes_per_axis, patch_size in Configurations:
            directory = directory_of(volumes_per_axis, patch_size)
            runtime = build_and_run(
                volumes_per_axis,
                patch_size,
                directory,
                os.path.abspath(args.peano_root),
            )
            t, max_errors, l2_errors = errors_over_time(directory, volumes_per_axis)
            results[(volumes_per_axis, patch_size)] = {
                "runtime": runtime,
                "t": t,
                "max errors": max_errors,
                "L2 errors": l2_errors,
            }

        reference_directory = directory_of(*Reference)
        plot_mesh(reference_directory, *Reference)
        plot_profile(reference_directory, Reference[0])
        plot_probe(reference_directory)
        plot_convergence(results)

        print("Errors after the last time step:")
        print(
            "{:>8} {:>6} {:>8} {:>12} {:>8} {:>12} {:>8} {:>10}".format(
                "volumes",
                "patch",
                "t",
                "max error",
                "order",
                "L2 error",
                "order",
                "runtime",
            )
        )
        for (volumes_per_axis, patch_size), values in sorted(
            results.items(), key=lambda item: (item[0][1], item[0][0])
        ):
            coarser = results.get((volumes_per_axis // 3, patch_size))
            order_max = order_l2 = float("nan")
            if coarser is not None:
                order_max = order(coarser["max errors"][-1], values["max errors"][-1])
                order_l2 = order(coarser["L2 errors"][-1], values["L2 errors"][-1])
            print(
                "{:>8} {:>6} {:>8.4f} {:>12.4e} {:>8.2f} {:>12.4e} {:>8.2f} {:>9.1f}s".format(
                    volumes_per_axis,
                    patch_size,
                    values["t"][-1],
                    values["max errors"][-1],
                    order_max,
                    values["L2 errors"][-1],
                    order_l2,
                    values["runtime"],
                )
            )

        print("Max errors of patch size 3 over time:")
        print(
            "{:>8} {:>12} {:>12} {:>12} {:>8} {:>8}".format(
                "t", "h=1/9", "h=1/27", "h=1/81", "order", "order"
            )
        )
        for snapshot in [1, 2, 4, 10, 20]:
            errors = [
                results[(volumes, 3)]["max errors"][snapshot] for volumes in [9, 27, 81]
            ]
            print(
                "{:>8.3f} {:>12.4e} {:>12.4e} {:>12.4e} {:>8.2f} {:>8.2f}".format(
                    results[(81, 3)]["t"][snapshot],
                    *errors,
                    order(errors[0], errors[1]),
                    order(errors[1], errors[2]),
                )
            )
