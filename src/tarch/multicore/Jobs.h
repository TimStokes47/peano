// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_JOBS_H_
#define _TARCH_MULTICORE_JOBS_H_


#include <functional>


namespace tarch {
  namespace multicore {
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
    namespace jobs {
       enum class BackgroundJobType {
         BackgroundJob,
		 /**
		  * Task implies that there are no dependencies.
		  */
		 IsTaskAndRunAsSoonAsPossible,
         LongRunningBackgroundJob,
		 /**
		  * It does not really make sense to specify this flag by a user.
		  * But it is used internally if background threads are disabled.
		  */
		 ProcessImmediately
       };

       /**
        * Effectively switches off all background tasks. This could lead to
        * deadlocks!
        */
       constexpr int DontUseAnyBackgroundJobs            = -1;
       /*
        * Standard background tasks are never enqueued. Instead, we process them
        * immediately.
        */
       constexpr int ProcessNormalBackgroundJobsImmediately = -2;

       /**
        * Abstract superclass of background jobs.
        */
       class BackgroundJob {
         public:
           const BackgroundJobType _jobType;

           static int  _maxNumberOfRunningBackgroundThreads;
         public:
           BackgroundJob( BackgroundJobType jobType );
           /**
            * Background jobs can interrupt if they want. They then should
            * return true telling the job system that they need to be rerun.
            *
            * @return Shall be rescheduled again
            */
           virtual bool run() = 0;
           virtual ~BackgroundJob();
           bool isLongRunning() const;
           BackgroundJobType getJobType() const;

           /**
            * If jobs are enqueued, they are typically not processed
            * immediately (unless we run on a single core machine), but Peano
            * puts them into a queue. This queue then is processed by
            * tasks/threads/whatever they are called. The maximum nmber of
            * these guys that work themselves through the queue of jobs is
            * set by this routine. By default, there is no upper number, so
            * you might end up with all your cores doing background jobs.
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
           static void setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads);
       };

       /**
        * Frequently used helper class to write background jobs for functors.
        */
       class GenericBackgroundJobWithCopyOfFunctor: public BackgroundJob {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<bool()>   _functor;
         public:
           GenericBackgroundJobWithCopyOfFunctor(const std::function<bool()>& functor, BackgroundJobType jobType );

           bool run() override;

           virtual ~GenericBackgroundJobWithCopyOfFunctor();
       };

       template <typename T>
       class GenericBackgroundWithPointer: public BackgroundJob {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   T*   _functor;
         public:
    	   GenericBackgroundWithPointer(T* functor, BackgroundJobType jobType ):
    		  BackgroundJob(jobType),
    		  _functor(functor) {
    	   }

           bool run() override {
             return (*_functor)();
           }

           virtual ~GenericBackgroundWithPointer() {
             delete _functor;
           }
       };

       /**
        * Ownership for pointer goes to multicore component, i.e. the multicore
        * component deletes the task once it has finished.
        */
       void spawnBackgroundJob(BackgroundJob* task);

       /**
        * Work through the background tasks and let the caller know whether some
        * tasks have been processed.
        */
       bool processBackgroundJobs();

       /**
        * This is the logical number of background tasks, i.e. how many things
        * could, in theory, run the the background.
        */
       int getNumberOfWaitingBackgroundJobs();

       /**
        * Abstract super class for a job. Job class is an integer. A job may
        * depend on input data from other jobs and may write out data to
        * another job as well (through a shared memory region protected by
        * a semaphore). However, it should never exchange information with
        * another job of the same class.
        */
       class Job {
         private:
    	   const bool _isTask;
    	   const int  _jobClass;
         public:
    	   /**
    	    * A task is a job without any dependencies on other jobs though it
    	    * might have children. Tasks form a tree structure. Jobs may form
    	    * a DAG.
    	    */
    	   Job( bool isTask, int jobClass );
           virtual void run() = 0;
           virtual ~Job();
           bool isTask() const;
           int getClass() const;
       };

       /**
        * Frequently used implementation for job with a functor.
        */
       class GenericJobWithCopyOfFunctor: public Job {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<void()>   _functor;
         public:
           GenericJobWithCopyOfFunctor(const std::function<void()>& functor, bool isTask, int jobClass  );

           void run() override;

           virtual ~GenericJobWithCopyOfFunctor();
       };

       /**
        * Frequently used implementation for job with a functor.
        */
       class GenericJobWithoutCopyOfFunctor: public Job {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<void()>&   _functor;
         public:
           GenericJobWithoutCopyOfFunctor(std::function<void()>& functor, bool isTask, int jobClass );

           void run() override;

           virtual ~GenericJobWithoutCopyOfFunctor();
       };


       template <typename T>
       class GenericJobWithPointer: public Job {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   T*   _functor;
         public:
    	   GenericJobWithPointer(T* functor, bool isTask, int jobClass  ):
             Job(isTask,jobClass),
             _functor(functor)  {
    	   }


           void run() override {
             (*_functor)();
           }

           virtual ~GenericJobWithPointer() {
        	 delete _functor;
           }
       };

       /**
        * Kick out a new job. The job's type has to be set properly: It
        * has to be clear whether the job is a job or even a task, i.e. a
        * special type of job. In the latter case, a runtime system could
        * deploy the pointer to a different thread and return immediately.
        *
        * Ownership goes over to Peano's job namespace, i.e. you don't have
        * to delete the pointer.
        */
       void spawn(Job*  job);

       /**
        * Wrapper around other spawn operation.
        */
       void spawn(std::function<void()>& job, bool isTask, int jobClass);

       void spawnAndWait(
         std::function<void()>&  job0,
         std::function<void()>&  job1,
		 bool                    isTask0,
		 bool                    isTask1,
		 int                     jobClass0,
		 int                     jobClass1
       );

       void spawnAndWait(
         std::function<void()>& job0,
         std::function<void()>& job1,
         std::function<void()>& job2,
		 bool                    isTask0,
		 bool                    isTask1,
		 bool                    isTask2,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2
       );

       void spawnAndWait(
         std::function<void()>& job0,
         std::function<void()>& job1,
         std::function<void()>& job2,
         std::function<void()>& job3,
		 bool                    isTask0,
		 bool                    isTask1,
		 bool                    isTask2,
		 bool                    isTask3,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2,
		 int                     jobClass3
       );

       void spawnAndWait(
         std::function<void()>& job0,
         std::function<void()>& job1,
         std::function<void()>& job2,
         std::function<void()>& job3,
         std::function<void()>& job4,
		 bool                    isTask0,
		 bool                    isTask1,
		 bool                    isTask2,
		 bool                    isTask3,
		 bool                    isTask4,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2,
		 int                     jobClass3,
		 int                     jobClass4
       );

       void spawnAndWait(
         std::function<void()>& job0,
         std::function<void()>& job1,
         std::function<void()>& job2,
         std::function<void()>& job3,
         std::function<void()>& job4,
         std::function<void()>& job5,
		 bool                    isTask0,
		 bool                    isTask1,
		 bool                    isTask2,
		 bool                    isTask3,
		 bool                    isTask4,
		 bool                    isTask5,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2,
		 int                     jobClass3,
		 int                     jobClass4,
		 int                     jobClass5
       );

       int getNumberOfPendingJobs();

       /**
        * Handle only jobs of one job class.
        */
       bool processJobs(int jobClass);

       /**
        * Process any pending job. This includes background jobs.
        */
       bool processJobs();
    }
  }
}

#endif

