"""Unit tests for multiple plotters per solver (@ref exahype2.api.Solver.Solver.add_plotter())
and the checkpoints (@ref exahype2.api.fv.WriteCheckpoint, @ref exahype2.api.rkfd4.WriteCheckpoint)."""

import types

import peano4.api
import peano4.api.taskgraph
import exahype2.api
import pytest


def _solver(scheme="fv", dirichlet=False):
    solver = exahype2.api.Solver("Probe", {"rho": 1, "j": 2, "E": 1})
    solver.add_pde_term(exahype2.api.Flux(implementation=""))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation=""))
    solver.add_pde_term(exahype2.api.InitPointWisely(implementation=""))
    if dirichlet:
        solver.add_pde_term(exahype2.api.DirichletBoundaryConditions(implementation=""))
    if scheme == "fv":
        solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
    else:
        solver.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
            solver, patch_size=9, rk_order=2
        )
    return solver


def _plot_graph(solver, tmp_path, periodic=True):
    peano_project = peano4.api.Project(["probe"], "probe", directory=str(tmp_path))
    exahype2_project = types.SimpleNamespace(periodic_BC=[periodic] * 3)
    graph = peano4.api.taskgraph.Graph()
    for plotter in solver.plotters:
        plotter.construct_output(exahype2_project, peano_project, graph)
    return graph


def _method_definition(solver, declaration):
    matches = [
        generator("probe::AbstractProbe")
        for existing, generator in solver._abstract_solver_methods
        if existing == declaration
    ]
    assert len(matches) == 1, declaration
    return matches[0]


def test_plotters_get_names_from_class_and_counter():
    solver = _solver()
    solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver))
    solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver, filename="other"))
    solver.add_plotter(exahype2.api.fv.WriteCheckpoint(solver))
    assert [p.name for p in solver.plotters] == [
        "PlotAsVTKFile",
        "PlotAsVTKFile2",
        "WriteCheckpoint",
    ]


def test_plotter_names_have_to_be_unique_identifiers():
    solver = _solver()
    solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver, name="Plot"))
    with pytest.raises(ValueError):
        solver.add_plotter(
            exahype2.api.fv.PlotAsVTKFile(solver, filename="b", name="Plot")
        )
    with pytest.raises(ValueError):
        exahype2.api.fv.PlotAsVTKFile(solver, name="not an identifier")


def test_plotters_must_not_share_files():
    solver = _solver()
    solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver))
    with pytest.raises(ValueError):
        solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver))


def test_plotter_shorthand_replaces_all_plotters():
    solver = _solver()
    assert solver.plotter is None
    solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver))
    solver.add_plotter(exahype2.api.fv.WriteCheckpoint(solver))
    with pytest.raises(RuntimeError):
        solver.plotter
    checkpoint = exahype2.api.fv.WriteCheckpoint(solver)
    solver.plotter = checkpoint
    assert solver.plotters == [checkpoint]
    solver.plotter = None
    assert solver.plotters == []


def test_each_plotter_gets_own_bookkeeping_and_wants_to_plot_combines_them(tmp_path):
    solver = _solver()
    solver.add_plotter(
        exahype2.api.fv.PlotAsVTKFile(solver, time_in_between_plots=0.25)
    )
    solver.add_plotter(
        exahype2.api.fv.WriteCheckpoint(solver, first_plot_time_stamp=0.5)
    )
    _plot_graph(solver, tmp_path)
    solver._add_wants_to_plot_to_solver()
    solver._add_wants_to_plot_to_solver()

    assert (
        "double _nextPlotAsVTKFileTimeStamp = 0.0;"
        in solver._abstract_solver_attributes
    )
    assert (
        "double _nextWriteCheckpointTimeStamp = 0.5;"
        in solver._abstract_solver_attributes
    )
    assert "_nextPlotAsVTKFileTimeStamp += 0.25;" in _method_definition(
        solver, "void finishPlotAsVTKFile();"
    )
    wants_to_plot = _method_definition(solver, "bool wantsToPlot() const;")
    assert "isPlotAsVTKFileActive() or isWriteCheckpointActive()" in wants_to_plot


