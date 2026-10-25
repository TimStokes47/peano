# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Create the PNG illustrations of the tutorial 13_euler-with-amr.ipynb

The script builds the tutorial's point explosion on a regular grid, runs
it and writes all pictures into the directory of this script. It then
builds the setup with the point-wise refinement criterion, runs it on the
resulting adaptive mesh, and draws the mesh, the solution, a comparison
with the regular mesh, plus the setup's task graph. The PDE
terms are injected from Python, i.e. we do not need any hand-written C++
file. Call it from anywhere:

     python3 tutorials/exahype2/illustrations/13_euler-with-amr.py

All builds and outputs go into a temporary directory (or --work-directory).

"""
import argparse
import os
import types
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
  constexpr double gamma  = 1.4;
  constexpr double radius = 0.1;
  const bool   inside = x[0] * x[0] + x[1] * x[1] < radius * radius;
  const double rho    = inside ? 1.0 : 0.125;
  const double p      = inside ? 1.0 : 0.1;
  Q[0] = rho;
  Q[1] = 0.0;
  Q[2] = 0.0;
  Q[3] = p / (gamma - 1.0);
"""

VolumeSize = 0.005
AdaptiveVolumeSize = 0.02
AdaptiveMinVolumeSize = 0.002
RefineErasePointwisely = """
  if (Q[0] > 0.8) *refinementCommand = ::exahype2::RefinementCommand::Refine;
"""
EndTime = 0.2
TimeInBetweenPlots = 0.01
PatchFile = "solution-EulerFV.peano-patch-file"


def create_solver(adaptive):
    """! The tutorial's solver over a regular or an adaptive mesh"""
    solver = exahype2.api.Solver("EulerFV", 4, 0)
    solver.add_user_solver_includes("#include <cmath>\n#include <algorithm>\n")
    solver.add_pde_term(exahype2.api.Flux(implementation=Flux, stateless=False))
    solver.add_pde_term(
        exahype2.api.MaxEigenvalue(implementation=MaxEigenvalue, stateless=False)
    )
    solver.add_pde_term(
        exahype2.api.InitPointWisely(implementation=InitPointWisely, stateless=False)
    )
    if adaptive:
        solver.add_pde_term(
            exahype2.api.RefineErasePointwisely(implementation=RefineErasePointwisely)
        )
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
    if adaptive:
        solver.grid_construction = (
            exahype2.api.fv.SetupSimulationWithUserCodedRefinementCriterion(
                solver,
                volume_size=AdaptiveVolumeSize,
                min_volume_size=AdaptiveMinVolumeSize,
            )
        )
    else:
        solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
            solver, volume_size=VolumeSize
        )
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver, first_plot_time_stamp=0.0, time_in_between_plots=TimeInBetweenPlots
    )
    return solver


def build_and_run(directory, adaptive=False):
    """! Generate, compile and run the tutorial setup within directory"""
    end_time = EndTime
    project = exahype2.api.Project(
        namespace=["tutorials", "exahype2", "euler"],
        project_name="Euler",
        directory=directory,
        executable="euler",
    )
    solver = create_solver(adaptive)
    project.add_solver(solver)
    project.set_global_simulation_parameters(
        dimensions=2,
        offset=[-0.5, -0.5],
        size=[1.0, 1.0],
        min_end_time=end_time,
        max_end_time=end_time,
        periodic_BC=[True, True, True],
    )
    project.set_Peano4_installation(PeanoRoot, peano4.api.output.CompileMode.RELEASE)
    peano4_project = project.generate_Peano4_project()
    peano4_project.generate()
    subprocess.run(["make", "-j"], cwd=directory, check=True, stdout=subprocess.DEVNULL)
    subprocess.run(["./euler"], cwd=directory, check=True, stdout=subprocess.DEVNULL)


