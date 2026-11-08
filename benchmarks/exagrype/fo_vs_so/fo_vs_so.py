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

## Schemes, formulations and grid sizes

The benchmark covers two numerical schemes:

- FD4: fourth-order Finite Differences with classic Runge-Kutta 4 and lopsided
  advection (exahype2.api.rkfd4.CentralDifferencesWithRungeKutta). The
  second-order solver reconstructs the derivatives with the fourth-order
  stencil in each Runge-Kutta stage.
- FV: Finite Volumes with a Rusanov flux and explicit Euler
  (exahype2.api.fv.RusanovWithExplicitEuler). The second-order solver
  reconstructs the derivatives with second-order central differences
  once per time step. Neither advection nor dissipation are used, which is
  fine as the shift of the gauge wave is zero.

Each of the two comes in the formulations FO (exagrype.api.FOCCZ4Solver) and
SO (exagrype.api.FOSOCCZ4Solver). The grid has cells x cells x cells grid
points (patch size 9 and 3^k patches per axis, i.e. cells is 27, 81, ... (Peano refines to at least three patches per axis)). The grid size is
compiled into the executable, so each combination is built in a directory of
its own (run-scheme-formulation-cells).

    python3 fo_vs_so.py                         # all combinations, table at the end
    python3 fo_vs_so.py run FV SO 27            # one combination
    python3 fo_vs_so.py compare                 # re-print the table from existing runs

The environment variable FO_VS_SO_RELEASE=1 selects the release build, end time
0.1, and silences the generator logging. The default is the debug build.
FO_VS_SO_SIZES=27 overrides the grid sizes.

