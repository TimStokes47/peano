"""

First-order vs. second-order reconstructed CCZ4: gauge wave benchmark

This is the gauge wave benchmark (@ref benchmarks_exagrype_gauge_wave) with the
fourth-order Finite Differences (exahype2.api.rkfd4), but we instantiate two
solvers in one setup: exagrype.api.FOCCZ4Solver, which evolves all 59 CCZ4
unknowns, and exagrype.api.FOSOCCZ4Solver, which evolves 25 of them and obtains
the 34 first derivatives from its exagrype.api.FOSOCCZ4Reconstructor. Both solvers
use the same Runge-Kutta scheme, the same lopsided advection and no
dissipation. The two do not yield identical results: The first-order solver
evolves the derivatives as unknowns, whereas the second-order solver
re-computes them from the 25 unknowns in each Runge-Kutta stage. The script
builds and runs the code, and then quantifies the difference of the 25
evolved unknowns of both solvers, and the deviation of both from the
analytical solution of the gauge wave.

Two exahype2.api.rkfd4 solvers cannot live in one project yet, as both
generate the solver-independent kernel rungekutta_linear_combination into the
same file (@ref page_exagrype_fo_so_project_plan). The script therefore
builds and runs each solver on its own, in a directory of its own
(run-FO and run-SO), and then compares the plot files.

@todo This post-run comparison of plot files is a stop-gap. We have to return
  to it and replace it by an on-the-fly check, i.e. a check within the
  simulation that compares both solvers' data while the code runs.

The gauge wave is a pure gauge perturbation of flat spacetime, i.e. an
analytical solution of the Einstein equations that travels along the x-axis
with speed one. It is periodic over the unit cube and hence a standard first
test of any numerical relativity code. This script uses exagrype.api, i.e.
the CCZ4 solvers of the new Python API. It requires a Peano build with

    --enable-exahype --enable-blockstructured --enable-loadbalancing --enable-exagrype

and is run from this directory via

    python3 fo_vs_so.py

One solver alone is built and run via python3 fo_vs_so.py FO or SO, and the
plots of both are compared via python3 fo_vs_so.py compare.

"""

import os
import shutil
import subprocess
import sys

import numpy as np

import peano4.api
import exahype2.api
import exagrype.api


create_release_build = False

if create_release_build:
    build_mode = peano4.api.output.CompileMode.RELEASE
    volume_size = 1.0 / 81.0
    end_time = 1.0
    time_in_between_plots = 0.1
    verbosity = peano4.api.LoggerVerbosity.VISUALISATION
else:
    build_mode = peano4.api.output.CompileMode.DEBUG
    volume_size = 1.0 / 9.0
    end_time = 0.02
    time_in_between_plots = 0.01
    verbosity = peano4.api.LoggerVerbosity.VISUALISATION

patch_size = 9


