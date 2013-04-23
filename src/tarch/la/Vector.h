// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_LA_VECTOR_H_
#define _TARCH_LA_VECTOR_H_


namespace tarch {
  namespace la {
    template<int Size, typename Scalar>
    struct Vector;
  }
}


#include "tarch/la/VectorAssignList.h"
#include "tarch/la/VectorOperations.h"
#include "tarch/la/VectorScalarOperations.h"
#include "tarch/la/VectorVectorOperations.h"
#include "tarch/la/VectorSlice.h"


#include "tarch/compiler/CompilerSpecificSettings.h"



template<int Size, typename Scalar>
class tarch::la::Vector {
  private:
//    Scalar _values[Size] __attribute__((aligned(VectorisationAlignment)));
    Scalar _values[Size];

  public:
    Vector ();

    /**
     * Assignment operator for any vector type.
     */
    Vector<Size,Scalar>& operator= (const Vector<Size,Scalar>& toAssign);

    /**
     * Assignment operator for list of comma separated scalar values, that has to
     * match the number of components of the vector. Otherwise a runtime assertion
     * goes wrong.
     */
    VectorAssignList<Size,Scalar> operator=(const Scalar& value);

    /**
     * Copy constructor to copy from any vector type.
     *
     * The only way to accomplish this with enable-if is to specify a second
     * dummy argument with default value, which is (hopefully) optimized away.
     */
    Vector(const Vector<Size,Scalar>&  toCopy);


    /**
     * Construct new vector and initialize all components with initialValue.
     */
    Vector (const Scalar& initialValue);

    /**
     * Construct new 2D vector and initialize components separately.
     *
     * Runtime assertion when dimension is not 2.
     */
    Vector (const Scalar& initialValue0,
            const Scalar& initialValue1);

    /**
     * Construct new 3D vector and initialize components separately.
     *
     * Runtime assertion when dimension is not 3.
     */
    Vector (const Scalar& initialValue0,
            const Scalar& initialValue1,
            const Scalar& initialValue2);

    /**
     * Construct new 4D vector and initialize components separately.
     *
     * Runtime assertion when dimension is not 4.
     */
    Vector (const Scalar& initialValue0,
            const Scalar& initialValue1,
            const Scalar& initialValue2,
            const Scalar& initialValue3);

    /**
     * Returns the number of components of the vector.
     */
    int size() const;

    /**
     * Returns read-only ref. to component of given index.
     */
    const Scalar & operator[] (int index) const;

    /**
     * Returns ref. to component of given index.
     */
    Scalar & operator[] (int index);

    /**
     * Returns read-only ref. to component of given index.
     */
    const Scalar & operator() (int index) const;

    /**
     * Returns ref. to component of given index.
     */
    Scalar & operator() (int index);

    std::string toString() const;

    template <typename NewScalarType>
    tarch::la::Vector<Size,NewScalarType> convertScalar() const;
};


#include "tarch/la/Vector.cpph"


#endif
