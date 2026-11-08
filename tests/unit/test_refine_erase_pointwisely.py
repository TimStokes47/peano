"""Unit tests for the point-wise refinement criterion
(@ref exahype2.api.RefineErasePointwisely)."""

import exahype2.api


def test_signature_returns_refinement_command_through_pointer():
    assert exahype2.api.RefineErasePointwisely.SIGNATURE == (
        "const double* __restrict__ Q, "
        "const tarch::la::Vector<Dimensions, double>& x, "
        "double t, "
        "::exahype2::RefinementCommand* __restrict__ refinementCommand"
    )


def test_declaration_is_void_and_pure_virtual_without_implementation():
    term = exahype2.api.RefineErasePointwisely()
    declaration = term.generator.generate_declaration()
    assert declaration.startswith("virtual void refineErasePointwisely(")
    assert (
        "::exahype2::RefinementCommand* __restrict__ refinementCommand" in declaration
    )
    assert declaration.rstrip().endswith(") = 0;")


def test_implementation_becomes_body():
    body = "if (Q[0] > 1.0) *refinementCommand = ::exahype2::RefinementCommand::Refine;"
    term = exahype2.api.RefineErasePointwisely(implementation=body)
    definition = term.generator.generate_out_of_class_definitions(
        "probe::AbstractProbe"
    )
    assert definition.startswith("void probe::AbstractProbe::refineErasePointwisely(")
    assert body in definition


def test_stub_for_user_solver():
    term = exahype2.api.RefineErasePointwisely()
    assert "override" in term.generator.generate_stub_declaration()
    stub = term.generator.generate_stub_definition("probe::Probe")
    assert stub.startswith("void probe::Probe::refineErasePointwisely(")
    assert "not implemented yet" in stub


def test_solver_accepts_term():
    solver = exahype2.api.Solver("Probe", 4)
    solver.add_pde_term(exahype2.api.RefineErasePointwisely(implementation=""))
    assert any(
        isinstance(term, exahype2.api.RefineErasePointwisely)
        for term in solver.pde_terms
    )
