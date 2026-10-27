"""Unit tests for the gradient-based refinement criterion
(@ref exahype2.api.GradientRefinement)."""

import exahype2.api


def test_both_criteria_are_adaptivity_criteria():
    assert issubclass(exahype2.api.GradientRefinement, exahype2.api.AdaptivityCriterion)
    assert issubclass(
        exahype2.api.RefineErasePointwisely, exahype2.api.AdaptivityCriterion
    )


def test_signature_takes_gradient_and_normal():
    assert exahype2.api.GradientRefinement.SIGNATURE == (
        "const double* __restrict__ gradient, "
        "int normal, "
        "::exahype2::RefinementCommand* __restrict__ refinementCommand"
    )


def test_declaration_is_void_and_pure_virtual_without_implementation():
    declaration = exahype2.api.GradientRefinement().generator.generate_declaration()
    assert declaration.startswith("virtual void refineEraseWithGradient(")
    assert declaration.rstrip().endswith(") = 0;")


def test_implementation_becomes_body():
    body = "if (gradient[0] > 1.0) *refinementCommand = ::exahype2::RefinementCommand::Refine;"
    term = exahype2.api.GradientRefinement(implementation=body)
    definition = term.generator.generate_out_of_class_definitions(
        "probe::AbstractProbe"
    )
    assert definition.startswith("void probe::AbstractProbe::refineEraseWithGradient(")
    assert body in definition


def test_stub_traces_normal_only():
    stub = exahype2.api.GradientRefinement().generator.generate_stub_definition(
        "probe::Probe"
    )
    assert stub.startswith("void probe::Probe::refineEraseWithGradient(")
    assert 'logTraceInWith1Argument("refineEraseWithGradient(...)", normal);' in stub
    assert "not implemented yet" in stub
