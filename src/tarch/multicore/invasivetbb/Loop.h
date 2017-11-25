#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include "SHMInvade.hpp"


#define pfor(counter,from,to,minGrainSize) \
  { \
  SHMInvade invade( minGrainSize>1 ? (to-from)/minGrainSize : 1 );
  const int tailoredMinGrainSize = minGrainSize==0 ? to-from+1 : minGrainSize; \
  tbb::parallel_for( tbb::blocked_range<int>(from,to,tailoredMinGrainSize),\
  [&](const tbb::blocked_range<int>& r) { for(int counter=r.begin(); counter!=r.end(); ++counter) {

#define endpfor \
  }}); }
