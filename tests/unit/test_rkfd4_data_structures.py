import pytest

import peano4.api
import peano4.api.datamodel

import exahype2.api


def _scheme(patch_size=5, rk_order=3):
    solver = exahype2.api.Solver("Probe", 4, 2)
    scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
        solver, patch_size=patch_size, rk_order=rk_order
    )
    solver.numerical_scheme = scheme
    return solver, scheme


def test_patches_have_halo_of_three():
    solver, scheme = _scheme()
    assert scheme.overlap == 3
    assert solver._patch.dim == (5, 5, 5)
    assert solver._patch.no_of_unknowns == 6
    assert solver._patch_with_halo.dim == (11, 11, 11)
    assert solver._patch_with_halo.no_of_unknowns == 6
    for overlap in (
        solver._patch_overlap_current_time_step,
        solver._patch_overlap_update,
    ):
        assert overlap.dim == (6, 5, 5)
        assert overlap.no_of_unknowns == 6


def test_rhs_estimates_hold_one_patch_of_unknowns_per_stage():
    solver, scheme = _scheme(rk_order=3)
    assert scheme.rk_order == 3
    assert solver._rhs_estimates.dim == (15, 5, 5)
    # Auxiliary variables do not evolve in time, so there is no estimate for them
    assert solver._rhs_estimates.no_of_unknowns == 4


def test_data_names_carry_solver_name():
    solver, _ = _scheme()
    assert [
        data.name
        for data in [
            solver._patch,
            solver._patch_with_halo,
            solver._rhs_estimates,
            solver._patch_overlap_current_time_step,
            solver._patch_overlap_update,
            solver._max_eigenvalue,
        ]
    ] == [
        "ProbeQ",
        "ProbeQWithHalo",
        "ProbeQRhsEstimates",
        "ProbeQCurrentTimeStep",
        "ProbeQUpdate",
        "ProbeMaxEigenvalue",
    ]


def test_add_data_to_repository_exchanges_current_face_data_only():
    solver, scheme = _scheme()
    project = peano4.api.Project(["probe"], "probe", ".")
    scheme._CentralDifferencesWithRungeKutta__add_data_to_repository(project)

    for patch in (
        solver._patch,
        solver._patch_with_halo,
        solver._rhs_estimates,
        solver._patch_overlap_current_time_step,
        solver._patch_overlap_update,
    ):
        assert isinstance(
            patch.generator, peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer
        )
    face_guard = solver.grid_selector.createFaceGuard()
    assert (
        solver._patch_overlap_current_time_step.generator.send_condition == face_guard
    )
    assert solver._patch_overlap_update.generator.send_condition == "false"
    assert solver._patch_overlap_update.generator.receive_and_merge_condition == "false"


@pytest.mark.parametrize("patch_size", [1, 2])
def test_patch_smaller_than_halo_is_rejected(patch_size):
    with pytest.raises(ValueError):
        _scheme(patch_size=patch_size)


@pytest.mark.parametrize("rk_order", [0, 5])
def test_unsupported_runge_kutta_order_is_rejected(rk_order):
    with pytest.raises(ValueError):
        _scheme(rk_order=rk_order)


def test_negative_kreiss_oliger_sigma_is_rejected():
    solver = exahype2.api.Solver("Probe", 1, 0)
    with pytest.raises(ValueError):
        exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
            solver, patch_size=5, rk_order=2, kreiss_oliger_sigma=-1.0
        )
