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
 *
 * !!! Catching
 *
 * My default pfor implementations catch the environment via references. Please
 * also take into account that some older gcc versions (<=4.6.1) have a bug and
 * do not capture const variables from outside:
 *
 * http://stackoverflow.com/questions/6529177/capturing-reference-variable-by-copy-in-c0x-lambda
 *
 *
 * @param counter   Name of the counter (identifier).
 * @param from      Start value of counter (integer).
 * @param to        End value of counter (integer).
 * @param minGrainSize Smallest grain size (integer). If the loop is split up
 *                  into subloops, none of them has less than minGrainSize
 *                  entries. In the serial version, this parameter has no
 *                  impact/meaning.
 */
#ifndef SharedMemoryParallelisation
#define pfor(counter,from,to,minGrainSize) \
  for (int counter=from; counter<to; counter++) {


#define endpfor }
#endif
