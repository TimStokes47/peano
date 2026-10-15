# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.api.solversteps.ActionSet import ActionSet


class SpawnCellTaskActionSet(ActionSet):
    """!

    Producer-side wrapper around an action set, which spawns touchCellFirstTime() as a task

    The wrapper delegates all events to the wrapped action set with the
    following exceptions:

    - touchCellFirstTime() evaluates the wrapped action set's guard. If the
      guard holds, the wrapper takes a new task number from a
      tarch::multicore::TaskEnumerator, stores it in the cell's task label
      and spawns the wrapped action set's body as a task with this number.
      If the guard does not hold, the task label is set to
      tarch::multicore::TaskEnumerator::NoNumber.
    - unprepareTraversal() and all touch...LastTime() events are empty.
      They are moved into the consumer, i.e. into
      @ref peano4.api.taskgraph.compiler.WaitForCellTaskActionSet.WaitForCellTaskActionSet.
    - createCell() furthermore initialises the task label with NoNumber.

    The spawned task captures its environment by value ([=]). Cell data
    managed via smart pointers (e.g.
    @ref peano4.api.datamodel.PatchToDoubleArrayWithSmartPointer) is copied
    shallowly, i.e. the task writes into the cell's data. Plain arrays are
    copied, and enumerators over vertices and faces hold pointers into
    Peano's stacks, which are invalid once touchCellFirstTime() has
    terminated. @ref peano4.api.taskgraph.compiler.CellTaskProducerConsumer
    warns about such cases.

    The task number enumerator is a static attribute of the generated class.
    The consumer-side action set derives from the generated class, i.e.
    shares all static attributes with it.

    """

    def __init__(
        self,
        action_set: ActionSet,
        task_labels_name: str,
        task_number_attribute_name: str,
        previous_task_number_attribute_name: str,
    ):
        """!

        @param action_set: peano4.api.solversteps.ActionSet
          The wrapped action set.

        @param task_labels_name: str
          Name of the DaStGen2 cell data type holding the task labels. The
          generated observer passes it into the event as fineGridCell<name>.

        @param task_number_attribute_name: str or None
          Name of the attribute within the task labels which holds the task
          number. If None, touchCellFirstTime() spawns no task but is
          delegated to the wrapped action set, too.

        @param previous_task_number_attribute_name: str or None
          Name of the attribute holding the task number of the task spawned
          by the previous action set of the same producer. The spawned task
          depends on this task, so the action sets' bodies are executed in
          their original order. If None, the task has no incoming
          dependencies.

        """
        super().__init__(descend_invocation_order=action_set.descend_invocation_order, parallel=action_set.parallel)
        self.action_set = action_set
        self._task_labels_name = task_labels_name
        self._task_number_attribute_name = task_number_attribute_name
        self._previous_task_number_attribute_name = previous_task_number_attribute_name

    MovedOperations = [
        ActionSet.OPERATION_TOUCH_VERTEX_LAST_TIME,
        ActionSet.OPERATION_TOUCH_FACE_LAST_TIME,
        ActionSet.OPERATION_TOUCH_CELL_LAST_TIME,
    ]

    def _accessor(self, attribute_name: str) -> str:
        return attribute_name[0].capitalize() + attribute_name[1:]

    def get_constructor_body(self):
        return self.action_set.get_constructor_body()

    def get_destructor_body(self):
        return self.action_set.get_destructor_body()

    def get_static_initialisations(self, full_qualified_classname):
        return (
            self.action_set.get_static_initialisations(full_qualified_classname)
            + "tarch::multicore::TaskEnumerator "
            + full_qualified_classname
            + "::_taskEnumerator;\n"
        )

    def get_body_of_createGridControlEventsForNextTraversal(self):
        return self.action_set.get_implementation_of_createGridControlEventsForNextTraversal()

    def get_body_of_prepareTraversal(self):
        return self.action_set.get_implementation_of_prepareTraversal()

    def get_body_of_unprepareTraversal(self):
        return "\n"

    def get_body_of_operation(self, operation_name):
        # Not used, as get_implementation_of_operation() is overwritten
        return ""

    def get_implementation_of_operation(self, operation_name):
        """!

        Delegate to the wrapped action set besides the events listed in the
        class description

        """
        task_label = f"fineGridCell{self._task_labels_name}"
        if operation_name in SpawnCellTaskActionSet.MovedOperations:
            return ""
        elif operation_name == ActionSet.OPERATION_CREATE_CELL and self._task_number_attribute_name is not None:
            return (
                self.action_set.get_implementation_of_operation(operation_name)
                + f"\n  {task_label}.set{self._accessor(self._task_number_attribute_name)}( tarch::multicore::TaskEnumerator::NoNumber );\n"
            )
        elif operation_name == ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME and self._task_number_attribute_name is not None:
            if self._previous_task_number_attribute_name is None:
                in_dependencies = "tarch::multicore::NoInDependencies"
            else:
                previous_task_number = f"{task_label}.get{self._accessor(self._previous_task_number_attribute_name)}()"
                in_dependencies = f"{previous_task_number}>=0 ? std::set<tarch::multicore::TaskNumber>{{ {previous_task_number} }} : tarch::multicore::NoInDependencies"
            return f"""
  {task_label}.set{self._accessor(self._task_number_attribute_name)}( tarch::multicore::TaskEnumerator::NoNumber );
  if ({self.action_set.get_guard_of_operation(operation_name)}) {{
    const int taskNumber = _taskEnumerator.getNumber();
    {task_label}.set{self._accessor(self._task_number_attribute_name)}( taskNumber );
    tarch::multicore::spawnTask(
      new tarch::multicore::TaskWithCopyOfFunctor(
        tarch::multicore::Task::DontFuse,
        tarch::multicore::Task::DefaultPriority,
        [=]() mutable {{
{self.action_set.get_body_of_operation(operation_name)}
        }}
      ),
      {in_dependencies},
      taskNumber
    );
  }}
"""
        else:
            return self.action_set.get_implementation_of_operation(operation_name)

    def get_action_set_name(self):
        return self.action_set.get_action_set_name()

    def user_should_modify_template(self):
        return False

    def get_attributes(self):
        return (
            self.action_set.get_attributes()
            + """
    static tarch::multicore::TaskEnumerator  _taskEnumerator;
"""
        )

    def get_includes(self):
        return (
            self.action_set.get_includes()
            + """
#include <set>
#include "tarch/multicore/multicore.h"
#include "tarch/multicore/Task.h"
#include "tarch/multicore/TaskEnumerator.h"
"""
        )
