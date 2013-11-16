#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"


#define pfor(counter,from,to) \
  tbb::parallel_for(from,to,1,[&](int counter) {


#define endpfor \
  });