"""

import json
import os
import shutil
import subprocess
import sys
import time

import numpy as np

import peano4.api
import exahype2.api
import exagrype.api


create_release_build = os.environ.get("FO_VS_SO_RELEASE", "0") == "1"

if create_release_build:
    build_mode = peano4.api.output.CompileMode.RELEASE
    end_time = 0.1
    verbosity = peano4.api.LoggerVerbosity.OFF
    grid_sizes = [27, 81]
else:
    build_mode = peano4.api.output.CompileMode.DEBUG
    end_time = 0.02
    verbosity = peano4.api.LoggerVerbosity.VISUALISATION
    grid_sizes = [27]
if "FO_VS_SO_SIZES" in os.environ:
    grid_sizes = [int(size) for size in os.environ["FO_VS_SO_SIZES"].split(",")]

end_time = float(os.environ.get("FO_VS_SO_END", end_time))
time_in_between_plots = end_time / 2.0
patch_size = 9
schemes = ["FD4", "FV"]
formulations = ["FO", "SO"]


def directory_of(scheme, formulation, cells):
    return f"run-{scheme}-{formulation}-{cells}"


def build_and_run(scheme, formulation, cells):
    """!

    Generate, build and run one solver on one grid in its own directory

    @param scheme: str
      FD4 or FV.

    @param formulation: str
      FO or SO.

    @param cells: int
      Grid points per axis. Has to be a multiple of the patch size.

    """
    # Peano's regular grid has 3^k patches per axis
    assert cells in [patch_size * 3**k for k in range(1, 6)], cells
    name = f"{scheme}{formulation}"
    directory = directory_of(scheme, formulation, cells)
    executable = f"fo-vs-so-{scheme}-{formulation}-{cells}"
    # The generator appends to kernels of an earlier run, so start from scratch
    shutil.rmtree(directory, ignore_errors=True)
    project = exahype2.api.Project(
        namespace=["benchmarks", "exagrype", "fo_vs_so", name],
        project_name=executable,
        directory=directory,
        executable=executable,
    )
    solver_class = (
        exagrype.api.FOCCZ4Solver
        if formulation == "FO"
        else exagrype.api.FOSOCCZ4Solver
    )
    solver = solver_class(
        name, initial_condition=exagrype.api.initialvalues.gauge_wave()
    )
    # The grid cells have to be strictly smaller than volume_size times the patch size
    volume_size = 1.01 / cells
    match scheme:
        case "FD4":
            Kernel = (
                exahype2.api.rkfd4.CentralDifferencesWithRungeKutta.RightHandSideKernel
            )
            solver.numerical_scheme = (
                exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
                    solver,
                    patch_size,
                    rk_order=4,
                    right_hand_side_kernel=Kernel.CentralDifferencesWithLopsidedAdvection,
                    advection_velocity=exagrype.api.FOCCZ4Solver.ADVECTION_VELOCITY,
                    advected_quantities=solver.lopsided_advection_quantities(),
                )
            )
            solver.grid_construction = exahype2.api.rkfd4.SetupSimulationOverRegularGrid(
                solver, volume_size=volume_size
            )
            solver.plotter = exahype2.api.rkfd4.PlotAsVTKFile(
                solver,
                first_plot_time_stamp=0.0,
                time_in_between_plots=time_in_between_plots,
            )
        case "FV":
            # No advection, no dissipation: the shift of the gauge wave is zero
            solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(
                solver, patch_size, cfl_constant=float(os.environ.get("FO_VS_SO_CFL", "0.9"))
            )
            solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
                solver, volume_size=volume_size
            )
            solver.plotter = exahype2.api.fv.PlotAsVTKFile(
                solver,
                first_plot_time_stamp=0.0,
                time_in_between_plots=time_in_between_plots,
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
    start = time.time()
    if subprocess.call([f"./{executable}"], cwd=directory) != 0:
        sys.exit(f"{scheme} {formulation} {cells} run failed")
    wall_time = time.time() - start
    with open(os.path.join(directory, "wall-time.json"), "w") as file:
        json.dump({"wall_time": wall_time}, file)


def snapshot(scheme, formulation, cells, unknowns, snapshot_index=-1):
    """!

    Return the unknowns of one snapshot of one solver

    @param scheme: str
      FD4 or FV.

    @param formulation: str
      FO or SO.

    @param cells: int
      Grid points per axis.

    @param unknowns: int
      Number of unknowns per grid point in the plot file

    @param snapshot_index: int
      Index of the snapshot, -1 is the last one.

    @return time stamp, values, positions
      The values and the x coordinates of the grid points, both sorted by the
      offset of the patches. Values has one row per grid point.

    """
    from tarch.plotter.api.input.PatchFileParser import PatchFileParser
    from tarch.plotter.api.notebook import read_snapshots

    name = f"{scheme}{formulation}"
    directory = directory_of(scheme, formulation, cells)
    time_stamp, patch_files = read_snapshots(
        f"{directory}/solution-{name}.peano-patch-file"
    )[snapshot_index]
    result = {}
    for patch_file in patch_files:
        parser = PatchFileParser(patch_file, "", 0)
        parser.parse_file()
        for attributes in parser.unknown_attributes.values():
            for patch in attributes.cell_data:
                values = np.array(patch.values).reshape(-1, unknowns)
                n = round(len(values) ** (1.0 / 3.0))
                # x runs fastest within a patch
                x = patch.offset[0] + (np.arange(len(values)) % n + 0.5) * patch.size[0] / n
                result[tuple(patch.offset)] = (values, x)
    keys = sorted(result)
    return (
        time_stamp,
        np.concatenate([result[key][0] for key in keys]) if keys else None,
        np.concatenate([result[key][1] for key in keys]) if keys else None,
        keys,
    )


def gauge_wave_lapse(x, t):
    """! Analytical lapse, see exagrype::initialvalues::gaugeWave() """
    return np.sqrt(1.0 - 0.1 * np.sin(2.0 * np.pi * (x - t)))


def compare_one(scheme, cells, verbose):
    """!

    Compare FO and SO on one grid

    @return dict
      Summary numbers, or None if one of the runs is missing.

    """
    evolved = sum(exagrype.api.FOSOCCZ4Solver.Unknowns.values())
    try:
        t, fo, x, keys_fo = snapshot(scheme, "FO", cells, 59)
        _, so, _, keys_so = snapshot(scheme, "SO", cells, evolved)
        _, initial, _, _ = snapshot(scheme, "FO", cells, 59, 0)
    except (FileNotFoundError, IndexError):
        return None
    if fo is None or keys_fo != keys_so:
        sys.exit("FAILED: the plots of FO and SO hold different or no patches")
    fo = fo[:, :evolved]
    initial = initial[:, :evolved]
    difference = so - fo
    if verbose:
        names = exagrype.api.FOSOCCZ4Solver("names").variable_names_per_entry()
        print(f"\n{scheme}, {cells}^3 grid points, t={t}")
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
    exact = gauge_wave_lapse(x, t)
    lapse = 16
    summary = {
        "time": t,
        "max_difference": float(np.max(np.abs(difference))),
        "lapse_difference": float(np.max(np.abs(difference[:, lapse]))),
        "lapse_error_FO": float(np.linalg.norm(fo[:, lapse] - exact) / np.linalg.norm(exact)),
        "lapse_error_SO": float(np.linalg.norm(so[:, lapse] - exact) / np.linalg.norm(exact)),
    }
    for formulation in formulations:
        with open(os.path.join(directory_of(scheme, formulation, cells), "wall-time.json")) as file:
            summary["wall_time_" + formulation] = json.load(file)["wall_time"]
    return summary


def compare():
    rows = []
    for scheme in schemes:
        for cells in grid_sizes:
            summary = compare_one(scheme, cells, verbose=len(grid_sizes) == 1)
            if summary is not None:
                rows.append((scheme, cells, summary))
    print(
        f"\n{'scheme':>6} {'cells':>6} {'t':>7} {'max|SO-FO|':>11} {'max|SO-FO| lapse':>17} "
        f"{'lapse err FO':>13} {'lapse err SO':>13} {'wall FO [s]':>12} {'wall SO [s]':>12}"
    )
    for scheme, cells, s in rows:
        print(
            f"{scheme:>6} {cells:>6} {s['time']:7.3f} {s['max_difference']:11.2e} {s['lapse_difference']:17.2e} "
            f"{s['lapse_error_FO']:13.2e} {s['lapse_error_SO']:13.2e} "
            f"{s['wall_time_FO']:12.2f} {s['wall_time_SO']:12.2f}"
        )


if len(sys.argv) > 1 and sys.argv[1] == "compare":
    compare()
elif len(sys.argv) > 1 and sys.argv[1] == "run":
    build_and_run(sys.argv[2], sys.argv[3], int(sys.argv[4]))
else:
    for scheme in schemes:
        for cells in grid_sizes:
            for formulation in formulations:
                subprocess.check_call(
                    [sys.executable, __file__, "run", scheme, formulation, str(cells)]
                )
    compare()
