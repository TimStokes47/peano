# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from typing import List

from .ActionSet import ActionSet


class SynchroniseActionSets(ActionSet):
    """!

    Marker which ends a group of action sets that run in parallel

    The generated observer invokes the action sets of a step one by one.
    Consecutive action sets with parallel=True form a task group instead:
    Per event, their code is spawned as one task per action set, and the
    observer waits for all of these tasks before it continues
    (@ref peano4.api.solversteps.SynchroniseActionSets.get_task_groups()).
    An action set with parallel=False ends a group, too, but it is then
    invoked itself.

    This marker ends a group without invoking anything. Use it to separate
    two task groups which follow each other directly. Without the marker,
    they would merge into one group, i.e. action sets of the second group
    might run concurrently to the ones of the first group although they
    depend on them. @ref peano4.api.taskgraph.compiler.FuseSteps inserts
    such markers.

    The marker is ignored when we generate the C++ code: It yields neither
    an action set class nor any code in the observer. Append it directly to
    a step's action_sets. Do not use
    @ref peano4.api.solversteps.Step.Step.add_action_set(), as this routine
    sorts the action sets according to their descend_invocation_order, i.e.
    it might move the marker.

    """

    def __init__(self):
        super().__init__(descend_invocation_order=0, parallel=False)

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_prepareTraversal(self):
        return ""

    def get_body_of_unprepareTraversal(self):
        return ""

    def get_body_of_operation(self, operation_name):
        return ""

    def get_action_set_name(self):
        return "SynchroniseActionSets"

    def user_should_modify_template(self):
        return False

    def get_attributes(self):
        return ""

    def get_includes(self):
        return ""


def get_task_groups(action_sets: List[ActionSet]) -> List[List[ActionSet]]:
    """!

    Split a sequence of action sets into groups which are invoked one after
    the other

    We run through the action sets in their order. Consecutive action sets
    with parallel=True form one group, i.e. they may run in parallel. An
    action set with parallel=False forms a group of its own and ends the
    current group. A
    @ref peano4.api.solversteps.SynchroniseActionSets.SynchroniseActionSets
    ends the current group, too, but does not show up in any group.

    For the sequence V1, V2, V3, V4, V5 with V1, V2, V4 and V5 being
    parallel, we get the groups [V1, V2], [V3] and [V4, V5]: The observer
    spawns V1 and V2 as tasks and waits for them, then invokes V3, and
    finally spawns V4 and V5 and waits for them.

    @param action_sets: [peano4.api.solversteps.ActionSet]
      Action sets in the order in which they are invoked.

    @return [[peano4.api.solversteps.ActionSet]]
      Groups in invocation order. No group is empty.

    """
    groups = []
    current_group = None
    for action_set in action_sets:
        if isinstance(action_set, SynchroniseActionSets):
            current_group = None
        elif action_set.parallel and current_group is not None:
            current_group.append(action_set)
        else:
            current_group = [action_set]
            groups.append(current_group)
            if not action_set.parallel:
                current_group = None
    return groups
