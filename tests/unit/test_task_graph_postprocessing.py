import types

import peano4.api
import peano4.api.taskgraph
import peano4.api.solversteps

import exahype2.api
import toolbox.blockstructured.api

Task = peano4.api.taskgraph.Task


def _cell_task(name):
    return Task(name=name, entity=Task.GridEntity.CELL)


def test_insert_after_redirects_successors():
    graph = peano4.api.taskgraph.Graph()
    a, b, c, d = (_cell_task(name) for name in "abcd")
    b.add_dependency(a)
    c.add_dependency(a)
    c.add_dependency(b)
    for task in [a, b, c]:
        graph.add_task(task)

    graph.insert_after(a, d)

    assert d in graph.tasks
    assert d.dependencies == [a]
    assert b.dependencies == [d]
    assert c.dependencies == [d, b]
    order = graph.ordered_tasks()
    assert order.index(a) < order.index(d) < order.index(b) < order.index(c)


def test_insert_after_last_task():
    graph = peano4.api.taskgraph.Graph()
    a, b = _cell_task("a"), _cell_task("b")
    graph.add_task(a)
    graph.insert_after(a, b)
    assert graph.ordered_tasks() == [a, b]


def test_find_tasks_by_tag():
    graph = peano4.api.taskgraph.Graph()
    a = Task(name="a", entity=Task.GridEntity.CELL, tags={"x"})
    b = _cell_task("b")
    c = _cell_task("c")
    c.tags.add("x")
    for task in [a, b, c]:
        graph.add_task(task)
    assert graph.find_tasks("x") == [a, c]
    assert graph.find_tasks("y") == []


def test_find_tasks_with_several_tags():
    graph = peano4.api.taskgraph.Graph()
    a = Task(name="a", entity=Task.GridEntity.CELL, tags=["x", "y"])
    b = Task(name="b", entity=Task.GridEntity.CELL, tags={"y"})
    c = _cell_task("c")
    for task in [a, b, c]:
        graph.add_task(task)
    assert graph.find_tasks("x") == [a]
    assert graph.find_tasks("y") == [a, b]
    assert graph.find_tasks("x", "y") == [a, b]


def _postprocessed_time_step(scheme_factory, tmp_path):
    """! Time step graph of a scheme which enforces the CCZ4 constraints"""
    solver = exahype2.api.Solver("Probe", 2, 1)
    solver.add_pde_term(exahype2.api.Flux(implementation="F[0] = Q[0]; F[1] = Q[1];"))
    solver.add_pde_term(exahype2.api.MaxEigenvalue(implementation="*lambdaMax = 1.0;"))
    solver.numerical_scheme = scheme_factory(solver)

    exahype2_project = types.SimpleNamespace(periodic_BC=[True, True, True])
    peano_project = peano4.api.Project(["probe"], "probe", str(tmp_path))
    time_step = peano4.api.taskgraph.Graph()
    solver.numerical_scheme.construct_output(exahype2_project, peano_project, time_step)
    solver.postprocess_task_graphs(
        exahype2_project, peano_project, None, time_step, None
    )
    return solver, {task.name: task for task in time_step.tasks}


def test_postprocessing_after_each_runge_kutta_stage(tmp_path):
    solver, tasks = _postprocessed_time_step(
        lambda solver: exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
            solver, patch_size=4, rk_order=3, enforce_ccz4_constraints=True
        ),
        tmp_path,
    )
    for anchor, successor, patch in [
        ("Prepare stage 1", "Project stage 1 onto faces", solver._stage_input),
        ("Prepare stage 2", "Project stage 2 onto faces", solver._stage_input),
        ("Complete time step", "Project solution onto faces", solver._patch),
    ]:
        clip = tasks[f"Probe_Enforce CCZ4 constraints after {anchor}"]
        assert clip.dependencies == [tasks[f"Probe_{anchor}"]]
        assert tasks[f"Probe_{successor}"].dependencies == [clip]
        [action_set] = clip.action_sets
        assert isinstance(
            action_set, toolbox.blockstructured.api.ApplyPointWiselyToPatch
        )
        body = action_set.get_body_of_operation(
            peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
        )
        assert "::exagrype::ccz4::enforceCCZ4constraints(Q);" in body
        assert f"fineGridCell{patch.name}.value" in body


def test_postprocessing_after_both_writers_of_optimistic_finite_volumes(tmp_path):
    solver, tasks = _postprocessed_time_step(
        lambda solver: exahype2.api.fv.RusanovWithOptimisticExplicitEuler(
            solver, 4, enforce_ccz4_constraints=True
        ),
        tmp_path,
    )
    clips = sorted(name for name in tasks if "_Enforce CCZ4 constraints after" in name)
    assert clips == [
        "Probe_Enforce CCZ4 constraints after Insert patch interior",
        "Probe_Enforce CCZ4 constraints after Update cell",
    ]
    update_clip = tasks["Probe_Enforce CCZ4 constraints after Update cell"]
    insert_clip = tasks["Probe_Enforce CCZ4 constraints after Insert patch interior"]
    # The guards read the eigenvalue data, so the clips have to declare it
    for clip in [update_clip, insert_clip]:
        assert solver._max_eigenvalue_and_time_step_sizes in clip.cell_data
    for consumer in [
        "Project on faces",
        "Compute global CFL condition",
        "Spawn patch interior update",
    ]:
        dependencies = tasks[f"Probe_{consumer}"].dependencies
        assert update_clip in dependencies and insert_clip in dependencies
        assert tasks["Probe_Update cell"] not in dependencies
        assert tasks["Probe_Insert patch interior"] not in dependencies


def test_postprocessing_after_plain_finite_volumes(tmp_path):
    solver, tasks = _postprocessed_time_step(
        lambda solver: exahype2.api.fv.RusanovWithExplicitEuler(
            solver, 4, enforce_ccz4_constraints=True
        ),
        tmp_path,
    )
    clip = tasks["Probe_Enforce CCZ4 constraints after Update cell"]
    assert tasks["Probe_Project on faces"].dependencies == [clip]
