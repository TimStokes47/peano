# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api.taskgraph

from .LoadBalancing import LoadBalancing


class TriggerLoadBalancing(peano4.api.taskgraph.Task):
    """!

    Task which informs the load balancing about the end of a grid sweep

    A GLOBAL task (@ref peano4.api.taskgraph.Task.Task.GridEntity) holding
    one LoadBalancing action set, i.e. it calls finishStep() on the strategy
    installed in toolbox::loadbalancing::LoadBalancingService once the
    traversal it ends up in has finished. As it does not touch any mesh
    data, task graph compilers which fuse tasks append it to the traversal
    of its last dependency (@ref peano4.api.taskgraph.compiler.FuseSteps.translate()).
    Make it depend on the last task(s) of your task graph.

    @param name: str
      Name of the task.

    """

    def __init__(self, name: str = "TriggerLoadBalancing"):
        super(TriggerLoadBalancing, self).__init__(name=name, entity=peano4.api.taskgraph.Task.GridEntity.GLOBAL)
        self.add_action_set(LoadBalancing())
