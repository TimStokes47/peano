// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TOOLBOX_MULTIPRECISION_COMPRESSED_LINEAR_ALGEBRA_H_
#define _TOOLBOX_MULTIPRECISION_COMPRESSED_LINEAR_ALGEBRA_H_


#include "tarch/la/Vector.h"
#include "tarch/la/Matrix.h"


namespace toolbox {
  /**
   * The heap namespace holds all routines that we use in connection with
   * Peano's heap data storage. This means
   *
   * - the actual heap classes
   * - allocators used for heaps and architectures requiring concurrent and aligned layouts
   * - data conversion/compression routines
   */
  namespace multiprecision {
    void compress();
  }
}


#endif

