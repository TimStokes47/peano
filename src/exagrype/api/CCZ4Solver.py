""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

from typing import Dict, Union

import exahype2.api

from . import initialvalues


class CCZ4Solver(exahype2.api.Solver):
    """!

    Solver for the Einstein equations in the first-order CCZ4 formulation

    The solver is a plain exahype2.api.Solver with the 59 CCZ4 unknowns and
    the PDE terms of CCZ4 pre-registered. All terms delegate to the
    ExaGRyPE library (configure Peano with --enable-exagrype):

    - the non-conservative product to exagrype::ccz4::ncp(),
    - the volumetric source to exagrype::ccz4::source(),
    - the maximum eigenvalue to exagrype::ccz4::maxEigenvalue(),
    - the initial condition to one of the routines from
      exagrype::initialvalues, @see exagrype.api.initialvalues.

    CCZ4 has no conservative flux. You still have to assign a numerical
    scheme, a grid construction and (optionally) a plotter, exactly as for
    any other exahype2.api.Solver, and to add the solver to an
    exahype2.api.Project. The project has to be three-dimensional: the
    generated solver class refuses to compile otherwise.

    ## Shortcomings

    - The fourth-order Finite Differences of the production runs are
      available through exahype2.api.rkfd4, i.e. pass a
      CentralDifferencesWithRungeKutta as numerical scheme. Unlike the
      legacy ExaGRyPE, they do not support adaptive mesh refinement yet.
    - The solver pushes the solution back onto the CCZ4 constraints
      (exagrype::ccz4::enforceCCZ4constraints()) only if you pass
      enforce_constraints=True. Without it, long runs drift away from the
      constraint surface.
    - The second-order formulation, black holes (TwoPunctures), tracers and
      the ADM/Psi4 postprocessing are not available yet.

    """

    Unknowns = {
        "G": 6,
        "K": 6,
        "theta": 1,
        "Z": 3,
        "lapse": 1,
        "shift": 3,
        "b": 3,
        "dLapse": 3,
        "dxShift": 3,
        "dyShift": 3,
        "dzShift": 3,
        "dxG": 6,
        "dyG": 6,
        "dzG": 6,
        "traceK": 1,
        "phi": 1,
        "P": 3,
        "K0": 1,
    }
    """!

    The unknowns of the first-order CCZ4 formulation in the order the
    kernels in exagrype::ccz4 expect them. The names match the legacy
    ExaGRyPE solvers.

    """

    DefaultParameters = {
        "CCZ4LapseType": 0,
        "CCZ4SO": 0,
        "CCZ4ds": 1.0,
        "CCZ4c": 1.0,
        "CCZ4e": 1.0,
        "CCZ4f": 0.75,
        "CCZ4bs": 0.0,
        "CCZ4sk": 0.0,
        "CCZ4xi": 1.0,
        "CCZ4itau": 1.0,
        "CCZ4eta": 1.0,
        "CCZ4k1": 0.1,
        "CCZ4k2": 0.0,
        "CCZ4k3": 0.5,
        "CCZ4GLMc": 1.2,
        "CCZ4GLMd": 2.0,
        "CCZ4mu": 0.2,
    }
    """!

    Default values of the CCZ4 parameters, taken from the legacy ExaGRyPE
    solvers. Integer values become int constants of the generated solver
    class, all others double constants.

    """

    def __init__(
        self,
        name: str,
        parameters: Dict[str, Union[int, float]] = None,
        initial_condition: exahype2.api.InitPointWisely = None,
        enforce_constraints: bool = False,
    ):
        """!

        Create a CCZ4 solver

        @param name: str
          Name of the solver, @see exahype2.api.Solver.

        @param parameters: Dict[str, Union[int, float]]
          CCZ4 parameters that differ from DefaultParameters, e.g.
          {"CCZ4k1": 0.0}. Keys which are not in DefaultParameters are
          rejected, as they most likely are typos.

        @param initial_condition: exahype2.api.InitPointWisely
          Initial condition. Defaults to exagrype.api.initialvalues.gauge_wave().
          Pass any other routine from exagrype.api.initialvalues, or your
          own exahype2.api.InitPointWisely.

        @param enforce_constraints: bool
          If True, the solver pushes the solution back onto the algebraic
          CCZ4 constraints (exagrype::ccz4::enforceCCZ4constraints()) after
          each update, i.e. after each time step and, for Runge-Kutta
          schemes, after each Runge-Kutta stage. The legacy ExaGRyPE always
          does this. The solver realises the enforcement as point-wise
          postprocessing, @see exahype2.api.Solver.add_point_wise_postprocessing().

        """
        super().__init__(name, unknowns=self.Unknowns, auxiliary_variables=0)

        self._parameters = dict(self.DefaultParameters)
        for key, value in (parameters or {}).items():
            if key not in self.DefaultParameters:
                raise KeyError(
                    f"Solver {name}: unknown CCZ4 parameter {key!r}. "
                    f"Available parameters are {list(self.DefaultParameters)}."
                )
            self._parameters[key] = value

        self.add_abstract_solver_attribute(
            'static_assert(Dimensions == 3, "CCZ4 (ExaGRyPE) is three-dimensional only");'
        )
        for key, value in self._parameters.items():
            cpp_type = (
                "int" if isinstance(self.DefaultParameters[key], int) else "double"
            )
            self.add_abstract_solver_attribute(
                f"static constexpr {cpp_type} {key} = {value};"
            )

        self.add_user_solver_includes(
            """
#include "exagrype/ccz4/Kernels.h"
#include "exagrype/initialvalues/InitialValues.h"
#include "exagrype/initialvalues/ApplyTwoPunctures.h"
"""
        )

        self.add_pde_term(
            exahype2.api.NonconservativeProduct(
                implementation=self._ncp_implementation()
            )
        )
        self.add_pde_term(
            exahype2.api.VolumetricSource(implementation=self._source_implementation())
        )
        self.add_pde_term(
            exahype2.api.MaxEigenvalue(
                implementation=self._max_eigenvalue_implementation()
            )
        )
        self.add_pde_term(
            initial_condition
            if initial_condition is not None
            else initialvalues.gauge_wave()
        )

        if enforce_constraints:
            self.add_point_wise_postprocessing(
                "Enforce constraints", "::exagrype::ccz4::enforceCCZ4constraints(Q);"
            )

    @property
    def parameters(self) -> Dict[str, Union[int, float]]:
        """!
        The CCZ4 parameters the solver is generated with (read-only copy).
        """
        return dict(self._parameters)

    @staticmethod
    def _ncp_implementation() -> str:
        """!

        The Finite Volume kernels hand over the jump deltaQ along one normal,
        the fourth-order Finite Differences of exahype2.api.rkfd4 the
        derivative along one normal times the mesh width,
        whereas exagrype::ccz4::ncp() expects the gradient along all three
        directions. We hence embed the jump into an otherwise empty gradient.

        """
        return """
  double deltaQSerialised[NumberOfUnknowns * 3];
  for (int i = 0; i < NumberOfUnknowns; i++) {
    deltaQSerialised[i + 0 * NumberOfUnknowns] = 0.0;
    deltaQSerialised[i + 1 * NumberOfUnknowns] = 0.0;
    deltaQSerialised[i + 2 * NumberOfUnknowns] = 0.0;

    deltaQSerialised[i + normal * NumberOfUnknowns] = deltaQ[i];
  }
  ::exagrype::ccz4::ncp(BTimesDeltaQ, Q, deltaQSerialised, normal, CCZ4LapseType, CCZ4ds, CCZ4c, CCZ4e, CCZ4f, CCZ4bs, CCZ4sk, CCZ4xi, CCZ4mu, CCZ4SO);
"""

    @staticmethod
    def _source_implementation() -> str:
        return """
  tarch::memset(S, 0, NumberOfUnknowns * sizeof(double));
  ::exagrype::ccz4::source(S, Q, CCZ4LapseType, CCZ4ds, CCZ4c, CCZ4e, CCZ4f, CCZ4bs, CCZ4sk, CCZ4xi, CCZ4itau, CCZ4eta, CCZ4k1, CCZ4k2, CCZ4k3, CCZ4SO);
"""

    @staticmethod
    def _max_eigenvalue_implementation() -> str:
        return """
  ::exagrype::ccz4::maxEigenvalue(Q, normal, CCZ4e, CCZ4ds, CCZ4GLMc, CCZ4GLMd, lambdaMax);
"""
