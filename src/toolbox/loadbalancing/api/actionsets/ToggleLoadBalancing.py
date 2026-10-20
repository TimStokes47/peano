# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class ToggleLoadBalancing(ActionSet):
    """!

    Switch the load balancing on or off

    Calls enable(true) or enable(false) on the strategy that is currently
    installed in toolbox::loadbalancing::LoadBalancingService, either in
    prepareTraversal(), i.e. before the grid sweep, or in
    unprepareTraversal(), i.e. after the grid sweep. Both routines are
    invoked once per traversal per rank (and not per tree).

    The action set does not install a strategy itself. If no strategy is
    installed, i.e. getLoadBalancing() returns nullptr, the action set does
    nothing.

    """

    def __init__(self, enable_load_balancing: bool, plug_into_prepare_traversal: bool):
        """!

        @param enable_load_balancing: bool
          Switch the load balancing on (True) or off (False).

        @param plug_into_prepare_traversal: bool
          Toggle the load balancing in prepareTraversal() (True), i.e.
          before the grid sweep, or in unprepareTraversal() (False), i.e.
          after the grid sweep.

        """
        super(ToggleLoadBalancing, self).__init__(descend_invocation_order=0, parallel=False)
        self.enable_load_balancing = enable_load_balancing
        self.plug_into_prepare_traversal = plug_into_prepare_traversal

    def _toggle(self):
        return f"""
  if (::toolbox::loadbalancing::LoadBalancingService::getInstance().getLoadBalancing() != nullptr) {{
    ::toolbox::loadbalancing::LoadBalancingService::getInstance().getLoadBalancing()->enable({"true" if self.enable_load_balancing else "false"});
  }}
"""

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_prepareTraversal(self):
        return self._toggle() if self.plug_into_prepare_traversal else "\n"

    def get_body_of_unprepareTraversal(self):
        return "\n" if self.plug_into_prepare_traversal else self._toggle()

    def get_body_of_operation(self, operation_name):
        return "\n"

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")

    def user_should_modify_template(self):
        return False

    def get_attributes(self):
        return ""

    def get_includes(self):
        return """
#include "toolbox/loadbalancing/AbstractLoadBalancing.h"
#include "toolbox/loadbalancing/LoadBalancingService.h"
"""
