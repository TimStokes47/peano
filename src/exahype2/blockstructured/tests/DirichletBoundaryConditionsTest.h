// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/tests/TestCase.h"


namespace exahype2 {
  namespace blockstructured {
    namespace tests {
      class DirichletBoundaryConditionsTest;
    } // namespace tests
  }   // namespace blockstructured
} // namespace exahype2


/**
 * Test exahype2::blockstructured::applyDirichletBoundaryConditions()
 *
 * All tests work over the unit square/cube with patches of three grid cells
 * per axis, an overlap of two, and three doubles per grid cell. The
 * boundary condition functor stores the ghost cell's position along the
 * face normal, its position along the next axis, and the time stamp, so
 * the tests can validate where the routine has placed each ghost cell.
 * Entries the routine is not supposed to touch are initialised with a
 * marker value and have to keep it.
 */
class exahype2::blockstructured::tests::DirichletBoundaryConditionsTest:
  public tarch::tests::TestCase {
private:
  /**
   * Face on the lower boundary along each axis: the left half of the face
   * is set, layer 0 adjacent to the face, the right half is untouched.
   */
  void testLowerBoundary();

  /**
   * Face on the upper boundary along each axis: the right half of the
   * face is set, the left half is untouched.
   */
  void testUpperBoundary();

  /**
   * Boundary faces along periodic axes are left unchanged.
   */
  void testPeriodicBoundaryIsSkipped();

  /**
   * Faces within the domain are left unchanged.
   */
  void testInteriorFaceIsSkipped();

public:
  DirichletBoundaryConditionsTest();

  void run() override;
};
