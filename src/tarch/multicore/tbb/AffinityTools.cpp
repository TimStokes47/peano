#include "tarch/multicore/AffinityTools.h"
#include "tarch/multicore/Core.h"


#if defined(SharedTBB) || defined(SharedTBBInvade)

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>


std::vector<tarch::multicore::AffinityMask> tarch::multicore::getThreadAffinities() {
  std::vector<tarch::multicore::AffinityMask> coreAffinities( Core::getInstance().getNumberOfThreads(), -1 );

  tbb::parallel_for(
    tbb::blocked_range<size_t>(0,Core::getInstance().getNumberOfThreads(),1),
    [&] (const tbb::blocked_range<size_t>& r) {
      coreAffinities[r.begin()] = tarch::multicore::getCPUSet();
    },
    tbb::static_partitioner()
  );

  return coreAffinities;
}


std::vector<int> tarch::multicore::getCPUIdsThreadsAreRunningOn() {
  std::vector<int> result( Core::getInstance().getNumberOfThreads(), -1 );

  tbb::parallel_for(
    tbb::blocked_range<size_t>(0,Core::getInstance().getNumberOfThreads(),1),
    [&] (const tbb::blocked_range<size_t>& r) {
      result[r.begin()] = getCPUId();
    },
    tbb::static_partitioner()
  );

  return result;
}

#endif
