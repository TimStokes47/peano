# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
"""!

Peano space-filling curve as Peano's spacetree traversal runs through it

The routines replicate the traversal order of peano4::grid::Spacetree, i.e.
they do not implement an arbitrary Peano curve but exactly the one that
Peano's C++ core uses. The offline load balancing hence plans along the same
order in which Peano later hands out cells throughout a split.

The order is defined recursively. The root cell starts with all even flags
unset. A cell runs through its @f$ 3^d @f$ children in a serpentine
(boustrophedon) order: axis 0 is the fastest axis, axis d-1 the slowest one.
Along each axis, the cell runs in positive direction if the number of even
flags set for the other axes is even (peano4::grid::PeanoCurve::isTraversePositiveAlongAxis()),
and the serpentine then alternates the direction of all faster axes
(peano4::utils::zfor3). A child inherits the even flags of its parent, but
the middle child along an axis inverts the flag of this axis
(peano4::grid::Spacetree::refineState()).

Peano inverts the whole traversal in every other mesh sweep. The routines
here describe the non-inverted traversal. The inverted one runs through the
same curve backwards. The offline load balancing ignores this inversion and
always plans along the non-inverted curve: A split which Peano carries out in
an inverted sweep picks its cells from the other end of the curve, but it
still hands over the requested number of cells.

@image html PeanoSFC.png

"""
import functools


def _is_traversal_positive_along_axis(even_flags: tuple, axis: int) -> bool:
    """!
    Python counterpart of peano4::grid::PeanoCurve::isTraversePositiveAlongAxis() for a non-inverted traversal
    """
    number_of_other_even_flags = sum(
        1 for other_axis, flag in enumerate(even_flags) if flag and other_axis != axis
    )
    return number_of_other_even_flags % 2 == 0


@functools.lru_cache(maxsize=None)
def _children_in_traversal_order(directions: tuple) -> tuple:
    """!

    The 3^d children of a cell in the order of peano4::utils::zfor3

    @param directions: tuple of bool
      One flag per axis: True means that the traversal runs in positive
      direction along this axis.

    """
    if len(directions) == 1:
        return tuple((i,) for i in ([0, 1, 2] if directions[0] else [2, 1, 0]))

    faster_axes = _children_in_traversal_order(directions[:-1])
    result = []
    for step, i in enumerate([0, 1, 2] if directions[-1] else [2, 1, 0]):
        sweep = faster_axes if step % 2 == 0 else tuple(reversed(faster_axes))
        result += [child + (i,) for child in sweep]
    return tuple(result)


def _child_even_flags(even_flags: tuple, child: tuple) -> tuple:
    """! Even flags of a child: the middle child along an axis inverts this axis' flag"""
    return tuple(flag ^ (position == 1) for flag, position in zip(even_flags, child))


def _cartesian_to_peano(coordinates: tuple, size: int) -> int:
    dimensions = len(coordinates)
    even_flags = (False,) * dimensions
    result = 0
    coordinates = tuple(coordinates)
    while size > 1:
        sub_size = size // 3
        child = tuple(coordinate // sub_size for coordinate in coordinates)
        directions = tuple(
            _is_traversal_positive_along_axis(even_flags, axis)
            for axis in range(dimensions)
        )
        result = result * 3**dimensions + _children_in_traversal_order(
            directions
        ).index(child)
        even_flags = _child_even_flags(even_flags, child)
        coordinates = tuple(coordinate % sub_size for coordinate in coordinates)
        size = sub_size
    return result


def _peano_to_cartesian(peano_index: int, size: int, dimensions: int) -> tuple:
    digits = []
    remaining_size = size
    while remaining_size > 1:
        digits.append(peano_index % 3**dimensions)
        peano_index //= 3**dimensions
        remaining_size //= 3
    digits.reverse()

    even_flags = (False,) * dimensions
    coordinates = [0] * dimensions
    sub_size = size
    for digit in digits:
        sub_size //= 3
        directions = tuple(
            _is_traversal_positive_along_axis(even_flags, axis)
            for axis in range(dimensions)
        )
        child = _children_in_traversal_order(directions)[digit]
        coordinates = [
            coordinate + position * sub_size
            for coordinate, position in zip(coordinates, child)
        ]
        even_flags = _child_even_flags(even_flags, child)
    return tuple(coordinates)


def Cartesian_to_Peano_2d(coordinates: tuple[int, int], size: int) -> int:
    """!

    Map Cartesian index to Peano SFC index.

    This operation accepts a Cartesian index and returns the position of
    this cell along Peano's traversal (see the file documentation). In the
    example below

    @image html PeanoSFC.png

    we can hand in (4,2) and get a 10 back.

    Args:
        coordinates: (x, y) index with entries in [0, size-1]
        size:        Side length of the square grid; must be a power of 3.

    Returns:
        Integer Peano SFC index in [0, size*size - 1].
    """
    return _cartesian_to_peano(coordinates, size)


def Cartesian_to_Peano_3d(coordinates: tuple[int, int, int], size: int) -> int:
    """!

    Map 3-D Cartesian index to Peano SFC index.

    Three-dimensional counterpart of `Cartesian_to_Peano_2d`.

    Args:
        coordinates: (x, y, z) index with entries in [0, size-1]
        size:        Side length of the cubic grid; must be a power of 3.

    Returns:
        Integer Peano SFC index in [0, size**3 - 1].
    """
    return _cartesian_to_peano(coordinates, size)


def Peano_to_Cartesian_2d(peano_index: int, size: int) -> tuple[int, int]:
    """!

    Inverse of `Cartesian_to_Peano_2d`.

    Args:
        peano_index: SFC index in [0, size*size - 1].
        size:        Side length of the square grid; must be a power of 3.

    Returns:
        (x, y) Cartesian coordinates.

    """
    return _peano_to_cartesian(peano_index, size, 2)


def Peano_to_Cartesian_3d(peano_index: int, size: int) -> tuple[int, int, int]:
    """!

    Inverse of `Cartesian_to_Peano_3d`.

    Args:
        peano_index: SFC index in [0, size**3 - 1].
        size:        Side length of the cubic grid; must be a power of 3.

    Returns:
        (x, y, z) Cartesian coordinates.

    """
    return _peano_to_cartesian(peano_index, size, 3)
