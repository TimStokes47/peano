"""Unit tests for the setup with user-coded refinement criterion
(@ref exahype2.api.fv.SetupSimulationWithUserCodedRefinementCriterion)."""

import types

import peano4.api
import peano4.api.taskgraph
import exahype2.api
import pytest

SetupOverRegularGrid = exahype2.api.fv.SetupSimulationOverRegularGrid
SetupWithCriterion = exahype2.api.fv.SetupSimulationWithUserCodedRefinementCriterion


def _solver(with_criterion):
    solver = exahype2.api.Solver("Probe", 4, 0)
    solver.add_pde_term(exahype2.api.Flux(implementation=""))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation=""))
    solver.add_pde_term(exahype2.api.InitPointWisely(implementation=""))
    if with_criterion:
        solver.add_pde_term(
            exahype2.api.RefineErasePointwisely(
                implementation="if (Q[0] > 0.8) *refinementCommand = ::exahype2::RefinementCommand::Refine;"
            )
        )
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
    solver.grid_construction = SetupWithCriterion(
        solver, volume_size=0.02, min_volume_size=0.002
    )
    return solver


def _setup_graph(solver, tmp_path):
    peano_project = peano4.api.Project(["probe"], "probe", directory=str(tmp_path))
    exahype2_project = types.SimpleNamespace(periodic_BC=[True, True, True])
    graph = peano4.api.taskgraph.Graph()
    solver.grid_construction.construct_output(exahype2_project, peano_project, graph)
    return graph


def _task(graph, suffix):
    matches = [task for task in graph.tasks if task.name.endswith(suffix)]
    assert len(matches) == 1, [task.name for task in graph.tasks]
    return matches[0]


def test_regular_grid_tags_initial_condition(tmp_path):
    graph = _setup_graph(_solver(with_criterion=False), tmp_path)
    tagged = graph.find_tasks(SetupOverRegularGrid.FinishedInitialRegularMeshSetup)
    assert [task.name for task in tagged] == ["Probe Initial condition"]


def test_without_criterion_setup_equals_regular_grid(tmp_path):
    graph = _setup_graph(_solver(with_criterion=False), tmp_path)
    assert not any(task.name.endswith("Adapt mesh") for task in graph.tasks)
    assert _task(graph, "Project on faces").dependencies == [
        _task(graph, "Initial condition")
    ]


def test_adapt_mesh_task_sits_between_initial_condition_and_projection(tmp_path):
    graph = _setup_graph(_solver(with_criterion=True), tmp_path)
    adapt = _task(graph, "Adapt mesh")
    assert adapt.dependencies == [_task(graph, "Initial condition")]
    assert _task(graph, "Project on faces").dependencies == [adapt]
    assert adapt.has_to_repeat == SetupWithCriterion.RepeatCondition
    assert "isGridStationary" in adapt.has_to_repeat
    assert "RefinementControlService" in adapt.has_to_repeat
    # The projection is the very last task
    assert not any(
        _task(graph, "Project on faces") in task.dependencies for task in graph.tasks
    )


def test_adapt_mesh_task_hosts_criterion_and_initialisation(tmp_path):
    graph = _setup_graph(_solver(with_criterion=True), tmp_path)
    action_sets = _task(graph, "Adapt mesh").action_sets
    assert [type(action_set) for action_set in action_sets] == [
        exahype2.api.blockstructured.DynamicMeshRefinement,
        exahype2.api.fv.actionsets.InitialiseNewCells,
    ]
    criterion, initialisation = action_sets
    ActionSet = peano4.api.solversteps.ActionSet
    body = criterion.get_body_of_operation(ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME)
    assert "refineErasePointwisely(" in body
    assert (
        "::exahype2::RefinementCommand command = ::exahype2::RefinementCommand::Keep;"
        in body
    )
    # Volumes of 0.002 within patches of 9 volumes, regular mesh volumes of 0.02
    assert ">= 0.018" in body
    assert "< 0.18" in body
    assert "finishStep()" in criterion.get_body_of_unprepareTraversal()
    assert (
        "createGridControlEventsForNextTraversal()"
        in criterion.get_body_of_createGridControlEventsForNextTraversal()
    )
    create_cell = initialisation.get_body_of_operation(ActionSet.OPERATION_CREATE_CELL)
    assert "initPointWisely(" in create_cell
    assert (
        initialisation.get_body_of_operation(ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME)
        == ""
    )


def test_min_volume_size_must_not_exceed_volume_size():
    solver = _solver(with_criterion=True)
    with pytest.raises(AssertionError):
        SetupWithCriterion(solver, volume_size=0.002, min_volume_size=0.02)


def test_adapt_mesh_task_evaluates_gradient_criterion(tmp_path):
    solver = exahype2.api.Solver("Probe", 4, 0)
    solver.add_pde_term(exahype2.api.Flux(implementation=""))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation=""))
    solver.add_pde_term(exahype2.api.InitPointWisely(implementation=""))
    solver.add_pde_term(exahype2.api.GradientRefinement(implementation=""))
    solver.numerical_scheme = exahype2.api.fv.RusanovWithExplicitEuler(solver, 9)
    solver.grid_construction = SetupWithCriterion(
        solver, volume_size=0.02, min_volume_size=0.002
    )
    criterion, _ = _task(_setup_graph(solver, tmp_path), "Adapt mesh").action_sets
    assert isinstance(
        criterion, exahype2.api.blockstructured.DynamicMeshRefinementWithGradients
    )
    assert (
        "createGridControlEventsForNextTraversal()"
        in criterion.get_body_of_createGridControlEventsForNextTraversal()
    )
