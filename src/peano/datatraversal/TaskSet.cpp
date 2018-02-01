#include "peano/datatraversal/TaskSet.h"
#include "peano/performanceanalysis/Analysis.h"


tarch::logging::Log  peano::datatraversal::TaskSet::_log( "peano::datatraversal::TaskSet" );


#if defined(SharedTBB) || defined(SharedTBBInvade)
#include <tbb/parallel_invoke.h>


tbb::task_group peano::datatraversal::TaskSet::_loadCellsTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_loadVerticesTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_triggerEventsTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_storeCellsTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_storeVerticesTaskGroup;
#endif


#if defined(SharedTBBInvade)
#include "shminvade/SHMInvade.h"
#endif



void peano::datatraversal::TaskSet::waitForAllLoadCellsTasks() {
  #if defined(SharedTBB) || defined(SharedTBBInvade)
  _loadCellsTaskGroup.wait();
  #endif
}


void peano::datatraversal::TaskSet::waitForAllLoadVerticesTasks() {
  #if defined(SharedTBB) || defined(SharedTBBInvade)
  _loadVerticesTaskGroup.wait();
  #endif
}


void peano::datatraversal::TaskSet::waitForAllEventTasks() {
  #if defined(SharedTBB) || defined(SharedTBBInvade)
  _triggerEventsTaskGroup.wait();
  #endif
}


void peano::datatraversal::TaskSet::waitForAllStoreCellsTasks() {
  #if defined(SharedTBB) || defined(SharedTBBInvade)
  _storeCellsTaskGroup.wait();
  #endif
}


void peano::datatraversal::TaskSet::waitForAllStoreVerticesTasks() {
  #if defined(SharedTBB) || defined(SharedTBBInvade)
  _storeVerticesTaskGroup.wait();
  #endif
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&& function1,
  std::function<void ()>&& function2,
  TaskType                 type1,
  TaskType                 type2,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(2,2);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    #if defined(SharedTBBInvade)
    shminvade::SHMInvade invade(2);
    #endif
    tbb::parallel_invoke(
      function1,
	  function2
	);
    #if defined(SharedTBBInvade)
    invade.retreat();
    #endif
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
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(3,3);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    #if defined(SharedTBBInvade)
    shminvade::SHMInvade invade(3);
    #endif
    tbb::parallel_invoke(
      function1,
	  function2,
	  function3
    );
    #if defined(SharedTBBInvade)
    invade.retreat();
    #endif
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
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  TaskType                 type4,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(4,4);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    #if defined(SharedTBBInvade)
    shminvade::SHMInvade invade(4);
    #endif
    tbb::parallel_invoke(
      function1,
	  function2,
	  function3,
	  function4
    );
    #if defined(SharedTBBInvade)
    invade.retreat();
    #endif

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
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  TaskType                 type4,
  TaskType                 type5,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(4,4);
    #if defined(SharedTBB) || defined(SharedTBBInvade)
    #if defined(SharedTBBInvade)
    shminvade::SHMInvade invade(5);
    #endif
    tbb::parallel_invoke(
      function1,
	  function2,
	  function3,
	  function4,
	  function5
    );
    #if defined(SharedTBBInvade)
    invade.retreat();
    #endif
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
