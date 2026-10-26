# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import enum

import jinja2

from peano4.api.solversteps.ActionSet import ActionSet


class InterpolateAndRestrictCells(ActionSet):
    """!

    Initialise the patches of new cells from their parent, and hand the
    patches of erased cells over to their parent

    The action set realises the data transfer of a dynamically adaptive
    mesh for patches which live on the finest mesh level only:

    - If a mesh sweep refines a cell, Peano creates its 3^d children.
      createCell() interpolates the parent's patch into each child's patch
      (::toolbox::blockstructured::interpolateCell_AoS_...). The parent's
      patch is still available in this sweep, as the parent has not been
      refined when the sweep has started.
    - If a mesh sweep erases the children of a cell, the parent becomes a
      leaf again. touchCellFirstTime() clears the parent's patch, and
      destroyCell() then restricts each child's patch into the parent's patch
      (::toolbox::blockstructured::restrictCell_AoS_...). The averaging
      accumulates the children's contributions, so the parent has to be
      cleared first.

    The patch is a NxNxN patch, i.e. N volumes per axis.

    """

    class Interpolation(enum.Enum):
        """!

        Interpolation schemes

        The value of each entry is the suffix of the C++ routine
        ::toolbox::blockstructured::interpolateCell_AoS_<suffix>().

        - PiecewiseConstant copies the coarse value into all fine volumes
          which it covers.
        - Linear interpolates d-linearly. Along the boundary of the coarse
          patch, it extrapolates constantly, as linear extrapolation can
          produce unphysical values such as negative densities next to steep
          gradients.

        """

        PiecewiseConstant = "piecewise_constant"
        Linear = "linear_with_constant_extrapolation"

    class Restriction(enum.Enum):
        """!

        Restriction schemes

        The value of each entry is the suffix of the C++ routine
        ::toolbox::blockstructured::restrictCell_AoS_<suffix>().

        - Averaging sets each coarse volume to the average of the 3^d fine
          volumes it covers.

        """

        Averaging = "averaging"

    TemplateCreateCell = jinja2.Template(
        """
  logTraceInWith1Argument( "createCell(...)---InterpolateAndRestrictCells", marker.toString() );
  ::toolbox::blockstructured::interpolateCell_AoS_{{INTERPOLATION}}(
    marker,
    {{DOFS_PER_AXIS}},
    {{UNKNOWNS}},
    coarseGridCell{{CELL_NAME}}.value,
    fineGridCell{{CELL_NAME}}.value
  );
  logTraceOut( "createCell(...)---InterpolateAndRestrictCells" );
"""
    )

    TemplateTouchCellFirstTime = jinja2.Template(
        """
  if ( marker.hasBeenRefined() and not marker.willBeRefined() ) {
    logTraceInWith1Argument( "touchCellFirstTime(...)---InterpolateAndRestrictCells", marker.toString() );
    ::toolbox::blockstructured::clearCell(
      marker,
      {{DOFS_PER_AXIS}},
      {{UNKNOWNS}},
      fineGridCell{{CELL_NAME}}.value
    );
    logTraceOut( "touchCellFirstTime(...)---InterpolateAndRestrictCells" );
  }
"""
    )

    TemplateDestroyCell = jinja2.Template(
        """
  logTraceInWith1Argument( "destroyCell(...)---InterpolateAndRestrictCells", marker.toString() );
  ::toolbox::blockstructured::restrictCell_AoS_{{RESTRICTION}}(
    marker,
    {{DOFS_PER_AXIS}},
    {{UNKNOWNS}},
    fineGridCell{{CELL_NAME}}.value,
    coarseGridCell{{CELL_NAME}}.value
  );
  logTraceOut( "destroyCell(...)---InterpolateAndRestrictCells" );
"""
    )

    def __init__(
        self,
        patch,
        interpolation: "InterpolateAndRestrictCells.Interpolation" = None,
        restriction: "InterpolateAndRestrictCells.Restriction" = None,
        additional_includes: str = "",
    ):
        """!

        @param patch: peano4.api.datamodel.Patch
          The cell data which the action set interpolates and restricts.

        @param interpolation: InterpolateAndRestrictCells.Interpolation
          Interpolation scheme. Defaults to Interpolation.Linear.

        @param restriction: InterpolateAndRestrictCells.Restriction
          Restriction scheme. Defaults to Restriction.Averaging.

        @param additional_includes: str
          Further include statements.

        """
        super().__init__(descend_invocation_order=1, parallel=False)
        self.interpolation = (
            InterpolateAndRestrictCells.Interpolation.Linear
            if interpolation is None
            else interpolation
        )
        self.restriction = (
            InterpolateAndRestrictCells.Restriction.Averaging
            if restriction is None
            else restriction
        )
        assert isinstance(
            self.interpolation, InterpolateAndRestrictCells.Interpolation
        ), self.interpolation
        assert isinstance(
            self.restriction, InterpolateAndRestrictCells.Restriction
        ), self.restriction
        self.d = {
            "UNKNOWNS": patch.no_of_unknowns,
            "DOFS_PER_AXIS": patch.dim[0],
            "CELL_NAME": patch.name,
            "INTERPOLATION": self.interpolation.value,
            "RESTRICTION": self.restriction.value,
        }
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
        if operation_name == ActionSet.OPERATION_CREATE_CELL:
            return self.TemplateCreateCell.render(**self.d)
        if operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self.TemplateTouchCellFirstTime.render(**self.d)
        if operation_name == ActionSet.OPERATION_DESTROY_CELL:
            return self.TemplateDestroyCell.render(**self.d)
        return ""

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
#include "toolbox/blockstructured/Restriction.h"
"""
            + self.additional_includes
        )
