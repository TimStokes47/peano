"""

Single black hole benchmark for the ExaGRyPE Python API

A single Schwarzschild black hole at the origin of the domain [-9,9]^3. The
initial data stems from TwoPunctures. The script uses exagrype.api, i.e. the
CCZ4 solver of the new Python API, and discretises it with Finite Volumes on
a regular mesh. It mirrors the pure Finite Volume runs of the legacy
benchmark in benchmarks/exahype2/ccz4/single-black-hole. It requires a Peano
build with

    --enable-exahype --enable-blockstructured --enable-loadbalancing --enable-exagrype --with-gsl

and is run from this directory via

    python3 single-black-hole.py

"""

import peano4.api
import exahype2.api
import exagrype.api


create_release_build = False

if create_release_build:
    build_mode = peano4.api.output.CompileMode.RELEASE
    cells_per_axis = 27
    end_time = 10.0
    time_in_between_plots = 1.0
    verbosity = peano4.api.LoggerVerbosity.TEXT
else:
    build_mode = peano4.api.output.CompileMode.DEBUG
    cells_per_axis = 9
    end_time = 0.1
    time_in_between_plots = 0.05
    verbosity = peano4.api.LoggerVerbosity.VISUALISATION

# An even patch size ensures that no volume centre coincides with the
# puncture at the origin, where the initial data is singular.
patch_size = 6

domain_offset = [-9.0, -9.0, -9.0]
domain_size = [18.0, 18.0, 18.0]
black_hole_mass = 1.0

# The regular grid construction refines all cells whose edge length is greater
# than or equal to volume_size * patch_size. We hence add a small safety margin
# to get exactly cells_per_axis cells per axis.
volume_size = 1.01 * domain_size[0] / cells_per_axis / patch_size


project = exahype2.api.Project(
    namespace=["benchmarks", "exagrype", "singleblackhole"],
    project_name="single-black-hole",
    directory=".",
    executable="single-black-hole",
)

solver = exagrype.api.CCZ4Solver(
    "CCZ4",
    parameters={"CCZ4bs": 1.0, "CCZ4sk": 1.0},
    initial_condition=exagrype.api.initialvalues.single_black_hole(
        mass=black_hole_mass
    ),
)

# The legacy code uses Sommerfeld conditions. We impose the (static) initial
# data on the boundary instead, which is a reasonable approximation as long as
# the boundary is far away from the black hole.
solver.add_pde_term(
    exahype2.api.DirichletBoundaryConditions(
        implementation=f"""
  ::exagrype::initialvalues::singleBlackHole(Q, x, t, {black_hole_mass});
"""
    )
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
    offset=domain_offset,
    size=domain_size,
    min_end_time=end_time,
    max_end_time=end_time,
    periodic_BC=[False, False, False],
)

project.set_Peano4_installation("../../../", build_mode)
peano4_project = project.generate_Peano4_project(verbosity=verbosity)
peano4_project.generate()
peano4_project.build(make_clean_first=True)
