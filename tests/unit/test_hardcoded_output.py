import pytest

from toolbox.loadbalancing.api.input.RegularGridGenerator import RegularGridGenerator
import toolbox.loadbalancing.api.output.Hardcoded as Hardcoded
import toolbox.loadbalancing.api.strategies.Decomposition as Decomposition
import toolbox.loadbalancing.api.strategies.SplitTrees as split_strategy


def _split_regular_grid(number_of_subtrees, periodic_BC=None):
    """Split the 27x27 mesh of tutorial 35 into number_of_subtrees subtrees"""
    tree = RegularGridGenerator(name="pytest-hardcoded", max_level=3, dimensions=2)
    split_strategy.split_tree(
        tree,
        decomposition=Decomposition.equidistant_splitting(
            peano_tree=tree, number_of_subtrees=number_of_subtrees
        ),
        periodic_BC=periodic_BC,
    )
    return tree


def _cells_per_subtree(tree, number_of_subtrees):
    counts = [0] * number_of_subtrees
    for a in range(27):
        for b in range(27):
            weight, subtree = tree._tree_arrays[2][a][b]
            counts[subtree] += weight
    return counts


# ---------------------------------------------------------------------------
# Periodic boundaries
# ---------------------------------------------------------------------------


def test_periodic_frame_stays_on_subtree_zero():
    """All cells along a periodic boundary remain on subtree 0, and the shares still add up."""
    tree = _split_regular_grid(4, periodic_BC=[True, True])
    for a in range(27):
        for b in range(27):
            if min(a, b) == 0 or max(a, b) == 26:
                assert tree._tree_arrays[2][a][b][1] == 0, (a, b)
    assert _cells_per_subtree(tree, 4) == [183, 182, 182, 182]


def test_periodic_frame_only_along_periodic_axes():
    """Only the faces of periodic axes are kept on subtree 0."""
    tree = _split_regular_grid(4, periodic_BC=[True, False])
    owners_along_x_faces = {
        tree._tree_arrays[2][a][b][1] for a in (0, 26) for b in range(27)
    }
    owners_along_y_faces = {
        tree._tree_arrays[2][a][b][1] for a in range(27) for b in (0, 26)
    }
    assert owners_along_x_faces == {0}
    assert owners_along_y_faces != {0}


def test_no_periodic_boundaries_keeps_plain_split():
    """periodic_BC=None and all-False yield the same decomposition."""
    plain = _split_regular_grid(4)
    non_periodic = _split_regular_grid(4, periodic_BC=[False, False])
    assert (plain._tree_arrays[2] == non_periodic._tree_arrays[2]).all()


# ---------------------------------------------------------------------------
# Split mode markers
# ---------------------------------------------------------------------------


def test_single_marker_holds_for_all_splits():
    tree = _split_regular_grid(4)
    configuration, mode = Hardcoded.create_constructor_initialisation_string(
        tree, [0, 0, 0], modes=Hardcoded.BottomUp
    )
    assert mode == "peano4::SplitInstruction::Mode::BottomUp"
    assert (
        configuration
        == "{{0, 0, 0}, {182, 182, 182}, {0, 0, 0}, peano4::SplitInstruction::Mode::BottomUp}"
    )


def test_one_marker_per_split():
    tree = _split_regular_grid(4)
    configuration, mode = Hardcoded.create_constructor_initialisation_string(
        tree,
        [0, 0, 0],
        modes=[Hardcoded.TopDown, Hardcoded.TopDown, Hardcoded.BottomUp],
    )
    assert mode == (
        "{peano4::SplitInstruction::Mode::AggressiveTopDown, "
        "peano4::SplitInstruction::Mode::AggressiveTopDown, "
        "peano4::SplitInstruction::Mode::BottomUp}"
    )
    assert configuration.endswith(mode + "}")


@pytest.mark.parametrize("modes", [[Hardcoded.BottomUp], ["sideways"] * 3])
def test_invalid_markers_are_rejected(modes):
    tree = _split_regular_grid(4)
    with pytest.raises(ValueError):
        Hardcoded.create_constructor_initialisation_string(tree, [0, 0, 0], modes=modes)


# ---------------------------------------------------------------------------
# Round-robin assignment of subtrees to ranks
# ---------------------------------------------------------------------------


@pytest.mark.parametrize("number_of_ranks", [1, 2, 3, 8])
def test_round_robin_assignment_is_accepted(number_of_ranks):
    tree = _split_regular_grid(8)
    destination_ranks = Hardcoded.assign_trees_to_ranks_round_Robin(
        number_of_subtrees=8, max_ranks=number_of_ranks
    )
    Hardcoded.create_constructor_initialisation_string(
        tree, destination_ranks, number_of_ranks=number_of_ranks
    )


def test_other_assignments_are_rejected():
    """Subtree s has to live on rank s%R, as Peano numbers its trees round robin."""
    tree = _split_regular_grid(8)
    with pytest.raises(ValueError):
        Hardcoded.create_constructor_initialisation_string(tree, [1, 1, 1, 2, 2, 2, 0])


def test_number_of_ranks_is_taken_into_account():
    """Round robin over three ranks is no round robin over four ranks."""
    tree = _split_regular_grid(8)
    destination_ranks = Hardcoded.assign_trees_to_ranks_round_Robin(
        number_of_subtrees=8, max_ranks=3
    )
    with pytest.raises(ValueError):
        Hardcoded.create_constructor_initialisation_string(
            tree, destination_ranks, number_of_ranks=4
        )
