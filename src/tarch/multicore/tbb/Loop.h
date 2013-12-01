#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"


#define pfor(counter,from,to,minGrainSize) \
  tbb::parallel_for( tbb::blocked_range<int>(from,to,minGrainSize),\
  [&](const tbb::blocked_range<int>& r) { for(int counter=r.begin(); counter!=r.end(); ++counter) {

#define endpfor \
  }});
