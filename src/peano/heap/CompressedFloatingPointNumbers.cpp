#include "peano/heap/CompressedFloatingPointNumbers.h"
#include "tarch/Assertions.h"


void peano::heap::decompose(
  double        value,
  char          exponent[8],
  long int      mantissa[8],
  double        error[8]
) {
  assertion(value==value);

  int shiftExponent = 7;

  const long int sign = value < 0.0 ? -1 : 1;

  int           integerExponent;
  const double  significant          = std::frexp(value , &integerExponent);
  char*         pMantissasLowestByte = nullptr;

  for (int i=0; i<8; i++) {
    const double shiftMantissa    = std::pow( 2.0,shiftExponent );

    exponent[i]  = static_cast<char>( integerExponent-shiftExponent );
    mantissa[i]  = sign * static_cast<long int>( std::round(significant*shiftMantissa) );
    error[i]     = std::abs( std::ldexp(sign * mantissa[i],exponent[i]) - value );

    assertion5( mantissa[i]>=0, value, mantissa[i], exponent[i], error[i], sign );

    pMantissasLowestByte   = reinterpret_cast<char*>( &(mantissa[i]) );
    pMantissasLowestByte  += i;
    *pMantissasLowestByte *= sign;

    shiftExponent+=8;
  }
}


double peano::heap::compose(
  char         exponent,
  long int     mantissa,
  int          bytesUsed
) {
  assertion( bytesUsed>=1 );

  char*  pMantissasLowestByte = reinterpret_cast<char*>( &mantissa );
  pMantissasLowestByte += (bytesUsed-1);

  double sign = 1.0;
  if (*pMantissasLowestByte<0) {
    sign = -1.0;
    *pMantissasLowestByte *= -1;
  }
  //    pMantissasLowestByte   = reinterpret_cast<char*>( &(mantissa[i]) );
  return sign * std::ldexp(mantissa,exponent);
}