def read_snapshot(directory, snapshot):
    """! Return the time stamp and all patches (offset, size, values) of a snapshot"""
    snapshots = tarch.plotter.api.notebook.read_snapshots(
        os.path.join(directory, PatchFile)
    )
    time_stamp, patch_files = snapshots[snapshot]
    patches = [
        p for f in patch_files for p in tarch.plotter.api.notebook.read_patches(f)
    ]
    return time_stamp, patches


def number_of_snapshots(directory):
    return len(
        tarch.plotter.api.notebook.read_snapshots(os.path.join(directory, PatchFile))
    )


def density_along_x_axis(directory, snapshot):
    """! Return time stamp plus volume centres and densities along y=0, x>=0"""
    time_stamp, patches = read_snapshot(directory, snapshot)
    x, rho = [], []
    for offset, size, values in patches:
        if offset[1] <= 0.0 < offset[1] + size[1]:
            dof = values.shape[0]
            h = size[0] / dof
            row = int((0.0 - offset[1]) / (size[1] / dof))
            for i in range(dof):
                centre = offset[0] + (i + 0.5) * h
                if centre >= 0.0:
                    x.append(centre)
                    rho.append(values[row, i, 0])
    order = np.argsort(x)
    return time_stamp, np.array(x)[order], np.array(rho)[order]


def plot_mesh(directory):
    """! Initial density plus the patch boundaries"""
    file_name = os.path.join(directory, PatchFile)
    ax = tarch.plotter.api.notebook.plot_snapshot(file_name, 0, 0, label="density")
    _, patches = read_snapshot(directory, 0)
    for offset, size, values in patches:
        ax.add_patch(
            matplotlib.patches.Rectangle(
                offset, size[0], size[1], fill=False, edgecolor="white", linewidth=0.4
            )
        )
    ax.set_title(
        "Initial density\n{0}x{0} patches, {1}x{1} volumes per patch".format(
            round(1.0 / patches[0][1][0]), patches[0][2].shape[0]
        )
    )
    ax.figure.savefig(
        os.path.join(Here, "13_euler-with-amr-mesh.png"), dpi=100, bbox_inches="tight"
    )
    plt.close(ax.figure)


