# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet

from .SpawnCellTaskActionSet import SpawnCellTaskActionSet


class WaitForCellTaskActionSet(ActionSet):
    """!

    Consumer-side cousin of a @ref peano4.api.taskgraph.compiler.SpawnCellTaskActionSet.SpawnCellTaskActionSet

    The cousin realises the following events:

    - touchCellFirstTime() checks if the cell's task label holds a valid
      task number. If so, it waits for this task, returns the number to
      the task enumerator, and resets the label to
      tarch::multicore::TaskEnumerator::NoNumber.
    - createCell() initialises the task label with NoNumber.

    All other events are empty. The task enumerator is a static attribute of
    the task labels, i.e. the cousin returns the number to the enumerator
    from which the producer took it.

    """

    def __init__(
        self,
        producer_action_set: SpawnCellTaskActionSet,
    ):
        """!

        @param producer_action_set: peano4.api.taskgraph.compiler.SpawnCellTaskActionSet.SpawnCellTaskActionSet
          The producer-side action set this cousin belongs to.

        """
        super().__init__(
            descend_invocation_order=producer_action_set.descend_invocation_order,
            parallel=producer_action_set.parallel,
        )
        self._producer_action_set = producer_action_set

    def _task_label_accessor(self) -> tuple:
        attribute_name = self._producer_action_set._task_number_attribute_name
        return (
            f"fineGridCell{self._producer_action_set._task_labels_name}",
            attribute_name[0].capitalize() + attribute_name[1:],
        )

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return "  return std::vector< peano4::grid::GridControlEvent >();\n"

    def get_body_of_prepareTraversal(self):
        return "\n"

    def get_body_of_unprepareTraversal(self):
        return "\n"

    def get_body_of_operation(self, operation_name):
        # Not used, as get_implementation_of_operation() is overwritten
        return ""

    def get_implementation_of_operation(self, operation_name):
        task_label, accessor = self._task_label_accessor()
        if operation_name == ActionSet.OPERATION_CREATE_CELL:
            return f"\n  {task_label}.set{accessor}( tarch::multicore::TaskEnumerator::NoNumber );\n"
        elif operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            return f"""
  if ( {task_label}.get{accessor}()>=0 ) {{
    tarch::multicore::waitForTask( {task_label}.get{accessor}() );
    {task_label}.getTaskEnumerator().releaseNumber( {task_label}.get{accessor}() );
    {task_label}.set{accessor}( tarch::multicore::TaskEnumerator::NoNumber );
  }}
"""
        else:
            return ""

    def get_action_set_name(self):
        return self._producer_action_set.get_action_set_name() + "_WaitForTask"

    def user_should_modify_template(self):
        return False

    def get_attributes(self):
        return ""

    def get_includes(self):
        return """
#include "tarch/multicore/multicore.h"
#include "tarch/multicore/TaskEnumerator.h"
"""
