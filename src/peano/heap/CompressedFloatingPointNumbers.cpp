#include "peano/heap/CompressedFloatingPointNumbers.h"

void peano::heap::decompose(
  const double& value,
  char          exponent[8],
  long int      mantissa[8],
  double        error[8]
) {
  int shiftExponent = 7;

  int           integerExponent;
  const double  significant      = std::frexp(value , &integerExponent);

  for (int i=0; i<8; i++) {
    const double shiftMantissa    = std::pow( 2.0,shiftExponent );

    exponent[i]  = static_cast<char>( integerExponent-shiftExponent );
    mantissa[i]  = static_cast<long int>( std::round(significant*shiftMantissa) );
    error[i]     = std::abs( std::ldexp(mantissa[i],exponent[i]) - value );

    shiftExponent+=8;
  }
}
