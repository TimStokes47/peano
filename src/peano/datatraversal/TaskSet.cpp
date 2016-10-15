#include "peano/datatraversal/TaskSet.h"

#include "tarch/multicore/MulticoreDefinitions.h"


#ifdef SharedTBB
#include <tbb/parallel_invoke.h>
#endif


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()> function1,
  std::function<void ()> function2,
  bool                   parallelise
) {
  #ifdef SharedTBB
  if (parallelise) {
    tbb::parallel_invoke(
      function1,
      function2
    );
  }
  else {
    function1();
    function2();
  }
  #else
  function1();
  function2();
  #endif
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()> function1,
  std::function<void ()> function2,
  std::function<void ()> function3,
  bool                   parallelise
) {
  #ifdef SharedTBB
  if (parallelise) {
    tbb::parallel_invoke(
      function1,
      function2,
      function3
    );
  }
  else {
    function1();
    function2();
    function3();
  }
  #else
  function1();
  function2();
  function3();
  #endif
}


peano::datatraversal::TaskSet::TaskSet(
  std::function<void ()> function1,
  std::function<void ()> function2,
  std::function<void ()> function3,
  std::function<void ()> function4,
  bool                   parallelise
) {
  #ifdef SharedTBB
  if (parallelise) {
    tbb::parallel_invoke(
      function1,
      function2,
      function3,
      function4
    );
  }
  else {
    function1();
    function2();
    function3();
    function4();
  }
  #else
  function1();
  function2();
  function3();
  function4();
  #endif
}
