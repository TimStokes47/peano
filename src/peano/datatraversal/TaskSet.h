// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_TASK_SET_H_
#define _PEANO_DATA_TRAVERSAL_TASK_SET_H_

#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/logging/Log.h"


#include <functional>


#ifdef SharedTBB
#include <tbb/task_group.h>
#endif


namespace peano {
  namespace datatraversal {
    class TaskSet;
  }
}


/**
 * Task Set
 *
 * This is my very simple wrapper of task parallelism in Peano. The class
 * provides only one type of constructor. This constructor is passed a set of
 * instances representing one functor each. The constructor executes all of
 * them in parallel (if this is the user's wish) and returns. Consequently,
 * this wrapper is the right choice iff all the task are totally independent
 * of each other.
 *
 * The standard variants do not copy the task you hand in and I expect the
 * functors to be handed in as references. If the tasks are not independent
 * of each other, they thus might induce data races.
 *
 * Please see the documentation of the constructor with only one argument for
 * further details, as this one behaves differently.
 *
 * <h2> Issuing background tasks </h2>
 *
 * If you spawn a task, I cannot know when it is actually executed. As a
 * result, I never can rely where data is at the very moment the task then is
 * actually triggered. Notably if you directly try to access vertices/cells,
 * they might already have been moved to a different memory location. So what
 * you have to do is that you store the data you wanna manipulate on heaps as
 * data on heaps doesn't move. And then you equip your tasks with pointers
 * (indices) to the heap. These pointers/indices are copied but the heap index
 * itself remains valid, so it doesn't really matter to you if the vertex
 * holding pointers too moves around.
 *
 * If you want to get data back from a background tasks, you have to take into
 * account a few properties:
 * - The task you hand over to the TaskSet constructor is not directly executed.
 *   Instead, the runtime system makes a copy of your functor/task, stores it
 *   safely away and executes this one. That means once you hand over a
 *   functor, you can safely destroy this object. The tasking system has
 *   created a copy of its own.
 * - The actual task copy that the runtime system uses is destroyed right after
 *   it has terminated. There's no way to plug into its result. If you need the
 *   task to return data, my recommendation is that you
 *   - create some data on the heap
 *   - make your task hold a pointer (index) to this heap
 *   - write a task that writes its result into these heap entries
 *   The heap entry then later can be read by the mappings, e.g.
 *
 * Please note that you notably should have a boolean flag somewhere that
 * indicates whether your task has terminated. I often store entries with
 * an enum on a heap. The enum encodes the states BackGroundTaskSpawned and
 * BackGroundTaskCompleted. I do protect the access to these enums with one
 * global semaphore to ensure that no two threads read/write it
 * simultaneously. If a task is spawned, the corresponding value on the heap is
 * set to BackGroundTaskSpawned. The operator() of the task sets the value to
 * BackGroundTaskCompleted as very last action. In a mapping where I need the
 * task result, I add a while loop that looks whether the task has already
 * terminated:
 *
 * <pre>
bool taskHasTerminated = false;
while (!taskHasTerminated) {
  tarch::multicore::Lock myLock(_globalBackgroundTaskStatusSemphore);
  taskHasTerminated = MyStatusHeap.getIndex(...).getStatus()==BackGroundTaskCompleted;
  myLock.free();
}
</pre>
 *
 *
 * <h2> Number of background tasks </h2>
 *
 * I did occasionally run into situations where too many background tasks made the
 * overall system starve. So you can restrict the number of background tasks now
 * manually. The system queues all tasks and then checks whether a background task
 * is active already. If not, it launches a consumer task.
 *
 * You  can control the number of background tasks.
 *
 *
 * @author Tobias Weinzierl
 */
class peano::datatraversal::TaskSet {
  public:
    enum class TaskType {
      /**
       * Is deployed to background.
       */
  	  RunAsSoonAsPossible,
  	  RunImmediately,
	  LoadCells,
	  LoadVertices,
	  TriggerEvents,
	  StoreCells,
	  StoreVertices,
  	  Background,
  	  LongRunningBackground,
  	  PersistentBackground
    };
  private:
    static tarch::logging::Log  _log;

