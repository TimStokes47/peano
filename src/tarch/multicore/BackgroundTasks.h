// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_BACKGROUND_TASKS_H_
#define _TARCH_MULTICORE_BACKGROUND_TASKS_H_

namespace tarch {
  namespace multicore {
    enum class TaskType {
      ExecuteImmediately,
	  Default,
	  LongRunning,
	  Persistent
    };

    enum class MaxNumberOfRunningBackgroundThreads {
      /**
       * Still allows the multicore sublayer to invoke a background task if
       * there are tasks which are known to run really long.
       */
      DontUseBackgroundTasksForNormalTasks = 0,
      /**
       * Effectively switches off all background tasks. If the user submits any
       * background tasks, they are only enqueued. Once a thread says
       * sendTaskToBackground(), it first runs through all the pending tasks
       * before it really goes to the background (or continues).
       */
      DontUseAnyBackgroundTasks            = -1,
      /*
       * No background task is ever enqueued. Instead, we process them
       * immediately. This could lead to deadlocks!
       */
      ProcessBackgroundTasksImmediately    = -2,
      /**
       * Help which is not to be used.
       */
      SmallestValue                        = -3
    };

    /**
     * @see peano::datatraversal::TaskSet
     */
    class BackgroundTask {
      public:
        const TaskType _isLongRunning;

        static int  _maxNumberOfRunningBackgroundThreads;
      public:
        BackgroundTask( TaskType isLongRunning ):
        _isLongRunning(
          _maxNumberOfRunningBackgroundThreads==static_cast<int>(MaxNumberOfRunningBackgroundThreads::ProcessBackgroundTasksImmediately)
		  &&
		  isLongRunning != TaskType::Persistent ?
          TaskType::ExecuteImmediately : isLongRunning
        ) {}
        virtual void run() = 0;
        virtual ~BackgroundTask() {}
        bool isLongRunning() const {return _isLongRunning==TaskType::LongRunning;}
        TaskType getTaskType() const {return _isLongRunning;}
    };

    template <class Functor>
    class GenericTaskWithCopy: public BackgroundTask {
      private:
        /**
         * See the outer class description for an explanation why this is an
         * attribute, i.e. why we copy the functor here always.
         */
        Functor   _functor;
      public:
        GenericTaskWithCopy(const Functor& functor, TaskType isLongRunning ):
          BackgroundTask(isLongRunning),
          _functor(functor)  {
        }

        void run() override {
          _functor();
        }

        virtual ~GenericTaskWithCopy() {}
    };

    /**
     * Ownership for pointer goes to multicore component, i.e. the multicore
     * component deletes the task once it has finished.
     */
    void spawnBackgroundTask(BackgroundTask* task);

    /**
     * Work through the background tasks and let the caller know whether some
     * tasks have been processed.
     */
    bool processBackgroundTasks();

    /**
     * By default, we disable all background tasks in SHMInvade. Background
     * tasks are passed into TBB via enqueue and they seem to cause problems.
     * You can however re-enable them by invoking this routine. If background
     * tasks are enabled they try to concurrently work through the queue of
     * tasks spawned into the background until the queue is empty. Once it is
     * empty, the corresponding task terminates.
     *
     * The default setting of the invasive component equals calling this
     * routine with a value of -1.
     *
     * @param maxNumberOfRunningBackgroundThreads -1 Switch off any tasks
     *   running in the background.
     * @param maxNumberOfRunningBackgroundThreads 0 Do not use background tasks
     *   unless the user instructs the component that this background task is a
     *   very long running task. If a long-lasting task is issued, the component
     *   launches a task for it specifically.
     * @param maxNumberOfRunningBackgroundThreads >0 Enable the code to use up
     *   to a certain number of background tasks. Usually, 1 or any small
     *   number should be sufficient, where small is to be read relative to the
     *   threads available. You don't want your system to spend all of its tasks
     *   onto background activities at any time.
     * @see MaxNumberOfRunningBackgroundThreads
     */
    void setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads);

    /**
     * Wrapper around setMaxNumberOfRunningBackgroundThreads(int).
     */
    void setMaxNumberOfRunningBackgroundThreads(const MaxNumberOfRunningBackgroundThreads& maxNumberOfRunningBackgroundThreads);

    /**
     * This is the logical number of background tasks, i.e. how many things
     * could, in theory, run the the background.
     */
    int getNumberOfWaitingBackgroundTasks();
  }
}

#endif
