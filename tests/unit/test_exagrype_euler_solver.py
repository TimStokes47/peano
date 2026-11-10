import pytest

import exahype2.api
import exagrype.api


def test_solver_has_five_named_unknowns():
    solver = exagrype.api.euler.Euler("Euler")
    assert solver.unknowns == 5
    assert solver.number_of_auxiliary_variables == 0


def test_solver_registers_pde_terms():
    solver = exagrype.api.euler.Euler("Euler")
    names = {term.name for term in solver.pde_terms}
    assert names == {"flux", "volumetricSource", "maxEigenvalue", "initPointWisely"}
    assert not solver.requires_user_class


def test_gamma_has_to_exceed_one():
    with pytest.raises(ValueError):
        exagrype.api.euler.Euler("Euler", gamma=1.0)


def test_hooks_extend_the_solver():
    class WithSource(exagrype.api.euler.Euler):
        def _source_implementation(self):
            return "tarch::memset(S, 0, 5 * sizeof(double));"

    names = {term.name for term in WithSource("Euler").pde_terms}
    assert "volumetricSource" in names


def test_flux_source_and_eigenvalue_see_all_auxiliary_variables():
    solver = exagrype.api.euler.Euler("Euler")
    sets = [
        exahype2.api.AuxiliaryVariables("Spacetime", {"lapse": 1}),
        exahype2.api.AuxiliaryVariables("Matter", {"m": 2}),
    ]
    for auxiliary in sets:
        solver.add_auxiliary_variables(auxiliary)
    terms = {term.name: term.implementation for term in solver.pde_terms}
    for name in ["flux", "volumetricSource", "maxEigenvalue"]:
        assert "{ spacetime, matter }" in terms[name]
        assert "numberOfAuxiliarySets = 2" in terms[name]
