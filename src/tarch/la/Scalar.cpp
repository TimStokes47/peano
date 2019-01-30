#include "tarch/la/Scalar.h"

#include <cmath>
#include <algorithm>


double tarch::la::max( double a, double b, double c) {
  return std::max( a, std::max(b,c) );
}


double tarch::la::relativeEps( double valueA, double valueB, double eps ) {
  return std::max(valueA,valueB) <= 1.0 ? eps : eps * std::max(valueA,valueB);
}

