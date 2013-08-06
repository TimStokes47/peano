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



/**
 * Simple vector class
 */
template<int Size, typename Scalar>
struct tarch::la::Vector {
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



#include "tarch/la/VectorIntegerSpecialisation.h"


#include "tarch/la/Vector.cpph"



#endif
