# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import enum

import jinja2

from peano4.api.solversteps.ActionSet import ActionSet


class InterpolateHangingFaces(ActionSet):
    """!

    Initialise the overlap data on hanging faces from the coarser mesh

    A hanging face is a face of a fine cell which sits on the face of a
    coarser, unrefined cell. Peano does not store hanging faces. It creates
    them whenever a mesh sweep enters an adjacent fine cell and destroys them
    once the sweep leaves this cell again. The action set plugs into
    createHangingFace(): It takes the overlap data of the coarse face which
    the hanging face is part of and interpolates them onto the hanging face
    (::toolbox::blockstructured::interpolateHaloLayer_AoS_...). Afterwards,
    the half of the hanging face which belongs to the coarse cell holds
    interpolated coarse data, so the fine cell can reconstruct its halo as
    if its neighbour were a fine cell, too.

    The face overlap has to be a 2MxNxN patch: M is the overlap, N the
    number of volumes per patch axis.

    """

    class Scheme(enum.Enum):
        """!

        Interpolation schemes

        The value of each entry is the suffix of the C++ routine
        ::toolbox::blockstructured::interpolateHaloLayer_AoS_<suffix>().

        - PiecewiseConstant copies the coarse value into all fine volumes
          which it covers.
        - Linear interpolates linearly along the face. As the coarse face
          does not hold the coarse volumes beyond its two ends, the scheme
          extrapolates constantly there. Linear extrapolation would be more
          accurate, but it can produce unphysical values such as negative
          densities next to steep gradients
          (@ref toolbox::blockstructured::internal::createLinearInterpolationMatrix()).

        """

        PiecewiseConstant = "piecewise_constant"
        Linear = "linear_with_constant_extrapolation"

    TemplateCreateHangingFace = jinja2.Template(
        """
  logTraceInWith1Argument( "createHangingFace(...)---InterpolateHangingFaces", marker.toString() );
  ::toolbox::blockstructured::interpolateHaloLayer_AoS_{{SCHEME}}(
    marker,
    {{DOFS_PER_AXIS}},
    {{OVERLAP}},
    {{UNKNOWNS}},
    coarseGridFaces{{FACE_NAME}}(marker.getSelectedFaceNumber()).value,
    fineGridFace{{FACE_NAME}}.value
  );
  logTraceOut( "createHangingFace(...)---InterpolateHangingFaces" );
"""
    )

    def __init__(
        self,
        patch_overlap,
        scheme: "InterpolateHangingFaces.Scheme" = None,
        guard: str = "true",
        additional_includes: str = "",
    ):
        """!

        @param patch_overlap: peano4.api.datamodel.Patch
          The face data which the action set interpolates. The data of the
          coarse face are read, the data of the hanging face are written.

        @param scheme: InterpolateHangingFaces.Scheme
          Interpolation scheme. Defaults to Scheme.Linear.

        @param guard: str
          C++ boolean expression over the hanging face's
          peano4::datamanagement::FaceMarker marker.

        @param additional_includes: str
          Further include statements, e.g. required by the guard.

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        assert patch_overlap.dim[0] % 2 == 0, (
            "Patch associated to face has to have even number of volumes along "
            "the normal. Otherwise, it is not a symmetric overlap."
        )
        self.scheme = (
            InterpolateHangingFaces.Scheme.Linear if scheme is None else scheme
        )
        assert isinstance(self.scheme, InterpolateHangingFaces.Scheme), self.scheme
        self.d = {
            "UNKNOWNS": patch_overlap.no_of_unknowns,
            "DOFS_PER_AXIS": patch_overlap.dim[1],
            "OVERLAP": patch_overlap.dim[0] // 2,
            "FACE_NAME": patch_overlap.name,
            "SCHEME": self.scheme.value,
        }
        self.guard = guard
        self.additional_includes = additional_includes

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return "  return std::vector< peano4::grid::GridControlEvent >();\n"

    def get_body_of_prepareTraversal(self):
        return "\n"

    def get_body_of_unprepareTraversal(self):
        return "\n"

    def get_body_of_operation(self, operation_name):
        if operation_name == ActionSet.OPERATION_CREATE_HANGING_FACE:
            return self.TemplateCreateHangingFace.render(**self.d)
        return ""

    def get_guard_of_operation(self, operation_name):
        if operation_name == ActionSet.OPERATION_CREATE_HANGING_FACE:
            return self.guard
        return "true"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_attributes(self):
        return ""

    def get_includes(self):
        return (
            """
#include "toolbox/blockstructured/Interpolation.h"
"""
            + self.additional_includes
        )
