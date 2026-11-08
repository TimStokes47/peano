# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api

from ...rungekutta.LinearCombination import create_linear_combination_call
from .AbstractRKFD4ActionSet import AbstractRKFD4ActionSet


class ComputeLinearCombination(AbstractRKFD4ActionSet):
    """!

    Combine the solution with the right-hand side estimates of the
    Runge-Kutta stages

    The action set computes

    @f$ Q_{out} = Q^n + \\Delta t \\sum _j w_j K_j @f$

    per grid cell through the kernel of @ref exahype2.api.rungekutta.LinearCombination,
    with @f$ Q^n @f$ being solver._patch and the @f$ K_j @f$ the estimates in
    solver._rhs_estimates. The weights are either a row of the Butcher
    tableau's weight matrix, which yields the input of a stage, or the
    final estimate weights, which yield the solution of the next time step.
    In the latter case, the output is solver._patch itself.

    The auxiliary variables are copied from solver._patch.

    """

    def __init__(self, solver, weights, output, guard):
        """!

        @param solver: exahype2.api.Solver
          Has to have an exahype2.api.rkfd4.CentralDifferencesWithRungeKutta
          as numerical scheme.

        @param weights: [float]
          One weight per Runge-Kutta stage.

        @param output: peano4.api.datamodel.Patch
          The cell data to write the outcome into, i.e. solver._stage_input
          or solver._patch.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellFirstTime.

        """
        AbstractRKFD4ActionSet.__init__(self, solver)
        self.weights = list(weights)
        self.output = output
        self.guard = guard

    def get_body_of_operation(self, operation_name):
        if (
            operation_name
            != peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
        ):
            return ""
        scheme = self._solver.numerical_scheme
        n = scheme.patch_size

        def call(number_of_dofs):
            return create_linear_combination_call(
                namespace=scheme.kernel_namespace,
                number_of_dofs=number_of_dofs,
                unknowns=self._solver.unknowns,
                auxiliary_variables=0,
                weights=self.weights,
                solution=f"fineGridCell{self._solver._patch.name}.value",
                estimates=f"fineGridCell{self._solver._rhs_estimates.name}.value",
                output=f"fineGridCell{self.output.name}.value",
                time_step_size=f"repositories::{self._solver.get_name_of_global_instance()}.getMinTimeStepSize()",
            )

        return f"""
  #if Dimensions==2
  {call(n * n)}
  #else
  {call(n * n * n)}
  #endif
"""

    def get_guard_of_operation(self, operation_name):
        if (
            operation_name
            == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
        ):
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
