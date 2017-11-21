#include "peano/datatraversal/TaskSet.h"
#include "peano/performanceanalysis/Analysis.h"
#include "peano/performanceanalysis/Analysis.h"


tarch::logging::Log  peano::datatraversal::TaskSet::_log( "peano::datatraversal::TaskSet" );


int                  peano::datatraversal::TaskSet::_maxNumberOfRunningBackgroundThreads(1);



#if defined(SharedTBB) || defined(SharedTBBInvade)
#include <tbb/parallel_invoke.h>


tbb::task_group_context                 peano::datatraversal::TaskSet::_backgroundTaskContext;
tbb::atomic<int>                        peano::datatraversal::TaskSet::_numberOfRunningBackgroundThreads(0);

tbb::concurrent_queue<peano::datatraversal::TaskSet::BackgroundTask*>  peano::datatraversal::TaskSet::_backgroundTasks;


void peano::datatraversal::TaskSet::kickOffBackgroundTask(BackgroundTask* task) {
  _backgroundTasks.push(task);
  peano::performanceanalysis::Analysis::getInstance().fireAndForgetBackgroundTask(1);


  const int currentlyRunningBackgroundThreads = _numberOfRunningBackgroundThreads;
  if (
    currentlyRunningBackgroundThreads<_maxNumberOfRunningBackgroundThreads
    ||
    task->isLongRunning()
  ) {
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "no consumer task running yet; kick off" );
    if ( !task->isLongRunning() ) {
      _numberOfRunningBackgroundThreads.fetch_and_add(1);
    }
    ConsumerTask* tbbTask = new(tbb::task::allocate_root(_backgroundTaskContext)) ConsumerTask();
    tbb::task::enqueue(*tbbTask);
    _backgroundTaskContext.set_priority(tbb::priority_low);
    logDebug( "kickOffBackgroundTask(BackgroundTask*)", "it is out now" );
  }
}


peano::datatraversal::TaskSet::ConsumerTask::ConsumerTask() {
}



tbb::task* peano::datatraversal::TaskSet::ConsumerTask::execute() {
  processBackgroundTasks();
  return 0;
}
#endif


void peano::datatraversal::TaskSet::processBackgroundTasks() {
  #if defined(SharedTBB) || defined(SharedTBBInvade)
  logDebug( "execute()", "background consumer task becomes awake" );

  BackgroundTask* myTask = nullptr;
  bool gotOne = _backgroundTasks.try_pop(myTask);
  bool taskHasBeenLongRunning = false;
  while (gotOne) {
    logDebug( "execute()", "consumer task found job to do" );
    peano::performanceanalysis::Analysis::getInstance().terminatedBackgroundTask(1);
    myTask->run();
    taskHasBeenLongRunning = myTask->isLongRunning();
    delete myTask;
    gotOne = taskHasBeenLongRunning ? false : _backgroundTasks.try_pop(myTask);
  }

  if (!taskHasBeenLongRunning) {
    _numberOfRunningBackgroundThreads.fetch_and_add(-1);
  }

  logDebug( "execute()", "background task consumer is done and kills itself" );
  #endif
}


void peano::datatraversal::TaskSet::setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads) {
  assertion(maxNumberOfRunningBackgroundThreads>=1);
  _maxNumberOfRunningBackgroundThreads = maxNumberOfRunningBackgroundThreads;
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&& function1,
  std::function<void ()>&& function2,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(2,2);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    tbb::parallel_invoke(
      function1,
      function2
    );
    #elif SharedOMP
      #pragma omp parallel sections
      {
        #pragma omp section
        function1();
        #pragma omp section
        function2();
      }
    #else
    function1();
    function2();
    #endif
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(-2,-2);
  }
  else {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,2);
    function1();
    function2();
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,-2);
  }
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&& function1,
  std::function<void ()>&& function2,
  std::function<void ()>&& function3,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(3,3);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    tbb::parallel_invoke(
      function1,
      function2,
      function3
    );
    #elif SharedOMP
      #pragma omp parallel sections
      {
        #pragma omp section
        function1();
        #pragma omp section
        function2();
        #pragma omp section
        function3();
      }
    #else
    function1();
    function2();
    function3();
    #endif
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(-3,-3);
  }
  else {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,3);
    function1();
    function2();
    function3();
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,-3);
  }
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&& function1,
  std::function<void ()>&& function2,
  std::function<void ()>&& function3,
  std::function<void ()>&& function4,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(4,4);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    tbb::parallel_invoke(
      function1,
      function2,
      function3,
      function4
    );
    #elif SharedOMP
      #pragma omp parallel sections
      {
        #pragma omp section
        function1();
        #pragma omp section
        function2();
        #pragma omp section
        function3();
        #pragma omp section
        function4();
      }
    #else
    function1();
    function2();
    function3();
    function4();
    #endif
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(-4,-4);
  }
  else {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,4);
    function1();
    function2();
    function3();
    function4();
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,-4);
  }
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&& function1,
  std::function<void ()>&& function2,
  std::function<void ()>&& function3,
  std::function<void ()>&& function4,
  std::function<void ()>&& function5,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(4,4);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    tbb::parallel_invoke(
      function1,
      function2,
      function3,
      function4,
      function5
    );
    #elif SharedOMP
      #pragma omp parallel sections
      {
        #pragma omp section
        function2();
        #pragma omp section
        function3();
        #pragma omp section
        function4();
        #pragma omp section
        function5();
      }
    #else
    function1();
    function2();
    function3();
    function4();
    function5();
    #endif
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(-4,-4);
  }
  else {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,4);
    function1();
    function2();
    function3();
    function4();
    function5();
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,-4);
  }
}
