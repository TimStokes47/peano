#include "peano/datatraversal/TaskSet.h"


#ifdef SharedTBB
#include <tbb/parallel_invoke.h>



tbb::task_group_context  peano::datatraversal::TaskSet::_backgroundTaskContext;
#endif


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()>&& function1,
  std::function<void ()>&& function2,
  bool                     parallelise
) {
  if (parallelise) {
    peano::performanceanalysis::Analysis::getInstance().changeConcurrencyLevel(2,2);
    #ifdef SharedTBB
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
    #ifdef SharedTBB
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
    #ifdef SharedTBB
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
    #ifdef SharedTBB
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