def test_checkpoint_meta_data_lists_added_attributes(tmp_path):
    solver = _solver()
    solver.add_plotter(exahype2.api.fv.PlotAsVTKFile(solver))
    solver.add_plotter(exahype2.api.fv.WriteCheckpoint(solver))
    _plot_graph(solver, tmp_path)
    definition = _method_definition(
        solver, "std::string toCheckpointMetaData() const override;"
    )
    assert definition.count("toCheckpointMetaDataEntry(") == 2
    assert (
        'toCheckpointMetaDataEntry("nextWriteCheckpointTimeStamp", _nextWriteCheckpointTimeStamp)'
        in definition
    )


def test_vtk_plotter_selects_quantities(tmp_path):
    solver = _solver()
    plotter = exahype2.api.fv.PlotAsVTKFile(solver, quantities=["j", 3])
    plotter.add_plot_filter((0.0, 0.0), (0.5, 0.5))
    solver.add_plotter(plotter)
    graph = _plot_graph(solver, tmp_path)
    (action_set,) = graph.tasks[0].action_sets
    assert action_set.d["SELECT_DOFS"] == [1, 2, 3]
    assert action_set.d["PLOTTED_UNKNOWNS"] == "3"
    assert action_set.d["DESCRIPTION"] == "j[0], j[1], E"
    assert "marker.overlaps" in action_set.d["GUARD_PREDICATE"]
    assert action_set.get_guard_of_operation("touchCellFirstTime").startswith(
        "repositories::instanceOfProbe.isPlotAsVTKFileActive()"
    )


def test_checkpoint_has_neither_filters_nor_quantities():
    solver = _solver()
    checkpoint = exahype2.api.fv.WriteCheckpoint(solver)
    assert not hasattr(checkpoint, "add_plot_filter")
    with pytest.raises(TypeError):
        exahype2.api.fv.WriteCheckpoint(solver, quantities=["rho"])


@pytest.mark.parametrize("scheme,halo", [("fv", 1), ("fd4", 2)])
def test_checkpoint_reconstructs_and_writes_patch_with_halo(tmp_path, scheme, halo):
    solver = _solver(scheme, dirichlet=True)
    solver.add_plotter(
        exahype2.api.fv.WriteCheckpoint(solver)
        if scheme == "fv"
        else exahype2.api.rkfd4.WriteCheckpoint(solver)
    )
    graph = _plot_graph(solver, tmp_path, periodic=False)
    names = sorted(task.name for task in graph.tasks)
    assert names == [
        "Probe_Apply Dirichlet BCs for WriteCheckpoint",
        "Probe_WriteCheckpoint_Reconstruct",
        "Probe_WriteCheckpoint_Write checkpoint",
    ]
    active = "repositories::instanceOfProbe.isWriteCheckpointActive()"
    for task in graph.tasks:
        for action_set in task.action_sets:
            for operation in [
                "touchCellFirstTime",
                "touchFaceFirstTime",
                "createHangingFace",
            ]:
                guard = action_set.get_guard_of_operation(operation)
                assert guard == "true" or active in guard, (task.name, operation, guard)

    (write,) = [t for t in graph.tasks if t.name.endswith("Write checkpoint")]
    (action_set,) = write.action_sets
    assert action_set.d["DOFS_PER_AXIS"] == str(9 + 2 * halo)
    assert action_set.d["UNKNOWNS"] == "4"
    assert action_set.d["SELECT_DOFS"] is None
    assert action_set.d["PATCH_SCALING"] == (9 + 2 * halo) / 9
    assert action_set.d["PRECISION"] == 17
    assert action_set.d["WRITER_PRECISION"] == 17
    assert action_set.d["METADATA_EVALUATION"] == (
        "repositories::instanceOfProbe.toCheckpointMetaData()"
    )
    assert "finishWriteCheckpoint();" in action_set.get_body_of_unprepareTraversal()
