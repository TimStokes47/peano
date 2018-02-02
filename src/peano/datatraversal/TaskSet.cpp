#include "peano/datatraversal/TaskSet.h"
#include "peano/performanceanalysis/Analysis.h"
#include "tarch/multicore/BackgroundTasks.h"


#include <chrono>
#include <thread>

tarch::logging::Log  peano::datatraversal::TaskSet::_log( "peano::datatraversal::TaskSet" );


#if defined(SharedTBB) || defined(SharedTBBInvade)
#include <tbb/parallel_invoke.h>


tbb::task_group peano::datatraversal::TaskSet::_genericTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_loadCellsTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_loadVerticesTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_triggerEventsTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_storeCellsTaskGroup;
tbb::task_group peano::datatraversal::TaskSet::_storeVerticesTaskGroup;


tbb::task_group&  peano::datatraversal::TaskSet::getTaskGroup( TaskType type ) {
  switch ( type ) {
    case TaskType::RunAsSoonAsPossible:
    case TaskType::RunImmediately:
        return _genericTaskGroup;
    case TaskType::LoadCells:
        return _loadCellsTaskGroup;
    case TaskType::LoadVertices:
        return _loadVerticesTaskGroup;
    case TaskType::TriggerEvents:
        return _triggerEventsTaskGroup;
    case TaskType::StoreCells:
        return _storeCellsTaskGroup;
    case TaskType::StoreVertices:
      return _storeVerticesTaskGroup;
    case TaskType::Background:
    case TaskType::LongRunningBackground:
    case TaskType::PersistentBackground:
      assertion(false);
      return _genericTaskGroup;
  }
  return _genericTaskGroup;
}
#endif


#if defined(SharedTBBInvade)
#include "shminvade/SHMInvade.h"
#endif



// @todo the wait does not work. Actually, it would even trigger an assertion if
//       there are no tasks. Thanks god, I do veto assertions.
void peano::datatraversal::TaskSet::waitForAllLoadCellsTasks() {
  logDebug( "waitForAllLoadCellsTasks()", "start to wait for load cell tasks" );
  #if defined(SharedTBB) || defined(SharedTBBInvade)
//  _loadCellsTaskGroup.wait();
  _loadCellsTaskGroup.run_and_wait( []()->void {} );
  #endif
  logDebug( "waitForAllLoadCellsTasks()", "wait for load cell tasks finished" );
}


void peano::datatraversal::TaskSet::waitForAllLoadVerticesTasks() {
  logDebug( "waitForAllLoadVerticesTasks()", "start to wait for load vertices tasks" );
  #if defined(SharedTBB) || defined(SharedTBBInvade)
/*
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  _loadVerticesTaskGroup.wait();
*/
  _loadVerticesTaskGroup.run_and_wait( []()->void {} );
  #endif
  logDebug( "waitForAllLoadVerticesTasks()", "wait for load vertices tasks finished" );
}


void peano::datatraversal::TaskSet::waitForAllEventTasks() {
  logDebug( "waitForAllEventTasks()", "start to wait for event tasks" );
  #if defined(SharedTBB) || defined(SharedTBBInvade)
/*
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  _triggerEventsTaskGroup.wait();
*/
  _triggerEventsTaskGroup.run_and_wait( []()->void {} );
  #endif
  logDebug( "waitForAllEventTasks()", "wait for event tasks finished" );
}


void peano::datatraversal::TaskSet::waitForAllStoreCellsTasks() {
  logDebug( "waitForAllStoreCellsTasks()", "start to wait for store cell tasks" );
  #if defined(SharedTBB) || defined(SharedTBBInvade)
/*
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  _storeCellsTaskGroup.wait();
*/
  _storeCellsTaskGroup.run_and_wait( []()->void {} );
  #endif
  logDebug( "waitForAllStoreCellsTasks()", "wait for store cell tasks finished" );
}


void peano::datatraversal::TaskSet::waitForAllStoreVerticesTasks() {
  logDebug( "waitForAllStoreVerticesTasks()", "start to wait for store vertices tasks" );
  #if defined(SharedTBB) || defined(SharedTBBInvade)
/*
  std::this_thread::sleep_for(std::chrono::milliseconds(2));
  _storeVerticesTaskGroup.wait();
*/
  _storeVerticesTaskGroup.run_and_wait( []()->void {} );
  #endif
  logDebug( "waitForAllStoreVerticesTasks()", "wait for store vertices tasks finished" );
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
/*
      function1,
	  function2
*/
      [type1,function1] () -> void {
    	getTaskGroup(type1).run_and_wait( function1 );
      },
      [type2,function2] () -> void {
    	getTaskGroup(type2).run_and_wait( function2 );
      }
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
      [type1,function1] () -> void {
    	getTaskGroup(type1).run_and_wait( function1 );
      },
      [type2,function2] () -> void {
    	getTaskGroup(type2).run_and_wait( function2 );
      },
      [type3,function3] () -> void {
    	getTaskGroup(type3).run_and_wait( function3 );
      }
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
      [type1,function1] () -> void {
    	getTaskGroup(type1).run_and_wait( function1 );
      },
      [type2,function2] () -> void {
    	getTaskGroup(type2).run_and_wait( function2 );
      },
      [type3,function3] () -> void {
    	getTaskGroup(type3).run_and_wait( function3 );
      },
      [type4,function4] () -> void {
    	getTaskGroup(type4).run_and_wait( function4 );
      }
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
      [type1,function1] () -> void {
    	getTaskGroup(type1).run_and_wait( function1 );
      },
      [type2,function2] () -> void {
    	getTaskGroup(type2).run_and_wait( function2 );
      },
      [type3,function3] () -> void {
    	getTaskGroup(type3).run_and_wait( function3 );
      },
      [type4,function4] () -> void {
    	getTaskGroup(type4).run_and_wait( function4 );
      },
      [type5,function5] () -> void {
    	getTaskGroup(type5).run_and_wait( function5 );
      }
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

