# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class LoadBalancing(ActionSet):
    """!

    Inform the load balancing about the end of a grid sweep

    Add this action set to every solver step that should drive the load
    balancing. Its unprepareTraversal() calls finishStep() on the strategy
    that is currently installed in
    toolbox::loadbalancing::LoadBalancingService. As unprepareTraversal()
    is invoked once per traversal per rank (and not per tree), the strategy
    is informed exactly once after each grid sweep.

    The action set does not install a strategy itself. Some other code, the
    main routine typically, hands one over via
    toolbox::loadbalancing::LoadBalancingService::setLoadBalancing(). If
    no strategy is installed, i.e. getLoadBalancing() returns nullptr, the
    action set does nothing.

    If constructed with enable_load_balancing=True, the action set first
    switches the strategy on (enable(true)) and then calls finishStep(). The
    time stepping schemes use this variant in their last mesh sweep, as they
    switch the load balancing off throughout the time step
    (toolbox.loadbalancing.api.actionsets.ToggleLoadBalancing).

    """

    def __init__(self, enable_load_balancing: bool = False):
        """!

        @param enable_load_balancing: bool
          Switch the load balancing on right before finishStep() is called.

        """
        super(LoadBalancing, self).__init__(descend_invocation_order=0, parallel=False)
        self.enable_load_balancing = enable_load_balancing

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_prepareTraversal(self):
        return "\n"

    def get_body_of_unprepareTraversal(self):
        enable = (
            "    ::toolbox::loadbalancing::LoadBalancingService::getInstance().getLoadBalancing()->enable(true);\n"
            if self.enable_load_balancing
            else ""
        )
        return f"""
  if (::toolbox::loadbalancing::LoadBalancingService::getInstance().getLoadBalancing() != nullptr) {{
{enable}    ::toolbox::loadbalancing::LoadBalancingService::getInstance().getLoadBalancing()->finishStep();
  }}
"""

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
