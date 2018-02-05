#include "peano/datatraversal/TaskSet.h"
#include "peano/performanceanalysis/Analysis.h"


#include <chrono>
#include <thread>


#include "tarch/multicore/Jobs.h"
#include "tarch/Assertions.h"

tarch::logging::Log  peano::datatraversal::TaskSet::_log( "peano::datatraversal::TaskSet" );



int  peano::datatraversal::TaskSet::translateIntoJobClass( TaskType type ) {
  const int Default = 32;
  switch ( type ) {
    case TaskType::RunAsSoonAsPossible:
   	  return 0;
    case TaskType::RunImmediately:
     	  return Default;
    case TaskType::LoadCells:
 	  return 1;
    case TaskType::LoadVertices:
 	  return 2;
    case TaskType::TriggerEvents:
 	  return 3;
    case TaskType::StoreCells:
 	  return 4;
    case TaskType::StoreVertices:
 	  return 5;
    case TaskType::Background:
    case TaskType::LongRunningBackground:
    case TaskType::PersistentBackground:
      assertion(false);
   	  return Default;
  }
  return Default;
}


bool peano::datatraversal::TaskSet::isTask( TaskType type ) {
  switch ( type ) {
    case TaskType::RunAsSoonAsPossible:
    case TaskType::RunImmediately:
   	  return true;
    case TaskType::LoadCells:
    case TaskType::LoadVertices:
    case TaskType::TriggerEvents:
    case TaskType::StoreCells:
    case TaskType::StoreVertices:
   	  return false;
    case TaskType::Background:
    case TaskType::LongRunningBackground:
    case TaskType::PersistentBackground:
      assertion(false);
   	  return false;
  }
  return false;
}


void peano::datatraversal::TaskSet::waitForLoadCellsTask() {
  logDebug( "waitForAllLoadCellsTasks()", "start to wait for load cell tasks" );
  tarch::multicore::jobs::processJobs(translateIntoJobClass(TaskType::LoadCells));
  logDebug( "waitForAllLoadCellsTasks()", "wait for load cell tasks finished" );
}


void peano::datatraversal::TaskSet::waitForLoadVerticesTask() {
  logDebug( "waitForAllLoadVerticesTasks()", "start to wait for load vertices tasks" );
  tarch::multicore::jobs::processJobs(translateIntoJobClass(TaskType::LoadVertices));
  logDebug( "waitForAllLoadVerticesTasks()", "wait for load vertices tasks finished" );
}


void peano::datatraversal::TaskSet::waitForEventTask() {
  logDebug( "waitForAllEventTasks()", "start to wait for event tasks" );
  tarch::multicore::jobs::processJobs(translateIntoJobClass(TaskType::TriggerEvents));
  logDebug( "waitForAllEventTasks()", "wait for event tasks finished" );
}


void peano::datatraversal::TaskSet::waitForStoreCellsTask() {
  logDebug( "waitForAllStoreCellsTasks()", "start to wait for store cell tasks" );
  tarch::multicore::jobs::processJobs(translateIntoJobClass(TaskType::StoreCells));
  logDebug( "waitForAllStoreCellsTasks()", "wait for store cell tasks finished" );
}


void peano::datatraversal::TaskSet::waitForStoreVerticesTask() {
  logDebug( "waitForAllStoreVerticesTasks()", "start to wait for store vertices tasks" );
  tarch::multicore::jobs::processJobs(translateIntoJobClass(TaskType::StoreVertices));
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

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
      isTask(type1),
	  isTask(type2),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2)
    );

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

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
      isTask(type1),
	  isTask(type2),
	  isTask(type3),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3)
    );

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

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
	  function4,
      isTask(type1),
	  isTask(type2),
	  isTask(type3),
	  isTask(type4),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3),
	  translateIntoJobClass(type4)
    );

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

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
	  function4,
	  function5,
      isTask(type1),
	  isTask(type2),
	  isTask(type3),
	  isTask(type4),
	  isTask(type5),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3),
	  translateIntoJobClass(type4),
	  translateIntoJobClass(type5)
    );

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


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&&  myTask,
  TaskType                  taskType
) {
  typedef tarch::multicore::jobs::GenericBackgroundJobWithCopyOfFunctor BackgroundJob;
  typedef tarch::multicore::jobs::GenericJobWithCopyOfFunctor           Job;
  switch (taskType) {
    case TaskType::RunImmediately:
      myTask();
      break;
    case TaskType::RunAsSoonAsPossible:
    case TaskType::LoadCells:
    case TaskType::LoadVertices:
    case TaskType::TriggerEvents:
    case TaskType::StoreCells:
    case TaskType::StoreVertices:
      tarch::multicore::jobs::spawn( new Job(myTask,false,translateIntoJobClass(taskType) ) );
      break;
    case TaskType::Background:
      tarch::multicore::jobs::spawnBackgroundJob( new BackgroundJob(myTask,tarch::multicore::jobs::BackgroundJobType::BackgroundJob) );
      break;
    case TaskType::LongRunningBackground:
      tarch::multicore::jobs::spawnBackgroundJob( new BackgroundJob(myTask,tarch::multicore::jobs::BackgroundJobType::LongRunningBackgroundJob) );
      break;
    case TaskType::PersistentBackground:
      tarch::multicore::jobs::spawnBackgroundJob( new BackgroundJob(myTask,tarch::multicore::jobs::BackgroundJobType::PersistentBackgroundJob) );
      break;
  }
}


