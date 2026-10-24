import pytest

import exahype2.api.rungekutta


def _dot(u, v):
    return sum(a * b for a, b in zip(u, v))


def _times(matrix, v):
    return [_dot(row, v) for row in matrix]


def _elementwise(u, v):
    return [a * b for a, b in zip(u, v)]


@pytest.mark.parametrize("order", [1, 2, 3, 4])
def test_tableau_is_explicit_and_consistent(order):
    tableau = exahype2.api.rungekutta.ButcherTableau(order)
    a = tableau.weight_matrix()
    c = tableau.relative_time_step_sizes()
    b = tableau.final_estimate_weights()

    assert tableau.number_of_stages == order
    assert len(a) == order and all(len(row) == order for row in a)
    assert len(b) == order and len(c) == order
    for k in range(order):
        assert all(a[k][j] == 0.0 for j in range(k, order)), "not explicit"
        assert sum(a[k]) == pytest.approx(c[k])
    assert sum(b) == pytest.approx(1.0)


@pytest.mark.parametrize("order", [1, 2, 3, 4])
def test_tableau_satisfies_order_conditions(order):
    tableau = exahype2.api.rungekutta.ButcherTableau(order)
    a = tableau.weight_matrix()
    c = tableau.relative_time_step_sizes()
    b = tableau.final_estimate_weights()

    # One entry per rooted tree, up to order four
    conditions = {
        2: [(_dot(b, c), 1 / 2)],
        3: [
            (_dot(b, _elementwise(c, c)), 1 / 3),
            (_dot(b, _times(a, c)), 1 / 6),
        ],
        4: [
            (_dot(b, _elementwise(c, _elementwise(c, c))), 1 / 4),
            (_dot(b, _elementwise(c, _times(a, c))), 1 / 8),
            (_dot(b, _times(a, _elementwise(c, c))), 1 / 12),
            (_dot(b, _times(a, _times(a, c))), 1 / 24),
        ],
    }
    for p in range(2, order + 1):
        for value, expected in conditions[p]:
            assert value == pytest.approx(expected)


@pytest.mark.parametrize("order", [0, 5])
def test_unsupported_order_is_rejected(order):
    with pytest.raises(ValueError):
        exahype2.api.rungekutta.ButcherTableau(order)
