#include "peano4/parallel/Tasks.h"


#include "tarch/Assertions.h"
#include "tarch/multicore/Tasks.h"


tarch::logging::Log  peano4::parallel::Tasks::_log( "peano4::parallel::Tasks" );
int                  peano4::parallel::Tasks::_locationCounter(0);


int peano4::parallel::Tasks::getLocationIdentifier(const std::string&  trace) {
  _locationCounter++;
  return _locationCounter-1;
}


#if defined(TBBInvade)
#include "tarch/multicore/Core.h"


shminvade::SHMInvade*  peano4::parallel::Tasks::_backgroundTaskInvade = nullptr;
#endif


peano4::parallel::Tasks::Tasks(
  std::function<bool ()>& function,
  TaskType                type,
  int                     location
) {
  switch (type) {
    case TaskType::Task:
      tarch::multicore::spawnTask( new tarch::multicore::TaskWithCopyOfFunctor(function) );
      break;
    case TaskType::HighBandwidthTask:
      tarch::multicore::spawnHighBandwidthTask( new tarch::multicore::TaskWithCopyOfFunctor(function) );
      break;
    case TaskType::HighPriorityTask:
      tarch::multicore::spawnHighPriorityTask( new tarch::multicore::TaskWithCopyOfFunctor(function) );
      break;
  }
}


peano4::parallel::Tasks::Tasks(
  tarch::multicore::Task*  task,
  TaskType                 type,
  int                      location
) {
  switch (type) {
    case TaskType::Task:
      tarch::multicore::spawnTask( task );
      break;
    case TaskType::HighBandwidthTask:
      tarch::multicore::spawnHighBandwidthTask( task );
      break;
    case TaskType::HighPriorityTask:
      tarch::multicore::spawnHighPriorityTask( task );
      break;
  }
}


peano4::parallel::Tasks::Tasks(
  const std::vector< tarch::multicore::Task* >& tasks,
  TaskType                 type,
  int                      location
) {
  switch (type) {
    case TaskType::Task:
      tarch::multicore::spawnAndWait( tasks );
      break;
    case TaskType::HighBandwidthTask:
      for (auto& p: tasks) {
        tarch::multicore::spawnHighBandwidthTask( p );
      }
      break;
    case TaskType::HighPriorityTask:
      for (auto& p: tasks) {
        tarch::multicore::spawnHighPriorityTask( p );
      }
      break;
  }
}



