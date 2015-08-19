#include "peano/heap/CompressedFloatingPointNumbers.h"
#include "tarch/Assertions.h"


#include <bitset>


void peano::heap::decompose(
  double        value,
  char          exponent[8],
  long int      mantissa[8],
  double        error[8]
) {
  assertion(value==value);

  // We may not use 7 though we use seven out of eight bits for the first byte.
  // If we shift by seven, we can end up with the highest byte set for
  // 0.0155759, e.g.
  int shiftExponent = 6;

  const long int sign = value < 0.0 ? -1 : 1;

  if (sign<0) {
    value = -value;
  }

  int           integerExponent;
  const double  significand          = std::frexp(value , &integerExponent);

  for (int i=0; i<8; i++) {
    const double shiftMantissa    = std::pow( 2.0,shiftExponent );

    exponent[i]  = static_cast<char>( integerExponent-shiftExponent );
    mantissa[i]  = static_cast<long int>( std::round(significand*shiftMantissa) );
    error[i]     = std::abs( std::ldexp(mantissa[i],exponent[i]) - value );

    assertion5( mantissa[i]>=0, value, mantissa[i], exponent[i], error[i], sign );

    std::bitset<64>*  mantissaAsBitset = reinterpret_cast<std::bitset<64>*>( &(mantissa[i]) );

    #ifdef Asserts
    for (int j=(i+1)*8-1; j<64; j++) {
      assertion9(
        !(*mantissaAsBitset)[j],
        *mantissaAsBitset, value, static_cast<int>( exponent[i] ), mantissa[i], error[i], i, j, significand, integerExponent
      );
    }
    #endif

    if (sign<0) {
      assertion ( (*mantissaAsBitset)[ (i+1)*8-1 ]==false );
      mantissaAsBitset->flip( (i+1)*8-1 );
    }

    shiftExponent+=8;
  }
}


double peano::heap::compose(
  char         exponent,
  long int     mantissa,
  int          bytesUsed
) {
  assertion( bytesUsed>=1 );

  std::bitset<64>*  mantissaAsBitset = reinterpret_cast<std::bitset<64>*>( &mantissa );

  if ( (*mantissaAsBitset)[ bytesUsed*8-1 ] ) {
    mantissaAsBitset->flip( bytesUsed*8-1 );
    mantissa = -mantissa;
  }

  return std::ldexp(mantissa,exponent);
}


