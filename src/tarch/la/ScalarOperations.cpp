#include "tarch/la/ScalarOperations.h"
#include <cmath>


int tarch::la::abs (int value) {
  return value < 0 ? - value : value;
}


double tarch::la::abs(double value) {
  return std::abs(value);
}


int tarch::la::aPowI(int i,int a) {
  int result = 1;
  for (int d=0; d<i; d++) {
    result *= a;
  }
  return result;
}


bool tarch::la::greater (
  double lhs,
  double rhs,
  double tolerance
) {
  return lhs - rhs >  tolerance;
}


bool tarch::la::greaterEquals (
  double lhs,
  double rhs,
  double tolerance
) {
  return lhs - rhs >= - tolerance;
}


bool tarch::la::smaller (
  double lhs,
  double rhs,
  double tolerance
) {
  return lhs - rhs < -tolerance;
}


bool tarch::la::smallerEquals (
  double lhs,
  double rhs,
  double tolerance
) {
  return lhs - rhs <= tolerance;
}


bool tarch::la::equals (
  double lhs,
  double rhs,
  double tolerance
) {
  return std::abs(rhs - lhs) <= tolerance;
}


int tarch::la::sign (double number, double tolerance) {
  if ( greater(number, tolerance) ) {
     return 1;
  }
  else if ( greater(tolerance, number) ) {
     return -1;
  }
  return 0;
}


int tarch::la::round(double value) {
  return static_cast<int>(std::round(value));
}


int tarch::la::round(float value) {
  return static_cast<int>(std::round(value));
}
