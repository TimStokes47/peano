"""Unit tests for hosts of coupled solvers (@ref exahype2.api.SolverHost) and for the
flag add_global_tasks, with which a host leaves the load balancing switch and the
dynamic mesh refinement to one of its solvers."""

import types

import pytest

import peano4.api
import peano4.api.taskgraph
import exahype2.api
import toolbox.loadbalancing.api.actionsets

Task = peano4.api.taskgraph.Task

SCHEMES = {
    "fv": lambda solver: exahype2.api.fv.RusanovWithExplicitEuler(solver, 4),
    "fv-optimistic": lambda solver: exahype2.api.fv.RusanovWithOptimisticExplicitEuler(
        solver, 4
    ),
    "rkfd4": lambda solver: exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
        solver, patch_size=4, rk_order=2
    ),
}


def _solver(scheme, name="Probe"):
    solver = exahype2.api.Solver(name, 2)
    solver.add_pde_term(exahype2.api.Flux(implementation="F[0] = Q[0]; F[1] = Q[1];"))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation="*lambdaMax = 1.0;"))
    solver.add_pde_term(exahype2.api.RefineErasePointwisely(implementation=""))
    solver.add_pde_term(
        exahype2.api.InitPointWisely(implementation="Q[0] = 0.0; Q[1] = 0.0;")
    )
    solver.numerical_scheme = SCHEMES[scheme](solver)
    return solver


def _construct(solver, tmp_path, add_global_tasks):
    exahype2_project = types.SimpleNamespace(periodic_BC=[True, True, True])
    peano_project = peano4.api.Project(["probe"], "probe", str(tmp_path))
    time_step = peano4.api.taskgraph.Graph()
    solver.numerical_scheme.construct_output(
        exahype2_project, peano_project, time_step, add_global_tasks
    )
    return time_step


def _toggles_load_balancing(task):
    return any(
        isinstance(action_set, toolbox.loadbalancing.api.actionsets.ToggleLoadBalancing)
        or isinstance(action_set, toolbox.loadbalancing.api.actionsets.LoadBalancing)
        for action_set in task.action_sets
    )


def test_graph_append_unites_graphs_and_keeps_the_dependencies():
    first, second, merged = (peano4.api.taskgraph.Graph() for _ in range(3))
    a = Task(name="a", entity=Task.GridEntity.CELL)
    b = Task(name="b", entity=Task.GridEntity.CELL)
    c = Task(name="c", entity=Task.GridEntity.CELL)
    b.add_dependency(a)
    for task in (a, b):
        first.add_task(task)
    second.add_task(c)
    merged.append(first)
    merged.append(second)
    merged.append(first)
    assert merged.tasks == [a, b, c]
    assert b.dependencies == [a]
    assert first.tasks == [a, b]


@pytest.mark.parametrize("scheme", SCHEMES)
def test_schemes_add_the_global_tasks_by_default(scheme, tmp_path):
    name = exahype2.api.Solver.NumericalScheme.DYNAMIC_MESH_REFINEMENT_TASK_NAME
    graph = _construct(_solver(scheme), tmp_path, True)
    assert f"Probe_{name}" in [task.name for task in graph.tasks]
    assert any(_toggles_load_balancing(task) for task in graph.tasks)


@pytest.mark.parametrize("scheme", SCHEMES)
def test_schemes_leave_the_global_tasks_to_the_host_on_request(scheme, tmp_path):
    graph = _construct(_solver(scheme), tmp_path, False)
    assert not any("Dynamic mesh refinement" in task.name for task in graph.tasks)
    assert not any("Disable load balancing" in task.name for task in graph.tasks)
    assert not any(_toggles_load_balancing(task) for task in graph.tasks)
    # The solver proper is intact
    assert any("Reconstruct" in task.name for task in graph.tasks)


def test_solver_without_global_tasks_does_not_contribute_to_the_mesh_adaptation(
    tmp_path,
):
    solver = _solver("fv")
    exahype2_project = exahype2.api.Project(
        ["probe"], "probe", "probe", directory=str(tmp_path)
    )
    exahype2_project.set_global_simulation_parameters(
        dimensions=2,
        offset=[0.0, 0.0],
        size=[1.0, 1.0],
        min_end_time=1.0,
        periodic_BC=[True, True],
    )
    solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
        solver, volume_size=0.1
    )
    peano_project = peano4.api.Project(["probe"], "probe", str(tmp_path))

    def realise(add_global_tasks):
        graphs = [peano4.api.taskgraph.Graph() for _ in range(4)]
        solver.generate_solver_realisation(
            exahype2_project, peano_project, *graphs, add_global_tasks=add_global_tasks
        )
        return graphs

    assert len(realise(True)[3].tasks) > 0
    assert len(realise(False)[3].tasks) == 0


class _Chain(exahype2.api.SolverHost):
    """! Host which makes the first task of the second solver wait for the last of the first"""

    def couple(self, exahype2_project, peano_project, own_graphs, host_graphs):
        first, second = self.solvers
        last = own_graphs[first].time_step.sink_tasks()
        for task in own_graphs[second].time_step.tasks:
            if not task.dependencies:
                for predecessor in last:
                    task.add_dependency(predecessor)


def test_host_realises_each_solver_into_graphs_of_its_own_and_concatenates(tmp_path):
    first, second = _solver("fv", "First"), _solver("fv", "Second")
    host = _Chain("Pair", [first, second])
    assert host.global_tasks_solver is first
    for solver in (first, second):
        solver.grid_construction = exahype2.api.fv.SetupSimulationOverRegularGrid(
            solver, volume_size=0.1
        )
    exahype2_project = exahype2.api.Project(
        ["probe"], "probe", "probe", directory=str(tmp_path)
    )
    exahype2_project.set_global_simulation_parameters(
        dimensions=2,
        offset=[0.0, 0.0],
        size=[1.0, 1.0],
        min_end_time=1.0,
        periodic_BC=[True, True],
    )
    peano_project = peano4.api.Project(["probe"], "probe", str(tmp_path))
    graphs = [peano4.api.taskgraph.Graph() for _ in range(4)]
    host.generate_solver_realisation(exahype2_project, peano_project, *graphs)

    names = [task.name for task in graphs[1].tasks]
    assert [n for n in names if n.startswith("First_")] and [
        n for n in names if n.startswith("Second_")
    ]
    # Only the first solver adds the global tasks
    assert [n for n in names if "Dynamic mesh refinement" in n] == [
        "First_Dynamic mesh refinement and load balancing"
    ]
    # The coupling of the host connects the otherwise independent chains
    second_reconstruct = next(
        t for t in graphs[1].tasks if t.name == "Second_Reconstruct"
    )
    assert second_reconstruct.dependencies


def test_project_registers_the_solvers_of_a_host_but_realises_the_host(tmp_path):
    first, second = _solver("fv", "First"), _solver("fv", "Second")
    third = _solver("fv", "Third")
    host = _Chain("Pair", [first, second])
    project = exahype2.api.Project(["probe"], "probe", "probe", directory=str(tmp_path))
    project.add_solver(third)
    project.add_solver(host)
    assert project._solvers == [third, first, second]
    assert project._realisations == [third, host]


def test_a_host_needs_solvers():
    with pytest.raises(ValueError):
        _Chain("Empty", [])
