""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

from typing import List, Union

import exahype2.api
import exahype2.api.rkfd4

from .actionsets import AddKreissOligerDissipation


class FD4WithKODissipation(exahype2.api.rkfd4.CentralDifferencesWithRungeKutta):
    """!

    Fourth-order Finite Differences with Kreiss-Oliger dissipation

    The numerical scheme is exahype2.api.rkfd4.CentralDifferencesWithRungeKutta
    plus a Kreiss-Oliger dissipation of sixth order on top: Per Runge-Kutta
    stage, it first evaluates the plain fourth-order right-hand side and
    then adds

    @f$ \\frac{\\sigma}{64 \\Delta x_d} (Q_{i-3} - 6 Q_{i-2} + 15 Q_{i-1} - 20 Q_i + 15 Q_{i+1} - 6 Q_{i+2} + Q_{i+3}) @f$

    per axis d to the selected quantities (@ref
    exagrype.api.actionsets.AddKreissOligerDissipation). Both kernels read
    the same stage input, i.e. the outcome is the right-hand side with
    dissipation. The dissipation stencil reaches three grid cells into the
    neighbouring patches, so the scheme widens the halo from two to three
    grid cells. Everything else is inherited.

    To use the dissipation, replace the numerical scheme of a solver:

    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    solver.numerical_scheme  = exagrype.api.FD4WithKODissipation(solver, patch_size=9, rk_order=4, kreiss_oliger_sigma=8.0)
    solver.grid_construction = exahype2.api.rkfd4.SetupSimulationOverRegularGrid(solver, volume_size=0.04)
    solver.plotter           = exahype2.api.rkfd4.PlotAsVTKFile(solver, time_in_between_plots=0.1)
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    The grid construction and the plotter remain those of
    exahype2.api.rkfd4. The legacy ExaGRyPE runs used
    kreiss_oliger_sigma=8.0. The scheme is three-dimensional only, as it
    relies on exagrype::ccz4::addKreissOligerDissipation().

    """

    HALO_SIZE = 3

    def __init__(
        self,
        solver: exahype2.api.Solver,
        patch_size: int,
        rk_order: int,
        kreiss_oliger_sigma: float,
        quantities: List[Union[str, int]] = None,
        time_step_size: float = None,
        cfl_constant: float = 0.5,
    ):
        """!

        @param solver: exahype2.api.Solver
          The solver this numerical scheme realises.

        @param patch_size: int
          Number of grid cells per axis within one patch. Has to be at least
          HALO_SIZE=3.

        @param rk_order: int
          Order of the Runge-Kutta scheme, i.e. its number of stages.

        @param kreiss_oliger_sigma: float
          Scaling of the Kreiss-Oliger dissipation. Has to be non-negative.

        @param quantities: List[str or int] or None
          The unknowns which are subject to the dissipation. None, the
          default, selects all unknowns. A string is the name of an unknown
          as passed to the solver (e.g. "lapse" or "G" for exagrype.api.CCZ4Solver)
          and selects all of its entries. An int is the index of one entry
          of the unknowns. Auxiliary variables do not evolve in time and
          hence cannot be selected.

        @param time_step_size: float or None
          Fixed time step size. If None, the time step size is adaptive.

        @param cfl_constant: float
          CFL constant for adaptive time step sizes.

        """
        super().__init__(
            solver,
            patch_size,
            rk_order,
            time_step_size=time_step_size,
            cfl_constant=cfl_constant,
        )
        if kreiss_oliger_sigma < 0.0:
            raise ValueError(
                f"Solver {solver.name}: kreiss_oliger_sigma has to be non-negative, "
                f"but got {kreiss_oliger_sigma}"
            )
        self.kreiss_oliger_sigma = kreiss_oliger_sigma
        self.dissipative_quantities = self.__resolve_quantities(solver, quantities)

    @staticmethod
    def __resolve_quantities(solver, quantities) -> List[int]:
        """!

        Translate the quantities into sorted indices of the unknowns

        """
        if quantities is None:
            return list(range(solver.unknowns))
        if len(quantities) == 0:
            raise ValueError(
                f"Solver {solver.name}: quantities must not be empty. Pass None "
                "to apply the Kreiss-Oliger dissipation to all unknowns."
            )
        names = solver.variable_names_per_entry()[: solver.unknowns]
        indices = set()
        for quantity in quantities:
            if isinstance(quantity, int):
                if not 0 <= quantity < solver.unknowns:
                    raise ValueError(
                        f"Solver {solver.name}: quantity index {quantity} is out of "
                        f"range, as the solver has {solver.unknowns} unknowns"
                    )
                indices.add(quantity)
            elif isinstance(quantity, str):
                matches = [
                    index
                    for index, name in enumerate(names)
                    if name == quantity or name.startswith(quantity + "[")
                ]
                if not matches:
                    raise ValueError(
                        f"Solver {solver.name}: {quantity!r} is not the name of an "
                        f"unknown. Available names are "
                        f"{sorted(set(name.split('[')[0] for name in names))}."
                    )
                indices.update(matches)
            else:
                raise TypeError(
                    f"Solver {solver.name}: quantities have to be names (str) or "
                    f"indices (int) of unknowns, but got {quantity!r}"
                )
        return sorted(indices)

    def _right_hand_side_action_sets(self, stage, cell_guard):
        """!
        The plain right-hand side of the superclass, followed by the dissipation
        """
        return super()._right_hand_side_action_sets(stage, cell_guard) + [
            AddKreissOligerDissipation(
                self._solver,
                stage,
                self.kreiss_oliger_sigma,
                self.dissipative_quantities,
                guard=cell_guard,
            )
        ]