    #ifdef SharedTBB
    static tbb::task_group _genericTaskGroup;
    static tbb::task_group _loadCellsTaskGroup;
	static tbb::task_group _loadVerticesTaskGroup;
	static tbb::task_group _triggerEventsTaskGroup;
	static tbb::task_group _storeCellsTaskGroup;
	static tbb::task_group _storeVerticesTaskGroup;

	static tbb::task_group&  getTaskGroup( TaskType type );
    #endif
  public:
    /**
     * Spawn One Asynchronous Task
     *
     * Different to other tasks, I have to copy the functor here. Otherwise,
     * this operation might return, the calling code might destroy the functor,
     * and the asynchronous task then tries to invoke it. This would result in
     * a seg fault.
     *
     * As a consequence, you have to very carefully when the destructor is not
     * empty. Task objects might be copied multiple times and you never know
     * which destructor is the one belonging to the task object that is really
     * executed.
     *
     * <h2> TBB </h2>
     *
     * I do not use the spawn command of TBB here but the enqueue. In
     * particular on Windows systems, I often encounter starvation processes if
     * a load vertices or store vertices task splits very often and spawns too
     * many task children due to this operation.
     *
     * <h2> Deadlocks </h2>
     *
     * Please note that your code might deadlock if you spawn a task without
     * multicore support and if you hope/rely on the fact that this task cannot
     * complete right at the moment but will later on be able to do so.
     *
     * <h2> Lambda calculus </h2>
     *
     * In principle, you can simply pass in a lambda expression instead of a
     * defined functor, but I often struggle with this. I can solve it by
     * constructing a function object explicitly:
     *
     * <pre>
        std::function<void ()> myFunctor = [=] () {
          // do something
        };

        peano::datatraversal::TaskSet backgroundTask(myFunctor,true);
       </pre>
     *
     * It is important that myFunctor catches everything via copy. As a
     * consequence do something only includes static and const accessors.
     *
     * Note: This interface is only used for tasks that do not have to run
     * persistently (in the background) for a very long time. It is only
     * to be used for tasks that either are reasonably short.
     */
    template <class Functor>
    inline TaskSet(
      Functor&   task,
      TaskType   taskType
    );

    /**
     * Invoke operations in parallel. Works fine with lambda
     * expressions:
     *
  peano::datatraversal::TaskSet runParallelTasks(
    [&]() -> void {

    ...
    },
    [&]() -> void {

    ...
    },
    true
  );
     *
     * Please do not invoke any background threads through this operation.
     */
    TaskSet(
      std::function<void ()>&& function1,
      std::function<void ()>&& function2,
	  TaskType                 taskType1,
	  TaskType                 taskType2,
      bool                     parallelise
    );

    TaskSet(
      std::function<void ()>&& function1,
      std::function<void ()>&& function2,
      std::function<void ()>&& function3,
	  TaskType                 taskType1,
	  TaskType                 taskType2,
	  TaskType                 taskType3,
      bool                     parallelise
    );

    TaskSet(
      std::function<void ()>&& function1,
      std::function<void ()>&& function2,
      std::function<void ()>&& function3,
      std::function<void ()>&& function4,
	  TaskType                 taskType1,
	  TaskType                 taskType2,
	  TaskType                 taskType3,
	  TaskType                 taskType4,
      bool                     parallelise
    );

    TaskSet(
      std::function<void ()>&& function1,
      std::function<void ()>&& function2,
      std::function<void ()>&& function3,
      std::function<void ()>&& function4,
      std::function<void ()>&& function5,
	  TaskType                 taskType1,
	  TaskType                 taskType2,
	  TaskType                 taskType3,
	  TaskType                 taskType4,
	  TaskType                 taskType5,
      bool                     parallelise
    );

    /**
     * The name is slightly wrong. We actually wait for all load cell tasks
     * spawned by the original load cell task (root) to complete. We cannot
     * wait for the original task, as this task might have been triggered
     * together with an other task triggering this wait. Which would inevitably
     * lead into a deadlock.
     */
    static void waitForAllLoadCellsTasks();
	static void waitForAllLoadVerticesTasks();
	static void waitForAllEventTasks();
	static void waitForAllStoreCellsTasks();
	static void waitForAllStoreVerticesTasks();

};


#include "peano/datatraversal/TaskSet.cpph"

#endif

