#include "tarch/la/Scalar.h"

#include <cmath>
#include <algorithm>


double tarch::la::absoluteWeight(
  double value,
  double relativeError
) {
  const double weight = std::max(
    1.0, std::abs(value)
  );
  return relativeError * weight;
}


double tarch::la::absoluteWeight(
  double value0,
  double value1,
  double relativeError
) {
  const double weight = std::max(
    1.0, std::min(
      std::abs(value0), std::abs(value1)
    )
  );
  return relativeError * weight;
}

