#include "tarch/multicore/AffinityTools.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/logging/Log.h"


#include <sys/sysinfo.h>
#include <sstream>


int tarch::multicore::getNumberOfPhysicalCores() {
  return get_nprocs();
}


std::string tarch::multicore::tailoredAffinityMask( const AffinityMask& mask ) {
  std::ostringstream msg;
  for (int i=0; i<getNumberOfPhysicalCores(); i++) {
    msg << mask[i] ? "x" : "0";
  }
  return msg;
}


std::bitset<sizeof(long int)*8> tarch::multicore::getCPUSet() {
  std::bitset<sizeof(long int)*8> result = 0;

  cpu_set_t cpuset;
  int coreMaskAnalysis = sched_getaffinity(0, sizeof(cpuset), &cpuset);

  for (long i = 0; i < getNumberOfPhysicalCores(); i++) {
    if (CPU_ISSET(i, &cpuset)) {
      result[i] = true;
    }
  }

  return result;
}


void tarch::multicore::logThreadAffinities() {
  static tarch::logging::Log _log("tarch::multicore");

  logInfo( "logThreadAffinities()", "number of physical cores=" << getNumberOfPhysicalCores() );
  logInfo( "logThreadAffinities()", "cpuset=" << tailoredAffinityMask(getCPUSet()) << " (" << getCPUSet().count() << " cores available to application/rank)" );

  std::vector<AffinityMask> coreAffinities = getThreadAffinities();
  for (int i=0; i<static_cast<int>(coreAffinities.size()); i++) {
    logInfo( "logThreadAffinities()",
      "thread " << i << " core affinity: " << tailoredAffinityMask( coreAffinities[i] )
    );
  }
}



#ifndef SharedMemoryParallelisation
std::vector<tarch::multicore::AffinityMask> tarch::multicore::getThreadAffinities() {
  return std::vector<tarch::multicore::AffinityMask>();
}
#endif



