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
    refine or keep based on cell_size alone. That is what makes it
    "generic" - solver-independent - and hence a candidate to sit in
    peano4.api.actionsets rather than one of the discretisation- or
    extension-specific actionsets packages (e.g. exahype2.api.fv.actionsets).

    The resulting C++ file will be really simplistic, basically consisting of
    only a few lines of code in the touchVertexFirstTime operation, the beginTraversal()
    event and the routine that returns the grid control events.

    """

    def __init__(self, cell_size: float, descend_invocation_order=0, parallel=True):
        """!

        cell_size: float
          Target edge length of one cell on the regular grid this action
          set is supposed to build.

        """
        super().__init__(descend_invocation_order, parallel)
        self.cell_size = cell_size

    def user_should_modify_template(self):
        return False

    def get_constructor_body(self):
        return "// @todo Not implemented yet\n"

    def get_destructor_body(self):
        return "// @todo Not implemented yet\n"

    def get_body_of_getGridControlEvents(self):
        return "return _gridControlEvents;"

    def get_body_of_operation(self, operation_name):
        if operation_name==ActionSet.OPERATION_BEGIN_TRAVERSAL:
            return """
  _gridControlEvents.clear();
"""
        if operation_name==ActionSet.OPERATION_TOUCH_VERTEX_FIRST_TIME:
            return """
            /**
    refinementCriterion = ::exahype2::RefinementCommand::Keep;
  }
  {% endif %}
    
  _localRefinementControl.addCommand( marker.x(), marker.h(), refinementCriterion, {{EVENT_LIFETIME}} );
  logTraceOutWith1Argument( "touchCellFirstTime(...)", toString(refinementCriterion) );
**/
"""
        return ""

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def get_attributes(self):
        return """
std::vector< peano4::grid::GridControlEvent > _gridControlEvents;
"""

    def get_includes(self):
        return ""
