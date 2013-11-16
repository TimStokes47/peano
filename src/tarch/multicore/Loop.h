// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org


#include "tarch/multicore/MulticoreDefinitions.h"

#ifdef SharedTBB
#include "tarch/multicore/tbb/Loop.h"
#elif SharedOMP
#include "tarch/multicore/omp/Loop.h"
#endif

/**
 * Basically is a for loop that can be parallelised.
 *
 * If you have multicore switched on, the multicore variant replaces the
 * statement with a parallel loop fragment. Please take care that all
 * three arguments have exactly the same time, i.e. avoid to mix integer
 * with signed integer or similar things.
 */
#ifndef SharedMemoryParallelisation
#define pfor(counter,from,to) \
  for (int counter=from; i<to; counter++) {


#define endpfor }
#endif
