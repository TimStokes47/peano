# This file is part of the ExaHyPE2 project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet

import peano4.api
import jinja2


class RollOverUpdatedFace(ActionSet):
    """!

    Copy patch_overlap_update - the face overlap
    exahype2.api.fv.actionsets.ProjectPatchOntoFaces has just written the
    latest projection into - onto patch_overlap_current_time_step, the
    persistent face data toolbox.blockstructured.api.ReconstructPatchWithHalo
    reads from to rebuild a patch's halo. Both halves of the face overlap
    (left and right, relative to the face's normal) are copied in full,
    unconditionally subject only to guard.

    Ported and substantially slimmed down from the legacy
    exahype2.solvers.fv.actionsets.RollOverUpdatedFace: that version
    additionally rolled a third ("old") buffer over and gated each half
    of the copy on a per-half "updated" flag read from a face label -
    neither the third buffer nor the face label exist yet in the new
    exahype2.api's data structures (@see
    exahype2.api.fv.RusanovWithExplicitEuler.__create_data_structures(),
    which creates exactly solver._patch_overlap_current_time_step and
    solver._patch_overlap_update, and nothing else on the face side).

    """

    TemplateHandleFace = jinja2.Template("""
    if ( {{GUARD}} ) {
      const int normal = marker.getSelectedFaceNumber() % Dimensions;

      logTraceInWith2Arguments( "touchFaceLastTime(...)---RollOverUpdatedFace", marker.toString(), normal );

      ::exahype2::fv::copyHalfOfHalo(
        {{UNKNOWNS}},
        {{DOFS_PER_AXIS}},
        {{OVERLAP}},
        normal,
        false,    // left half
        {{UPDATE_ACCESSOR}}.value,
        {{CURRENT_TIME_STEP_ACCESSOR}}.value
      );
      ::exahype2::fv::copyHalfOfHalo(
        {{UNKNOWNS}},
        {{DOFS_PER_AXIS}},
        {{OVERLAP}},
        normal,
        true,    // right half
        {{UPDATE_ACCESSOR}}.value,
        {{CURRENT_TIME_STEP_ACCESSOR}}.value
      );

      logTraceOut( "touchFaceLastTime(...)---RollOverUpdatedFace" );
    }
""")

    def __init__(self, patch_overlap_current_time_step, patch_overlap_update, guard, includes):
        """!

        patch_overlap_current_time_step: peano4.api.datamodel.Patch
          The persistent face overlap to roll patch_overlap_update into -
          @see exahype2.api.fv.RusanovWithExplicitEuler.

        patch_overlap_update: peano4.api.datamodel.Patch
          The face overlap exahype2.api.fv.actionsets.ProjectPatchOntoFaces
          has just written the latest projection into - @see
          exahype2.api.fv.RusanovWithExplicitEuler. Its dim[1] is assumed to
          equal the cell patch's dofs-per-axis, i.e. patch_overlap_update
          and patch_overlap_current_time_step have to be face-associated
          overlaps of a cell patch in the sense of
          exahype2.api.fv.actionsets.ProjectPatchOntoFaces.

        guard: str
          C++ boolean expression, evaluated per face in touchFaceLastTime,
          guarding whether that face's overlap is rolled over.

        includes: str
          Additional includes this action set's generated code needs -
          callers typically pass solver._get_default_includes() +
          solver.user_solver_includes.

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self.additional_includes = includes
        self.d = {}
        self.d["GUARD"] = guard
        self.d["UNKNOWNS"] = str(patch_overlap_update.no_of_unknowns)
        self.d["DOFS_PER_AXIS"] = str(patch_overlap_update.dim[1])
        self.d["OVERLAP"] = str(int(patch_overlap_update.dim[0] / 2))
        self.d["UPDATE_ACCESSOR"] = "fineGridFace" + patch_overlap_update.name
        self.d["CURRENT_TIME_STEP_ACCESSOR"] = "fineGridFace" + patch_overlap_current_time_step.name

    def get_body_of_operation(self, operation_name):
        result = ""
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_TOUCH_FACE_LAST_TIME:
            result = self.TemplateHandleFace.render(**self.d)
        return result

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_includes(self):
        return (
            """
#include <functional>
#include "exahype2/fv/PatchUtils.h"
"""
            + self.additional_includes
        )
