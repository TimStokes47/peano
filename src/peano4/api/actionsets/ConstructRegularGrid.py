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

        @param cell_size: float
          Upper bound on the edge length of the cells of the regular grid.
          Peano refines every cell whose edge length is greater than or equal
          to cell_size, i.e. the resulting cells are strictly smaller than
          cell_size. Peano always refines the root cell at least once.

        """
        super().__init__(descend_invocation_order, parallel)
        self.cell_size = cell_size

    def user_should_modify_template(self):
        return False

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return f"""
const std::vector< peano4::grid::GridControlEvent > result = {{
  peano4::grid::GridControlEvent(
    peano4::grid::GridControlEvent::RefinementControl::Refine,
    tarch::la::Vector<Dimensions,double>( -0.5 * std::numeric_limits<double>::max() ),
    tarch::la::Vector<Dimensions,double>( std::numeric_limits<double>::max() ),
    tarch::la::Vector<Dimensions,double>( {self.cell_size} )
  )
}};
return result;
"""

    def get_includes(self):
        return """
#include <limits>
"""

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
