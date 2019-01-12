// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_TASKS_H_
#define _TARCH_MULTICORE_TASKS_H_


#include <functional>
#include <vector>


namespace tarch {
  namespace multicore {

    /**
     * Tells task/thread to yield, i.e. to allow other tasks/threads to run.
     * Typically to be used within busy-waiting/polling loops.
     */
    void yield();

    /**
     * Jobs are Peano's abstraction of tasks. They generalise the
     * term tasks. A task in Peano's notion follows Intel's TBB and is an
     * atomic unit. That is, it may have children which have to be processed
     * first, but it can not be interrupted and it does not depend on anybody
     * besides its children. Notably, the only way to interrupt a task is to
     * spawn children which implies that the task stops immediately, waits
     * for the children and then returns. Peano's jobs are a generalisation:
     * they may depend on other jobs. If they do not, a job is equivalent to
     * a task.
     */
       /**
        * Abstract super class for a job.
        */
       class Task {
         public:
           virtual bool run() = 0;
           /**
            * This operation is called prior to run(). We try to make it as
            * close to run as possible. The idea is that codes use it to
            * insert their prefetch macros. Peano gives no guarantee that this
            * operation is called, i.e. everything done here is solely
            * optional.
            *
            * Prefetching for Intel is specified at
            *
            * https://software.intel.com/en-us/node/684213
            *
            * or
            *
            * https://software.intel.com/en-us/cpp-compiler-developer-guide-and-reference-cacheability-support-intrinsics-1
            *
            * There are basically the following modes: _MM_HINT_T0,
            * _MM_HINT_T1, _MM_HINT_T2 and _MM_HINT_NTA. T0 means a
            * prefetch into all cache levels. T1 means into all levels
            * except L1 (L1 might be populated but we do not enforce it),
            * T2 means at least L3. NTA is the most defensive one.
            * It loads stuff as close to the core as possible without
            * polluting any other caches.
            *
            * We usually use mode/hint _MM_HINT_NTA for data on machines with
            * Optane. This makes the data be loaded into L3. For urgent
            * computations it might however make sense to use T0.
            *
            */
           virtual void prefetch() = 0;
       };

       /**
        * Frequently used implementation for job with a functor.
        */
       class TaskWithCopyOfFunctor: public Task {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<bool()>   _taskFunctor;
         public:
    	   TaskWithCopyOfFunctor(
    	     const std::function<bool()>& taskFunctor
           );

           bool run() override;
           void prefetch() override;
       };

       /**
        * Frequently used implementation for job with a functor.
        */
       class TaskWithoutCopyOfFunctor: public Task {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<bool()>&   _taskFunctor;
         public:
    	   TaskWithoutCopyOfFunctor(
    	     std::function<bool()>& taskFunctor
    	   );

           bool run() override;
           void prefetch() override;
       };
       
       /**
        * @return There have been tasks
        */
       bool processPendingTasks();

       /**
        * Kick out a new job. The job's type has to be set properly: It
        * has to be clear whether the job is a job or even a task, i.e. a
        * special type of job. See JobType.
        *
        * Ownership goes over to Peano's job namespace, i.e. you don't have
        * to delete the pointer.
        */
      void spawnTask(Task*  job);
      void spawnHighBandwidthTask(Task*  job);
      void spawnHighPriorityTask(Task*  job);

      void spawnAndWait(
        std::function<bool()>&  job0,
        std::function<bool()>&  job1
      );

      /**
       * The routine deletes all the passed arguments.
       */
      void spawnAndWait(
        const std::vector< Task* >&  tasks
      );

/*
       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3,
         std::function<bool()>&  job4
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3,
         std::function<bool()>&  job4,
         std::function<bool()>&  job5
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3,
         std::function<bool()>&  job4,
         std::function<bool()>&  job5,
         std::function<bool()>&  job6,
         std::function<bool()>&  job7,
         std::function<bool()>&  job8,
         std::function<bool()>&  job9,
         std::function<bool()>&  job10,
         std::function<bool()>&  job11
       );
*/
/*

       bool processBackgroundJobs(int maxNumberOfJobs = 1);
       bool processHighPriorityJobs(int maxNumberOfJobs = 1);
       bool processHighBandwidthJobs(int maxNumberOfJobs = 1);
*/
  }
}

#endif