/*
peano4::parallel::Tasks::Tasks(
  std::function<bool ()>&& function1,
  std::function<bool ()>&& function2,
  TaskType                 type,
  int                      location
) {
  switch (type) {
    case TaskType::Task:
      tarch::multicore::tasks::spawnAndWait(
        function1,
        function2
	  );
      break;
    case TaskType::BackgroundTask:
      tarch::multicore::tasks::spawnBackgroundTask( new );
      break;
    case TaskType::BandwidthBoundBackgroundTask:
      break;
  }
 translateIntoJobType(type1),
	  translateIntoJobType(type2),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2)
    );

    #if defined(TBBInvade)
    invade.retreat();
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


peano4::parallel::Tasks::Tasks(
  std::function<bool ()>&& function1,
  std::function<bool ()>&& function2,
  std::function<bool ()>&& function3,
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  int                      location
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(3,3);

    #if defined(TBBInvade)
    shminvade::SHMInvade invade( 3 );
    #endif

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
	  translateIntoJobType(type1),
	  translateIntoJobType(type2),
	  translateIntoJobType(type3),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3)
    );

    #if defined(TBBInvade)
    invade.retreat();
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


peano4::parallel::Tasks::Tasks(
  std::function<bool ()>&& function1,
  std::function<bool ()>&& function2,
  std::function<bool ()>&& function3,
  std::function<bool ()>&& function4,
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  TaskType                 type4,
  int                      location
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(4,4);

    #if defined(TBBInvade)
    shminvade::SHMInvade invade( 4 );
    #endif

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
	  function4,
	  translateIntoJobType(type1),
	  translateIntoJobType(type2),
	  translateIntoJobType(type3),
	  translateIntoJobType(type4),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3),
	  translateIntoJobClass(type4)
    );

    #if defined(TBBInvade)
    invade.retreat();
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


peano4::parallel::Tasks::Tasks(
  std::function<bool ()>&& function1,
  std::function<bool ()>&& function2,
  std::function<bool ()>&& function3,
  std::function<bool ()>&& function4,
  std::function<bool ()>&& function5,
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  TaskType                 type4,
  TaskType                 type5,
  int                      location
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(4,4);

    #if defined(TBBInvade)
    shminvade::SHMInvade invade( 5 );
    #endif

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
	  function4,
	  function5,
	  translateIntoJobType(type1),
	  translateIntoJobType(type2),
	  translateIntoJobType(type3),
	  translateIntoJobType(type4),
	  translateIntoJobType(type5),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3),
	  translateIntoJobClass(type4),
	  translateIntoJobClass(type5)
    );

    #if defined(TBBInvade)
    invade.retreat();
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





peano4::parallel::Tasks::Tasks(
  std::function<bool ()>&& function1,
  std::function<bool ()>&& function2,
  std::function<bool ()>&& function3,
  std::function<bool ()>&& function4,
  std::function<bool ()>&& function5,
  std::function<bool ()>&& function6,
  std::function<bool ()>&& function7,
  std::function<bool ()>&& function8,
  std::function<bool ()>&& function9,
  std::function<bool ()>&& function10,
  std::function<bool ()>&& function11,
  std::function<bool ()>&& function12,
  TaskType                 type1,
  TaskType                 type2,
  TaskType                 type3,
  TaskType                 type4,
  TaskType                 type5,
  TaskType                 type6,
  TaskType                 type7,
  TaskType                 type8,
  TaskType                 type9,
  TaskType                 type10,
  TaskType                 type11,
  TaskType                 type12,
  int                      location
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(12-1,12-1);

    #if defined(TBBInvade)
    shminvade::SHMInvade invade( 12 );
    #endif

    tarch::multicore::jobs::spawnAndWait(
      function1,
	  function2,
	  function3,
	  function4,
	  function5,
      function6,
	  function7,
	  function8,
	  function9,
	  function10,
	  function11,
	  function12,
	  translateIntoJobType(type1),
	  translateIntoJobType(type2),
	  translateIntoJobType(type3),
	  translateIntoJobType(type4),
	  translateIntoJobType(type5),
	  translateIntoJobType(type6),
	  translateIntoJobType(type7),
	  translateIntoJobType(type8),
	  translateIntoJobType(type9),
	  translateIntoJobType(type10),
	  translateIntoJobType(type11),
	  translateIntoJobType(type12),
	  translateIntoJobClass(type1),
	  translateIntoJobClass(type2),
	  translateIntoJobClass(type3),
	  translateIntoJobClass(type4),
	  translateIntoJobClass(type5),
	  translateIntoJobClass(type6),
	  translateIntoJobClass(type7),
	  translateIntoJobClass(type8),
	  translateIntoJobClass(type9),
	  translateIntoJobClass(type10),
	  translateIntoJobClass(type11),
	  translateIntoJobClass(type12)
    );

    #if defined(TBBInvade)
    invade.retreat();
    #endif

    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(-(12-1),-(12-1));
  }
  else {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,12-1);
    function1();
    function2();
    function3();
    function4();
    function5();
    function6();
    function7();
    function8();
    function9();
    function10();
    function11();
    function12();
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(0,-(12-1));
  }
}


peano4::parallel::Tasks::Tasks(
  std::function<bool ()>&&  myTask,
  TaskType                  taskType
) {
  typedef tarch::multicore::jobs::GenericJobWithCopyOfFunctor           Job;
  tarch::multicore::jobs::spawn( new Job(myTask,translateIntoJobType(taskType),translateIntoJobClass(taskType) ) );
}
*/


/*
void peano4::parallel::Tasks::processBackgroundTasks() {
  #if defined(TBBInvade)
  if (_backgroundTaskInvade==nullptr) {
	_backgroundTaskInvade = new shminvade::SHMInvade(
	  tarch::multicore::Core::getInstance().getNumberOfThreads()
    );
  }
  #endif

  tarch::multicore::jobs::startToProcessBackgroundJobs();
}


bool peano4::parallel::Tasks::finishToProcessBackgroundJobs() {
  bool result = tarch::multicore::jobs::finishToProcessBackgroundJobs();
  peano::performanceanalysis::Analysis::getInstance().minuteNumberOfBackgroundTasks(
    tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs()
  );

  #if defined(TBBInvade)
  if (_backgroundTaskInvade!=nullptr) {
	delete _backgroundTaskInvade;
	_backgroundTaskInvade= nullptr;
  }
  #endif

  return result;
}
*/
