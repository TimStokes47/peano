# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.toolbox.blockstructured.api


class ProjectPatchOntoFaces(peano4.toolbox.blockstructured.api.ProjectPatchOntoFaces):
    """!

    Project a Finite Volume patch's data onto the overlap held on its
    surrounding faces, so the halo layers can be reconstructed from it later
    on - @see exahype2.api.fv.actionsets.ReconstructPatchWithHalo.

    Ported from the legacy python/exahype2/solvers/fv/actionsets/ProjectPatchOntoFaces.py,
    slimmed down: the legacy version additionally set the isUpdated()/
    UpdatedTimeStamp() face metadata (read solver._face_label/solver._cell_label)
    so exahype2.solvers.fv.actionsets.RollOverUpdatedFace knew which half of a
    face had actually been written. The new exahype2.api's data structures
    (@see exahype2.api.fv.RusanovWithExplicitEuler) do not create a cell/face
    label yet, and RollOverUpdatedFace has not been ported either, so this
    version does purely what the base class
    peano4.toolbox.blockstructured.api.ProjectPatchOntoFaces already does:
    copy solver._patch's boundary layer into solver._patch_overlap_update.
    Once cell/face labels exist in the new API, the metadata-update behaviour
    can be reinstated here.

    """

    def __init__(self, solver, guard):
        """!

        solver: exahype2.api.Solver
          Has to have solver._patch and solver._patch_overlap_update set -
          @see exahype2.api.fv.RusanovWithExplicitEuler.

        guard: str
          C++ boolean expression, evaluated per cell in touchCellLastTime,
          guarding whether that cell's patch is projected onto its faces.

        """
        super().__init__(
            solver._patch,
            solver._patch_overlap_update,
            guard,
            solver._get_default_includes() + solver.user_solver_includes,
        )

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
