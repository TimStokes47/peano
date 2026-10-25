import pathlib

import peano4.api
import peano4.api.solversteps

import exahype2.api


def _solver():
    solver = exahype2.api.Solver("Probe", 3, 1)
    solver.add_pde_term(exahype2.api.InitPointWisely(implementation="Q[0] = 0.0;"))
    solver.numerical_scheme = exahype2.api.rkfd4.CentralDifferencesWithRungeKutta(
        solver, patch_size=7, rk_order=4
    )
    return solver


def test_initial_condition_loops_over_grid_cells():
    solver = _solver()
    action_set = exahype2.api.rkfd4.actionsets.InitialCondition(solver, guard="true")
    body = action_set.get_body_of_operation(
        peano4.api.solversteps.ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME
    )
    assert "parallelDfor( gridCell, 7 )" in body
    assert (
        "::exahype2::fd::getGridCellCentre( marker.x(), marker.h(), 7, gridCell)"
        in body
    )
    assert "* (3 + 1)" in body
    assert '#include "exahype2/fd/PatchUtils.h"' in action_set.get_includes()


def test_dirichlet_boundary_conditions_fill_all_halo_layers():
    solver = _solver()
    action_set = exahype2.api.rkfd4.actionsets.ApplyDirichletBoundaryConditions(
        solver, solver._patch_overlap_current_time_step, guard="true"
    )
    body = action_set.get_body_of_operation(
        peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME
    )
    assert "::exahype2::blockstructured::applyDirichletBoundaryConditions(" in body
    # 7 grid cells per axis, an overlap of 2, 3+1 doubles per grid cell
    assert "    7,\n    2,\n    3 + 1,\n" in body
    assert "fineGridFaceProbeQCurrentTimeStep.value" in body
    assert (
        '#include "exahype2/blockstructured/DirichletBoundaryConditions.h"'
        in action_set.get_includes()
    )


def test_roll_over_copies_both_halves_of_the_overlap():
    solver = _solver()
    action_set = exahype2.api.rkfd4.actionsets.RollOverUpdatedFace(
        solver._patch_overlap_current_time_step,
        solver._patch_overlap_update,
        guard="true",
        includes="",
    )
    body = action_set.get_body_of_operation(
        peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_LAST_TIME
    )
    assert body.count("::exahype2::fv::copyHalfOfHalo(") == 2
    assert "      4,\n      7,\n      2," in body


def test_rkfd4_does_not_import_the_finite_volume_package():
    package = pathlib.Path(exahype2.api.rkfd4.__file__).parent
    for source in package.rglob("*.py"):
        code = source.read_text()
        assert "from ..fv" not in code and "import exahype2.api.fv" not in code, source
