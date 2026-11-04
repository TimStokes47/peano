"""

First-order vs. second-order CCZ4: gauge wave benchmark

This is the gauge wave benchmark (@ref benchmarks_exagrype_gauge_wave) copied
1:1, but we instantiate two solvers in one setup: exagrype.api.FOCCZ4Solver
and exagrype.api.FOSOCCZ4Solver. For the time being, the latter is an exact
copy of the former, i.e. the two solutions have to be identical bit by bit.
This script builds and runs the code and then compares the plots of the two
solvers.

@todo This post-run comparison of plot files is a stop-gap. We have to return
  to it and replace it by an on-the-fly check, i.e. a check within the
  simulation that compares both solvers' data while the code runs, as soon as
  the second-order solver actually differs from the first-order one.

The gauge wave is a pure gauge perturbation of flat spacetime, i.e. an
analytical solution of the Einstein equations that travels along the x-axis
with speed one. It is periodic over the unit cube and hence a standard first
test of any numerical relativity code. This script uses exagrype.api, i.e.
the CCZ4 solver of the new Python API, and discretises it with Finite
Volumes. It requires a Peano build with

    --enable-exahype --enable-blockstructured --enable-loadbalancing --enable-exagrype

and is run from this directory via

    python3 fo_vs_so.py

"""

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
    volume_size = 1.0 / 27.0
    end_time = 0.02
    time_in_between_plots = 0.01
    verbosity = peano4.api.LoggerVerbosity.VISUALISATION

patch_size = 9


project = exahype2.api.Project(
    namespace=["benchmarks", "exagrype", "fo_vs_so"],
    project_name="fo-vs-so",
    directory=".",
    executable="fo-vs-so",
)

solvers = [
    exagrype.api.FOCCZ4Solver(
        "FO", initial_condition=exagrype.api.initialvalues.gauge_wave()
    ),
    exagrype.api.FOSOCCZ4Solver(
        "SO", initial_condition=exagrype.api.initialvalues.gauge_wave()
    ),
]
for solver in solvers:
    solver.numerical_scheme = exahype2.api.fv.RusanovWithOptimisticExplicitEuler(
        solver, patch_size
    )
    solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        solver, volume_size=volume_size
    )
    solver.plotter = exahype2.api.fv.PlotAsVTKFile(
        solver, first_plot_time_stamp=0.0, time_in_between_plots=time_in_between_plots
    )
    project.add_solver(solver)

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

project.set_Peano4_installation("../../../", build_mode)
peano4_project = project.generate_Peano4_project(verbosity=verbosity)
peano4_project.generate()
peano4_project.build(make_clean_first=True)


def last_snapshot_values(solver_name):
    """!

    Return the patches of the last snapshot of one solver

    @param solver_name: str
      Name of the solver whose plot file we read

    @return dict
      Maps the offset of a patch to its raw values as written by the plotter.
      The plotter writes binary files, i.e. the doubles are bit-exact.

    """
    from tarch.plotter.api.input.PatchFileParser import PatchFileParser
    from tarch.plotter.api.notebook import read_snapshots

    _, patch_files = read_snapshots(f"solution-{solver_name}.peano-patch-file")[-1]
    result = {}
    for patch_file in patch_files:
        parser = PatchFileParser(patch_file, "", 0)
        parser.parse_file()
        for attributes in parser.unknown_attributes.values():
            for patch in attributes.cell_data:
                result[tuple(patch.offset)] = np.array(patch.values)
    return result


def run_and_compare():
    if subprocess.call(["./fo-vs-so"]) != 0:
        sys.exit("fo-vs-so run failed")
    first_order = last_snapshot_values("FO")
    second_order = last_snapshot_values("SO")
    if not first_order or first_order.keys() != second_order.keys():
        sys.exit("FAILED: the plots of FO and SO hold different or no patches")
    for offset, values in first_order.items():
        if not np.array_equal(values, second_order[offset]):
            sys.exit(f"FAILED: FO and SO differ in patch at {offset}")
    print(f"PASSED: FO and SO are identical in all {len(first_order)} patches")


run_and_compare()
