# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class ApplyPointWiselyToPatch(ActionSet):
    """!

    Apply a C++ snippet to each entry of a cell's patch

    The action set hooks into touchCellFirstTime() and runs over all
    NxNxN entries (volumes or grid points) of the patch. For each entry, it
    runs implementation, i.e. a C++ snippet which may use

    - Q: a pointer (double*) to the patch.no_of_unknowns doubles of the
      entry, which the snippet may read and write;
    - x: the position (tarch::la::Vector<Dimensions,double>) of the entry,
      i.e. the centre of the volume if we interpret the patch as N^d
      volumes which tessellate the cell.

    A typical use case is the postprocessing of a solution, e.g. the
    enforcement of algebraic constraints or the clipping of unphysical
    values.

    As the action set plugs into touchCellFirstTime(), it runs after all
    action sets of tasks it depends on which plug into touchCellFirstTime()
    as well, and before all action sets plugging into touchCellLastTime()
    such as toolbox.blockstructured.api.ProjectPatchOntoFaces, if the task
    graph compiler fuses the tasks into one mesh traversal.

    """

    def __init__(
        self,
        patch,
        implementation: str,
        guard: str = "true",
        additional_includes: str = "",
    ):
        """!

        @param patch: peano4.api.datamodel.Patch
          The cell's patch. It has to be a cube, i.e. have the same number of
          entries along each axis.

        @param implementation: str
          C++ snippet that is applied to each entry, @see class
          documentation for the variables it may use.

        @param guard: str
          C++ boolean expression. The action set only processes cells for
          which the guard holds.

        @param additional_includes: str
          Includes that the snippet needs.

        """
        super(ApplyPointWiselyToPatch, self).__init__(
            descend_invocation_order=1, parallel=False
        )
        assert patch.dim[0] == patch.dim[1], "Can only handle square patches"
        self._patch = patch
        self._implementation = implementation
        self._guard = guard
        self.additional_includes = additional_includes

    @property
    def guard(self):
        return self._guard

    @guard.setter
    def guard(self, value):
        self._guard = value

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return "  return std::vector< peano4::grid::GridControlEvent >();\n"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_body_of_operation(self, operation_name):
        if operation_name != ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return "\n"
        entries_per_axis = self._patch.dim[0]
        unknowns = self._patch.no_of_unknowns
        return f"""
  const tarch::la::Vector<Dimensions, double> entryH = marker.h() / {entries_per_axis}.0;
  dfor(entry, {entries_per_axis}) {{
    tarch::la::Vector<Dimensions, double> x;
    for (int d = 0; d < Dimensions; d++) {{
      x(d) = marker.x()(d) - 0.5 * marker.h()(d) + (entry(d) + 0.5) * entryH(d);
    }}
    double* __restrict__ Q = fineGridCell{self._patch.name}.value + peano4::utils::dLinearised(entry, {entries_per_axis}) * {unknowns};
    {{
      {self._implementation}
    }}
  }}
"""

    def get_guard_of_operation(self, operation_name):
        if operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return self._guard
        return "true"

    def get_attributes(self):
        return ""

    def get_includes(self):
        return (
            """
#include "peano4/utils/Loop.h"
"""
            + self.additional_includes
        )
