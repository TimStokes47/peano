import pytest

import exahype2.api
import exagrype.api


def test_solver_has_59_named_unknowns():
    solver = exagrype.api.CCZ4Solver("CCZ4")
    assert solver.unknowns == 59
    assert solver.number_of_material_parameters == 0
    assert solver.variable_names_per_entry()[16] == "lapse"
    assert solver.variable_names_per_entry()[54] == "phi"


def test_solver_registers_ccz4_pde_terms():
    solver = exagrype.api.CCZ4Solver("CCZ4")
    names = {term.name for term in solver.pde_terms}
    assert names == {
        "nonconservativeProduct",
        "volumetricSource",
        "maxEigenvalue",
        "initPointWisely",
    }
    assert (
        "::exagrype::ccz4::ncp("
        in solver.get_pde_term("nonconservativeProduct").implementation
    )
    assert (
        "::exagrype::ccz4::source("
        in solver.get_pde_term("volumetricSource").implementation
    )
    assert (
        "::exagrype::ccz4::maxEigenvalue("
        in solver.get_pde_term("maxEigenvalue").implementation
    )
    # All terms are generated, so no user class is required
    assert not solver.requires_user_class


def test_default_initial_condition_is_gauge_wave():
    solver = exagrype.api.CCZ4Solver("CCZ4")
    assert "gaugeWave" in solver.get_pde_term("initPointWisely").implementation


def test_initial_condition_can_be_replaced():
    solver = exagrype.api.CCZ4Solver(
        "CCZ4", initial_condition=exagrype.api.initialvalues.flat()
    )
    assert (
        "::exagrype::initialvalues::flat("
        in solver.get_pde_term("initPointWisely").implementation
    )


def test_parameters_override_defaults():
    solver = exagrype.api.CCZ4Solver(
        "CCZ4", parameters={"CCZ4k1": 0.0, "CCZ4LapseType": 1}
    )
    assert solver.parameters["CCZ4k1"] == 0.0
    assert solver.parameters["CCZ4LapseType"] == 1
    assert (
        solver.parameters["CCZ4GLMd"]
        == exagrype.api.CCZ4Solver.DefaultParameters["CCZ4GLMd"]
    )


def test_unknown_parameter_is_rejected():
    with pytest.raises(KeyError):
        exagrype.api.CCZ4Solver("CCZ4", parameters={"CCZ4kk1": 0.0})


def test_constraint_enforcement_is_opt_in():
    solver = exagrype.api.CCZ4Solver("CCZ4")
    Enforce = exagrype.api.CCZ4Solver.EnforceCCZ4Constraint
    assert solver._enforce_ccz4_constraint == Enforce.No
    solver.enforce_ccz4_constraint(Enforce.IntermediateSteps)
    assert solver._enforce_ccz4_constraint == Enforce.IntermediateSteps
    solver.enforce_ccz4_constraint(Enforce.No)
    assert solver._enforce_ccz4_constraint == Enforce.No
    with pytest.raises(TypeError):
        solver.enforce_ccz4_constraint(True)
