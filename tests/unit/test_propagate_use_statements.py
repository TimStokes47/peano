import pytest

import peano4.api.solversteps
from peano4.api.taskgraph.compiler.PropagateUseStatements import propagate_use_statements


class _FakeSubmodel:
    """
    Minimal stand-in for a peano4.api.datamodel.Patch: propagate_use_statements()
    only ever needs submodel identity, never anything Patch-specific.
    """

    def __init__(self, name):
        self.name = name

    def __repr__(self):
        return self.name


def _make_steps(count):
    return [
        peano4.api.solversteps.Step(name=f"pytest-step-{i}", add_user_defined_actions=False)
        for i in range(count)
    ]


def _used_step_indices(steps, submodel, attribute_name="cell_data"):
    return [i for i, step in enumerate(steps) if submodel in getattr(step, attribute_name)]


def test_single_step_sequence_is_left_untouched():
    """
    A sequence of fewer than two steps has no "later"/"earlier" step to
    relay a submodel through, so propagate_use_statements() must be a no-op.
    """
    # Arrange
    steps = _make_steps(1)
    submodel = _FakeSubmodel("Q")
    steps[0].use_cell(submodel)

    # Act
    propagate_use_statements(steps)

    # Assert
    assert _used_step_indices(steps, submodel) == [0]


@pytest.mark.parametrize("initial_index", [0, 1, 2, 3])
def test_use_anywhere_in_a_multi_step_sequence_propagates_everywhere(initial_index):
    """
    Rule (b) - step N used implies N+1, N+3, N+5, ... used - reapplied to
    every step it just switched on, forces a submodel used at any single
    step to end up used at every later step; rule (a) - last used implies
    first used - then feeds that back to the front of the sequence. So for
    any sequence with at least two steps, a submodel used anywhere ends up
    used everywhere, regardless of where it started.
    """
    # Arrange
    steps = _make_steps(4)
    submodel = _FakeSubmodel("Q")
    steps[initial_index].use_cell(submodel)

    # Act
    propagate_use_statements(steps)

    # Assert
    assert _used_step_indices(steps, submodel) == list(range(4))


def test_propagation_does_not_add_duplicate_use_statements():
    """
    Step.use_cell()/use_face()/use_vertex() append without deduplicating, so
    propagate_use_statements() has to check what a step already declares
    before adding to it - otherwise a submodel that was already used at a
    step it also wants to switch on would end up listed twice, corrupting
    the generated Observer's signature.
    """
    # Arrange
    steps = _make_steps(3)
    submodel = _FakeSubmodel("Q")
    steps[0].use_cell(submodel)
    steps[1].use_cell(submodel)

    # Act
    propagate_use_statements(steps)

    # Assert
    for step in steps:
        assert step.cell_data.count(submodel) == 1


def test_propagation_is_idempotent():
    """
    Running propagate_use_statements() again once everything is already
    fully propagated must not change anything further.
    """
    # Arrange
    steps = _make_steps(4)
    submodel = _FakeSubmodel("Q")
    steps[2].use_cell(submodel)

    # Act
    propagate_use_statements(steps)
    state_after_first_run = [list(step.cell_data) for step in steps]
    propagate_use_statements(steps)
    state_after_second_run = [list(step.cell_data) for step in steps]

    # Assert
    assert state_after_first_run == state_after_second_run


def test_cell_face_and_vertex_data_are_propagated_independently():
    """
    A submodel declared via use_face() must only ever end up added back via
    use_face() (never leaking into cell_data or vertex_data), and likewise
    for use_cell()/use_vertex() - the three kinds of mesh data are entirely
    independent inputs to propagate_use_statements().
    """
    # Arrange
    steps = _make_steps(3)
    cell_submodel = _FakeSubmodel("Cell")
    face_submodel = _FakeSubmodel("Face")
    vertex_submodel = _FakeSubmodel("Vertex")
    steps[0].use_cell(cell_submodel)
    steps[0].use_face(face_submodel)
    steps[0].use_vertex(vertex_submodel)

    # Act
    propagate_use_statements(steps)

    # Assert
    for step in steps:
        assert step.cell_data == [cell_submodel]
        assert step.face_data == [face_submodel]
        assert step.vertex_data == [vertex_submodel]


def test_distinct_submodels_of_the_same_kind_are_propagated_independently():
    """
    Two distinct cell submodels, each used at a different step, must each
    independently propagate to every step without contaminating one
    another's use statements.
    """
    # Arrange
    steps = _make_steps(4)
    submodel_a = _FakeSubmodel("A")
    submodel_b = _FakeSubmodel("B")
    steps[0].use_cell(submodel_a)
    steps[3].use_cell(submodel_b)

    # Act
    propagate_use_statements(steps)

    # Assert
    for step in steps:
        assert submodel_a in step.cell_data
        assert submodel_b in step.cell_data
        assert len(step.cell_data) == 2
