# This file is part of the Peano load balancing toolbox. For conditions of
# distribution and use, please see the copyright notice at www.peano-framework.org
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from toolbox.loadbalancing.api.Tree import Tree


TopDown = "top-down"
BottomUp = "bottom-up"

_CppModes = {
    TopDown: "peano4::SplitInstruction::Mode::AggressiveTopDown",
    BottomUp: "peano4::SplitInstruction::Mode::BottomUp",
}


def assign_trees_to_ranks_round_Robin(
    number_of_subtrees: int,
    max_ranks: int,
):
    """!

    Take the trees and assign them to the ranks in a round-robin fashion

    This is likely not the best option in many cases. In many cases, it
    might be better to assign the trees in chunks to preserve some
    locality.

    Note that we implicitly assume that the first tree remains on rank 0,
    so we really only assign number_of_subtrees-1 entries.

    """
    result = []
    for i in range(1, number_of_subtrees):
        result.append(i % max_ranks)
    return result


def _format_initialisation_lists(
    parent_trees,
    subpartition_sizes,
    destination_ranks,
):
    """!
    Turn the enumerated splits into the C++ initialiser lists of Hardcoded

    Entry k of each list describes the kth split.

    @param parent_trees: [int]
      Tree which splits in the kth split.

    @param subpartition_sizes: [int]
      Number of cells that the kth split hands over.

    @param destination_ranks: [int]
      Rank which receives the new tree of the kth split.
    """
    assert len(parent_trees) == len(subpartition_sizes)
    assert len(destination_ranks) == len(subpartition_sizes)

    def initialiser_list(values):
        return "{" + ", ".join(str(value) for value in values) + "}"

    return (
        initialiser_list(parent_trees)
        + ", "
        + initialiser_list(subpartition_sizes)
        + ", "
        + initialiser_list(destination_ranks)
    )


def _markers_from_flags(bottom_up_flags):
    """!
    Translate the per-split flags of extract_split_instructions() into markers

    @param bottom_up_flags: [bool]
      True means that the corresponding split is a bottom-up split.
    """
    return [BottomUp if bottom_up else TopDown for bottom_up in bottom_up_flags]


def _normalise_markers(modes, number_of_splits):
    """!
    Turn the user's modes argument into one marker per split

    @param modes: str or [str]
      Either a single marker which holds for all splits, or a sequence with
      one marker per split. Each marker is TopDown ("top-down") or BottomUp
      ("bottom-up").

    @param number_of_splits: int
      Number of splits that the markers have to describe.
    """
    if isinstance(modes, str):
        modes = [modes] * number_of_splits

    modes = list(modes)
    if len(modes) != number_of_splits:
        raise ValueError(
            "got {} split mode(s), but the decomposition requires {} split(s)".format(
                len(modes), number_of_splits
            )
        )
    for mode in modes:
        if mode not in _CppModes:
            raise ValueError(
                "unknown split mode '{}'. Use '{}' or '{}'".format(
                    mode, TopDown, BottomUp
                )
            )
    return modes


def _format_modes(markers):
    """!
    Turn the markers into the C++ mode argument of Hardcoded

    The result is a single mode if all splits share the same mode, and an
    initialiser list with one mode per split otherwise.

    @param markers: [str]
      One marker per split.
    """
    if not markers:
        return _CppModes[TopDown]
    if len(set(markers)) == 1:
        return _CppModes[markers[0]]
    return "{" + ", ".join(_CppModes[marker] for marker in markers) + "}"


def _normalise_destination_ranks(destination_ranks, subtrees):
    """!
    Normalise destination rank input to one rank per split instruction.

    Accepted input formats:
    1. Per-instruction ranks: len(destination_ranks) == len(subtrees)
    2. Per-subtree ranks (excluding subtree 0): len(destination_ranks) == max(subtrees)
       The rank for subtree s>0 is taken from destination_ranks[s-1] and repeated
       for every occurrence of s in subtrees.
    """
    if len(destination_ranks) == len(subtrees):
        return destination_ranks

    max_subtree_id = max(subtrees, default=0)
    if len(destination_ranks) == max_subtree_id:
        return [destination_ranks[subtree_id - 1] for subtree_id in subtrees]

    raise ValueError(
        "destination_ranks ({}) must either match the number of split instructions ({}) "
        "or provide one rank per subtree excluding subtree 0 ({})".format(
            len(destination_ranks), len(subtrees), max_subtree_id
        )
    )