def build_and_run(variant):
    """!

    Generate, build and run one of the two solvers in its own directory

    @param variant: str
      FO or SO.

    """
    directory = f"run-{variant}"
    # The generator appends to kernels of an earlier run, so start from scratch
    shutil.rmtree(directory, ignore_errors=True)
    project = exahype2.api.Project(
        namespace=["benchmarks", "exagrype", "fo_vs_so", variant],
        project_name=f"fo-vs-so-{variant}",
        directory=directory,
        executable=f"fo-vs-so-{variant}",
    )
    if variant == "FO":
        solver = exagrype.api.FOCCZ4Solver(
            variant, initial_condition=exagrype.api.initialvalues.gauge_wave()
        )
    else:
        solver = exagrype.api.FOSOCCZ4Solver(
            variant, initial_condition=exagrype.api.initialvalues.gauge_wave()
        )
    Kernel = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta.RightHandSideKernel
    solver.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
        solver,
        patch_size,
        rk_order=4,
        right_hand_side_kernel=Kernel.CentralDifferencesWithLopsidedAdvection,
        advection_velocity=exagrype.api.FOCCZ4Solver.ADVECTION_VELOCITY,
        advected_quantities=solver.lopsided_advection_quantities(),
    )
    solver.grid_construction = exahype2.api.rkfd4.SetupSimulationOverRegularGrid(
        solver, volume_size=volume_size
    )
    solver.plotter = exahype2.api.rkfd4.PlotAsVTKFile(
        solver, first_plot_time_stamp=0.0, time_in_between_plots=time_in_between_plots
    )
    project.add_solver(solver)
    # The reconstructor of the second-order solver is part of the solver

    project.compiler = (
        peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate_with_fusion
    )
    project.set_global_simulation_parameters(
        dimensions=3,
        offset=[0.0, 0.0, 0.0],
        size=[1.0, 1.0, 1.0],
        min_end_time=end_time,
        max_end_time=end_time,
        periodic_BC=[True, True, True],
    )
    # Absolute, as the run directories are one level deeper than this script
    project.set_Peano4_installation(
        os.path.abspath("../../../"),
        build_mode,
    )
    peano4_project = project.generate_Peano4_project(verbosity=verbosity)
    peano4_project.generate()
    peano4_project.build(make_clean_first=True)
    if subprocess.call([f"./fo-vs-so-{variant}"], cwd=directory) != 0:
        sys.exit(f"{variant} run failed")


def snapshot_values(variant, unknowns, snapshot=-1):
    """!

    Return the unknowns of one snapshot of one solver

    @param variant: str
      FO or SO, i.e. the solver whose plot file we read

    @param unknowns: int
      Number of unknowns per grid point in the plot file

    @param snapshot: int
      Index of the snapshot, -1 is the last one.

    @return dict
      Maps the offset of a patch to its values as written by the plotter,
      reshaped into one row per grid point.

    """
    from tarch.plotter.api.input.PatchFileParser import PatchFileParser
    from tarch.plotter.api.notebook import read_snapshots

    _, patch_files = read_snapshots(
        f"run-{variant}/solution-{variant}.peano-patch-file"
    )[snapshot]
    result = {}
    for patch_file in patch_files:
        parser = PatchFileParser(patch_file, "", 0)
        parser.parse_file()
        for attributes in parser.unknown_attributes.values():
            for patch in attributes.cell_data:
                result[tuple(patch.offset)] = np.array(patch.values).reshape(
                    -1, unknowns
                )
    return result


def compare():
    evolved = sum(exagrype.api.FOSOCCZ4Solver.Unknowns.values())

    def stack(values, columns):
        return np.concatenate([values[key][:, :columns] for key in sorted(values)])

    first = snapshot_values("FO", 59)
    second = snapshot_values("SO", evolved)
    if not first or first.keys() != second.keys():
        sys.exit("FAILED: the plots of FO and SO hold different or no patches")
    fo = stack(first, evolved)
    so = stack(second, evolved)
    initial = stack(snapshot_values("FO", 59, 0), evolved)
    difference = so - fo
    names = exagrype.api.FOSOCCZ4Solver("names").variable_names_per_entry()
    print(
        f"{'unknown':>10} {'max |SO-FO|':>13} {'rel. L2 |SO-FO|':>16} {'max |FO(t)-FO(0)|':>18}"
    )
    for k, name in enumerate(names):
        norm = np.linalg.norm(fo[:, k])
        relative = np.linalg.norm(difference[:, k]) / norm if norm > 0 else 0.0
        movement = np.max(np.abs(fo[:, k] - initial[:, k]))
        print(
            f"{name:>10} {np.max(np.abs(difference[:, k])):13.3e} {relative:16.3e} {movement:18.3e}"
        )
    print(
        f"Largest difference of all evolved unknowns: {np.max(np.abs(difference)):.3e}"
    )


if len(sys.argv) > 1 and sys.argv[1] == "compare":
    compare()
elif len(sys.argv) > 1:
    build_and_run(sys.argv[1])
else:
    for variant in ["FO", "SO"]:
        subprocess.check_call([sys.executable, __file__, variant])
    compare()
