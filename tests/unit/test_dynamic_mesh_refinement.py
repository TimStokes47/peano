import types

import pytest

import peano4.api
import peano4.api.taskgraph
import peano4.api.solversteps

import exahype2.api
import toolbox.loadbalancing.api.actionsets

Task = peano4.api.taskgraph.Task
ActionSet = peano4.api.solversteps.ActionSet

SCHEMES = {
    "fv": lambda solver: exahype2.api.fv.RusanovWithExplicitEuler(solver, 4),
    "fv-optimistic": lambda solver: exahype2.api.fv.RusanovWithOptimisticExplicitEuler(
        solver, 4
    ),
    "rkfd4": lambda solver: exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
        solver, patch_size=4, rk_order=2
    ),
}


class _MeshAdaptation(exahype2.api.Solver.MeshAdaptation):
    def construct_output(self, exahype2_project, peano_project, mesh_adaptation):
        pass


def _solver(scheme, criterion=True, mesh_adaptation=None):
    solver = exahype2.api.Solver("Probe", 2, 0)
    solver.add_pde_term(exahype2.api.Flux(implementation="F[0] = Q[0]; F[1] = Q[1];"))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation="*lambdaMax = 1.0;"))
    if criterion:
        solver.add_pde_term(exahype2.api.RefineErasePointwisely(implementation=""))
    if mesh_adaptation is not None:
        solver.mesh_adaptation = mesh_adaptation(solver)
    solver.numerical_scheme = SCHEMES[scheme](solver)
    return solver


def _last_task(solver, tmp_path):
    """! The one task of the time step on which no other task depends"""
    exahype2_project = types.SimpleNamespace(periodic_BC=[True, True, True])
    peano_project = peano4.api.Project(["probe"], "probe", str(tmp_path))
    time_step = peano4.api.taskgraph.Graph()
    solver.numerical_scheme.construct_output(exahype2_project, peano_project, time_step)
    dependencies = {
        id(dependency) for task in time_step.tasks for dependency in task.dependencies
    }
    [last] = [task for task in time_step.tasks if id(task) not in dependencies]
    return last


def _criterion_body(action_set):
    return action_set.get_body_of_operation(ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME)


@pytest.mark.parametrize("scheme", SCHEMES)
def test_last_task_evaluates_criterion_and_switches_on_load_balancing(scheme, tmp_path):
    last = _last_task(_solver(scheme), tmp_path)
    assert last.name == "Probe_Dynamic mesh refinement"
    assert last.entity == Task.GridEntity.CELL
    dynamic_mesh_refinement, load_balancing = last.action_sets
    assert isinstance(
        dynamic_mesh_refinement, exahype2.api.blockstructured.DynamicMeshRefinement
    )
    assert isinstance(
        load_balancing, toolbox.loadbalancing.api.actionsets.LoadBalancing
    )
    body = load_balancing.get_body_of_unprepareTraversal()
    assert body.index("enable(true)") < body.index("finishStep()")


@pytest.mark.parametrize("scheme", SCHEMES)
def test_time_step_never_hands_events_to_peano(scheme, tmp_path):
    [dynamic_mesh_refinement, _] = _last_task(_solver(scheme), tmp_path).action_sets
    assert "RefinementControlService" not in (
        dynamic_mesh_refinement.get_body_of_createGridControlEventsForNextTraversal()
    )
    assert "refineErasePointwisely" in _criterion_body(dynamic_mesh_refinement)
    assert "RefinementControlService::getInstance().finishStep()" in (
        dynamic_mesh_refinement.get_body_of_unprepareTraversal()
    )


def test_no_criterion_means_no_evaluation(tmp_path):
    [dynamic_mesh_refinement, _] = _last_task(
        _solver("fv", criterion=False), tmp_path
    ).action_sets
    assert _criterion_body(dynamic_mesh_refinement) == ""
    assert dynamic_mesh_refinement.get_body_of_unprepareTraversal().strip() == ""


def test_mesh_size_is_unconstrained_by_default(tmp_path):
    [dynamic_mesh_refinement, _] = _last_task(_solver("fv"), tmp_path).action_sets
    body = _criterion_body(dynamic_mesh_refinement)
    assert "tarch::la::max( marker.h() ) >=" not in body
    assert "3.0 * tarch::la::max( marker.h() ) <" not in body


def test_mesh_adaptation_bounds_mesh_size(tmp_path):
    solver = _solver(
        "fv",
        mesh_adaptation=lambda solver: _MeshAdaptation(
            solver, min_volume_size=0.25, max_volume_size=1.0
        ),
    )
    [dynamic_mesh_refinement, _] = _last_task(solver, tmp_path).action_sets
    body = _criterion_body(dynamic_mesh_refinement)
    # Volume sizes times patch size 4
    assert "tarch::la::max( marker.h() ) >= 1.0" in body
    assert "3.0 * tarch::la::max( marker.h() ) < 4.0" in body


def test_mesh_adaptation_rejects_inverted_bounds():
    solver = exahype2.api.Solver("Probe", 2, 0)
    with pytest.raises(AssertionError):
        _MeshAdaptation(solver, min_volume_size=1.0, max_volume_size=0.5)
