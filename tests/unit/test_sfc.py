import pytest

from toolbox.loadbalancing.api.strategies.SFC import (
    Cartesian_to_Peano_2d,
    Peano_to_Cartesian_2d,
    Cartesian_to_Peano_3d,
    Peano_to_Cartesian_3d,
)


# ---------------------------------------------------------------------------
# 2D: docstring example
# ---------------------------------------------------------------------------

def test_cartesian_to_peano_2d_docstring_example():
    """Docstring example: (4, 2) with size=9 must map to 10."""
    assert Cartesian_to_Peano_2d((4, 2), 9) == 10


def test_peano_to_cartesian_2d_docstring_example():
    """Inverse of docstring example: index 10 with size=9 must map to (4, 2)."""
    assert Peano_to_Cartesian_2d(10, 9) == (4, 2)


# ---------------------------------------------------------------------------
# 2D: base case (size=1)
# ---------------------------------------------------------------------------

def test_cartesian_to_peano_2d_size1():
    assert Cartesian_to_Peano_2d((0, 0), 1) == 0


def test_peano_to_cartesian_2d_size1():
    assert Peano_to_Cartesian_2d(0, 1) == (0, 0)


# ---------------------------------------------------------------------------
# 2D: bijection and round-trip for size=3 and size=9
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("size", [3, 9])
def test_cartesian_to_peano_2d_bijection(size):
    """Every cell maps to a unique index in [0, size²)."""
    seen = set()
    for x in range(size):
        for y in range(size):
            idx = Cartesian_to_Peano_2d((x, y), size)
            assert 0 <= idx < size * size, f"Index {idx} out of range for ({x},{y})"
            assert idx not in seen, f"Duplicate index {idx} for ({x},{y})"
            seen.add(idx)
    assert len(seen) == size * size


@pytest.mark.parametrize("size", [3, 9])
def test_peano_to_cartesian_2d_round_trip(size):
    """Forward then inverse returns the original coordinates for all cells."""
    for x in range(size):
        for y in range(size):
            idx = Cartesian_to_Peano_2d((x, y), size)
            assert Peano_to_Cartesian_2d(idx, size) == (x, y), (
                f"Round-trip failed for ({x},{y}): got {Peano_to_Cartesian_2d(idx, size)}"
            )


# ---------------------------------------------------------------------------
# 3D: base case (size=1)
# ---------------------------------------------------------------------------

def test_cartesian_to_peano_3d_size1():
    assert Cartesian_to_Peano_3d((0, 0, 0), 1) == 0


def test_peano_to_cartesian_3d_size1():
    assert Peano_to_Cartesian_3d(0, 1) == (0, 0, 0)


# ---------------------------------------------------------------------------
# 3D: bijection and round-trip for size=3 (and size=9 marked slow)
# ---------------------------------------------------------------------------

@pytest.mark.parametrize("size", [3])
def test_cartesian_to_peano_3d_bijection(size):
    """Every 3-D cell maps to a unique index in [0, size³)."""
    seen = set()
    for x in range(size):
        for y in range(size):
            for z in range(size):
                idx = Cartesian_to_Peano_3d((x, y, z), size)
                assert 0 <= idx < size ** 3, f"Index {idx} out of range for ({x},{y},{z})"
                assert idx not in seen, f"Duplicate index {idx} for ({x},{y},{z})"
                seen.add(idx)
    assert len(seen) == size ** 3


@pytest.mark.parametrize("size", [3])
def test_peano_to_cartesian_3d_round_trip(size):
    """Forward then inverse returns the original coordinates for all 3-D cells."""
    for x in range(size):
        for y in range(size):
            for z in range(size):
                idx = Cartesian_to_Peano_3d((x, y, z), size)
                assert Peano_to_Cartesian_3d(idx, size) == (x, y, z), (
                    f"Round-trip failed for ({x},{y},{z}): "
                    f"got {Peano_to_Cartesian_3d(idx, size)}"
                )


@pytest.mark.slow
def test_cartesian_to_peano_3d_bijection_size9():
    """Full bijection check for size=9 (729 cells); marked slow."""
    size = 9
    seen = set()
    for x in range(size):
        for y in range(size):
            for z in range(size):
                idx = Cartesian_to_Peano_3d((x, y, z), size)
                assert 0 <= idx < size ** 3
                assert idx not in seen, f"Duplicate index {idx} for ({x},{y},{z})"
                seen.add(idx)
    assert len(seen) == size ** 3


@pytest.mark.slow
def test_peano_to_cartesian_3d_round_trip_size9():
    """Round-trip check for size=9 (729 cells); marked slow."""
    size = 9
    for x in range(size):
        for y in range(size):
            for z in range(size):
                idx = Cartesian_to_Peano_3d((x, y, z), size)
                assert Peano_to_Cartesian_3d(idx, size) == (x, y, z)


# ---------------------------------------------------------------------------
# The curve has to match Peano's spacetree traversal
# ---------------------------------------------------------------------------

import os
import re

from toolbox.loadbalancing.api.strategies.SFC import _children_in_traversal_order


def _peano_zfor3_tables(dimensions):
    """Read the lookup tables of zfor3 from src/peano4/utils/Loop.h"""
    loop_header = os.path.join(
        os.path.dirname(__file__), "..", "..", "src", "peano4", "utils", "Loop.h"
    )
    with open(loop_header) as header:
        text = header.read()
    children = 3**dimensions
    start = text.index(
        "static const int lookupzfor3[{}][{}][{}]".format(2**dimensions, children, dimensions)
    )
    block = text[start : text.index("};", start)]
    entries = [
        tuple(int(value) for value in entry.split(","))
        for entry in re.findall(r"\{(\d+(?:,\s*\d+)+)\}", block)
    ]
    return [entries[i * children : (i + 1) * children] for i in range(2**dimensions)]


@pytest.mark.parametrize("dimensions", [2, 3])
def test_children_order_matches_peano_zfor3(dimensions):
    """The order of the 3^d children replicates peano4::utils::zfor3 for all loop directions."""
    for direction, table in enumerate(_peano_zfor3_tables(dimensions)):
        positive = tuple(bool(direction >> axis & 1) for axis in range(dimensions))
        assert list(_children_in_traversal_order(positive)) == table


@pytest.mark.parametrize("size", [3, 9, 27])
def test_curve_2d_is_continuous(size):
    """Consecutive cells along the curve share a face, i.e. the curve has no jumps."""
    cells = [Peano_to_Cartesian_2d(i, size) for i in range(size * size)]
    for a, b in zip(cells, cells[1:]):
        assert abs(a[0] - b[0]) + abs(a[1] - b[1]) == 1, (a, b)


@pytest.mark.parametrize("size", [3, 9])
def test_curve_3d_is_continuous(size):
    """Consecutive cells along the curve share a face, i.e. the curve has no jumps."""
    cells = [Peano_to_Cartesian_3d(i, size) for i in range(size**3)]
    for a, b in zip(cells, cells[1:]):
        assert sum(abs(x - y) for x, y in zip(a, b)) == 1, (a, b)
