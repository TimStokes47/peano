import exahype2.api.rungekutta.LinearCombination as linear_combination


def _call(weights, solution="Q", output="Y"):
    return linear_combination.create_linear_combination_call(
        namespace=["probe"],
        number_of_dofs=25,
        unknowns=4,
        auxiliary_variables=1,
        weights=weights,
        solution=solution,
        estimates="K",
        output=output,
        time_step_size="dt",
    )


def test_stages_with_zero_weight_are_not_read():
    # Row three of Kutta's 3/8 rule: the estimate of stage three does not
    # exist yet and may hold NaN
    call = _call([1.0, -1.0, 1.0, 0.0])
    assert "static constexpr double weights[3] = { 1.0, -1.0, 1.0 };" in call
    assert "double* estimatePointers[3] = { K + 0, K + 100, K + 200 };" in call
    assert "K + 300" not in call
    assert "      3\n    >(solutionPointers" in call


def test_weights_keep_their_stage():
    # Midpoint rule: the final combination uses stage one only
    call = _call([0.0, 1.0])
    assert "static constexpr double weights[1] = { 1.0 };" in call
    assert "double* estimatePointers[1] = { K + 100 };" in call


def test_all_weights_zero_copies_solution():
    assert "std::copy_n( Q, 125, Y );" in _call([0.0, 0.0])
    assert _call([0.0], solution="Q", output="Q") == "{}"


def test_kernel_translates(tmp_path):
    linear_combination.generate_linear_combination_kernel(["probe"], str(tmp_path))
    code = (tmp_path / "kernels" / "rungekutta_linear_combination.cpph").read_text()
    assert "void linearCombinationOfRungeKuttaEstimates(" in code
    assert "weights[" in code
