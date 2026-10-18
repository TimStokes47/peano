# This file is part of the ExaParty project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.api
import peano4.api.toolbox.particles

from .TaskGraphKernelWrappers import (
    construct_touch_vertex_first_time_call,
    construct_touch_cell_first_time_call,
    construct_touch_vertex_last_time_call,
)


class UpdateParticle_MultiLevelInteraction_Sets(
    peano4.api.toolbox.particles.UpdateParticle_MultiLevelInteraction_Sets
):
    """!
    
    Slight modification of toolbox's update particle action set
    
    The interesting stuff is actually happing in 
    TaskGarphKernelWrappers.py however.
    
    """
    
    def __init__(
        self,
        particle_set,
        particle_particle_interaction_kernel,
        touch_vertex_first_time_compute_particle_update_kernel,
        touch_vertex_last_time_compute_particle_update_kernel,
        prepare_traversal_kernel,
        unprepare_traversal_kernel,
        additional_includes,
        active_particle_set,
        alter_particle_position_or_interaction_radius,
        wait_for_all_tasks_to_finish_after_traversal,
    ):
        """!

        Construct


        @param wait_for_all_tasks_to_finish_after_traversal: Boolean
          If this one is set, we insert a wait into the unprepare traversal
          event. Each rank then waits for all tasks to be completed before
          it continues. We have a strong synchronisation point.

        @param alter_particle_position_or_interaction_radius: Boolean
          Set if the underlying AlgorithmStep's effect might change a
          particle's position or interaction radius.
          @see exaparty.api.particle.AlgorithmStep.AlgorithmStep.may_trigger_rerun()

        """
        if wait_for_all_tasks_to_finish_after_traversal:
            unprepare_traversal_kernel = (
                unprepare_traversal_kernel
                + """
              tarch::multicore::waitForAllTasks();
              """
            )

        super(UpdateParticle_MultiLevelInteraction_Sets, self).__init__(
            particle_set,
            particle_particle_interaction_kernel=construct_touch_cell_first_time_call(
                particle_set,
                particle_particle_interaction_kernel,
                use_multilevel_dependencies=True
            ),
            touch_vertex_first_time_compute_particle_update_kernel=construct_touch_vertex_first_time_call(
                particle_set,
                touch_vertex_first_time_compute_particle_update_kernel,
                use_multilevel_dependencies=True
            ),
            touch_vertex_last_time_compute_particle_update_kernel=construct_touch_vertex_last_time_call(
                particle_set,
                touch_vertex_last_time_compute_particle_update_kernel,
                use_multilevel_dependencies=True,
                alter_particle_position_or_interaction_radius=alter_particle_position_or_interaction_radius
            ),
            prepare_traversal_kernel=prepare_traversal_kernel,
            unprepare_traversal_kernel=unprepare_traversal_kernel,
            additional_includes=additional_includes
            + """
#include "tarch/multicore/Task.h"
#include "exaparty/exaparty.h"                
#include "exaparty/TaskNumber.h"                
                 """,
            active_particle_set=active_particle_set,
        )

    def get_body_of_operation(self, operation_name):
        if operation_name == peano4.api.solversteps.ActionSet.OPERATION_BEGIN_TRAVERSAL:
            return "_pendingDependencies.clear();"
        else:
            return super(
                UpdateParticle_MultiLevelInteraction_Sets, self
            ).get_body_of_operation(operation_name)

    def get_attributes(self):
        return (
            super(UpdateParticle_MultiLevelInteraction_Sets, self).get_attributes()
            + """
          ::exaparty::PendingDependencies  _pendingDependencies;
        """
        )

    def get_action_set_name(self):
        return __name__.replace(".py", "").replace(".", "_")
