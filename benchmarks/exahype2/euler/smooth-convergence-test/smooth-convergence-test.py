import os, sys
import peano4.api
import exahype2


create_release_build = False
if (create_release_build):
    build_mode = peano4.api.output.CompileMode.RELEASE
    volume_size = 0.0001
    end_time    = 0.001
    time_in_between_plots = 0.001
    verbosity = peano4.api.LoggerVerbosity.TEXT
else:
    build_mode = peano4.api.output.CompileMode.DEBUG
    volume_size = 0.001
    end_time    = 0.01
    time_in_between_plots = 0.001
    verbosity   = peano4.api.LoggerVerbosity.VISUALISATION


project = exahype2.api.Project(
    namespace=["benchmarks", "exahype2", "euler", "smooth_convergence_test"],
    project_name="smooth-convergence-test",
    directory=".",
    executable="smooth-convergence-test",
)

solver_name = "EulerFVSmooth"
unknowns = 4
auxiliary_variables = 0
patch_size = 9
dimensions = 2

thesolver = exahype2.api.Solver(
    solver_name,
    unknowns,
    auxiliary_variables,
)

thesolver.add_pde_term( exahype2.api.Flux(stateless=False,))
thesolver.add_pde_term(exahype2.api.MaxEigenvalue(stateless=False,))
thesolver.add_pde_term(exahype2.api.InitPointWisely(stateless=False,))
thesolver.add_pde_term(exahype2.api.DirichletBoundaryConditions(stateless=False,))

thesolver.numerical_scheme = exahype2.api.fv.RusanovWithOptimisticExplicitEuler(thesolver, patch_size)
thesolver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
    thesolver, volume_size=volume_size
)
thesolver.plotter = exahype2.api.fv.PlotAsVTKFile(
    thesolver, first_plot_time_stamp=0.0, time_in_between_plots=time_in_between_plots
)
project.add_solver(thesolver)



#project.compiler = peano4.api.taskgraph.compiler.Sequential.translate
#project.compiler = peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate_with_sequential
project.compiler = peano4.api.taskgraph.compiler.CellTaskProducerConsumer.translate_with_fusion


project.set_global_simulation_parameters(
    dimensions=dimensions,
    offset=[-0.1, -0.1],
    size=[0.2, 0.2],
    min_end_time=end_time,
    max_end_time=end_time,
    periodic_BC=[False, False, False],
#    periodic_BC=[True, True, True],
)

project.set_load_balancing("toolbox::loadbalancing::strategies::SpreadOutOnceGridStagnates", "new ::exahype2::LoadBalancingConfiguration()")
project.set_Peano4_installation("../../../../", build_mode)
peano4_project = project.generate_Peano4_project(verbosity=verbosity)
peano4_project.generate()
peano4_project.build(make_clean_first=True)
