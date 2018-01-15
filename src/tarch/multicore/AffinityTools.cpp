#include "tarch/multicore/AffinityTools.h"
#include "tarch/multicore/Core.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/compiler/CompilerSpecificSettings.h"


#ifdef CompilerHasSysinfo
#include <sys/sysinfo.h>
#include <sched.h>
#else
#include <thread>
#endif

#include <sstream>


int tarch::multicore::getNumberOfPhysicalCores() {
  #ifdef CompilerHasSysinfo
  return get_nprocs();
  #else
  return std::thread::hardware_concurrency();
  #endif
}


std::string tarch::multicore::tailoredAffinityMask( const AffinityMask& mask ) {
  std::ostringstream msg;
  for (int i=0; i<getNumberOfPhysicalCores(); i++) {
    msg << (mask[i] ? "x" : "0");
  }
  return msg.str();
}


std::bitset<sizeof(long int)*8> tarch::multicore::getCPUSet() {
  std::bitset<sizeof(long int)*8> result = 0;

//  https://yyshen.github.io/2015/01/18/binding_threads_to_cores_osx.html
  #ifdef CompilerHasSysinfo
  cpu_set_t cpuset;
  sched_getaffinity(0, sizeof(cpuset), &cpuset);

  for (long i = 0; i < getNumberOfPhysicalCores(); i++) {
    if (CPU_ISSET(i, &cpuset)) {
      result[i] = true;
    }
  }
  #endif

  return result;
}

/*
void tarch::multicore::logThreadAffinities() {
  static tarch::logging::Log _log("tarch::multicore");

  logInfo( "logThreadAffinities()", "number of physical cores=" << getNumberOfPhysicalCores() );
  logInfo( "logThreadAffinities()", "cpuset=" << tailoredAffinityMask(getCPUSet()) << " (" << getCPUSet().count() << " cores available to application/rank)" );

  std::vector<AffinityMask> coreAffinities = getThreadAffinities();
  std::vector<int>          coreCPUIds     = getCPUIdsThreadsAreRunningOn();

  AffinityMask combinedAffinityMask = 0;
  bool         overbookedCPU        = false;

  assertion1( coreAffinities.size()<128, coreAffinities.size() );

  for (int i=0; i<static_cast<int>(coreAffinities.size()); i++) {
    logInfo( "logThreadAffinities()",
      "thread " << i << " is running on cpu " << coreCPUIds[i] << " with core affinity " << tailoredAffinityMask( coreAffinities[i] )
    );
    overbookedCPU         = (combinedAffinityMask | coreAffinities[i]).any();
    combinedAffinityMask |= coreAffinities[i];
  }

  if (overbookedCPU) {
    logInfo( "logThreadAffinities()", "no or weak affinities set, i.e. individual cores might be overbooked with multiple threads" );
  }
  if (static_cast<int>(combinedAffinityMask.count())<tarch::multicore::Core::getInstance().getNumberOfThreads()) {
    logWarning( "logThreadAffinities()", "fewer cores made available to process than process' logical thread count" );
  }
  logInfo( "logThreadAffinities()", combinedAffinityMask.count() << " CPUs made available through affinity masks in total" );
}*/


int tarch::multicore::getCPUId() {
  #ifdef CompilerHasSysinfo
  return sched_getcpu();
  #else
  //  https://stackoverflow.com/questions/33745364/sched-getcpu-equivalent-for-os-x
  return 1;
  #endif
}


#ifndef SharedMemoryParallelisation
std::vector<int> tarch::multicore::getCPUIdsThreadsAreRunningOn() {
  std::vector<int> result;
  result.push_back( getCPUId() );
  return result;
}
#endif



