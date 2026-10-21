import peano4.api
import peano4.api.datamodel
import peano4.api.solversteps

import exahype2.api


class _NumericalSchemeStub(exahype2.api.Solver.NumericalScheme):
    """Stands in for the rkfd4 numerical scheme, which is not there yet."""

    def __init__(self, solver, patch_size, overlap):
        super().__init__(solver)
        self.patch_size = patch_size
        total_unknowns = solver.unknowns + solver.auxiliary_variables
        identifier = solver._unknown_identifier()
        solver._patch = peano4.api.datamodel.Patch(
            (patch_size, patch_size, patch_size), total_unknowns, identifier
        )
        solver._patch_overlap_current_time_step = peano4.api.datamodel.Patch(
            (2 * overlap, patch_size, patch_size),
            total_unknowns,
            identifier + "CurrentTimeStep",
        )
        solver._patch_overlap_update = peano4.api.datamodel.Patch(
            (2 * overlap, patch_size, patch_size),
            total_unknowns,
            identifier + "Update",
        )

    def construct_output(self, exahype2_project, peano_project, time_step):
        pass


def _solver():
    solver = exahype2.api.Solver("Probe", 3, 1)
    solver.add_pde_term(exahype2.api.InitPointWisely(implementation="Q[0] = 0.0;"))
    solver.numerical_scheme = _NumericalSchemeStub(solver, patch_size=7, overlap=3)
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


def test_dirichlet_boundary_conditions_fill_all_three_halo_layers():
    solver = _solver()
    action_set = exahype2.api.rkfd4.actionsets.ApplyDirichletBoundaryConditions(
        solver, solver._patch_overlap_current_time_step, guard="true"
    )
    body = action_set.get_body_of_operation(
        peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_FIRST_TIME
    )
    assert "::exahype2::fd::getGridCellSize( marker.h(), 7 )" in body
    assert "for (int layer=0; layer<3; layer++)" in body
    assert "fineGridFaceProbeQCurrentTimeStep.value" in body


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
    assert "      4,\n      7,\n      3," in body


def test_rkfd4_does_not_import_the_finite_volume_package():
    import pathlib

    package = pathlib.Path(exahype2.api.rkfd4.__file__).parent
    for source in package.rglob("*.py"):
        code = source.read_text()
        assert "from ..fv" not in code and "import exahype2.api.fv" not in code, source