def plot_snapshots(directory):
    """! Density at three time stamps side by side"""
    file_name = os.path.join(directory, PatchFile)
    snapshots = number_of_snapshots(directory)
    fig, axes = plt.subplots(1, 3, figsize=(15, 4.5))
    for ax, snapshot in zip(
        axes, [(snapshots - 1) // 4, (snapshots - 1) // 2, snapshots - 1]
    ):
        tarch.plotter.api.notebook.plot_snapshot(
            file_name, snapshot, 0, label="density", ax=ax
        )
    fig.tight_layout()
    fig.savefig(
        os.path.join(Here, "13_euler-with-amr-snapshots.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_profile(directory):
    """! Density along the positive x-axis at a few time stamps"""
    fig, ax = plt.subplots(figsize=(7, 4))
    snapshots = number_of_snapshots(directory)
    for snapshot, colour in zip(
        [0, (snapshots - 1) // 4, (snapshots - 1) // 2, snapshots - 1],
        ["C0", "C1", "C2", "C3"],
    ):
        t, x, rho = density_along_x_axis(directory, snapshot)
        ax.plot(
            x,
            rho,
            "o-",
            color=colour,
            markersize=2,
            linewidth=0.8,
            label="t={:.2f}".format(t),
        )
    ax.set_xlabel("distance r from the centre (along y=0)")
    ax.set_ylabel("density")
    ax.set_title("Density profile of the point explosion")
    ax.legend()
    fig.savefig(
        os.path.join(Here, "13_euler-with-amr-profile.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_active_patches(directory):
    """!

    Highlight the patches which hold steep density gradients at the end

    A patch counts as active if the largest density jump between two
    neighbouring volumes within the patch exceeds 20% of the largest such
    jump in the whole domain. This is the kind of quantity a refinement
    criterion would look at.

    """
    file_name = os.path.join(directory, PatchFile)
    snapshot = number_of_snapshots(directory) - 1
    ax = tarch.plotter.api.notebook.plot_snapshot(
        file_name, snapshot, 0, label="density"
    )
    t, patches = read_snapshot(directory, snapshot)

    def max_jump(values):
        rho = values[..., 0]
        return max(
            np.abs(np.diff(rho, axis=0)).max(), np.abs(np.diff(rho, axis=1)).max()
        )

    jumps = [max_jump(values) for _, _, values in patches]
    threshold = 0.2 * max(jumps)
    active = 0
    for (offset, size, values), jump in zip(patches, jumps):
        if jump > threshold:
            active += 1
            ax.add_patch(
                matplotlib.patches.Rectangle(
                    offset,
                    size[0],
                    size[1],
                    facecolor=(1.0, 0.0, 0.0, 0.35),
                    edgecolor="red",
                    linewidth=0.5,
                )
            )
    ax.set_title(
        "t={:.2f}: {} of {} patches ({:.0f}%) hold steep gradients".format(
            t, active, len(patches), 100.0 * active / len(patches)
        )
    )
    ax.figure.savefig(
        os.path.join(Here, "13_euler-with-amr-active-patches.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(ax.figure)
    print("{} of {} patches are active".format(active, len(patches)))


def plot_adaptive_mesh(directory):
    """! Initial density on the adaptive mesh plus the patch boundaries"""
    file_name = os.path.join(directory, PatchFile)
    ax = tarch.plotter.api.notebook.plot_snapshot(file_name, 0, 0, label="density")
    _, patches = read_snapshot(directory, 0)
    for offset, size, values in patches:
        ax.add_patch(
            matplotlib.patches.Rectangle(
                offset, size[0], size[1], fill=False, edgecolor="white", linewidth=0.3
            )
        )
    sizes = sorted({round(size[0], 6) for _, size, _ in patches}, reverse=True)
    counts = [sum(1 for _, size, _ in patches if round(size[0], 6) == h) for h in sizes]
    ax.set_title(
        "Initial density on the adaptive mesh\n"
        + ", ".join(
            "{} patches of size 1/{}".format(count, round(1.0 / h))
            for h, count in zip(sizes, counts)
        ),
        fontsize=9,
    )
    ax.figure.savefig(
        os.path.join(Here, "13_euler-with-amr-adaptive-mesh.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(ax.figure)
    print("adaptive mesh: {}".format(dict(zip(sizes, counts))))


def plot_adaptive_snapshots(directory):
    """! Density at three time stamps on the adaptive mesh plus patch boundaries"""
    file_name = os.path.join(directory, PatchFile)
    snapshots = number_of_snapshots(directory)
    fig, axes = plt.subplots(1, 3, figsize=(15, 4.5))
    for ax, snapshot in zip(
        axes, [(snapshots - 1) // 4, (snapshots - 1) // 2, snapshots - 1]
    ):
        tarch.plotter.api.notebook.plot_snapshot(
            file_name, snapshot, 0, label="density", ax=ax
        )
        _, patches = read_snapshot(directory, snapshot)
        for offset, size, values in patches:
            ax.add_patch(
                matplotlib.patches.Rectangle(
                    offset,
                    size[0],
                    size[1],
                    fill=False,
                    edgecolor="white",
                    linewidth=0.2,
                )
            )
    fig.tight_layout()
    fig.savefig(
        os.path.join(Here, "13_euler-with-amr-adaptive-snapshots.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def plot_profile_comparison(regular_directory, adaptive_directory):
    """! Density along the positive x-axis: regular against adaptive mesh"""
    fig, ax = plt.subplots(figsize=(7, 4))
    snapshots = number_of_snapshots(regular_directory)
    assert snapshots == number_of_snapshots(adaptive_directory)
    for snapshot, colour in zip(
        [(snapshots - 1) // 4, (snapshots - 1) // 2, snapshots - 1],
        ["C1", "C2", "C3"],
    ):
        t, x, rho = density_along_x_axis(regular_directory, snapshot)
        ax.plot(
            x,
            rho,
            "-",
            color=colour,
            linewidth=1.0,
            label="regular, t={:.2f}".format(t),
        )
        t, x, rho = density_along_x_axis(adaptive_directory, snapshot)
        ax.plot(
            x,
            rho,
            "o",
            color=colour,
            markersize=1.5,
            label="adaptive, t={:.2f}".format(t),
        )
    ax.set_xlabel("distance r from the centre (along y=0)")
    ax.set_ylabel("density")
    ax.set_title("Regular mesh (lines) against adaptive mesh (dots)")
    ax.legend(fontsize=8)
    fig.savefig(
        os.path.join(Here, "13_euler-with-amr-profile-comparison.png"),
        dpi=100,
        bbox_inches="tight",
    )
    plt.close(fig)


def print_conservation(directory):
    """! Total mass and energy of the first and the last snapshot"""
    for snapshot in [0, number_of_snapshots(directory) - 1]:
        t, patches = read_snapshot(directory, snapshot)
        mass = sum(
            values[..., 0].sum() * (size[0] / values.shape[0]) ** 2
            for _, size, values in patches
        )
        energy = sum(
            values[..., 3].sum() * (size[0] / values.shape[0]) ** 2
            for _, size, values in patches
        )
        print("t={:.3f}: mass={:.8f}, energy={:.8f}".format(t, mass, energy))


def plot_setup_task_graph():
    """! Construct the setup task graph with the refinement criterion and draw it"""
    peano4.api.Logger().file_name = os.path.join(
        tempfile.gettempdir(), "13-euler-with-amr-log.md"
    )
    with tempfile.TemporaryDirectory() as directory:
        peano_project = peano4.api.Project(
            ["illustration"], "illustration", directory=directory
        )
        solver = create_solver(adaptive=True)
        exahype2_project = types.SimpleNamespace(periodic_BC=[True, True, True])
        setup_simulation = peano4.api.taskgraph.Graph()
        solver.grid_construction.construct_output(
            exahype2_project, peano_project, setup_simulation
        )
    setup_simulation.visualise(
        title="Setup task graph with point-wise refinement criterion"
    )
    output = os.path.join(Here, "13_euler-with-amr-setup-task-graph.png")
    plt.gcf().savefig(output, dpi=90)
    plt.close(plt.gcf())
    print("Wrote", output)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Create the PNGs of the tutorial 13_euler-with-amr.ipynb"
    )
    parser.add_argument(
        "--work-directory",
        default=None,
        help="Directory for builds and outputs (default: temporary directory)",
    )
    parser.add_argument(
        "--skip-run",
        action="store_true",
        help="Reuse the output of a previous run within the work directory",
    )
    args = parser.parse_args()

    work_directory = args.work_directory or tempfile.mkdtemp(
        prefix="13_euler-with-amr-"
    )
    directory = os.path.join(work_directory, "regular-grid")
    os.makedirs(directory, exist_ok=True)
    if not args.skip_run:
        print("build and run in {}".format(directory))
        build_and_run(directory)

    plot_mesh(directory)
    plot_snapshots(directory)
    plot_profile(directory)
    plot_active_patches(directory)

    adaptive_directory = os.path.join(work_directory, "adaptive-mesh")
    os.makedirs(adaptive_directory, exist_ok=True)
    if not args.skip_run:
        print("build and run over adaptive mesh in {}".format(adaptive_directory))
        build_and_run(adaptive_directory, adaptive=True)
    plot_adaptive_mesh(adaptive_directory)
    plot_adaptive_snapshots(adaptive_directory)
    plot_profile_comparison(directory, adaptive_directory)
    print_conservation(adaptive_directory)
    plot_setup_task_graph()
    print("wrote PNGs into {}".format(Here))
