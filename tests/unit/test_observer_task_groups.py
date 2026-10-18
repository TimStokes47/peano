import re

import pytest

import peano4.api.output


def _generate_observer(tmp_path, task_groups):
    tmp_path.mkdir(parents=True, exist_ok=True)
    observer = peano4.api.output.Observer(
        "MyObserver",
        ["project", "observers"],
        "observers",
        ["project::observers::A", "project::observers::B", "project::observers::C"],
        [],
        [],
        [],
        task_groups,
    )
    observer.generate(peano4.api.output.Overwrite.Always, str(tmp_path))
    return (tmp_path / "observers" / "MyObserver.cpp").read_text()


def _function(code, name):
    """Body of the function name in the generated code"""
    start = code.index(f"::{name}(")
    return code[start : code.index("\n}\n", start)]


def test_without_task_groups_observer_is_unchanged(tmp_path):
    default = _generate_observer(tmp_path / "default", None)
    singletons = _generate_observer(tmp_path / "singletons", [[0], [1], [2]])
    assert default == singletons
    assert "spawnAndWait" not in _function(default, "enterCell")


@pytest.mark.parametrize("function, order", [("enterCell", [0, 1, 2]), ("leaveCell", [2, 1, 0])])
def test_task_group_is_spawned_and_waited_for(tmp_path, function, order):
    body = _function(_generate_observer(tmp_path, [[0, 1], [2]]), function)
    assert body.count("tarch::multicore::spawnAndWait( parallelActionSets );") == 1
    assert "std::function<void()> invokeActionSet0 = [&]() -> void {" in body
    assert "std::function<void()> invokeActionSet1 = [&]() -> void {" in body
    assert "invokeActionSet2" not in body
    # Action sets are still invoked in order (reversed in leaveCell), and
    # action set 2 is invoked after the wait in enterCell, before the group
    # in leaveCell
    positions = [body.index(f"_actionSet{index}.") for index in order]
    assert positions == sorted(positions)
    wait = body.index("spawnAndWait")
    assert (body.index("_actionSet2.") > wait) == (function == "enterCell")


def test_task_groups_have_to_enumerate_all_action_sets_in_order(tmp_path):
    with pytest.raises(Exception):
        _generate_observer(tmp_path, [[1, 0], [2]])
