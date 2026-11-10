""" This file is part of the ExaGRyPE project. For conditions of distribution and
 use, please see the copyright notice at www.peano-framework.org
"""

import exahype2.api


class Euler(exahype2.api.Solver):
    """!

    Compressible Euler equations of an ideal gas

    The solver evolves the five conserved quantities density, momentum and
    total energy,

    @f$
      \\partial_t \\begin{pmatrix} \\rho \\\\ j \\\\ E \\end{pmatrix}
      + \\nabla \\cdot \\begin{pmatrix} j \\\\ j \\otimes j / \\rho + p I \\\\ (E+p) j / \\rho \\end{pmatrix} = 0,
      \\qquad p = (\\gamma-1) \\left(E - \\frac{|j|^2}{2\\rho}\\right),
    @f$

    in three dimensions. It has a flux, a maximum eigenvalue, a source term and an initial
    condition, all of them stateless. The source term is zero.
    Flux, source and eigenvalue see all auxiliary variables of the solver,
    even though they ignore them at the moment. By default, the
    initial condition is a density wave which the fluid advects with
    constant velocity (smooth_wave()).

    The solver is deliberately plain. Its purpose within ExaGRyPE is to be
    the matter ingredient which one couples to exagrype.api.FOCCZ4Solver. The
    assembly of such a coupled system is not part of this class, but done by
    the user (see benchmarks/exagrype/gauge-wave-with-Euler), who typically
    attaches auxiliary variables (@ref exahype2.api.AuxiliaryVariables) with
    the spacetime to the solver, and replaces the flux and the source term
    through the hooks _flux_implementation() and _source_implementation().

    ## Shortcomings

    - The equations are those of flat-space hydrodynamics, i.e. the solver
      knows nothing about general relativity on its own.
    - Only ideal gases are supported, and there is no positivity limiter.

    """

    Unknowns = {
        "rho": 1,
        "j": 3,
        "E": 1,
    }
    """! The five conserved quantities in the order the flux expects them """

    def __init__(
        self,
        name: str,
        gamma: float = 1.4,
        initial_condition: exahype2.api.InitPointWisely = None,
    ):
        """!

        Create an Euler solver

        @param name: str
          Name of the solver, @ref exahype2.api.Solver.

        @param gamma: float
          Adiabatic index of the ideal gas. Has to be greater than one.

        @param initial_condition: exahype2.api.InitPointWisely
          Initial condition. Defaults to smooth_wave().

        """
        if gamma <= 1.0:
            raise ValueError(f"Solver {name}: gamma has to be greater than one")
        super().__init__(name, unknowns=self.Unknowns)
        self._gamma = gamma

        self.add_user_solver_includes('#include "exagrype/euler/EulerKernels.h"\n')
        self.add_abstract_solver_attribute(
            'static_assert(Dimensions == 3, "The Euler solver of ExaGRyPE is three-dimensional only");'
        )
        self.add_abstract_solver_attribute(
            f"static constexpr double Gamma = {gamma!r};"
        )

        self._flux = exahype2.api.Flux(implementation="", stateless=True)
        self._source = exahype2.api.VolumetricSource(implementation="", stateless=True)
        self._max_eigenvalue = exahype2.api.MaxEigenvalue(
            implementation="", stateless=True
        )
        self._refresh_pde_terms()
        self.add_pde_term(self._flux)
        self.add_pde_term(self._source)
        self.add_pde_term(self._max_eigenvalue)
        self.add_pde_term(
            initial_condition if initial_condition is not None else self.smooth_wave()
        )

    def add_auxiliary_variables(self, auxiliary: exahype2.api.AuxiliaryVariables):
        """!

        Attach a set of auxiliary variables and hand it to flux, source and eigenvalue

        The sets are not known when the constructor creates the PDE terms, so
        the terms are rebuilt from the hooks whenever a set arrives.

        @param auxiliary: exahype2.api.AuxiliaryVariables
          The set, @ref exahype2.api.Solver.add_auxiliary_variables().

        """
        super().add_auxiliary_variables(auxiliary)
        self._refresh_pde_terms()

    def _refresh_pde_terms(self):
        """! Rebuild the implementations of flux, source and eigenvalue from the hooks """
        self._flux.implementation = self._flux_implementation()
        self._source.implementation = self._source_implementation()
        self._max_eigenvalue.implementation = self._max_eigenvalue_implementation()

    def _auxiliary_arguments(self) -> str:
        """!

        C++ which declares the array of pointers to all auxiliary variables

        The code declares the array auxiliary and the constant
        numberOfAuxiliarySets in the scope of a PDE term. The kernels of
        exagrype::euler take both. The array has one element if the solver has
        no auxiliary variables, as C++ has no empty arrays.

        """
        sets = self.auxiliary_variables
        pointers = ", ".join(a.argument_name for a in sets) if sets else "nullptr"
        return f"""
  const double* const auxiliary[] = {{ {pointers} }};
  const int numberOfAuxiliarySets = {len(sets)};"""

    @property
    def gamma(self) -> float:
        """! Adiabatic index of the gas"""
        return self._gamma

    @staticmethod
    def smooth_wave(
        amplitude: float = 0.2, velocity: float = 1.0, pressure: float = 1.0
    ) -> exahype2.api.InitPointWisely:
        """!

        Density wave on constant pressure

        The density is 1 + amplitude * sin(2 pi x) and the fluid moves along
        the x-axis with constant velocity. Pressure and velocity are
        constant, i.e. this is an exact solution of the Euler equations
        which merely advects the wave. Any additional entries behind the
        five unknowns, i.e. auxiliary variables which the solver owns, are
        not touched.

        @param amplitude: float
          Amplitude of the density wave. Has to be smaller than one to keep
          the density positive.

        @param velocity: float
          Speed along the x-axis.

        @param pressure: float
          Constant pressure.

        """
        if abs(amplitude) >= 1.0:
            raise ValueError("amplitude has to be smaller than one in modulus")
        return exahype2.api.InitPointWisely(
            implementation=f"""
  ::exagrype::euler::smoothWave( x(0), {amplitude!r}, {velocity!r}, {pressure!r}, Gamma, Q );
""",
        )

    def _flux_implementation(self) -> str:
        """!

        C++ of the flux along normal

        Hook for subclasses, which can scale or modify the flux, e.g. by the
        lapse of a spacetime they hold as auxiliary variables. The code has
        the variables Q, x, normal and F at hand, plus one pointer per set of
        auxiliary variables, and auxiliary (array of all pointers) and
        numberOfAuxiliarySets (see _auxiliary_arguments()). The default
        calls exagrype::euler::flux().

        """
        return self._auxiliary_arguments() + """
  ::exagrype::euler::flux( Q, auxiliary, numberOfAuxiliarySets, normal, Gamma, F );
"""

    def _source_implementation(self) -> str:
        """!

        C++ of the volumetric source

        Hook for subclasses. The code has Q, x, t and S at hand, plus the
        arguments of _flux_implementation(). The default calls
        exagrype::euler::source(), which yields zero.

        """
        return self._auxiliary_arguments() + """
  ::exagrype::euler::source( Q, auxiliary, numberOfAuxiliarySets, Gamma, S );
"""

    def _max_eigenvalue_implementation(self) -> str:
        return self._auxiliary_arguments() + """
  *lambdaMax = ::exagrype::euler::maxEigenvalue( Q, auxiliary, numberOfAuxiliarySets, normal, Gamma );
"""
