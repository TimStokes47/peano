#ifndef _PEANO_HEAP_COMPRESSED_FLOATING_POINT_NUMBERS_H_
#define _PEANO_HEAP_COMPRESSED_FLOATING_POINT_NUMBERS_H_


namespace peano {
  namespace heap {
    /**
     * Takes a double and returns the exponent and the mantissa.
     */
    template <class T>
    void decompose(
      const double& value,
      char&         exponent,
      T&            mantissa
    );

    /**
     * Decompose floating point value
     *
     * Decomposes the input value into eight different variants with
     * 1,2,3,... bytes (chars) per mantissa. It also gives the errors.
     *
     * You may want to cast one of the integers into a sequence of chars
     * or other data types. While the array mantissa comprises long ints,
     * only the first kth bytes are used in the k-1th entry.
     *
     * @return exponent Sequence of exponents belonging to the eight decompositions.
     * @return mantissa Sequence of mantissas belonging to the eight decompositions.
     * @return error    Errors associated to the decompositions. They are all positive values.
     */
    void decompose(
      const double& value,
      char          exponent[8],
      long int      mantissa[8],
      double        error[8]
    );

    /**
     * Counterpart of decompose()
     */
    template <class T>
    double compose(
      const char&  exponent,
      const T&     mantissa
    );
  }
}


#include "peano/heap/CompressedFloatingPointNumbers.cpph"

#endif
