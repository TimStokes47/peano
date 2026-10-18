from peano4.api.solversteps import SynchroniseActionSets, get_task_groups


class _FakeActionSet:
    def __init__(self, name, parallel):
        self.name = name
        self.parallel = parallel


def _names(groups):
    return [[action_set.name for action_set in group] for group in groups]


def test_runs_of_parallel_action_sets_form_groups():
    v = [_FakeActionSet(f"v{i}", parallel) for i, parallel in enumerate([True, True, False, True, True], start=1)]
    assert _names(get_task_groups(v)) == [["v1", "v2"], ["v3"], ["v4", "v5"]]


def test_sequential_action_sets_form_groups_of_their_own():
    v = [_FakeActionSet(f"v{i}", False) for i in range(1, 4)]
    assert _names(get_task_groups(v)) == [["v1"], ["v2"], ["v3"]]


def test_marker_separates_groups_and_is_not_part_of_any_group():
    v1, v2, v3, v4 = (_FakeActionSet(f"v{i}", True) for i in range(1, 5))
    assert _names(get_task_groups([v1, v2, SynchroniseActionSets(), v3, v4])) == [["v1", "v2"], ["v3", "v4"]]


def test_parallel_action_set_after_sequential_one_starts_a_new_group():
    v1, v2, v3 = _FakeActionSet("v1", False), _FakeActionSet("v2", True), _FakeActionSet("v3", True)
    assert _names(get_task_groups([v1, v2, v3])) == [["v1"], ["v2", "v3"]]
