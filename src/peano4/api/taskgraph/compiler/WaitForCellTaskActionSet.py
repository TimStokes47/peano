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
    - unprepareTraversal() and all touch...LastTime() events are those of the
      action set wrapped by the producer. They are moved from the producer
      into the consumer, i.e. they are invoked once the tasks have
      terminated.
    - createCell() initialises the task label with NoNumber.

    All other events are empty.

    The generated C++ class derives from the generated class of the
    producer-side action set
    (@ref peano4.api.solversteps.ActionSet.ActionSet.get_base_class()). It
    hence shares all static attributes with the producer, e.g. reduction
    variables which unprepareTraversal() evaluates, and the task enumerator.
    It does not declare any attributes itself. Non-static attributes exist,
    but they are not preserved from the producer's sweep: The producer's
    action set objects are destroyed after the producer's mesh traversal,
    and the consumer's objects are constructed anew.

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

    def get_base_class(self):
        """!

        The generated class of the producer-side action set. The name is
        only known once the producer's step has been translated into C++
        (@ref peano4.api.solversteps.StepToActionSets.StepToActionSets.construct_output()).

        """
        return self._producer_action_set.generated_class_name

    def get_constructor_body(self):
        return ""

    def get_destructor_body(self):
        return ""

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return "  return std::vector< peano4::grid::GridControlEvent >();\n"

    def get_body_of_prepareTraversal(self):
        return "\n"

    def get_body_of_unprepareTraversal(self):
        return self._producer_action_set.action_set.get_implementation_of_unprepareTraversal()

    def get_body_of_operation(self, operation_name):
        # Not used, as get_implementation_of_operation() is overwritten
        return ""

    def get_implementation_of_operation(self, operation_name):
        if operation_name in SpawnCellTaskActionSet.MovedOperations:
            return self._producer_action_set.action_set.get_implementation_of_operation(operation_name)
        elif operation_name == ActionSet.OPERATION_CREATE_CELL:
            task_label, accessor = self._task_label_accessor()
            return f"\n  {task_label}.set{accessor}( tarch::multicore::TaskEnumerator::NoNumber );\n"
        elif operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
            task_label, accessor = self._task_label_accessor()
            return f"""
  if ( {task_label}.get{accessor}()>=0 ) {{
    tarch::multicore::waitForTask( {task_label}.get{accessor}() );
    _taskEnumerator.releaseNumber( {task_label}.get{accessor}() );
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
        return f"""
#include "{self._producer_action_set.generated_header_file}"
#include "tarch/multicore/multicore.h"
#include "tarch/multicore/TaskEnumerator.h"
"""
