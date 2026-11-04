"""

Gauge wave benchmark for the ExaGRyPE Python API

The gauge wave is a pure gauge perturbation of flat spacetime, i.e. an
analytical solution of the Einstein equations that travels along the x-axis
with speed one. It is periodic over the unit cube and hence a standard first
test of any numerical relativity code. This script uses exagrype.api, i.e.
the CCZ4 solver of the new Python API, and discretises it with Finite
Volumes. It requires a Peano build with

    --enable-exahype --enable-blockstructured --enable-loadbalancing --enable-exagrype

and is run from this directory via

    python3 gauge-wave.py

"""

import peano4.api
import exahype2.api
import exagrype.api


create_release_build = False

if create_release_build:
    build_mode = peano4.api.output.CompileMode.RELEASE
    volume_size = 1.0 / 81.0
    end_time = 1.0
    time_in_between_plots = 0.1
    verbosity = peano4.api.LoggerVerbosity.TEXT
else:
    build_mode = peano4.api.output.CompileMode.DEBUG
    volume_size = 1.0 / 27.0
    end_time = 0.02
    time_in_between_plots = 0.01
    verbosity = peano4.api.LoggerVerbosity.VISUALISATION

patch_size = 9


project = exahype2.api.Project(
    namespace=["benchmarks", "exagrype", "gaugewave"],
    project_name="gauge-wave",
    directory=".",
    executable="gauge-wave",
)

solver = exagrype.api.FOCCZ4Solver(
    "CCZ4",
    initial_condition=exagrype.api.initialvalues.gauge_wave(),
)
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
