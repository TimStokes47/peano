// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_JOBS_H_
#define _TARCH_MULTICORE_JOBS_H_


#include <functional>


namespace tarch {
  namespace multicore {
    /**
     * Jobs are Peano's abstraction of tasks. However, they generalise the
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
         PersistentBackgroundJob,
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

       class BackgroundJob {
         public:
           const BackgroundJobType _jobType;

           static int  _maxNumberOfRunningBackgroundThreads;
         public:
           BackgroundJob( BackgroundJobType jobType );
           virtual void run() = 0;
           virtual ~BackgroundJob();
           bool isLongRunning() const;
           BackgroundJobType getJobType() const;

           /**
            * By default, we disable all background tasks. Background
            * tasks are passed into TBB via enqueue and they seem to cause problems.
            * You can however re-enable them by invoking this routine. If background
            * tasks are enabled they try to concurrently work through the queue of
            * tasks spawned into the background until the queue is empty. Once it is
            * empty, the corresponding task terminates.
            *
            * The default setting of the invasive component equals calling this
            * routine with a value of -1.
            *
            * @todo Documentation needs revision
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

       class GenericBackgroundJobWithCopyOfFunctor: public BackgroundJob {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<void()>   _functor;
         public:
           GenericBackgroundJobWithCopyOfFunctor(const std::function<void()>& functor, BackgroundJobType jobType );

           void run() override;

           virtual ~GenericBackgroundJobWithCopyOfFunctor();
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

       void spawn(Job*  job);
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
        * Process any pending job. This includes background threads.
        */
       bool processJobs();
    }
  }
}

#endif

