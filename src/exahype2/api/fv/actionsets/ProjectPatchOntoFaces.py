# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import toolbox.blockstructured.api


class ProjectPatchOntoFaces(toolbox.blockstructured.api.ProjectPatchOntoFaces):
    """!

    Project a Finite Volume patch's data onto the overlap held on its
    surrounding faces, so the halo layers can be reconstructed from it later
    on - @see toolbox.blockstructured.api.ReconstructPatchWithHalo.

    Ported from the legacy python/exahype2/solvers/fv/actionsets/ProjectPatchOntoFaces.py,
    slimmed down: the legacy version additionally set the isUpdated()/
    UpdatedTimeStamp() face metadata (read solver._face_label/solver._cell_label)
    so exahype2.solvers.fv.actionsets.RollOverUpdatedFace knew which half of a
    face had actually been written. The new exahype2.api's data structures
    (@see exahype2.api.fv.RusanovWithExplicitEuler) do not create a cell/face
    label yet, and RollOverUpdatedFace has not been ported either, so this
    version does purely what the base class
    toolbox.blockstructured.api.ProjectPatchOntoFaces already does:
    copy solver._patch's boundary layer into solver._patch_overlap_update.
    Once cell/face labels exist in the new API, the metadata-update behaviour
    can be reinstated here.

    """

    def __init__(self, patch, patch_overlap_update, guard, includes):
        """!

        patch: peano4.api.datamodel.Patch
          The cell-associated patch to project onto its surrounding faces -
          @see exahype2.api.fv.RusanovWithExplicitEuler.

        patch_overlap_update: peano4.api.datamodel.Patch
          The face-associated overlap patch to project patch's boundary
          layer into - @see exahype2.api.fv.RusanovWithExplicitEuler.

        guard: str
          C++ boolean expression, evaluated per cell in touchCellLastTime,
          guarding whether that cell's patch is projected onto its faces.

        includes: str
          Additional includes this action set's generated code needs -
          callers typically pass solver._get_default_includes() +
          solver.user_solver_includes.

        """
        super().__init__(
            patch,
            patch_overlap_update,
            guard,
            includes,
        )

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