def _validate_round_robin(subtrees, destination_ranks, number_of_ranks=None):
    """!
    Ensure that the subtrees are assigned to the ranks round robin

    Peano's tree numbers encode the rank which hosts a tree:
    peano4::parallel::Node::getId() gives the nth tree on rank r the number
    n*R+r for R ranks, and peano4::parallel::Node::getRank() hence maps tree
    number t onto rank t%R. The offline plan refers to the trees through its
    subtree numbers, i.e. a split of subtree s is meant to split the tree
    with number s in Peano. This only holds if subtree s lives on rank s%R,
    which is exactly what assign_trees_to_ranks_round_Robin() yields.

    @param subtrees: [int]
      Subtree created by the kth split.

    @param destination_ranks: [int]
      Rank which receives the subtree of the kth split.

    @param number_of_ranks: int or None
      Number of ranks R. None derives R from the largest destination rank.
    """
    if number_of_ranks is None:
        number_of_ranks = max(destination_ranks, default=0) + 1
    for subtree, rank in zip(subtrees, destination_ranks):
        if rank != subtree % number_of_ranks:
            raise ValueError(
                "subtree {} is assigned to rank {}, but Peano can only host a tree with number {} on rank {} "
                "for {} rank(s). Assign the trees to the ranks round robin, e.g. through "
                "assign_trees_to_ranks_round_Robin()".format(
                    subtree, rank, subtree, subtree % number_of_ranks, number_of_ranks
                )
            )


def create_constructor_initialisation_string(
    peano_tree: "Tree",
    destination_ranks: list[int],
    verbose: bool = False,
    modes=None,
    number_of_ranks: int | None = None,
) -> tuple[str, str]:
    """!
    Construct the constructor arguments for an instance of Hardcoded

    The routine extracts the enumerated splits from the split tree, i.e.
    a sequence of splits where the kth split reads "tree t hands n cells
    over to a new tree on rank r and uses mode m to select these cells".
    toolbox::loadbalancing::strategies::Hardcoded decides itself when it
    issues the kth split.

    By default, the mode of each split is inferred (see image below):
    - `AggressiveTopDown`: all subtrees assigned to rank k start on the same level l.
    - `BottomUp`: subtrees assigned to rank k start on different levels.

    @image html src/toolbox/loadbalancing/api/output/create_constructor_initialisation_string.png

    You can overwrite the inferred modes through modes.

    @param peano_tree: toolbox.loadbalancing.api.Tree.Tree
      Tree after the split, i.e. after SplitTrees.split_tree().

    @param destination_ranks: [int]
      Either one rank per split or one rank per subtree excluding subtree 0.

    @param verbose: bool
      Print the extracted splits.

    @param modes: None, str or [str]
      None infers the mode of each split. Otherwise, pass a single marker
      TopDown ("top-down") or BottomUp ("bottom-up") which holds for all
      splits, or a sequence with one marker per split.

    @param number_of_ranks: int or None
      Number of MPI ranks. None derives it from the largest destination
      rank. See _validate_round_robin() for why we need it.

    @return `(hardcoded_config, hardcoded_mode)` where hardcoded_config is the
      complete constructor argument string and hardcoded_mode is the mode
      argument within this string.
    """
    from toolbox.loadbalancing.api.strategies.SplitTrees import (
        extract_split_instructions,
    )

    (
        parent_trees,
        subpartition_sizes,
        subtrees,
        bottom_up_flags,
    ) = extract_split_instructions(peano_tree)

    if modes is None:
        markers = _markers_from_flags(bottom_up_flags)
    else:
        markers = _normalise_markers(modes, len(subtrees))
    hardcoded_mode = _format_modes(markers)

    destination_ranks = _normalise_destination_ranks(destination_ranks, subtrees)
    _validate_round_robin(subtrees, destination_ranks, number_of_ranks)

    if verbose:
        print("Extracted hardcoded parameters:")
        for k in range(len(subtrees)):
            print(
                "  split {}: tree {} hands {} cells over to tree {} on rank {}, {}".format(
                    k,
                    parent_trees[k],
                    subpartition_sizes[k],
                    subtrees[k],
                    destination_ranks[k],
                    markers[k],
                )
            )

    hardcoded_config = (
        "{"
        + _format_initialisation_lists(
            parent_trees,
            subpartition_sizes,
            destination_ranks,
        )
        + ", "
        + hardcoded_mode
        + "}"
    )
    return hardcoded_config, hardcoded_mode
