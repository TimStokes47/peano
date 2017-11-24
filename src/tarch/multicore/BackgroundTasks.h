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

    void setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads);
  }
}

#endif
