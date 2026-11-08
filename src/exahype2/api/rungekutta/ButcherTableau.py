# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List


class ButcherTableau:
    """!

    Butcher tableau of an explicit Runge-Kutta scheme

    An explicit Runge-Kutta scheme with s stages advances

    @f$ \\partial _t Q = F(t,Q) @f$

    from @f$ Q^n @f$ to @f$ Q^{n+1} @f$ through

    @f$ K_k = F \\left( t^n + c_k \\Delta t, Q^n + \\Delta t \\sum _{j<k} a_{kj} K_j \\right) @f$

    @f$ Q^{n+1} = Q^n + \\Delta t \\sum _k b_k K_k @f$

    The tableau holds the coefficients:

    - relative_time_step_sizes(): the @f$ c_k @f$, i.e. the left column of
      the classic tableau.
    - weight_matrix(): the @f$ a_{kj} @f$, i.e. the strictly lower
      triangular matrix in the middle of the classic tableau. Row k holds the
      weights of the estimates which make up the input of stage k.
    - final_estimate_weights(): the @f$ b_k @f$, i.e. the bottom row of the
      classic tableau.

    We support the orders one to four with one stage per order:

    | Order | Scheme                       |
    |-------|------------------------------|
    | 1     | explicit Euler               |
    | 2     | explicit midpoint rule       |
    | 3     | Kutta's third-order scheme   |
    | 4     | Kutta's 3/8 rule             |

    The tableau does not know anything about the discretisation in space,
    so Finite Differences (@ref exahype2.api.rkfd4) and Discontinuous
    Galerkin schemes alike can build on it.

    Ported from the legacy python/exahype2/solvers/ButcherTableau.py, which
    remains in place, as the legacy Runge-Kutta solvers import it.

    """

    def __init__(self, order: int):
        """!

        @param order: int
          Order of the Runge-Kutta scheme. Has to be between one and four.
          It equals the number of stages.

        """
        if order < 1 or order > 4:
            raise ValueError(
                f"Runge-Kutta order has to be between 1 and 4, but got {order}"
            )
        self._order = order

    @property
    def order(self) -> int:
        return self._order

    @property
    def number_of_stages(self) -> int:
        return self._order

    def relative_time_step_sizes(self) -> List[float]:
        """!

        The @f$ c_k @f$, i.e. the time stamp of stage k relative to the
        time step size: Stage k evaluates the right-hand side at
        @f$ t^n + c_k \\Delta t @f$.

        """
        return {
            1: [0.0],
            2: [0.0, 1 / 2],
            3: [0.0, 1 / 2, 1.0],
            4: [0.0, 1 / 3, 2 / 3, 1.0],
        }[self._order]

    def weight_matrix(self) -> List[List[float]]:
        """!

        The @f$ a_{kj} @f$ as number_of_stages x number_of_stages matrix.
        The matrix is strictly lower triangular, as the scheme is explicit.

        """
        return {
            1: [
                [0.0],
            ],
            2: [
                [0.0, 0.0],
                [1 / 2, 0.0],
            ],
            3: [
                [0.0, 0.0, 0.0],
                [1 / 2, 0.0, 0.0],
                [-1.0, 2.0, 0.0],
            ],
            4: [
                [0.0, 0.0, 0.0, 0.0],
                [1 / 3, 0.0, 0.0, 0.0],
                [-1 / 3, 1.0, 0.0, 0.0],
                [1.0, -1.0, 1.0, 0.0],
            ],
        }[self._order]

    def final_estimate_weights(self) -> List[float]:
        """!

        The @f$ b_k @f$, i.e. the weights of the stages' estimates within
        the final linear combination which yields @f$ Q^{n+1} @f$.

        """
        return {
            1: [1.0],
            2: [0.0, 1.0],
            3: [1 / 6, 2 / 3, 1 / 6],
            4: [1 / 8, 3 / 8, 3 / 8, 1 / 8],
        }[self._order]
