#define pfor(counter,from,to,minGrainSize) \
  for (int counter=from; counter<to; counter++) {
//  #pragma omp parallel for

#define endpfor \
  }
