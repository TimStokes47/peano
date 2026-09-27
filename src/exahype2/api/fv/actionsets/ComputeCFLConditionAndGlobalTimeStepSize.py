# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .AbstractFVActionSet import AbstractFVActionSet

import peano4.api


class ComputeCFLConditionAndGlobalTimeStepSize(AbstractFVActionSet):
    """!

    Evaluate the CFL condition over solver._patch's volumes and feed the
    resulting admissible time step size into the solver's global time step
    size (repositories::<solver instance>.update(...) in the legacy
    version).

    @todo Not implemented yet: the actual per-volume eigenvalue evaluation
    and the resulting repositories::<solver instance>.update(...) call are
    missing - @see exahype2.api.fv.actionsets.UpdateCell's own @todo, which
    this depends on (the eigenvalues this class would evaluate come out of
    the same Riemann solver / PDE kernel call UpdateCell does not call yet).

    """

    def __init__(self, solver, guard="true"):
        """!

        solver: exahype2.api.Solver
          Has to have solver._patch set - @see
          exahype2.api.fv.RusanovWithExplicitEuler.

        guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime,
          guarding whether that cell's patch contributes to the CFL
          condition/global time step size.

        """
        AbstractFVActionSet.__init__(self, solver)
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            result = f"""
  if ({self.guard}) {{
    // @todo Evaluate the CFL condition over fineGridCell{self._solver._patch.name}
    //       and feed the resulting time step size into
    //       repositories::{self._solver.get_name_of_global_instance()}.update(...) -
    //       not implemented yet, @see this class' docstring.
  }}
"""
        return result

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
