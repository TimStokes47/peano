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

    def __init__(
        self,
        cell_size: float,
        descend_invocation_order=0,
        parallel=True,
        guard: str = "true",
        includes: str = "",
    ):
        """!

        @param cell_size: float
          Target edge length of one cell on the regular grid this action
          set is supposed to build.

        @param guard: str
          C++ boolean expression. The action set only returns its
          refinement instruction for the next traversal if the expression
          holds. Otherwise, it does not ask for any refinement, i.e. the
          mesh construction pauses. The expression is evaluated in
          createGridControlEventsForNextTraversal(). By default, the action
          set always refines.

        @param includes: str
          Include statements required by guard.

        """
        super().__init__(descend_invocation_order, parallel)
        self.cell_size = cell_size
        self.guard = guard
        self.includes = includes

    def user_should_modify_template(self):
        return False

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return f"""
if ( not ({self.guard}) ) {{
  return std::vector< peano4::grid::GridControlEvent >();
}}

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
""" + self.includes

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
