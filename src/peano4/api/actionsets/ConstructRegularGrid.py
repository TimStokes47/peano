# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps import ActionSet


class ConstructRegularGrid(ActionSet):
    """!

    Refine the mesh - solver-agnostic - down to a regular grid of a given
    resolution.

    Unlike exahype2.api.fv.actionsets.AdaptivityCriterion, this action set
    does not evaluate any solver data (there is none yet while the grid is
    still being built): it only ever looks at a cell's size and decides to
    refine or keep based on volume_size alone. That is what makes it
    "generic" - solver-independent - and hence a candidate to sit in
    peano4.api.actionsets rather than one of the discretisation- or
    extension-specific actionsets packages (e.g. exahype2.api.fv.actionsets).

    Currently just an empty stub - a placeholder for the actual refinement
    criterion to grow into. @see get_body_of_operation().

    """

    def __init__(self, volume_size: float, descend_invocation_order=0, parallel=True):
        """!

        volume_size: float
          Target edge length of one Finite Volume on the regular grid this
          action set is supposed to build.

        """
        super().__init__(descend_invocation_order, parallel)
        self.volume_size = volume_size

    def user_should_modify_template(self):
        return False

    def get_constructor_body(self):
        return "// @todo Not implemented yet\n"

    def get_destructor_body(self):
        return "// @todo Not implemented yet\n"

    def get_body_of_getGridControlEvents(self):
        return "return std::vector< peano4::grid::GridControlEvent >();\n"

    def get_body_of_operation(self, operation_name):
        return "// @todo Not implemented yet\n"

    def get_action_set_name(self):
        return "ConstructRegularGrid"

    def get_attributes(self):
        return """
return std::vector< peano4::grid::GridControlEvent >();\n
"""

    def get_includes(self):
        return ""
