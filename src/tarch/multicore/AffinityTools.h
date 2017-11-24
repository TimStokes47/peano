// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_AFFINITY_TOOLS_H_
#define _TARCH_MULTICORE_AFFINITY_TOOLS_H_


#include <vector>
#include <bitset>


namespace tarch {
  namespace multicore {
    void logThreadAffinities();

    int getNumberOfPhysicalCores();

    typedef std::bitset<sizeof(long int)*8>   AffinityMask;
    /**
     * Returns an bitset (encoded as integer) where per core an entry is set if
     * the very core is available to the program. Cast it into a bitset of
     * length getNumberOfPhysicalCores() to get a better output.
     */
    AffinityMask getCPUSet();

    std::vector<AffinityMask> getThreadAffinities();

    std::string tailoredAffinityMask( const AffinityMask& mask );
  }
}


#endif
