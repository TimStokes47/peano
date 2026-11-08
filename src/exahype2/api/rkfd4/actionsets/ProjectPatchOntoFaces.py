# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import toolbox.blockstructured.api


class ProjectPatchOntoFaces(toolbox.blockstructured.api.ProjectPatchOntoFaces):
    """!

    Project a patch's data onto the overlap held on its surrounding faces,
    so the halo layers can be reconstructed from it later on, see
    toolbox.blockstructured.api.ReconstructPatchWithHalo.

    The fourth-order Finite Differences of @ref exahype2.api.rkfd4 use an
    overlap of HALO_SIZE grid cells, i.e. each face holds 2*HALO_SIZE layers
    along its normal. The overlap is taken from the face data, so the action set does
    not need to know it.

    The class does exactly what its base class
    toolbox.blockstructured.api.ProjectPatchOntoFaces does: copy the
    patch's boundary layers into the face overlap. It is a copy of
    exahype2.api.fv.actionsets.ProjectPatchOntoFaces, as rkfd4 does not
    depend on the Finite Volume package.

    """

    def __init__(self, patch, patch_overlap_update, guard, includes):
        """!

        @param patch: peano4.api.datamodel.Patch
          The cell-associated patch to project onto its surrounding faces,
          i.e. solver._patch.

        @param patch_overlap_update: peano4.api.datamodel.Patch
          The face-associated overlap patch to project patch's boundary
          layers into, i.e. solver._patch_overlap_update.

        @param guard: str
          C++ boolean expression, evaluated per cell in touchCellLastTime,
          guarding whether that cell's patch is projected onto its faces.

        @param includes: str
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
