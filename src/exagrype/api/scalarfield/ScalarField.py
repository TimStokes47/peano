""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import exahype2.api


class ScalarField(exahype2.api.Solver):
    """!

    Massless scalar field in first-order form

    The solver has four unknowns: The time derivative Pi of the scalar field
    and the three spatial derivatives Psi_i of the field. They obey the
    first-order wave equation

        d_t Pi - d_i Psi_i = 0,    d_t Psi_i - d_i Pi = 0,

    with wave speed one, in three dimensions. Flux, source (zero), maximum
    eigenvalue and initial condition are stateless. By default, the initial
    condition is a sine wave along the x-axis.

    The solver is deliberately plain. It is the matter ingredient which one
    couples to exagrype.api.FOCCZ4Solver (see
    benchmarks/exagrype/gauge-wave-with-scalar-field). The user typically
    attaches auxiliary variables (@ref exahype2.api.AuxiliaryVariables) and
    adapts the PDE terms through the hooks _flux_implementation(),
    _source_implementation() and _max_eigenvalue_implementation(). The terms
    are (re)built from the hooks whenever a set of auxiliary variables is
    attached.

    """

    Unknowns = {"Pi": 1, "Psi": 3}
    """! The four unknowns in the order the flux expects them """

    def __init__(
        self,
        name: str,
        wave_number: int = 1,
        amplitude: float = 0.2,
        initial_condition: exahype2.api.InitPointWisely = None,
    ):
        """!

        @param name: str
          Name of the solver, @ref exahype2.api.Solver.

        @param wave_number: int
          Number of periods of the default initial wave over the unit cube.

        @param amplitude: float
          Amplitude of the default initial wave.

        @param initial_condition: exahype2.api.InitPointWisely
          Overwrites the default initial condition.

        """
        super().__init__(name, unknowns=self.Unknowns)
        self.add_abstract_solver_attribute(
            'static_assert(Dimensions == 3, "The scalar field solver is three-dimensional only");'
        )
        self._flux = exahype2.api.Flux(implementation="", stateless=True)
        self._source = exahype2.api.VolumetricSource(implementation="", stateless=True)
        self._max_eigenvalue = exahype2.api.MaxEigenvalue(
            implementation="", stateless=True
        )
        self.add_pde_term(self._flux)
        self.add_pde_term(self._source)
        self.add_pde_term(self._max_eigenvalue)
        self.add_pde_term(
            initial_condition
            if initial_condition is not None
            else exahype2.api.InitPointWisely(
                implementation=self.sine_wave(wave_number, amplitude)
            )
        )
        self._refresh_pde_terms()

    @staticmethod
    def sine_wave(wave_number: int = 1, amplitude: float = 0.2) -> str:
        """! C++ of the default initial condition: a standing sine wave along x """
        return f"""
  Q[0] = {amplitude!r} * 2.0 * M_PI * {wave_number} * std::cos( 2.0 * M_PI * {wave_number} * x(0) );
  Q[1] = 0.0;
  Q[2] = 0.0;
  Q[3] = 0.0;
"""

    def add_auxiliary_variables(self, auxiliary: exahype2.api.AuxiliaryVariables):
        """! Attach a set of auxiliary variables and rebuild the PDE terms from the hooks"""
        super().add_auxiliary_variables(auxiliary)
        self._refresh_pde_terms()

    def _refresh_pde_terms(self):
        self._flux.implementation = self._flux_implementation()
        self._source.implementation = self._source_implementation()
        self._max_eigenvalue.implementation = self._max_eigenvalue_implementation()

    def _flux_implementation(self) -> str:
        """!

        C++ of the flux along normal

        Hook for subclasses. The code has Q, x, normal and F at hand, plus
        one pointer per set of auxiliary variables.

        """
        return """
  for (int i=0; i<4; i++) F[i] = 0.0;
  F[0]        = -Q[1+normal];
  F[1+normal] = -Q[0];
"""

    def _source_implementation(self) -> str:
        """! C++ of the volumetric source (zero). Hook for subclasses. """
        return """
  for (int i=0; i<4; i++) S[i] = 0.0;
"""

    def _max_eigenvalue_implementation(self) -> str:
        """! C++ of the maximum eigenvalue (one). Hook for subclasses. """
        return """
  *lambdaMax = 1.0;
"""
