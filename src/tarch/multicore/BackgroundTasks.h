// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_BACKGROUND_TASKS_H_
#define _TARCH_MULTICORE_BACKGROUND_TASKS_H_

namespace tarch {
  namespace multicore {
    /**
     * @see peano::datatraversal::TaskSet
     */
    class BackgroundTask {
      private:
        const bool _isLongRunning;
      public:
        BackgroundTask( bool isLongRunning ): _isLongRunning(isLongRunning) {}
        virtual void run() = 0;
        virtual ~BackgroundTask() {}
        bool isLongRunning() const {return _isLongRunning;}
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
        GenericTaskWithCopy(const Functor& functor,  bool isLongRunning ):
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
     */
    void setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads);

    /**
     * This is the logical number of background tasks, i.e. how many things
     * could, in theory, run the the background.
     */
    int getNumberOfWaitingBackgroundTasks();
  }
}

#endif
