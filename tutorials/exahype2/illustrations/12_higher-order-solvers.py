# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 12_higher-order-solvers.ipynb

The script builds the Euler setup of the tutorials 10_euler and
12_higher-order-solvers with the first-order Finite Volumes and with the
fourth-order Finite Differences plus Runge-Kutta schemes of order one to
four, runs them on three meshes, and writes the following pictures into
the directory of this script:

- 12_higher-order-solvers-convergence.png: error over mesh width;
- 12_higher-order-solvers-profile.png: density along y=0 after one period;
- 12_higher-order-solvers-instability.png: the error over time of the
  explicit Euler time stepping (Runge-Kutta of order one).

The PDE terms are injected from Python, i.e. we do not need any hand-written
C++ file. Call it from anywhere:

     python3 tutorials/exahype2/illustrations/12_higher-order-solvers.py

All builds and outputs go into a temporary directory (or --work-directory).
A run whose directory already holds the output is not repeated, so you can
rerun the script with the same --work-directory after you have changed a
plot. The script builds 20 executables, i.e. it runs for roughly 40 minutes.

"""
import argparse
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

DomainOffset = -0.1
DomainWidth = 0.2

# Volume sizes and the resulting number of grid cells per axis, as each mesh
# cell hosts 9x9 volumes or grid cells
VolumeSizes = [(0.0075, 27), (0.0025, 81), (0.00083, 243)]

ConvergenceEndTime = 0.05
PeriodEndTime = 0.2

# Runge-Kutta orders and the colours of the fourth-order Finite Differences
RungeKuttaColours = {1: "C3", 2: "C1", 3: "C2", 4: "C0"}


def exact_density(x, t):
    """! Density of the entropy wave advected with u=1 over [-0.1,0.1]"""
    return 1.0 + 0.2 * np.sin(
        2.0 * np.pi * (np.asarray(x) - t - DomainOffset) / DomainWidth
    )


def solver_name(rk_order):
    """! Name of the solver: EulerFV for rk_order None, EulerFD4 otherwise"""
    return "EulerFV" if rk_order is None else "EulerFD4"


def patch_file(directory, rk_order):
    return os.path.join(
        directory, "solution-{}.peano-patch-file".format(solver_name(rk_order))
    )


def build_and_run(
    rk_order, volume_size, end_time, time_in_between_plots, directory, peano_root
):
    """!

    Generate, compile and run one setup within directory

    @param rk_order: int or None
      None selects the Finite Volumes of the tutorial 10_euler, an integer
      the fourth-order Finite Differences with the Runge-Kutta scheme of this
      order.

    @param peano_root: str
      Configured and built Peano directory the executable links against.

    """
    if os.path.exists(patch_file(directory, rk_order)):
        print("reuse {}".format(directory))
        return
    print("build and run {}".format(directory))
    os.makedirs(directory, exist_ok=True)
    project = exahype2.api.Project(
        namespace=["tutorials", "exahype2", "higherorder"],
        project_name="HigherOrder",
        directory=directory,
        executable="higher-order",
    )
    solver = exahype2.api.Solver(solver_name(rk_order), 4, 0)
    solver.add_user_solver_includes("#include <cmath>\n#include <algorithm>\n")
    solver.add_pde_term(exahype2.api.Flux(implementation=Flux, stateless=False))
    solver.add_pde_term(
        exahype2.api.MaxEigenvalue(implementation=MaxEigenvalue, stateless=False)
    )
    solver.add_pde_term(
        exahype2.api.InitPointWisely(implementation=InitPointWisely, stateless=False)
    )
    if rk_order is None:
        solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
        solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
            solver, volume_size=volume_size
        )
        solver.plotter = exahype2.api.fv.PlotAsVTKFile(
            solver,
            first_plot_time_stamp=0.0,
            time_in_between_plots=time_in_between_plots,
        )
    else:
        solver.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
            solver, patch_size=9, rk_order=rk_order
        )
        solver.grid_construction = exahype2.api.rkfd4.SetupSimulationOverRegularGrid(
            solver, volume_size=volume_size
        )
        # The errors of the finer meshes are far below the default precision
        solver.plotter = exahype2.api.rkfd4.PlotAsVTKFile(
            solver,
            first_plot_time_stamp=0.0,
            time_in_between_plots=time_in_between_plots,
            plotter_precision=14,
        )
    project.add_solver(solver)
    project.set_global_simulation_parameters(
        dimensions=2,
        offset=[DomainOffset, DomainOffset],
        size=[DomainWidth, DomainWidth],
        min_end_time=end_time,
        max_end_time=end_time,
        periodic_BC=[True, True, True],
    )
    project.set_Peano4_installation(peano_root, peano4.api.output.CompileMode.RELEASE)
    peano4_project = project.generate_Peano4_project(
        verbosity=peano4.api.LoggerVerbosity.TEXT
    )
    peano4_project.generate()
    subprocess.run(["make", "-j"], cwd=directory, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(
        ["./higher-order"], cwd=directory, check=True, stdout=subprocess.DEVNULL
    )


def snap(offset, size):
    """!

    Snap a patch's offset and size onto the regular mesh over the domain

    The patch files hold offsets and sizes with seven significant digits
    only. Below an error of roughly 1e-7, this would dominate the error.

    """
    size = np.array([DomainWidth / round(DomainWidth / s) for s in size])
    offset = np.round((np.array(offset) - DomainOffset) / size) * size + DomainOffset
    return offset, size


def errors(directory, rk_order):
    """! Return a list of (time stamp, max error, rms error) of the density per snapshot"""
    result = []
    for time_stamp, files in tarch.plotter.api.notebook.read_snapshots(
        patch_file(directory, rk_order)
    ):
        squared_errors = []
        for file_name in files:
            for offset, size, values in tarch.plotter.api.notebook.read_patches(
                file_name
            ):
                offset, size = snap(offset, size)
                dofs = values.shape[0]
                x = offset[0] + (np.arange(dofs) + 0.5) * size[0] / dofs
                squared_errors.append(
                    (values[:, :, 0] - exact_density(x, time_stamp)) ** 2
                )
        squared_errors = np.concatenate([e.flatten() for e in squared_errors])
        result.append(
            (
                time_stamp,
                math.sqrt(np.max(squared_errors)),
                math.sqrt(np.mean(squared_errors)),
            )
        )
    return result


def density_along_x_axis(directory, rk_order, snapshot):
    """! Return time stamp plus grid cell centres and densities along y=0"""
    time_stamp, files = tarch.plotter.api.notebook.read_snapshots(
        patch_file(directory, rk_order)
    )[snapshot]
    x, rho = [], []
    for file_name in files:
        for offset, size, values in tarch.plotter.api.notebook.read_patches(file_name):
            offset, size = snap(offset, size)
            dofs = values.shape[0]
            if offset[1] <= 0.0 < offset[1] + size[1]:
                row = int((0.0 - offset[1]) / (size[1] / dofs))
                x += list(offset[0] + (np.arange(dofs) + 0.5) * size[0] / dofs)
                rho += list(values[row, :, 0])
    order = np.argsort(x)
    return time_stamp, np.array(x)[order], np.array(rho)[order]


def label(rk_order):
    if rk_order is None:
        return "Finite Volumes (Rusanov, explicit Euler)"
    return "FD4 with Runge-Kutta of order {}".format(rk_order)


def plot_convergence(runs):
    """!

    Max error at t=ConvergenceEndTime over the grid cells per axis

    @param runs: dict
      Maps rk_order (None for Finite Volumes) onto a list of
      (grid cells per axis, directory).

    """
    fig, ax = plt.subplots(figsize=(7, 5))
    for rk_order, directories in runs.items():
        cells = [c for c, _ in directories]
        max_errors = [errors(d, rk_order)[-1][1] for _, d in directories]
        style = {"marker": "o", "linestyle": "-"}
        if rk_order is None:
            style = {"marker": "s", "linestyle": "--"}
        elif rk_order == 3:
            # Runge-Kutta of order three and four yield the same errors. Big
            # hollow markers keep order three visible behind order four.
            style = {
                "marker": "o",
                "linestyle": "--",
                "markersize": 12,
                "markerfacecolor": "none",
            }
        ax.loglog(
            cells,
            max_errors,
            color="k" if rk_order is None else RungeKuttaColours[rk_order],
            label=label(rk_order),
            **style,
        )
    # reference slopes, anchored at the coarsest mesh
    cells = np.array([c for _, c in VolumeSizes], dtype=float)
    for order, anchor in [(1, 1.5e-1), (2, 1e-5), (4, 1e-4)]:
        ax.loglog(
            cells,
            anchor * (cells[0] / cells) ** order,
            ":",
            color="grey",
            linewidth=1,
        )
        ax.annotate(
            "order {}".format(order),
            (cells[-1], anchor * (cells[0] / cells[-1]) ** order),
            textcoords="offset points",
            xytext=(5, -3),
            color="grey",
        )
    ax.set_xticks(cells)
    ax.set_xticklabels([str(int(c)) for c in cells])
    ax.set_xlim(20, 400)
    ax.set_xlabel("grid cells (volumes) per axis")
    ax.set_ylabel("max |density - exact density| at t={}".format(ConvergenceEndTime))
    ax.set_title("Convergence for the entropy wave")
    ax.legend(loc="lower left", fontsize="small")
    fig.savefig(
        os.path.join(Here, "12_higher-order-solvers-convergence.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_profile(runs):
    """!

    Density along y=0 after one period on the coarsest mesh

    @param runs: list
      List of (rk_order, directory).

    """
    fig, ax = plt.subplots(figsize=(7, 4))
    x_exact = np.linspace(DomainOffset, DomainOffset + DomainWidth, 400)
    ax.plot(x_exact, exact_density(x_exact, 0.0), "k-", linewidth=1, label="exact")
    for rk_order, directory in runs:
        t, x, rho = density_along_x_axis(directory, rk_order, -1)
        ax.plot(
            x,
            rho,
            "s" if rk_order is None else "o",
            color="grey" if rk_order is None else RungeKuttaColours[rk_order],
            markersize=3,
            label=label(rk_order),
        )
    ax.set_xlabel("x")
    ax.set_ylabel("density")
    ax.set_title(
        "Density along y=0 at t={} on {}x{} grid cells".format(
            PeriodEndTime, VolumeSizes[0][1], VolumeSizes[0][1]
        )
    )
    ax.legend(loc="lower left", fontsize="small")
    fig.savefig(
        os.path.join(Here, "12_higher-order-solvers-profile.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_instability(runs):
    """!

    Max error over time for the explicit Euler time stepping vs the others

    @param runs: list
      List of (rk_order, directory) on the same mesh.

    """
    fig, (left, right) = plt.subplots(1, 2, figsize=(12, 4))
    for rk_order, directory in runs:
        e = np.array(errors(directory, rk_order))
        left.semilogy(
            e[1:, 0],
            e[1:, 1],
            "-",
            color=RungeKuttaColours[rk_order],
            label=label(rk_order),
        )
    left.set_xlabel("t")
    left.set_ylabel("max |density - exact density|")
    left.set_title("Error over time on {0}x{0} grid cells".format(VolumeSizes[1][1]))
    left.legend(loc="upper left", fontsize="small")

    rk_order, directory = runs[0]
    t, x, rho = density_along_x_axis(directory, rk_order, -1)
    right.plot(
        x,
        rho - exact_density(x, t),
        "-",
        color=RungeKuttaColours[rk_order],
        linewidth=1,
    )
    right.set_xlabel("x")
    right.set_ylabel("density - exact density")
    right.set_title("Error along y=0 at t={:.3f}, {}".format(t, label(rk_order)))
    fig.savefig(
        os.path.join(Here, "12_higher-order-solvers-instability.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 12_higher-order-solvers.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--peano-root",
        default=PeanoRoot,
        help="Configured and built Peano directory (default: the repository this script resides in)",
    )
    parser.add_argument(
        "--pictures",
        nargs="+",
        default=["convergence", "profile", "instability"],
        choices=["convergence", "profile", "instability"],
        help="Pictures to create (default: all)",
    )
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(
        prefix="12_higher-order-solvers-"
    )

    def directory(name):
        return os.path.join(work_directory, name)

    if "convergence" in args.pictures:
        runs = {}
        for rk_order in [None, 1, 2, 3, 4]:
            runs[rk_order] = []
            for volume_size, cells in VolumeSizes:
                d = directory(
                    "convergence-{}-{}".format(
                        "fv" if rk_order is None else "rk{}".format(rk_order), cells
                    )
                )
                build_and_run(
                    rk_order,
                    volume_size,
                    ConvergenceEndTime,
                    ConvergenceEndTime,
                    d,
                    args.peano_root,
                )
                runs[rk_order].append((cells, d))
        plot_convergence(runs)

    if "profile" in args.pictures:
        runs = []
        for rk_order in [None, 4]:
            d = directory(
                "profile-{}".format(
                    "fv" if rk_order is None else "rk{}".format(rk_order)
                )
            )
            build_and_run(
                rk_order,
                VolumeSizes[0][0],
                PeriodEndTime,
                PeriodEndTime,
                d,
                args.peano_root,
            )
            runs.append((rk_order, d))
        plot_profile(runs)

    if "instability" in args.pictures:
        runs = []
        for rk_order in [1, 2, 4]:
            d = directory("instability-rk{}".format(rk_order))
            build_and_run(
                rk_order, VolumeSizes[1][0], PeriodEndTime, 0.005, d, args.peano_root
            )
            runs.append((rk_order, d))
        plot_instability(runs)

    print("wrote PNGs into {}".format(Here))
